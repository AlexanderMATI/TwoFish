#include "thread.h"
#include "crypto.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QMainWindow>
#include <fstream>
#include <QFileInfo>
#include <string>
#include <cstring>
#include <iostream>
#include <QtDebug>
Thread::Thread(QString fileNameIn, QString fileNameOut, QString fileNameKey,bool ED, QString mode)
{


    this->_inputFile = std::ifstream(fileNameIn.toStdString());
     if ( !_inputFile.is_open() ) {
             qDebug() << "Ошибка открытия файла.\n";
             return;
         }


      this->_outputFile = std::ofstream(fileNameOut.toStdString());
      if ( !_outputFile.is_open() ) {
              qDebug()<< "Ошибка открытия файла.\n";
              return;
          }


    this->_keyFile = std::ifstream(fileNameKey.toStdString());
     if ( !_keyFile.is_open() ) {
             qDebug() << "Ошибка открытия файла.\n";
             return;
         }
     this->_ED = ED;
     this->_mode = mode;

     QFileInfo fileInfo(fileNameIn);
     this->_size = fileInfo.size();
     QFileInfo fileinfo2(fileNameOut);


}



void Thread::run()
{

    Crypto *crypto = new Crypto();


   qDebug()<< _mode;
    crypto->setMode(_mode.toStdString());

   std::string vi = ui->lineEdit->text().toStdString();


   if (this->_ED == true)
    crypto->encrypt(this->_inputFile, this->_outputFile, vi,this->_size);

       else

         crypto->decrypt(this->_inputFile, this->_outputFile,vi, this->_size);

   /*while (true){
       QFileInfo fileInfo1(fileNameIn);
       double sizeIn = fileInfo1.size();
       QFileInfo fileinfo2(fileNameOut);
       double sizeOut;

       sizeOut = fileinfo2.size();
       int temp = (int)((sizeOut/sizeIn)*100);
       if (temp > 99) break;
       else ui->progressBar->setValue(temp);

       }*/
   this->_inputFile.close();
   this->_outputFile.close();
   this->_keyFile.close();

   delete crypto;
}
