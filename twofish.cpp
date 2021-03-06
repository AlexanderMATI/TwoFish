#include "twofish.h"
#include <stdio.h>
#include <string.h>

#define STORE32L(x, y)  { ulong32 __t = (x); memcpy(y, &__t, 4); }
#define LOAD32L(x, y)   { memcpy(&(x), y, 4); x &= 0xFFFFFFFF; }

#define ROL(x, y) ( (((unsigned long)(x)<<(unsigned long)((y)&31)) | (((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)
#define ROR(x, y) ( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | ((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)

#define byte(x, n) ((unsigned char)((x) >> (8 * (n))))

#define MDS_POLY          0x169
#define RS_POLY           0x14D

static const unsigned char RS[4][8] = {
    { 0x01, 0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E },
    { 0xA4, 0x56, 0x82, 0xF3, 0X1E, 0XC6, 0X68, 0XE5 },
    { 0X02, 0XA1, 0XFC, 0XC1, 0X47, 0XAE, 0X3D, 0X19 },
    { 0XA4, 0X55, 0X87, 0X5A, 0X58, 0XDB, 0X9E, 0X03 }
};

static const unsigned char qord[4][5] = {
   { 1, 1, 0, 0, 1 },
   { 0, 1, 1, 0, 0 },
   { 0, 0, 0, 1, 1 },
   { 1, 0, 1, 1, 0 }
};

static const unsigned char qbox[2][4][16] = {
{
   { 0x8, 0x1, 0x7, 0xD, 0x6, 0xF, 0x3, 0x2, 0x0, 0xB, 0x5, 0x9, 0xE, 0xC, 0xA, 0x4 },
   { 0xE, 0XC, 0XB, 0X8, 0X1, 0X2, 0X3, 0X5, 0XF, 0X4, 0XA, 0X6, 0X7, 0X0, 0X9, 0XD },
   { 0XB, 0XA, 0X5, 0XE, 0X6, 0XD, 0X9, 0X0, 0XC, 0X8, 0XF, 0X3, 0X2, 0X4, 0X7, 0X1 },
   { 0XD, 0X7, 0XF, 0X4, 0X1, 0X2, 0X6, 0XE, 0X9, 0XB, 0X3, 0X0, 0X8, 0X5, 0XC, 0XA }
},
{
   { 0X2, 0X8, 0XB, 0XD, 0XF, 0X7, 0X6, 0XE, 0X3, 0X1, 0X9, 0X4, 0X0, 0XA, 0XC, 0X5 },
   { 0X1, 0XE, 0X2, 0XB, 0X4, 0XC, 0X3, 0X7, 0X6, 0XD, 0XA, 0X5, 0XF, 0X9, 0X0, 0X8 },
   { 0X4, 0XC, 0X7, 0X5, 0X1, 0X6, 0X9, 0XA, 0X0, 0XE, 0XD, 0X8, 0X2, 0XB, 0X3, 0XF },
   { 0xB, 0X9, 0X5, 0X1, 0XC, 0X3, 0XD, 0XE, 0X6, 0X4, 0X7, 0XF, 0X2, 0X0, 0X8, 0XA }
}
};

static ulong32 sbox(int i, ulong32 x)
{
   unsigned char a0,b0,a1,b1,a2,b2,a3,b3,a4,b4,y;

   a0 = (unsigned char)((x>>4)&15);
   b0 = (unsigned char)((x)&15);

   a1 = a0 ^ b0;
   b1 = (a0 ^ ((b0<<3)|(b0>>1)) ^ (a0<<3)) & 15;

   a2 = qbox[i][0][(int)a1];
   b2 = qbox[i][1][(int)b1];

   a3 = a2 ^ b2;
   b3 = (a2 ^ ((b2<<3)|(b2>>1)) ^ (a2<<3)) & 15;

   a4 = qbox[i][2][(int)a3];
   b4 = qbox[i][3][(int)b3];

   /* y = 16b4 + a4 */
   y = (b4 << 4) + a4;

   return (ulong32)y;
}

static ulong32 gf_mult(ulong32 a, ulong32 b, ulong32 p)
{
   ulong32 result, B[2], P[2];

   P[1] = p;
   B[1] = b;
   result = P[0] = B[0] = 0;
   for (int i = 0; i<7; i++ ){
     result ^= B[a&1]; a >>= 1;  B[1] = P[B[1]>>7] ^ (B[1] << 1);
   }
   result ^= B[a&1];

   return result;
}

