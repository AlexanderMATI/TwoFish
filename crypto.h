#ifndef crypto_HPP
#define crypto_HPP

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "twofish.h"
#include <fstream>
#include "thread"

#include <string>

class Crypto
{


    enum mode {
        ECB,
        CBC,
        CFB,
        OFB,
    };
    mode mode_;
    Twofish twofish;
    unsigned char vector[16];

public:
    Crypto(const unsigned char *_key = 0, int keylen = 0);
    void setKey(const unsigned char *_key, int keylen);
    void setMode(Crypto::mode mode);
    void setMode(std::string mode);
 signals:
    void send(int);


   // void send(std::istream &in, unsigned long int size);
    void encrypt(std::istream &in, std::ostream &out,std::string vi, unsigned long int size );
    void decrypt(std::istream &in, std::ostream &out,std::string vi, unsigned long int size);
    void Calc(std::istream &in, unsigned char plaintext[], int len);
};

#endif // crypto_HPP
