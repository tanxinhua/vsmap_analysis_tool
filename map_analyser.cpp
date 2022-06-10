#include "map_analyser.h"

#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QApplication>
#include <QDir>
#include <QBuffer>
#include <QDebug>

MapAnalyser::MapAnalyser(QObject *parent)
    : QObject{parent}
{

}

QString MapAnalyser::Undname(const QString& wrap_name){
    QProcess undname;
    undname.start("undname", QStringList() << wrap_name);
    if (!undname.waitForStarted()) {
        QMessageBox::warning(nullptr, "Warning!", "undanme start faile!");
        return false;
    }
    if (!undname.waitForFinished()) {
        QMessageBox::warning(nullptr, "Warning!", "undanme wait for finished fiale!");
        return false;
    }
    QByteArray byte_array = undname.readAll();
    QBuffer buffer(&byte_array);
    buffer.open(QIODevice::ReadOnly);
    QString ret;
    while (!buffer.atEnd()) {
        QString str_line = buffer.readLine();
        if(str_line.startsWith("is :-")){
            auto list = str_line.simplified().split("\"");
            ret = list[1];
            break;
        }
    }
    return ret;
}

QByteArray MapAnalyser::UndnameFileReadAll(const QString& input_path) {
    QFile in_file(input_path);
    if (!in_file.exists()) {
        QMessageBox::warning(nullptr, "Warning!", "input file is not exist!");
        return false;
    }

    QProcess undname;
    undname.start("undname", QStringList() << input_path);
    if (!undname.waitForStarted()) {
        QMessageBox::warning(nullptr, "Warning!", "undanme start faile!");
        return false;
    }
    if (!undname.waitForFinished()) {
        QMessageBox::warning(nullptr, "Warning!", "undanme wait for finished fiale!");
        return false;
    }
    return undname.readAll();
}

bool MapAnalyser::UndnameFile(const QString& input_path, const QString& output_path) {
    QString out_path = output_path;
    QFile in_file(input_path);
    if (!in_file.exists()) {
        QMessageBox::warning(nullptr,"Warning!","input file is not exist!");
        return false;
    }
    if (output_path.isEmpty()) {
        QFileInfo info(input_path);
        out_path = info.fileName()+".txt";
    }
    QProcess undname;
    undname.start("undname", QStringList() << input_path);
    if (!undname.waitForStarted()) {
        QMessageBox::warning(nullptr, "Warning!", "undanme start faile!");
        return false;
    }
    if (!undname.waitForFinished()) {
        QMessageBox::warning(nullptr, "Warning!", "undanme wait for finished fiale!");
        return false;
    }
    QByteArray result = UndnameFileReadAll(input_path);
    QFile file(out_path);
    file.open(QIODevice::WriteOnly);
    file.write(result);
    file.flush();
    file.close();
    return true;
}

void MapAnalyser::Clear(){
    finished_ = false;
    list_segment_data_.clear();
    list_segnum_fun_data_.clear();
    list_seg_length_.clear();
    map_lib_length_.clear();
    map_file_length_.clear();
    codeseg_size_ = 0;
    dataseg_size_ = 0;
}

