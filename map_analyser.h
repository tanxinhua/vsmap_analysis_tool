#ifndef MAPANALYSER_H
#define MAPANALYSER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QFileInfo>

using SegmentData = struct SegmentData {
    int seg_num = 0;
    uint32_t address = 0;
    uint32_t length = 0;
    QString seg_name;
    QString class_name;
};


using FunctionData = struct FunData {
    int seg_num = 0;
    uint32_t address = 0;
    uint32_t length = 0;
    QString fun_name;
    QString undname_name;
    uint32_t rva_base;
    int type = 0;//0, 1是f， 2是f，i
    QString lib_name;
    QString file_name;
    bool static_symbols;
};

class MapAnalyser : public QObject
{
    Q_OBJECT
public:
    explicit MapAnalyser(QObject *parent = nullptr);
    QString Undname(const QString& wrap_name);
    QByteArray UndnameFileReadAll(const QString& input_path);
    bool UndnameFile(const QString& input_path, const QString& output_path = "");
    bool Analysis(const QString& input_path);
    void Clear();
    bool Finished();
    QFileInfo GetMapFileInfo();

    QMap<QString, int> GetLibLength() const;
    QMap<QString, int> GetFileLength() const;
    int GetCodeSegSize() const;
    int GetDataSegSize() const;
    QList<QList<FunctionData>> GetFunData() const;
signals:
private:
    bool finished_ = false;
    QList<SegmentData> list_segment_data_;
    QList<QList<FunctionData>> list_segnum_fun_data_;
    QList<int> list_seg_length_;
    QMap<QString, int> map_lib_length_;
    QMap<QString, int> map_file_length_;
    int codeseg_size_ = 0;
    int dataseg_size_ = 0;
    QString mapfile_path_;
};

#endif // MAPANALYSER_H
