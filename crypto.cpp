#include "crypto.h"
#include <iostream>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <QCoreApplication>
#include <QtDebug>
#include <iostream>
#include "mainwindow.h"
#include "form.h"
#include <QString>
#include <QWidget>
#include <QInputDialog>

Crypto::Crypto(const unsigned char *_key, int keylen) :
    mode_(ECB),
    twofish(_key, keylen)
{
    srand(time(NULL));
}

void Crypto::setKey(const unsigned char *_key, int keylen)
{
    twofish.key_setup(_key, keylen);
}

void Crypto::setMode(Crypto::mode mode)
{
    mode_ = mode;
}

void Crypto::setMode(std::string mode)
{
    if ( mode == "ECB" )
        setMode(ECB);
    else if ( mode == "CBC" )
        setMode(CBC);
    else if ( mode == "CFB" )
        setMode(CFB);
    else if ( mode == "OFB" )
        setMode(OFB);

}

void memxor(unsigned char *a, unsigned char *b, const int len)
{
    for(int i=0;i<len;++i)
        a[i]^=b[i];
}
void memxor(unsigned char *a,unsigned char *b,unsigned char *c, const int len)
{
    for(int i=0;i<len;++i)
        a[i]=b[i]^c[i];
}
void memrand(unsigned char *a,const int len)
{
    for(int i=0;i<len;++i)
        a[i]=rand()&0xff;
}

void Crypto::Calc(std::istream &in, unsigned char plaintext[], int len) {
    in.read((char *)plaintext, 16);
    len = in.gcount();
    if ( len < 16 ) {
        memset(&plaintext[len-1], 0, 16-len+1);
    }
}
/*void Crypto::send(std::istream &in, unsigned long int size){

    emit send((double)(in.tellg() / size));
}*/
void Crypto::encrypt(std::istream &in, std::ostream &out, std::string vi, unsigned long int size)
{
    unsigned char plaintext[16];
    unsigned char ciphertext[16];
    unsigned char vector[16];
    unsigned char buff[16];

    unsigned long int num = 0;
    double temp;
    Form *pBar = new Form();
    pBar->show();
   ///if (vector )
      std::strcpy((char*)vector,vi.c_str());

   bool first = true;
   while (in){
       switch (mode_) {
       case ECB:
           Calc(in, plaintext, 16);
           twofish.encrypt(plaintext, ciphertext);
           out.write((char*)ciphertext,16);
           num +=16;
           temp = ((double)num/(double)size)*100;
           pBar->updateBar((int)temp);

           break;
       case CBC:
           if (first){
               if (vector[0]=='|') //если строка была пустой
               {
                   memrand(vector,16);
               }
           /*инит. вектор*/
            //strcpy(vector, MainWindow::vi());
           //if (vector == 0)
           //memrand(vector,16);


           out.write((char*)vector,16);
           first = false;
           }

               Calc(in, plaintext, 16);
               memxor(plaintext, vector, 16);
               twofish.encrypt(plaintext, ciphertext);
               out.write((char*)ciphertext,16);
               memcpy(vector, ciphertext, 16);
               num +=16;
               temp = ((double)num/(double)size)*100;
               pBar->updateBar((int)temp);


           break;

       case CFB:
           if (first){
           /*инит. вектор*/
            //strcpy(vector, MainWindow::vi());
           //if (vector == 0)
           //memrand(vector,16);


           out.write((char*)vector,16);
           first = false;
           }
           twofish.encrypt(vector, buff);
           Calc(in, plaintext, 16);
           memxor(ciphertext, buff, plaintext, 16);
           out.write((char *)ciphertext,16);
           memcpy(vector, ciphertext, 16);
           num +=16;
           temp = ((double)num/(double)size)*100;
           pBar->updateBar((int)temp);

           break;
       case OFB:
           if (first){
           /*инит. вектор*/
            //strcpy(vector, MainWindow::vi());
           //if (vector == 0)
           //memrand(vector,16);


           out.write((char*)vector,16);
           first = false;
           }
           twofish.encrypt(vector, buff);

           Calc(in, plaintext, 16);
           memxor(ciphertext, buff, plaintext, 16);
           out.write((char *)ciphertext,16);

           memcpy(vector, buff, 16);
           num +=16;
           temp = ((double)num/(double)size)*100;
           pBar->updateBar((int)temp);


           break;
       default:
           break;
       }



   }
   pBar->close();

}

#include <stdio.h>
void Crypto::decrypt(std::istream &in, std::ostream &out, std::string vi, unsigned long int size)
{
    unsigned char ciphertext[16];
    unsigned char plaintext[16];
    unsigned char vector[16];
    unsigned char buff[16];
    unsigned long int num = 0;
    double temp;
    Form *pBar = new Form();
    pBar->show();
     std::strcpy((char*)vector,vi.c_str());
    int len = 0;
    switch(mode_) {
    case ECB:
        while( in ) {
            in.read((char*)ciphertext,16);
            if ( !in ) break;
            len = in.gcount();
            if ( len < 16 ) {
                memset(&ciphertext[len+1], 0, 16-len);
            }
            twofish.decrypt(ciphertext, plaintext);
            out.write((char*)plaintext,len);
            num +=16;
            temp = ((double)num/(double)size)*100;
            pBar->updateBar((int)temp);
        }
        pBar->close();
        break;
    case CBC:
        in.read((char*)vector,16);

        while( in ) {
            in.read((char *)ciphertext,16);
            if ( !in ) break;
            len = in.gcount();
            if ( len < 16 ) {
                memset(&ciphertext[len+1], 0, 16-len);
            }
            twofish.decrypt(ciphertext, plaintext);
            memxor(plaintext, vector, 16);
            out.write((char*)plaintext,16);
            memcpy(vector, ciphertext, len);
            num +=16;
            temp = ((double)num/(double)size)*100;
            pBar->updateBar((int)temp);
        }
        pBar->close();
        break;
    case CFB:
        in.read((char*)vector,16);

        while( in ) {
            twofish.encrypt(vector, buff);

            in.read((char *)ciphertext,16);
            if ( !in ) break;
            memxor(plaintext, buff, ciphertext, 16);
            out.write((char *)plaintext,16);
            memcpy(vector, ciphertext, 16);
            num +=16;
            temp = ((double)num/(double)size)*100;
            pBar->updateBar((int)temp);
        }
        pBar->close();
        break;
    case OFB:
        in.read((char*)vector,16);

        while( in ) {
            twofish.encrypt(vector, buff);

            in.read((char *)ciphertext,16).eof();
            if ( !in ) break;
            memxor(plaintext, buff, ciphertext, 16);
            out.write((char *)plaintext,16);
            memcpy(vector, buff, 16);
            num +=16;
            temp = ((double)num/(double)size)*100;
            pBar->updateBar((int)temp);
        }
        pBar->close();

        break;
    default:
        break;
    }
}