bool MapAnalyser::Analysis(const QString& input_path){
    QFile buffer(input_path);
//    QByteArray byte_array = Undname(input_path);
//    QBuffer buffer(&byte_array);
    if(!buffer.open(QIODevice::ReadOnly)){
        QMessageBox::warning(nullptr,"error!","Map file cant open!");
        return false;
    }
    mapfile_path_ = input_path;
    Clear();
///////////segment size
    while (!buffer.atEnd()) {
        QString str_line = buffer.readLine();
        auto list = str_line.simplified().split(" ");
        if (list[0] == "Start")
            break;
    }
    //statistics SegmentData
    while (!buffer.atEnd()) {
        QString str_line = buffer.readLine();
        auto str = str_line.simplified();
        if (str.isEmpty()) {
            if (list_segment_data_.size() > 0) {
                while (list_segnum_fun_data_.size() < list_segment_data_.last().seg_num + 1)
                    list_segnum_fun_data_.push_back(QList<FunctionData>());
            }
            break;
        }
        auto list = str.split(" ");
        auto address = list[0].split(":");
        if (address.size() != 2) {
            qWarning("address parsing  error occurred while parsing segment data!");
        }
        SegmentData seg_data;
        bool ok;
        seg_data.seg_num = address[0].toInt();
        seg_data.address = address[1].toInt(&ok, 16);
        seg_data.length = list[1].remove(list[1].size() - 1, 1).toInt(&ok, 16);
        seg_data.seg_name = list[2];
        seg_data.class_name = list[3];
        list_segment_data_.push_back(seg_data);
    }
    //count dataseg codeseg size
    for (auto item : list_segment_data_) {
        if (item.class_name == "CODE") {
            codeseg_size_ += item.length;
        }
        else if (item.class_name == "DATA") {
            dataseg_size_ += item.length;
        }
    }
    qInfo() << "code seg size:"
        << (double)codeseg_size_ / 1024 / 1024 << "m"
        << "data seg size:"
        << (double)dataseg_size_ / 1024 / 1024 << "m"
        << Qt::flush;

/////////////no Static symbols
    auto GetFunctionData = [this](QString &str_line, QStringList& list, QStringList& address, bool is_static) {
        FunctionData fun_data;
        bool ok;
        fun_data.seg_num = address[0].toInt();
        fun_data.address = address[1].toInt(&ok, 16);
        fun_data.fun_name = list[1];
        //fun_data.undname_name = Undname(list[1]);
        fun_data.rva_base = list[2].toInt(&ok, 16);
        fun_data.static_symbols = is_static;
        auto GetlibFileName = [&fun_data, &list, this](int i) {
            auto infos = list[i].split(":");
            if (infos.size() == 1) {
                fun_data.lib_name = infos[0];
                fun_data.file_name = infos[0];
            }
            else if(infos.size() == 2) {
                fun_data.lib_name = infos[0];
                fun_data.file_name = infos[1];
            }else{
                QMessageBox::warning(nullptr,"error!","infos is null!");
            }
            bool ok;
            fun_data.rva_base = list[i - 1].toInt(&ok, 16);
        };

        if (list.size() == 4) {
            fun_data.type = 0;
            GetlibFileName(3);
        }
        else if (list.size() == 5) {
            fun_data.type = 1;
            GetlibFileName(4);
        }
        else if (list.size() == 6) {
            fun_data.type = 2;
            GetlibFileName(5);
        }
        else{
            if(str_line.endsWith("* CIL library *:* CIL module *")){
                return;
            }
        }

        if(fun_data.lib_name.isEmpty()){
            QMessageBox::warning(nullptr,"error!","lib_name is null!");
        }
        list_segnum_fun_data_[fun_data.seg_num].push_back(fun_data);
    };

    while (!buffer.atEnd()) {
        QString str_line = buffer.readLine();
        auto list = str_line.simplified().split(" ");
        if (list[0] == "Address")
            break;
    }
    //statistics FunctionData
    while (!buffer.atEnd()) {
        QString str_line = buffer.readLine();
        auto str = str_line.simplified();
        if (str.isEmpty()) {
            continue;
        }
        auto list = str.split(" ");
        auto address = list[0].split(":");
        if (address.size() != 2) {
            if (list[0] != "entry") {
                qWarning("address parsing  error occurred while parsing function data!");
            }
            break;
        }
        GetFunctionData(str, list, address, false);
    }

/////////////Static symbols
    while (!buffer.atEnd()) {
        QString str_line = buffer.readLine();
        auto list = str_line.simplified().split(" ");
        if (list[0] == "Static")
            break;
    }
    //statistics FunctionData
    while (!buffer.atEnd()) {
        QString str_line = buffer.readLine();
        auto str = str_line.simplified();
        if (str.isEmpty()) {
            continue;
        }
        auto list = str.split(" ");
        auto address = list[0].split(":");
        if (address.size() != 2) {
            break;
        }
        GetFunctionData(str, list, address, true);
    }

//////////////get FunctionData length
    //sort
    for (auto& item : list_segnum_fun_data_) {
        qSort(item.begin(), item.end(), [](const FunctionData& d1, const FunctionData& d2)->bool {
            return d1.address < d2.address;
        });
    }
    //get segment lenght
    QList<int> list_seg_length;
    for (auto item : list_segment_data_) {
        while (list_seg_length.size() < item.seg_num+1) {
            list_seg_length.push_back(0);
        }
        list_seg_length[item.seg_num] = list_seg_length[item.seg_num] + item.length;
    }
    //get length
    for (auto& list_item : list_segnum_fun_data_) {
        for (int i = 0; i < list_item.size()-1; i++) {
            list_item[i].length = list_item[i+1].address - list_item[i].address;
        }
        if (list_item.size() > 0) {
            list_item[list_item.size() - 1].length = list_seg_length[list_item[0].seg_num] - list_item[list_item.size() - 1].address;
        }
    }
    //get lib legth;
    for (auto& list_item : list_segnum_fun_data_) {
        for (auto&item : list_item) {
            map_lib_length_[item.lib_name] = map_lib_length_[item.lib_name] + item.length;
            map_file_length_[item.file_name] = map_file_length_[item.file_name] + item.length;
        }
    }

    finished_ = true;

    return finished_;
}

bool MapAnalyser::Finished(){
    return finished_;
}

QFileInfo MapAnalyser::GetMapFileInfo(){
    QFileInfo ret(mapfile_path_);
    return ret;
}

QMap<QString, int> MapAnalyser::GetLibLength() const{
    return map_lib_length_;
}

QMap<QString, int> MapAnalyser::GetFileLength() const{
    return map_file_length_;
}

int MapAnalyser::GetCodeSegSize() const{
    return codeseg_size_;
}

int MapAnalyser::GetDataSegSize() const{
    return dataseg_size_;
}

QList<QList<FunctionData>> MapAnalyser::GetFunData() const{
    return list_segnum_fun_data_;
}