static ulong32 mds_column_mult(unsigned char in, int col)
{
   ulong32 x01, x5B, xEF;

   x01 = in;
   x5B = gf_mult(in, 0x5B, MDS_POLY);
   xEF = gf_mult(in, 0xEF, MDS_POLY);

   switch (col) {
       case 0:
          return (x01 << 0 ) |
                 (x5B << 8 ) |
                 (xEF << 16) |
                 (xEF << 24);
       case 1:
          return (xEF << 0 ) |
                 (xEF << 8 ) |
                 (x5B << 16) |
                 (x01 << 24);
       case 2:
          return (x5B << 0 ) |
                 (xEF << 8 ) |
                 (x01 << 16) |
                 (xEF << 24);
       case 3:
          return (x5B << 0 ) |
                 (x01 << 8 ) |
                 (xEF << 16) |
                 (x5B << 24);
   }
   return 0;
}

static void mds_mult(const unsigned char *in, unsigned char *out)
{
  int x;
  ulong32 tmp;
  for (tmp = x = 0; x < 4; x++) {
      tmp ^= mds_column_mult(in[x], x);
  }
  STORE32L(tmp, out);
}

static void rs_mult(const unsigned char *in, unsigned char * const out)
{
  int x, y;
  for (x = 0; x < 4; x++) {
      out[x] = 0;
      for (y = 0; y < 8; y++) {
          out[x] ^= gf_mult(in[y], RS[x][y], RS_POLY);
      }
  }
}

static void h_func(const unsigned char *in, unsigned char *out, unsigned char *M, int k, int offset)
{
  int x;
  unsigned char y[4];
  for (x = 0; x < 4; x++) {
      y[x] = in[x];
 }
  switch (k) {
     case 4:
            y[0] = (unsigned char)(sbox(1, (ulong32)y[0]) ^ M[4 * (6 + offset) + 0]);
            y[1] = (unsigned char)(sbox(0, (ulong32)y[1]) ^ M[4 * (6 + offset) + 1]);
            y[2] = (unsigned char)(sbox(0, (ulong32)y[2]) ^ M[4 * (6 + offset) + 2]);
            y[3] = (unsigned char)(sbox(1, (ulong32)y[3]) ^ M[4 * (6 + offset) + 3]);
     case 3:
            y[0] = (unsigned char)(sbox(1, (ulong32)y[0]) ^ M[4 * (4 + offset) + 0]);
            y[1] = (unsigned char)(sbox(1, (ulong32)y[1]) ^ M[4 * (4 + offset) + 1]);
            y[2] = (unsigned char)(sbox(0, (ulong32)y[2]) ^ M[4 * (4 + offset) + 2]);
            y[3] = (unsigned char)(sbox(0, (ulong32)y[3]) ^ M[4 * (4 + offset) + 3]);
     case 2:
            y[0] = (unsigned char)(sbox(1, sbox(0, sbox(0, (ulong32)y[0]) ^ M[4 * (2 + offset) + 0]) ^ M[4 * (0 + offset) + 0]));
            y[1] = (unsigned char)(sbox(0, sbox(0, sbox(1, (ulong32)y[1]) ^ M[4 * (2 + offset) + 1]) ^ M[4 * (0 + offset) + 1]));
            y[2] = (unsigned char)(sbox(1, sbox(1, sbox(0, (ulong32)y[2]) ^ M[4 * (2 + offset) + 2]) ^ M[4 * (0 + offset) + 2]));
            y[3] = (unsigned char)(sbox(0, sbox(1, sbox(1, (ulong32)y[3]) ^ M[4 * (2 + offset) + 3]) ^ M[4 * (0 + offset) + 3]));
  }
  mds_mult(y, out);
}

static ulong32 g_func(ulong32 x, Twofish::Key  *key)
{
   unsigned char g, i, y, z;
   ulong32 res;

   res = 0;
   for (y = 0; y < 4; y++) {
       z = key->start;

       g = sbox(qord[y][z++], (x >> (8*y)) & 255);

       i = 0;

       while (z != 5) {
          g = g ^ key->S[0][(4*(i++)) + y];
          g = sbox(qord[y][z++], g);
       }

       res ^= mds_column_mult(g, y);
   }
   return res;
}

#define g1_func(x, key) g_func(ROL(x, 8), key)


Twofish::Key Twofish::getKey() const
{
    return real_key;
}

void Twofish::setKey(const Key &value)
{
    real_key = value;
}

Twofish::Twofish(const unsigned char *_key, int keylen) :
    skey(&real_key)
{
    key_setup(_key, keylen);
}

Twofish::~Twofish()
{
    memset(this->skey, 0, sizeof(*this->skey));
    memset(this->skey, 0, sizeof(*this->skey));
    memset(this->skey, 0, sizeof(*this->skey));
    memset(this->skey, 0, sizeof(*this->skey));
}

