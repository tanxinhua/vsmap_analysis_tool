#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include "map_analyser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    analyser_ = new MapAnalyser(this);
    analyser_2_= new MapAnalyser(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_file_open_pressed()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open the file"), tr(""), tr("Document(*.map)"));
    ui->lineEdit_file_path->setText(file_name);
}


void MainWindow::on_pushButton_analyze_pressed()
{
    auto file_path = ui->lineEdit_file_path->text();
    if(file_path.isEmpty()){
        file_path = QFileDialog::getOpenFileName(this, tr("Open the file"), tr(""), tr("Document(*.map)"));
        ui->lineEdit_file_path->setText(file_path);
    }
    if(!file_path.isEmpty()){
        analyser_->Analysis(file_path);
    }
}


void MainWindow::on_pushButton_lib_length_clicked()
{
    if(analyser_->Finished()){
        ui->plainTextEdit->clear();
        auto map_lib_length = analyser_->GetLibLength();
        auto item = map_lib_length.begin();
        while(item!=map_lib_length.end()){
            ui->plainTextEdit->appendPlainText(item.key()+" : "+ QString::number(item.value()/1024.0)+"KB");
            item++;
        }
    }else{
        QMessageBox::warning(this, "Remind info!", "first click analyze button!");
    }
}


void MainWindow::on_pushButton_file_length_clicked()
{
    if(analyser_->Finished()){
        ui->plainTextEdit->clear();
        auto map_file_length = analyser_->GetFileLength();
        auto item = map_file_length.begin();
        while(item!=map_file_length.end()){
            ui->plainTextEdit->appendPlainText(item.key()+" : "+ QString::number(item.value()/1024.0)+"KB");
            item++;
        }
    }else{
        QMessageBox::warning(this, "Remind info!", "first click analyze button!");
    }
}


void MainWindow::on_pushButton_analyze_2_clicked()
{
    auto file_path = ui->lineEdit_file_path_2->text();
    if(file_path.isEmpty()){
        file_path = QFileDialog::getOpenFileName(this, tr("Open the file"), tr(""), tr("Document(*.map)"));
        ui->lineEdit_file_path_2->setText(file_path);
    }
    if(!file_path.isEmpty()){
        analyser_2_->Analysis(file_path);
    }
}


void MainWindow::on_pushButton_lib_length_2_clicked()
{
    if(analyser_2_->Finished()){
        ui->plainTextEdit->clear();
        auto map_lib_length = analyser_2_->GetLibLength();
        auto item = map_lib_length.begin();
        while(item!=map_lib_length.end()){
            ui->plainTextEdit->appendPlainText(item.key()+" : "+ QString::number(item.value()/1024.0)+"KB");
            item++;
        }
    }else{
        QMessageBox::warning(this, "Remind info!", "first click analyze button!");
    }
}


void MainWindow::on_pushButton_file_length_2_clicked()
{
    if(analyser_2_->Finished()){
        ui->plainTextEdit->clear();
        auto map_file_length = analyser_2_->GetFileLength();
        auto item = map_file_length.begin();
        while(item!=map_file_length.end()){
            ui->plainTextEdit->appendPlainText(item.key()+" : "+ QString::number(item.value()/1024.0)+"KB");
            item++;
        }
    }else{
        QMessageBox::warning(this, "Remind info!", "first click analyze button!");
    }
}

void MainWindow::on_pushButton_compare_clicked()
{

}

