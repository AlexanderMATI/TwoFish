#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtDebug>
#include <QInputDialog>
#include <QProcess>
#include "thread.h"

#include "crypto.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonFile_clicked()
{
    fileNameIn = QFileDialog::getOpenFileName(this, tr("Выбрать входной файл"),
                                                    "D:/Projects/QT/CourseWork/Data",
                                                      tr("*"));
    /* inputFile = std::ifstream(fileName.toStdString());
     if ( !inputFile.is_open() ) {
             qDebug() << "Ошибка открытия файла.\n";
             return;
         }*/

      fileNameOut = QFileDialog::getSaveFileName(this, tr("Выбрать выходной файл"),
                                                     "D:/Projects/QT/CourseWork/Data",
                                                       tr("*"));
     /* outputFile = std::ofstream(fileName.toStdString());
      if ( !outputFile.is_open() ) {
              qDebug()<< "Ошибка открытия файла.\n";
              return;
          }*/

      fileNameKey = QFileDialog::getOpenFileName(this, tr("Выбрать файл с ключом"),
                                                      "D:/Projects/QT/CourseWork/Data",
                                                        tr("*"));
    /* keyFile = std::ifstream(fileName.toStdString());
     if ( !keyFile.is_open() ) {
             qDebug() << "Ошибка открытия файла.\n";
             return;
         }*/



}

void MainWindow::on_pushButtonEn_clicked()
{
    std::string vi;
    if (ui->lineEdit->text().isEmpty())
        vi = "|";
    vi = ui->lineEdit->text().toStdString();
    ED = true;
    QString mode = ui->comboBox->currentText();
    Thread *thread = new Thread(fileNameIn, fileNameOut,fileNameKey,ED, mode,vi);
    thread->start();


}

void MainWindow::update(double progress){
    double d = progress*100;
    QString temp = " Прогресс: "+ (int)d;
    temp +="%";

    ui->labelProgress->setText(temp);


}

void MainWindow::on_pushButtonDe_clicked()
{

    std::string vi = ui->lineEdit->text().toStdString();
    ED = false;
    QString mode = ui->comboBox->currentText();
    Thread *thread = new Thread(fileNameIn, fileNameOut,fileNameKey,ED, mode,vi);
    thread->start();


}


