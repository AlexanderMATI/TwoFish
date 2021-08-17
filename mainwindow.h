#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <fstream>
#include "crypto.h"
#include <iostream>
#include <string>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();




private slots:


    void on_pushButtonFile_clicked();

    void on_pushButtonEn_clicked();

    void on_pushButtonDe_clicked();

    void update(double progress);

protected:
    Ui::MainWindow *ui;
    QString fileNameIn;
    QString fileNameOut;
    QString fileNameKey;
    bool ED = true;
    unsigned char vector[16];


};
#endif // MAINWINDOW_H