void MainWindow::on_pushButton_save_to_xlsx_clicked()
{
    QXlsx::Document xlsx;
    QXlsx::Format format1;
    format1.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    format1.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    format1.setFontBold(true);
    xlsx.write("A1", "LibName_"+analyser_->GetMapFileInfo().baseName(),format1);
    xlsx.write("B1", "Length(KB)",format1);
    int i = 2;
    if(analyser_->Finished()){
        auto map_lib_length = analyser_->GetLibLength();
        auto item = map_lib_length.begin();
        while(item!=map_lib_length.end()){
            auto a_column = QString("A%1").arg(i);
            xlsx.write(a_column, item.key());
            auto b_column = QString("B%1").arg(i);
            xlsx.write(b_column, item.value()/1024.0);
            item++;
            i++;
        }
        i = i+1;
        xlsx.write(QString("A%1").arg(i), "CodeSegSize");
        xlsx.write(QString("B%1").arg(i), (double)(analyser_->GetCodeSegSize())/1024);
        i = i+1;
        xlsx.write(QString("A%1").arg(i), "DataSegSize");
        xlsx.write(QString("B%1").arg(i), (double)(analyser_->GetDataSegSize())/1024);

    }

    int ii = 2;
    xlsx.write("D1", "LibName_"+analyser_2_->GetMapFileInfo().baseName(),format1);
    xlsx.write("E1", "Length(KB)",format1);
    if(analyser_2_->Finished()){
        auto map_lib_length = analyser_2_->GetLibLength();
        auto item = map_lib_length.begin();
        while(item!=map_lib_length.end()){
            auto a_column = QString("D%1").arg(ii);
            xlsx.write(a_column, item.key());
            auto b_column = QString("E%1").arg(ii);
            xlsx.write(b_column, item.value()/1024.0);
            item++;
            ii++;
        }
        ii = ii+1;
        xlsx.write(QString("D%1").arg(ii), "CodeSegSize");
        xlsx.write(QString("E%1").arg(ii), (double)(analyser_2_->GetCodeSegSize())/1024);
        ii = ii+1;
        xlsx.write(QString("D%1").arg(ii), "DataSegSize");
        xlsx.write(QString("E%1").arg(ii), (double)(analyser_2_->GetDataSegSize())/1024);
    }
    if(i>2||ii>2){
        int index_num = 0;
        QString save_file_name;
        while(true){
            save_file_name = QString::number(index_num)
                            +"-"
                            +analyser_->GetMapFileInfo().baseName()
                            +"-"
                            +analyser_2_->GetMapFileInfo().baseName()
                            +".xlsx";
            QFileInfo info(save_file_name);
            if(!info.exists()){
                break;
            }
            index_num++;
        }
        xlsx.saveAs(save_file_name);
        QMessageBox::information(this, "save ok!", save_file_name+"save succeed!");
    }else{
        QMessageBox::warning(this, "Remind info!", "first click analyze button!");
    }
}


void MainWindow::on_pushButton_undname_clicked()
{
    auto file_path = ui->lineEdit_file_path->text();
    if(!file_path.isEmpty() && analyser_->UndnameFile(file_path)){

    }

    file_path = ui->lineEdit_file_path_2->text();
    if(!file_path.isEmpty() && analyser_2_->UndnameFile(file_path)){

    }
}


void MainWindow::on_pushButton_save_func_to_xlsx_clicked()
{
    QXlsx::Document xlsx;
    QXlsx::Format format1;
    format1.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    format1.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    format1.setFontBold(true);
    xlsx.write("A1", "LibName_"+analyser_->GetMapFileInfo().baseName(),format1);
    xlsx.write("B1", "DataName",format1);
    xlsx.write("C1", "Length(KB)",format1);
    int i = 2;
    if(analyser_->Finished()){
        QList<QList<FunctionData>> list_fun_data = analyser_->GetFunData();
        for(auto list_fun : list_fun_data ){
            auto item = list_fun.begin();
            while(item!=list_fun.end()){
                auto a_column = QString("A%1").arg(i);
                xlsx.write(a_column, item->lib_name);
                auto b_column = QString("B%1").arg(i);
                xlsx.write(b_column, item->fun_name);
                auto c_column = QString("C%1").arg(i);
                xlsx.write(c_column, item->length/1024.0);
                item++;
                i++;
            }
        }
    }

    xlsx.write("E1", "LibName_"+analyser_2_->GetMapFileInfo().baseName(),format1);
    xlsx.write("F1", "DataName",format1);
    xlsx.write("G1", "Length(KB)",format1);
    int ii = 2;
    if(analyser_2_->Finished()){
        QList<QList<FunctionData>> list_fun_data = analyser_2_->GetFunData();
        for(auto list_fun : list_fun_data ){
            auto item = list_fun.begin();
            while(item!=list_fun.end()){
                auto a_column = QString("E%1").arg(ii);
                xlsx.write(a_column, item->lib_name);
                auto b_column = QString("F%1").arg(ii);
                xlsx.write(b_column, item->fun_name);
                auto c_column = QString("G%1").arg(ii);
                xlsx.write(c_column, item->length/1024.0);
                item++;
                ii++;
            }
        }
    }
    if(i>2||ii>2){
        int index_num = 0;
        QString save_file_name;
        while(true){
            save_file_name = QString::number(index_num)
                            +"-Function-"
                            +analyser_->GetMapFileInfo().baseName()
                            +"-"
                            +analyser_2_->GetMapFileInfo().baseName()
                            +".xlsx";
            QFileInfo info(save_file_name);
            if(!info.exists()){
                break;
            }
            index_num++;
        }
        xlsx.saveAs(save_file_name);
        QMessageBox::information(this, "save ok!", save_file_name+"save succeed!");
    }else{
        QMessageBox::warning(this, "Remind info!", "first click analyze button!");
    }
}
