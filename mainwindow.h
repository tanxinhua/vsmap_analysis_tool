#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "xlsxdocument.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MapAnalyser;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_file_open_pressed();
    void on_pushButton_analyze_pressed();

    void on_pushButton_lib_length_clicked();

    void on_pushButton_file_length_clicked();

    void on_pushButton_analyze_2_clicked();

    void on_pushButton_lib_length_2_clicked();

    void on_pushButton_file_length_2_clicked();

    void on_pushButton_compare_clicked();

    void on_pushButton_save_to_xlsx_clicked();

    void on_pushButton_undname_clicked();

    void on_pushButton_save_func_to_xlsx_clicked();

private:
    Ui::MainWindow *ui = nullptr;
    MapAnalyser *analyser_ = nullptr;
    MapAnalyser *analyser_2_ = nullptr;
};
#endif // MAINWINDOW_H
