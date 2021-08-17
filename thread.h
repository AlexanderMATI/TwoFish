#ifndef THREAD_H
#define THREAD_H
#include <QThread>
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"


class Thread : public QThread, MainWindow
{

public:

    explicit Thread(QString fileNameIn, QString fileNameOut, QString fileNameKey, bool ED, QString mode, std::string vi);
    void run();



public:
    QString name;
    std::ofstream _outputFile;
    std::ifstream _inputFile;
    std::ifstream _keyFile;
    QString _mode;
    std::string _vi;
    bool _ED;
    unsigned long int _size;


};

#endif // THREAD_H