void Twofish::key_setup(const unsigned char *_key, int keylen)
{
   unsigned char key[32];
   int k, x, y;
   unsigned char tmp[4], tmp2[4], M[8*4];
   ulong32 A, B;

   memcpy(key, _key, keylen);
   if (keylen != 16 && keylen != 24 && keylen != 32) {
       if ( keylen < 16 ) {
           memset(key+8*keylen, 0, 16-keylen);
           keylen = 16;
       } else if ( keylen < 24 ) {
           memset(key+8*keylen, 0, 24*keylen);
           keylen = 24;
       } else if ( keylen < 32 ) {
           memset(key+8*keylen, 0, 32-keylen);
           keylen = 32;
       }
   }
   k = keylen / 8;
   for (x = 0; x < keylen; x++) {
       M[x] = key[x] & 255;
   }


   for (x = 0; x < k; x++) {
       rs_mult(M+(x*8), skey->S[0]+(x*4));
   }

   for (x = 0; x < 20; x++) {
       for (y = 0; y < 4; y++) {
           tmp[y] = x+x;
       }
       h_func(tmp, tmp2, M, k, 0);
       LOAD32L(A, tmp2);

       for (y = 0; y < 4; y++) {
           tmp[y] = (unsigned char)(x+x+1);
       }
       h_func(tmp, tmp2, M, k, 1);
       LOAD32L(B, tmp2);
       B = ROL(B, 8);

       skey->K[x+x] = (A + B) & 0xFFFFFFFFUL;
       skey->K[x+x+1] = ROL(B + B + A, 9);
   }

   switch (k) {
         case 4 : skey->start = 0; break;
         case 3 : skey->start = 1; break;
         default: skey->start = 2; break;
   }
}


void Twofish::encrypt(const unsigned char *pt, unsigned char *ct)
{
    ulong32 a,b,c,d,ta,tb,tc,td,t1,t2, *k;
    int r;

    LOAD32L(a,&pt[0]);
    LOAD32L(b,&pt[4]);
    LOAD32L(c,&pt[8]);
    LOAD32L(d,&pt[12]);

    a ^= skey->K[0];
    b ^= skey->K[1];
    c ^= skey->K[2];
    d ^= skey->K[3];


    k  = skey->K + 8;


    for (r = 8; r != 0; --r) {
        t2 = g_func(ROL(b, 8), skey);
        t1 = g_func(a, skey) + t2;
        c  = ROR(c ^ (t1 + k[0]), 1);
        d  = ROL(d, 1) ^ (t2 + t1 + k[1]);

        t2 = g_func(ROL(d, 8), skey);
        t1 = g_func(c, skey) + t2;
        a  = ROR(a ^ (t1 + k[2]), 1);
        b  = ROL(b, 1) ^ (t2 + t1 + k[3]);
        k += 4;
   }

    ta = c ^ skey->K[4];
    tb = d ^ skey->K[5];
    tc = a ^ skey->K[6];
    td = b ^ skey->K[7];

    STORE32L(ta,&ct[0]);
    STORE32L(tb,&ct[4]);
    STORE32L(tc,&ct[8]);
    STORE32L(td,&ct[12]);
}

void Twofish::decrypt(const unsigned char *ct, unsigned char *pt)
{
    ulong32 a,b,c,d,ta,tb,tc,td,t1,t2, *k;


    LOAD32L(ta,&ct[0]);
    LOAD32L(tb,&ct[4]);
    LOAD32L(tc,&ct[8]);
    LOAD32L(td,&ct[12]);

    a = tc ^ skey->K[6];
    b = td ^ skey->K[7];
    c = ta ^ skey->K[4];
    d = tb ^ skey->K[5];

    k = skey->K + 36;
    for (int r = 8; r != 0; --r) {
        t2 = g_func(ROL(d, 8), skey);
        t1 = g_func(c, skey) + t2;
        a = ROL(a, 1) ^ (t1 + k[2]);
        b = ROR(b ^ (t2 + t1 + k[3]), 1);

        t2 = g_func(ROL(b, 8), skey);
        t1 = g_func(a, skey) + t2;
        c = ROL(c, 1) ^ (t1 + k[0]);
        d = ROR(d ^ (t2 +  t1 + k[1]), 1);
        k -= 4;
    }

    a ^= skey->K[0];
    b ^= skey->K[1];
    c ^= skey->K[2];
    d ^= skey->K[3];

    STORE32L(a, &pt[0]);
    STORE32L(b, &pt[4]);
    STORE32L(c, &pt[8]);
    STORE32L(d, &pt[12]);
}
