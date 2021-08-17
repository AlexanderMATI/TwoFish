#include "form.h"
#include "ui_form.h"


Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}
void Form::updateBar(int num){

    /*if (num > 98){
        QMessageBox msgBox;
        msgBox.setWi6ndowTitle("Завершение");
        msgBox.setText("Работа завершена!");
        close();
    }*/

    ui->progressBar->setValue(num);
   // QCoreApplication::processEvents();
    //ui->progressBar->repaint();









}

Form::~Form()
{
    delete ui;
}
