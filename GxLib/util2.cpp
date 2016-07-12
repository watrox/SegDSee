//---------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <QFile>
#include <QDir>

#include <QDebug>

#include "util2.h"

#ifdef _MSC_VER
    #pragma warning( push )
    #pragma warning( disable : 4244 4267 4305) // implicit conversion, possible loss of data
#else
    #pragma GCC diagnostic ignored "-Wsign-compare"
#endif

//---------------------------------------------------------------------------

int isnumeric(char *str)
{
  while(*str)
  {
    if(!isdigit(*str))
      return 0;
    str++;
  }

  return 1;
}


char* strmem(char* str, char* buf, int blen)
{
    size_t slen = strlen(str);

    if(slen>blen) return NULL;

    int   p;
    char* s;

    for(p=0; p+slen<blen; p++)
    {
        s = buf+p;
        if(memcmp(s,str,slen)==0) return s;
    }

    return 0;
}


char hexc (char* buf, int n)
{
    n--;
    int nb = n/2;
    int nh = n%2;

    byte c = buf[nb];

    if(nh==0) c = c >> 4;
    else      c = c & 0xF;

    if(c<10) c=c+'0';
    else     c=c-10+'A';

    return c;
}

char* hexs(char* buf, int n, int count, char* str)
{
    int i,j;
    for(i=n,j=0; j<count; i++,j++) str[j]=hexc(buf, i);
    str[j]=0;
    return str;
}


int i4(byte* buf, int nbyte, int swap)
{
    int i;
    union
    {
        byte h[4];
        long x;
    } u;

    memcpy(&i,buf+nbyte-1,sizeof(int));
    u.x=i;

    if(swap)
    {
        swapb(u.h[0], u.h[3]);
        swapb(u.h[1], u.h[2]);
    }
    return u.x;
}

unsigned int ui4(byte *buf, int nbyte, int swap)
{
    union { byte h[4]; unsigned int x; } u;

    memcpy(&u,buf+nbyte-1,sizeof(int));

    if(swap)
    {
        swapb(u.h[0], u.h[3]);
        swapb(u.h[1], u.h[2]);
    }
    return u.x;
}



int i3(byte* buf, int nbyte, int swap)
{
    int i=0;

    byte* b = (byte*)&i;

    union
    {
        byte h[4];
        long x;
    } u;

    memcpy(b+1,buf+nbyte-1,3);
    u.x=i;

    if(swap)
    {
        swapb(u.h[0], u.h[3]);
        swapb(u.h[1], u.h[2]);
    }
    return u.x;
}


short i2(byte* buf, int nbyte, int swap)
{
    short i;
    union
    {
        byte h[2];
        int x;
    } u;

    memcpy(&i,buf+nbyte-1,sizeof(short));
    u.x=i;

    if(swap) swapb(u.h[0], u.h[1]);

    return u.x;
}

unsigned short ui2(byte *buf, int nbyte, int swap)
{
    short i;
    union { byte h[2]; unsigned short x; } u;

    memcpy(&i,buf+nbyte-1,sizeof(short));
    u.x=i;

    if(swap) swapb(u.h[0], u.h[1]);

    return u.x;
}


char i1(byte* buf, int nbyte)
{
    char i;

    memcpy(&i,buf+nbyte-1,sizeof(char));

    return i;
}

float ibmf4(byte* buf, int nbyte, int swap)
{
    float f;
    memcpy(&f,buf+nbyte-1,sizeof(int));
    ibm2ieee(f, swap);
    return f;
}

float ieee4(byte* buf, int nbyte, int swap)
{
    union { byte h[4]; float x; } u;

    memcpy(&u.x,buf+nbyte-1,sizeof(int));

    if(swap)
    {
        swapb(u.h[0], u.h[3]);
        swapb(u.h[1], u.h[2]);
    }
    return u.x;
}

double ieee8(byte* buf, int nbyte, int swap)
{
    union { byte h[8]; double x; } u;

    memcpy(&u.x,buf+nbyte-1,sizeof(double));

    if(swap)
    {
        swapb(u.h[0], u.h[7]);
        swapb(u.h[1], u.h[6]);
        swapb(u.h[2], u.h[5]);
        swapb(u.h[3], u.h[4]);
    }
    return u.x;
}


double iFmt(int frmt, byte* buf, int nbyte, int swap)
{
    double v;

    if      (frmt== 1)  v = i1   (buf,nbyte);
    else  if(frmt== 2)  v = i2   (buf,nbyte, swap);
    else  if(frmt==12)  v = ui2  (buf,nbyte, swap);
    else  if(frmt== 4)  v = i4   (buf,nbyte, swap);
    else  if(frmt==14)  v = ieee4(buf,nbyte, swap);
    else  if(frmt==18)  v = ieee4(buf,nbyte, swap);
    else  if(frmt==24)  v = ibmf4(buf,nbyte, swap);
    else                v=0;

    return v;
}


void toibm(long *addr, int ll)
{
    unsigned long real;

    unsigned long lng;
    unsigned char *ch;

    unsigned int gain,sign,t;

    //    ch= (long *)&lng;
    ch= (unsigned char*)&lng;

    while(ll--)
     {
        real = *addr;
        if(real)
         {
            gain = (real>>23);
            sign = (gain & 0x100)>>1;
            gain = (gain & 0xff) - 0x80 +2;
            real = ((real& 0x7FFFFFL)| 0x800000L);

            //      real = (real& 0x7FFFFFL);
            //      real = (real | 0x800000L);

            t = gain & 3;
            if(t)
             {
                real >>= 4-t;
                gain +=  4-t;
             }
            gain = ((gain>>2) + 0x40) & 0x7f;
            gain |= sign;
            lng = real;
            ch[3] = ch[0];
            t = ch[1];
            ch[1] = ch[2];
            ch[2] = t;
            ch[0] = gain;
            real = lng;
         }
        *addr++ = real;
     }
}

void si4(byte* buf, int nbyte, int i, int swap)
 {
    union { byte h[4]; int x; } u;

    u.x=i;

    if(swap)
    {
        swapb(u.h[0], u.h[3]);
        swapb(u.h[1], u.h[2]);
    }

    memcpy(buf+nbyte-1, &u.x, sizeof(int));
 }

void sieee4(byte* buf, int nbyte, float i, int swap)
 {
    union { byte h[4]; float x; } u;

    u.x=i;

    if(swap)
    {
      swapb(u.h[0], u.h[3]);
      swapb(u.h[1], u.h[2]);
    }

    memcpy(buf+nbyte-1, &u.x, sizeof(float));
 }


void sieee8(byte* buf, int nbyte, double i, int swap)
 {
    union { byte h[8]; double x; } u;

    u.x=i;

    if(swap)
    {
      swapb(u.h[0], u.h[7]);
      swapb(u.h[1], u.h[6]);
      swapb(u.h[2], u.h[5]);
      swapb(u.h[3], u.h[4]);
    }

    memcpy(buf+nbyte-1, &u.x, sizeof(float));
 }

void sibmf4(byte* buf, int nbyte, float i)
 {
    toibm((long*)&i, 1);

    union { byte h[4]; int x; } u;

    memcpy(&u.x,&i,4);
    memcpy(buf+nbyte-1, &u.x, sizeof(int));
 }

void si2(byte* buf,int nbyte, short i, int swap)
 {
    union { byte h[2]; short x; } u;

    u.x=i;

    if(swap) swapb(u.h[0], u.h[1]);

    memcpy(buf+nbyte-1, &u.x, sizeof(short));
 }

void si1(byte* buf, int nbyte, char i)
 {
    memcpy(buf+nbyte-1,&i,sizeof(char));
 }



void sFmt(int frmt, byte* buf, int nbyte, double v, int swap)
{
    if      (frmt== 1)  si1   (buf,nbyte,v);
    else  if(frmt== 2)  si2   (buf,nbyte,v, swap);
    else  if(frmt==12)  si2   (buf,nbyte,v, swap);
    else  if(frmt== 4)  si4   (buf,nbyte,v, swap);
    else  if(frmt==14)  sieee4(buf,nbyte,v, swap);
    else  if(frmt==18)  sieee4(buf,nbyte,v, swap);
    else  if(frmt==24)  sibmf4(buf,nbyte,v); //!!
}



char* strncpy1(char* d, char* s, int n)
{
    memcpy(d,s,n);
    *(d+n)=0;
    return d;
}

char* memstr(char* mem, int maxl, char* str)
{
    char* s;
    int   l = strlen(str);

    for(s=mem; s+l<mem+maxl; s++)
    {
        if(memcmp(s,str,l)==0) return s;
    }

    return NULL;
}

byte hexc2byte(char hex)
{
    byte b;
    if     (hex>='0' && hex<='9') b=hex-'0';
    else if(hex>='a' && hex<='f') b=hex-'a'+10;
    else if(hex>='A' && hex<='F') b=hex-'A'+10;
    else                          b=0;

    return b;
}

unsigned int hex2uint(char* hex)
{
    unsigned int b;
    unsigned int v=0;
    int          n,s;

    for(n=0,s=7*4; hex[n]&&s>=0; n++,s-=4)
    {
        b=hexc2byte(hex[n]);
        v=b<<s | v;
    }

    v=v>>(s+4);
    return v;
}

int hex2sint(char* hex)
{
    unsigned int b;
    int          v=0;
    int          n,s;

    for(n=0,s=7*4; hex[n]&&s>=0; n++,s-=4)
    {
        b=hexc2byte(hex[n]);
        v=b<<s | v;
    }

    v=v>>(s+4);
    return v;
}

unsigned long long hex2uint64(char* hex)
{
    unsigned long long b;
    unsigned long long v=0;
    int          n,s;

    for(n=0,s=15*4; hex[n]&&s>=0; n++,s-=4)
    {
        b=hexc2byte(hex[n]);
        v=b<<s | v;
    }

    v=v>>(s+4);
    return v;
}

long long hex2sint64(char* hex)
{
    unsigned long long b;
    long long          v=0;
    int          n,s;

    for(n=0,s=15*4; hex[n]&&s>=0; n++,s-=4)
    {
        b=hexc2byte(hex[n]);
        v=b<<s | v;
    }

    v=v>>(s+4);
    return v;
}



char* hex2ascii(char* hex)
{
    static char asc[2048];
    int  nc,nh;
    char c=0;

    for(nh=0,nc=0; hex[nh]; nh++)
    {
        if(nh%2==0)
        {
            c = 0;
            c = (hexc2byte(hex[nh]))<<4;
        }
        else
        {
            c = c | hexc2byte(hex[nh]);
            asc[nc++]=c;
        }
    }

    asc[nc]=0;

    return asc;
}


float hex2flt(char* hex)
{
    unsigned int i = hex2uint(hex);
    float*       f = (float*)&i;

    return *f;
}

double hex2dbl(char* hex)
{
    unsigned long long i = hex2uint64(hex);
    double*      f = (double*)&i;

    return *f;
}


char prbuf[2048];


int uns_item(char* buf, int pos, int count)
{
    char s[2048];
    int v=hex2uint(hexs(buf, pos, count,s));
    return v;
}

int int_item(char* buf, int pos, int count)
{
    char s[2048];
    int v=hex2sint(hexs(buf, pos, count,s));
    return v;
}

float flt_item(char* buf, int pos, int count)
{
    char s[2048];
    float  vf=hex2flt(hexs(buf, pos, count,s));
    return vf;
}

char* bcd_str_item(char* buf, int pos, int count, char* str)
{
    static char s[2048];

    hexs(buf, pos, count,s);

    if(str)
    {
        strcpy(str,s);
        return str;
    }

    return s;
}

int bcd_int_item(char* buf, int pos, int count)
{
    char s[2048];
    hexs(buf, pos, count,s);
    return atoi(s);
}


char* asc_item(char* buf, int pos, int count, char* str)
{
    static char s[2048];

    char* v = hex2ascii(hexs(buf, pos, count,s));

    if(str)
    {
        strcpy(str,v);
        return str;
    }

    return v;
}


const char *pritem(const char* descr, int type, char* buf, int n, int count, int p2, int eol)
{
    char      s[2048];
    int       v;
    int       v64;
    float     vf;
    double    vd;

    if     (type==0) // bcd
    {
        hexs(buf, n, count,s);
        sprintf(prbuf, "%s : %s",descr, s);
    }
    else if(type==1) // unsigned int
    {
        v=hex2uint(hexs(buf, n, count,s));
        sprintf(prbuf, "%-20s : %-10d [%s]",descr, v, s);
    }
    else if(type==2) // signed int
    {
        v=hex2sint(hexs(buf, n, count,s));
        sprintf(prbuf, "%-20s : %-10d [%s]",descr, v, s);
    }
    else if(type==12) // signed int 64 bit
    {
        v64=hex2sint64(hexs(buf, n, count,s));
        sprintf(prbuf, "%-20s : %-10g [%s]",descr, v64*pow(2,p2), s);
    }
    else if(type==3) // float iEEE
    {
        vf=hex2flt(hexs(buf, n, count,s));
        sprintf(prbuf, "%-20s : %-10.10g [%s]",descr, vf, s);
    }
    else if(type==4) // double iEEE
    {
        vd=hex2dbl(hexs(buf, n, count,s));
        sprintf(prbuf, "%-20s : %-10.10g [%s]",descr, vd, s);
    }
    else if(type==5) // asc
    {
        char* v = hex2ascii(hexs(buf, n, count,s));
        sprintf(prbuf, "%-20s : %-s",descr,v);
    }

    if(eol) strcat(prbuf, "\r\n");
    else    strcat(prbuf, " ");

    return prbuf;
}

//---------------------------------------------------------------------------


int fmtnum[] =
{
    15,
    22,
    24,
    36,
    38,
    42,
    44,
    48,
    58,
    8015,
    8022,
    8024,
    8036,
    8038,
    8042,
    8044,
    8048,
    8058,
    200,
    0
};

const char* fmtnam[] =
{
    "20 bit binary multiplexed",                     // 0015
    " 8 bit quaternary multiplexed",                 // 0022
    "16 bit quaternary multiplexed",                 // 0024
    "24 bit 2's compliment integer multiplexed",     // 0036
    "32 bit 2's compliment integer multiplexed",     // 0038
    " 8 bit hexadecimal multiplexed",                // 0042
    "16 bit hexadecimal multiplexed",                // 0044
    "32 bit hexadecimal multiplexed",                // 0048
    "32 bit IEEE multiplexed",                       // 0058
    "20 bit binary demultiplexed",                   // 8015
    " 8 bit quaternary demultiplexed",               // 8022
    "16 bit quaternary demultiplexed",               // 8024
    "24 bit 2's compliment integer demultiplexed",   // 8036
    "32 bit 2's compliment integer demultiplexed",   // 8038
    " 8 bit hexadecimal demultiplexed",              // 8042
    "16 bit hexadecimal demultiplexed",              // 8044
    "32 bit hexadecimal demultiplexed",              // 8048
    "32 bit IEEE demultiplexed",                     // 8058
    "Illegal, do not use",                           // 0200
    "Illegal, do not use",                           // 0000
    "Unknown"
};


const char* ctypnam [] =
{
    "Unused",                 // 0
    "Seis",                   // 1
    "Time break",             // 2
    "Up hole",                // 3
    "Water break",            // 4
    "Time counter",           // 5
    "External Data",          // 6
    "Other",                  // 7
    "Signature/unfiltered",   // 8
    "Signature/filtered",     // 9
    "Unknown",                // 10
    "Unknown",                // 11
    "Auxiliary Data Trailer", // 12
    "Unknown",                // 13
    "Unknown",                // 14
    "Unknown"                 // 15
};


const char* polnam[] =
{
    "Untested",       // 0000
    "Zero",           // 0001
    "45 degrees",     // 0010
    "90 degrees",     // 0011
    "135 degrees",    // 0100
    "180 degrees",    // 0101
    "225 degrees",    // 0110
    "270 degrees",    // 0111
    "315 degrees",    // 1000
    "Unknown",        // 1001
    "Unknown",        // 1010
    "Unknown",        // 1011
    "Unassigned",     // 1100
    "Unknown",        // 1101
    "Unknown",        // 1110
    "Unknown",        // 1111
    "Illegal"         // other
};

const char* mcnam[] =
{
    "Unknown",                                                                // 00
    "Alpine Geophysical Associates, Inc. (Obsolete)",                         // 01
    "Applied Magnetics Corporation (See 09)",                                 // 02
    "Western Geophysical Exploration Products (Litton Resources Systems)",    // 03
    "SIE, Inc. (Obsolete)",                                                   // 04
    "Dyna-Tronics Mfg. Corporation (Obsolete)",                               // 05
    "Electronic Instrumentation, Inc. (Obsolete)",                            // 06
    "Halliburton Geophysical Services, Inc.,(formerly, Electro-Technical",    // 07
    "Fortune Electronics, Inc. (Obsolete)",                                   // 08
    "Geo Space Corporation",                                                  // 09
    "Leach Corporation (Obsolete)",                                           // 10
    "Metrix Instrument Co. (Obsolete)",                                       // 11
    "Redcor Corporation (Obsolete)",                                          // 12
    "Sercel (Societe d'Etudes, Recherches Et Constructions Electroniques)",   // 13
    "Scientific Data Systems (SDS), (Obsolete)",                              // 14
    "Texas Instruments, Inc.",                                                // 15
    "Unknown",                                                                // 16
    "GUS Manufacturing, Inc.",                                                // 17
    "Input/Output, Inc.",                                                     // 18
    "Geco-Prakla",                                                            // 19
    "Fairfield Industries, Incorporated",                                     // 20
    "Unknown",                                                                // 21
    "Geco-Prakla",                                                            // 22
    "Unknown",                                                                // 23
    "Unknown",                                                                // 24
    "Unknown",                                                                // 25
    "Unknown",                                                                // 26
    "Unknown",                                                                // 27
    "Unknown",                                                                // 28
    "Unknown",                                                                // 29
    "Unknown",                                                                // 30
    "Japex Geoscience Institute 1991",                                        // 31
    "Halliburton Geophysical Services, Inc. 1991",                            // 32
    "Compuseis, Inc. 1993",                                                   // 33
    "Syntron, Inc. 1993",                                                     // 34
    "Syntron Europe Ltd. 1993",                                               // 35
    "Opseis 1994",                                                            // 36
    "Unknown",                                                                // 37
    "Unknown",                                                                // 38
    "Grant Geophysical 1995",                                                 // 39
    "Geo-X 1996",                                                             // 40
    "PGS Inc. 2001",                                                          // 41
    "SeaMap UK Ltd. 2003",                                                    // 42
    "Hydroscience 2004",                                                      // 43
    "JSC 2006",                                                               // 44
    "Fugro 2006",                                                             // 45
    "ProFocus Systems AS 2006",                                               // 46
    "Optoplan AS 2008",                                                       // 47
    "Wireless Seismic Inc 2008"                                               // 48
};


const char* polnum2nam(int num)
{
    int l=sizeof(polnam)/sizeof(char*);

    if(num>=0 && num<l) return polnam[num];

    return polnam[l-1];
}


const char* mcnum2nam(int num)
{
    int l=sizeof(mcnam)/sizeof(char*);

    if(num>0 && num<l) return mcnam[num];

    return mcnam[0];
}


const char* fmtnum2nam(int num)
{
    int n;
    int l=sizeof(fmtnum)/sizeof(int);

    for(n=0; n<l; n++)
        if(num==fmtnum[n]) return fmtnam[n];

    return fmtnam[n];
}


const char *typnum2nam(int num)
{
    if(num<0 || num>15) return ctypnam[15];

    return ctypnam[num];
}


//---------------------------------------------------------------------------

void swap4(void* x)
{
    byte* cbuf;
    byte  tem;

    cbuf=(byte*)x;        /* assign address of input to char array */

    tem=cbuf[0];
    cbuf[0]=cbuf[3];
    cbuf[3]=tem;
    tem=cbuf[2];
    cbuf[2]=cbuf[1];
    cbuf[1]=tem;
}

//---------------------------------------------------------------------------

void swap2(void* x)
{
    byte* cbuf;
    byte  tem;

    cbuf=(byte*)x;        /* assign address of input to char array */

    tem=cbuf[0];
    cbuf[0]=cbuf[1];
    cbuf[1]=tem;
}

//---------------------------------------------------------------------------

float ieee_to_ieee(float x)
{
    float     y;
    float     v;
    int      s;
    int      c;
    int      q;

    //  byte*         cbuf = (byte*)&x;
    unsigned int* ibuf = (unsigned int*)&x;

    s = ibuf[0] & 0x80000000;
    if(s) s = -1;
    else  s =  1;

    c = ibuf[0] & 0x7F800000;
    c = c >> 23;

    q = ibuf[0] & 0x007FFFFF;

    if(q==0 && c==0) return 0;

    v = q / 8388608.0;

    y = (1+v) * pow(2,c-127);

    return y*s;
}


float ibm2ieee(float inp, int swap)
{
    typedef unsigned char byte;
    typedef unsigned int  ulng;

    //  int swap=1;

    float   input;

    memcpy(&input, &inp, sizeof(inp));

    byte  *cbuf,expp,tem,sign;
    ulng  *umantis,expll;
    int   *mantis;
    int    shift;

    cbuf   =(byte*)&input;     // assign address of input to char array
    umantis=(ulng*)&input;     // two differnt points to the same spot
    mantis =(int*) &input;     // signned & unsigned

    if(swap)
    {
        // now byte reverce for PC use if swap true
        tem=cbuf[0]; cbuf[0]=cbuf[3]; cbuf[3]=tem;
        tem=cbuf[2]; cbuf[2]=cbuf[1]; cbuf[1]=tem;
    }

    // start extraction information from number

    expp=*mantis>>24;     // get expo fro upper byte
    *mantis=(*mantis)<<8; // shift off upper byte
    shift=1;              // set a counter to 1
    while(*mantis>0 && shift<23) // start of shifting data
    {
        *mantis=*mantis<<1;
        shift++;
    } // shift until a 1 in msb

    *mantis=*mantis<<1;   // need one more shift to get implied one bit
    sign=expp & 0x80;     // set sign to msb of exponent
    expp=expp & 0x7F;     // kill sign bit

    if(expp!=0)           // don't do anymore if zero exponent
    {
        expp=expp-64;              // compute what shift was (old exponent)
        *umantis=*umantis>>9;      // MOST IMPORTANT an UNSIGNED shift back down
        expll=0x7F+(expp*4-shift); // add in excess 172

        // now mantissa is correctly aligned, now create the other two pairs
        // needed the extended sign word and the exponent word

        expll=expll<<23;           // shift exponent up

        // combine them into a floating point IEEE format !

        //if(sign) *umantis=expll | *mantis | 0x80000000;
        //else     *umantis=expll | *mantis; // set or don't set sign bit
        *umantis=expll | *mantis;
    }

//  if(sign) input = -input;
    if(sign) *umantis = *mantis | 0x80000000;

    return input;
}

/*
void segd8015_to_ieee(void* inp, float* out, int ns)
{
    int n, nq ,no;

    unsigned char*  z = (unsigned char* )inp;
    unsigned short* i = (unsigned short*)inp;
    unsigned int*   o = (unsigned int*  )out;

    int             cs;
    unsigned int    c;
    unsigned int    q;
    unsigned int    s;
    unsigned short  x;

    int ss;
    float qq,fc;
    int   cc;


    for(n=0;n<ns;)
    {
      cs=12;
      z = (unsigned char*)i;
      for(nq=1;nq<5;nq++)
      {
        x = i[0 ];
        swap2(&x);
        c  = ((x>>cs)&0xF); cc=c;
        c=c+127;
        c = c<<23;
        x = i[nq];
        swap2(&x);
        q =   x&0x7FFF; qq=q;
        qq = qq*pow(2,-15);
        q = q<<9;
        s =   x&0x8000;
        if(s) ss = -1;
        else  ss =  1;
        s = s<<16;
        q =   s | c | q;
//      swap2(&q);
        o[n]=q;
        fc=pow(2,cc);
        out[n]=ss * (qq) * fc ;
        out[n]*=fmp;
        n++; cs-=4;
      }
      i+=10;
    }
}
*/

//From CWP - Thanks to Colorado Mining School
/* F8015_to_float - convert 20 bit binary demultiplexed data into floating numbers
 *
 * Credits:
 *      EOPG: Marc Schaming, Jean-Daniel Tissot
 *      SEP:  Stew Levin - fixed low-order bit error in conversion
 *            of negative values on 2's complement machines.
 *            Use ldexp() function instead of much slower value*pow(2,expo)
 *
 * Parameters:
 *    from   - input vector
 *    to     - output vector
 *    len    - number of packets of 4 floats in vectors
 *
 */
/*
 *
 * Format 8015 is a 10 byte per 4 words (2 1/2 bytes per word)
 * representation.  According to the SEG specifications, the
 * bit layout of the 10 bytes is:
 *
 *
 *  Bit       0     1     2     3     4     5     6     7
 *-----------------------------------------------------------
 * Byte 1    C3    C2    C1    C0    C3    C2    C1    C0    Exponents for
 * Byte 2    C3    C2    C1    C0    C3    C2    C1    C0    channels 1 thru 4
 *
 * Byte 3     S    Q-1   Q-2   Q-3   Q-4   Q-5   Q-6   Q-7   Channel 1
 * Byte 4    Q-8   Q-9   Q-10  Q-11  Q-12  Q-13  Q-14  Q-15
 * Byte 5     S    Q-1   Q-2   Q-3   Q-4   Q-5   Q-6   Q-7   Channel 2
 * Byte 6    Q-8   Q-9   Q-10  Q-11  Q-12  Q-13  Q-14  Q-15
 * Byte 7     S    Q-1   Q-2   Q-3   Q-4   Q-5   Q-6   Q-7   Channel 3
 * Byte 8    Q-8   Q-9   Q-10  Q-11  Q-12  Q-13  Q-14  Q-15
 * Byte 9     S    Q-1   Q-2   Q-3   Q-4   Q-5   Q-6   Q-7   Channel 4
 * Byte 10   Q-8   Q-9   Q-10  Q-11  Q-12  Q-13  Q-14  Q-15
 *
 * S=sign bit. - (One = negative number)
 * C=binary exponents. - This is a 4 bit positive binary exponent of 2
 *               CCCC
 *   written as 2     where CCCC can assume values of 0-15.  The four
 *   exponents are in channel number order for the four channels starting
 *   with channel one in bits 0-3 of Byte 1.
 * Q1-15-fraction. - This is a 15 bit one's complement binary fraction.
 *   The radix point is to the left of the most significant bit (Q-1)
 *                                  -1
 *   with the MSB being defined as 2 .  The sign and fraction can assume
 *                  -15       -15
 *   values from 1-2   to -1+2  .  Negative zero is invalid and must be
 *   converted to positive zero.
 *                                        CCCC    MP                   MP
 * Input signal = S.QQQQ,QQQQ,QQQQ,QQQ x 2    x 2    millivolts where 2
 *   is the value required to descale the data word to the recording
 *   system input level.  MP is defined in Byte 8 of each of the corre-
 *   sponding channel set descriptors in the scan type header.
 * Note that in utilizing this data recording method, the number of data
 *   channels per channel set must be exactly divisible by 4 in order to
 *   preserve the data grouping of this method.
 */

//void F8015_to_float (Sfio_t *from, float to[], int len)

static int nbyte;

static short GET_S(void* inp)
{
    int n1, n2;
    int n;
    short s;
    unsigned char*  inps = (unsigned char*) inp;

    n1 = inps[nbyte++]; //n1 = sfgetc(f);
    n2 = inps[nbyte++]; //n2 = sfgetc(f);
    n = (n1<<8);
    n = n | n2;
    s = (short) ((n > 32767)? n - 65536 : n);

    return (s);
}

static char GET_C(void* inp)
{
    int n;
    char c;

    unsigned char*  inps = (unsigned char*) inp;

    n = inps[nbyte++]; //n = sfgetc(f);
    c = (char) ((n > 127) ? (n-256) : n);

    return (c);
}

static unsigned int GET_UC(void* inp)
{
    int n;
    unsigned char uc;

    unsigned char*  inps = (unsigned char*) inp;

    n = inps[nbyte++]; //n = sfgetc(f);
    uc = (unsigned char) n;

    return (uc);
}


void F8015_to_float (void* inp, float* out, int len)
{
    register int i;
    register short ex1_4;
    int expo;
    short fraction;

//  float* ooo = out;

    nbyte=0;

    for (i = 0; i < len; i += 4)
    {
        ex1_4 = GET_S(inp);
        expo = ((ex1_4 >> 12) & 15) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -(~fraction);
        *(out++) = ldexp((double) fraction, expo);

        expo = ((ex1_4 >> 8) & 15) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -(~fraction);
        *(out++) = ldexp((double) fraction, expo);

        expo = ((ex1_4 >> 4) & 15) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -(~fraction);
        *(out++) = ldexp((double) fraction, expo);

        expo = (ex1_4 & 15) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -(~fraction);
        *(out++) = ldexp((double) fraction, expo);
    }

//   qDebug() << "len=" << len << "  x=" << out-ooo;
}


void B0015_to_float (void* inp, float* out, int len)

{
    register int i;
    register short ex1_4;
    int expo;
    short fraction;

    nbyte=0;

    for (i = 0; i < len; i += 4)
    {
        ex1_4 = GET_S(inp);
        expo = ((ex1_4 >> 12) & 0x0F) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -((~fraction)&(~1));
        *(out++) = ldexp((double) fraction, -expo);

        expo = ((ex1_4 >> 8) & 0x0F) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -((~fraction)&(~1));
        *(out++) = ldexp((double) fraction, -expo);

        expo = ((ex1_4 >> 4) & 0x0F) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -((~fraction)&(~1));
        *(out++) = ldexp((double) fraction, -expo);

        expo = (ex1_4 & 0x0F) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -((~fraction)&(~1));
        *(out++) = ldexp((double) fraction, -expo);
    }
}


void F0015_to_float (void* inp, float* out, int len)
{
    register int i;
    register short ex1_4;
    int expo;
    short fraction;

    nbyte=0;

    for (i = 0; i < len; i += 4)
    {
        ex1_4 = GET_S(inp);
        expo = ((ex1_4 >> 12) & 0x0F) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -((~fraction)&(~1));
        *(out++) = ldexp((double) fraction, expo);

        expo = ((ex1_4 >> 8) & 0x0F) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -((~fraction)&(~1));
        *(out++) = ldexp((double) fraction, expo);

        expo = ((ex1_4 >> 4) & 0x0F) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -((~fraction)&(~1));
        *(out++) = ldexp((double) fraction, expo);

        expo = (ex1_4 & 0x0F) - 15;
        fraction = GET_S(inp);
        if (fraction < 0) fraction = -((~fraction)&(~1));
        *(out++) = ldexp((double) fraction, expo);
    }
}

/* F8022_to_float - convert 8 bit quaternary demultiplexed data into floating numbers
 *
 * Credits:
 *      SEP:  Stew Levin
 *
 * Parameters:
 *    inp   - input sfio unit
 *    to     - output vector
 *    len    - number of packets of 4 floats in vectors
 *
 */
/*
 *
 * Format 8022 is a 1 byte per word representation.
 * According to the SEG specifications, the bit
 * layout of the byte is:
 *
 *
 *  Bit       0     1     2     3     4     5     6     7
 *-----------------------------------------------------------
 * Byte 1     S    C2    C1    C0    Q-1   Q-2   Q-3   Q-4
 *
 * S=sign bit. - (One = negative number)
 * C=quaternary exponent. - This is a 3 bit positive binary exponent of 4
 *               CCC
 *   written as 4    where CCC can assume values of 0-7.
 * Q1-4-fraction. - This is a 4 bit one's complement binary fraction.
 *   The radix point is to the left of the most significant bit (Q-1)
 *                                  -1
 *   with the MSB being defined as 2 .  The fraction can have values
 *           -4        -4
 *   inp 1-2   to -1+2  .  Negative zero is invalid and must be
 *   converted to positive zero.
 *                          CCC   MP                   MP
 * Input signal = S.QQQQ x 4   x 2   millivolts where 2    is the
 *   value required to descale the data word to the recording system
 *   input level.  MP is defined in Byte 8 of each of the corre-
 *   sponding channel set descriptors in the scan type header.
 */

void F8022_to_float (void* inp, float* out, int len)
{
    register int i;
    register int ex1_4;
    int expo;
    short fraction;

    nbyte=0;

    for (i = 0; i < len; i ++)
    {
        ex1_4 = GET_C(inp);
        expo = ((ex1_4 >> 3) & 14) - 4;
        fraction = ex1_4 & 15;
        if (ex1_4 & 128) fraction = -(15^fraction);
        *(out++) = ldexp((double) fraction, expo);
    }
}

/* F8024_to_float - convert 16 bit quaternary demultiplexed data into floating numbers
 *
 * Credits:
 *      SEP:  Stew Levin
 *
 * Parameters:
 *    inp   - input sfio unit
 *    out     - output vector
 *    len    - number of packets of 4 floats in vectors
 *
 */
/*
 *
 * Format 8024 is a 2 byte per word representation.
 * According out the SEG specifications, the bit
 * layout of the bytes is:
 *
 *
 *  Bit       0     1     2     3     4     5     6     7
 *-----------------------------------------------------------
 * Byte 1     S    C2    C1    C0    Q-1   Q-2   Q-3   Q-4
 * Byte 2    Q-5   Q-6   Q-7   Q-8   Q-9   Q-10  Q-11  Q-12
 *
 * S=sign bit. - (One = negative number)
 * C=quaternary exponent. - This is a 3 bit positive binary exponent of 4
 *               CCC
 *   written as 4    where CCC can assume values of 0-7.
 * Q1-12-fraction. - This is a 12 bit one's complement binary fraction.
 *   The radix point is out the left of the most significant bit (Q-1)
 *                                  -1
 *   with the MSB being defined as 2 .  The fraction can have values
 *           -12        -12
 *   inp 1-2    out -1+2   .  Negative zero is invalid and must be
 *   converted out positive zero.
 *                                    CCC   MP                   MP
 * Input signal = S.QQQQ,QQQQ,QQQQ x 4   x 2   millivolts where 2
 *   is the value required out descale the data word out the recording
 *   system input level.  MP is defined in Byte 8 of each of the corre-
 *   sponding channel set descriptors in the scan type header.
 */

void F8024_to_float (void* inp, float* out, int len)
{
    register int i;
    register int ex1_4;
    int expo;
    short fraction;

    nbyte=0;

    for (i = 0; i < len; i ++)
    {
        ex1_4 = GET_S(inp);
        expo = ((ex1_4 >> 11) & 14) - 12;
        fraction = ex1_4 & 4095;
        if (ex1_4 & 32768) fraction = -(4095^fraction);
        *(out++) = ldexp((double) fraction, expo);
    }
}

/* F8036_to_float - convert 24 bit quaternary demultiplexed data into floating numbers
 *
 * Credits:
 *      SEP:  Stew Levin
 *
 * Parameters:
 *    inp   - input sfio unit
 *    out     - output vector
 *    len    - number of packets of 4 floats in vectors
 *
 */
/*
 *
 * Format 8036 is a 3 byte per word representation.
 * According out the SEG specifications, the bit
 * layout of the bytes is:
 *
 *
 *  Bit       0     1     2     3     4     5     6     7
 *-----------------------------------------------------------
 * Byte 1    Q-1   Q-2   Q-3   Q-4   Q-5   Q-6   Q-7   Q-8
 * Byte 2    Q-9   Q-10  Q-11  Q-12  Q-13  Q-14  Q-15  Q-16
 * Byte 3    Q-17  Q-18  Q-19  Q-20  Q-21  Q-22  Q-23  Q-24
 *
 * Q1-24-integer. - This is a 24 bit two's complement binary integer.
 *                         MP                   MP
 * Input signal = Q...Q x 2   millivolts where 2
 *   is the value required out descale the data word out the recording
 *   system input level.  MP is defined in Byte 8 of each of the corre-
 *   sponding channel set descriptors in the scan type header.
 */

void F8036_to_float (void* inp, float* out, int len)
{
    register int i;
    register long int ival;

    nbyte=0;

    for (i = 0; i < len; i ++)
    {
        ival = GET_UC(inp);
        ival <<= 8;
        ival |= GET_UC(inp);
        ival <<= 8;
        ival |= GET_UC(inp);
        if(ival > 8388607) ival -= 16777216;
        *(out++) = (float)ival;
    }
}

/* F8038_to_float - convert 32 bit quaternary demultiplexed data into floating numbers
 *
 * Credits:
 *      SEP:  Stew Levin
 *
 * Parameters:
 *    inp   - input sfio unit
 *    out     - output vector
 *    len    - number of packets of 4 floats in vectors
 *
 */
/*
 *
 * Format 8038 is a 4 byte per word representation.
 * According out the SEG specifications, the bit
 * layout of the bytes is:
 *
 *
 *  Bit       0     1     2     3     4     5     6     7
 *-----------------------------------------------------------
 * Byte 1    Q-1   Q-2   Q-3   Q-4   Q-5   Q-6   Q-7   Q-8
 * Byte 2    Q-9   Q-10  Q-11  Q-12  Q-13  Q-14  Q-15  Q-16
 * Byte 3    Q-17  Q-18  Q-19  Q-20  Q-21  Q-22  Q-23  Q-24
 * Byte 4    Q-25  Q-26  Q-27  Q-28  Q-29  Q-30  Q-31  Q-32
 *
 * Q1-32-fraction. - This is a 32 bit two's complement binary integer.
 *                         MP                   MP
 * Input signal = Q...Q x 2   millivolts where 2
 *   is the value required out descale the data word out the recording
 *   system input level.  MP is defined in Byte 8 of each of the corre-
 */
/* Note this conversion routine assumes the target architecture is
 * already 2's complement.
 */

void F8038_to_float (void* inp, float* out, int len)
{
    int i;
    long int ex1_4;
    long int ex2_4;
    long int value;

    nbyte=0;

    for (i = 0; i < len; i ++)
    {
        ex1_4 = GET_S(inp);
        ex2_4 = GET_S(inp);
        value = (ex1_4<<16) | (ex2_4&65535);
        *(out++) = (float)value;
    }
}

/* F8042_to_float - convert 8 bit hexadecimal demultiplexed data into floating numbers
 *
 * Credits:
 *      SEP:  Stew Levin
 *
 * Parameters:
 *    inp   - input sfio unit
 *    out     - output vector
 *    len    - number of floats in vector
 *
 */
/*
 *
 * Format 8042 is a 1 byte per word representation.
 * According out the SEG specifications, the bit
 * layout of the byte is:
 *
 *
 *  Bit       0     1     2     3     4     5     6     7
 *-----------------------------------------------------------
 * Byte 1     S    C1    C0    Q-1   Q-2   Q-3   Q-4   Q-5
 *
 * S=sign bit. - (One = negative number)
 * C=hexadecimal exponent. - This is a 2 bit positive binary exponent of 16
 *                CC
 *   written as 16    where CC can assume values of 0-3.
 * Q1-5-fraction. - This is a 5 bit positive binary fraction.
 *   The radix point is out the left of the most significant bit (Q-1)
 *                                  -1
 *   with the MSB being defined as 2 .  The fraction can have values
 *           -5        -5
 *   inp 1-2   out -1+2  .
 *                             CC    MP                   MP
 * Input signal = S.QQQQ,Q x 16   x 2   millivolts where 2    is the
 *   value required out descale the data word out the recording system
 *   input level.  MP is defined in Byte 8 of each of the corre-
 *   sponding channel set descriptors in the scan type header.
 */

void F8042_to_float (void* inp, float* out, int len)
{
    register int i;
    register int ex1_4;
    int expo;
    short fraction;

    nbyte=0;

    for (i = 0; i < len; i ++)
    {
        ex1_4 = GET_C(inp);
        expo = ((ex1_4 >> 3) & 12) - 5;
        fraction = ex1_4 & 31;
        if (ex1_4 & 128) fraction = -fraction;
        *(out++) = ldexp((double) fraction, expo);
    }
}

/* F8044_to_float - convert 16 bit hexadecimal demultiplexed data into floating numbers
 *
 * Credits:
 *      SEP:  Stew Levin
 *
 * Parameters:
 *    inp   - input sfio unit
 *    out     - output vector
 *    len    - number of floats in vector
 *
 */
/*
 *
 * Format 8044 is a 2 byte per word representation.
 * According out the SEG specifications, the bit
 * layout of the bytes is:
 *
 *
 *  Bit       0     1     2     3     4     5     6     7
 *-----------------------------------------------------------
 * Byte 1     S    C1    C0    Q-1   Q-2   Q-3   Q-4   Q-5
 * Byte 2    Q-6   Q-7   Q-8   Q-9   Q-10  Q-11  Q-12  Q-13
 *
 * S=sign bit. - (One = negative number)
 * C=hexadecimal exponent. - This is a 2 bit positive binary exponent of 16
 *                CC
 *   written as 16    where CC can assume values of 0-3.
 * Q1-13-fraction. - This is a 13 bit positive binary fraction.
 *   The radix point is out the left of the most significant bit (Q-1)
 *                                  -1
 *   with the MSB being defined as 2 .  The fraction can have values
 *           -13        -13
 *   inp 1-2    out -1+2   .
 *                                       CC    MP                   MP
 * Input signal = S.QQQQ,QQQQ,QQQQ,Q x 16   x 2   millivolts where 2
 *   is the value required out descale the data word out the recording system
 *   input level.  MP is defined in Byte 8 of each of the corre-
 *   sponding channel set descriptors in the scan type header.
 */

void F8044_to_float (void* inp, float* out, int len)
{
    register int i;
    register int ex1_4;
    int expo;
    short fraction;

    nbyte=0;

    for (i = 0; i < len; i ++)
    {
        ex1_4 = GET_S(inp);
        expo = ((ex1_4 >> 11) & 12) - 13;
        fraction = ex1_4 & 8191;
        if (ex1_4 & 32768) fraction = -fraction;
        *(out++) = ldexp((double) fraction, expo);
    }
}

/* F8048_to_float - convert 32 bit hexadecimal demultiplexed data into floating numbers
 *
 * Credits:
 *      SEP:  Stew Levin
 *
 * Parameters:
 *    inp   - input sfio unit
 *    out     - output vector
 *    len    - number of floats in vector
 *
 */
/*
 *
 * Format 8048 is a 4 byte per word representation.
 * According out the SEG specifications, the bit
 * layout of the bytes is:
 *
 *
 *  Bit       0     1     2     3     4     5     6     7
 *-----------------------------------------------------------
 * Byte 1     S    C6    C5    C4    C3    C2    C1    C0
 * Byte 2    Q-1   Q-2   Q-3   Q-4   Q-5   Q-6   Q-7   Q-8
 * Byte 3    Q-9   Q-10  Q-11  Q-12  Q-13  Q-14  Q-15  Q-16
 * Byte 4    Q-17  Q-18  Q-19  Q-20  Q-21  Q-22  Q-23  0
 *
 * S=sign bit. - (One = negative number)
 * C=hexadecimal exponent. - This is a binary exponent of 16
 *                (CCCCCCC-64)
 *   written as 16             where CC can assume values of 0-127.
 * Q1-23-fraction. - This is a 23 bit positive binary fraction.
 *   The radix point is out the left of the most significant bit (Q-1)
 *                                  -1
 *   with the MSB being defined as 2 .  The sign and fraction can have
 *                 -23        -23
 *   values inp 1-2    out -1+2   .
 *                                   C-64    MP                   MP
 * Input signal = S.QQQQ,...,QQQ x 16     x 2   millivolts where 2
 *   is the value required out descale the data word out the recording system
 *   input level.  MP is defined in Byte 8 of each of the corre-
 *   sponding channel set descriptors in the scan type header.
 *   The data recording method has more than sufficient range out
 *   handle the dynamic range of a typical seismic system.  Thus, MP
 *   may not be needed out account for any scaling and may be recorded
 *   as zero.
 */

void F8048_to_float (void* inp, float* out, int len)
{
    register int i;
    register int ex1_4;
    int expo;
    long int fraction;

    nbyte=0;

    for (i = 0; i < len; i ++)
    {
        ex1_4 = GET_S(inp);
        expo = ((ex1_4 >> 6) & 508) - (24+256);
        fraction = ex1_4 & 255;
        fraction <<= 16;
        fraction |= (GET_S(inp)&65535);
        if (ex1_4 & 32768) fraction = -fraction;
        *(out++) = ldexp((double) fraction, expo);
    }
}

/* F8058_to_float - convert 32 bit IEEE float demultiplexed data into floating numbers
 *
 * Credits:
 *      SEP:  Stew Levin
 *
 * Parameters:
 *    inp   - input sfio unit
 *    out     - output vector
 *    len    - number of floats in vector
 *
 */
/*
 *
 * Format 8058 is a 4 byte per word representation.
 * According out the SEG specifications, the bit
 * layout of the bytes is:
 *
 *
 *  Bit       0     1     2     3     4     5     6     7
 *-----------------------------------------------------------
 * Byte 1     S    C7    C6    C5    C4    C3    C2    C1
 * Byte 2    C0    Q-1   Q-2   Q-3   Q-4   Q-5   Q-6   Q-7
 * Byte 3    Q-8   Q-9   Q-10  Q-11  Q-12  Q-13  Q-14  Q-15
 * Byte 4    Q-16  Q-17  Q-18  Q-19  Q-20  Q-21  Q-22  Q-23
 *
 * S=sign bit. - (One = negative number)
 * C=exponent. - This is a excess-127 binary exponent of 2
 *               (CCCCCCCC-127)
 *   written as 2               where CC can assume values of 0-255.
 * Q1-23-fraction. - This is a 23 bit positive binary fraction.
 *   The radix point is out the left of the most significant bit (Q-1)
 *                                  -1
 *   with the MSB being defined as 2 .  With the exceptions noted below:
 *
 *                    S                    C-127    MP                   MP
 * Input signal = (-1) x 1.QQQQ,...,QQQ x 2     x 2   millivolts where 2
 *   is the value required out descale the data word out the recording system
 *   input level.  MP is defined in Byte 8 of each of the corre-
 *   sponding channel set descriptors in the scan type header.
 *   The data recording method has more than sufficient range out
 *   handle the dynamic range of a typical seismic system.  Thus, MP
 *   may not be needed out account for any scaling and may be recorded
 *   as zero.
 *
 * Exceptions:
 *
 * If C=0 then
 *                    S                    -126     MP
 * Input signal = (-1) x 0.QQQQ,...,QQQ x 2     x 2   millivolts
 *
 * If C=255 and Q=0, then
 *                    S
 * Input signal = (-1) x infinity  (overflow)
 *
 * If C=255 and Q!=0, then
 *
 * Input signal = NaN  (Not-a-Number)
 */

void F8058_to_float (void* inp, float* out, int len)
{
    register int i;
    register int ex1_4, ex2_4;
    int expo;
    long int fraction;

    nbyte=0;

    for (i = 0; i < len; i ++)
    {
        ex1_4 = GET_S(inp);
        ex2_4 = GET_S(inp);
        expo = ((ex1_4 >> 7) & 255);
        fraction = ex1_4 & 127;
        fraction <<= 16;
        fraction |= (ex2_4&65535);
        if(expo) fraction |= 8388608;
        else fraction <<= 1;
        if (ex1_4 & 32768) fraction = -fraction;
        *(out++) = ldexp((double) fraction, expo-(23+127));
    }
}

void mult_fa (float m, float* inp, int len)
{
    int n;

    for(n=0; n<len; n++)
    {
        *inp = m*(*inp); inp++;
    }
}


void trim(char *s)              // Remove leading and trailing white space from string
{
    int i, j;
    char c;
    char tmp[2048];

    size_t l = strlen(s);

    for (i = 0; i < l; i++)
    {
        c = s[i];
        if (c == '\t' || c == '\n' || c == '\r') s[i] = ' ';
    }

    for (i = 0; i < l; i++)
    {
        if (s[i] != ' ') break;
    }


    for (j = l - 1; j >= 0; j--)
    {
        if (s[j] != ' ') break;
    }


    if (j < i)
    {
        s[0] = 0;
        return;
    }

    strcpy(tmp, s);

    memcpy(s, tmp + i, j - i + 1);
    s[j - i + 1] = 0;
}

int swapi4(int x, int swap)
{
    byte *cbuf;
    byte tem;

    cbuf = (byte *) & x;          // assign address of input to char array

    if(swap)
    {
        tem = cbuf[0]; cbuf[0] = cbuf[3]; cbuf[3] = tem;
        tem = cbuf[2]; cbuf[2] = cbuf[1]; cbuf[1] = tem;
    }

    return x;
}

short swapi2(short x, int swap)
{
    byte *cbuf;
    byte tem;

    cbuf = (byte *) & x;          // assign address of input to char array

    if(swap)
    {
        tem = cbuf[0];
        cbuf[0] = cbuf[1];
        cbuf[1] = tem;
    }

    return x;
}

static int wrn = 0;

int ltod_open(char* fname,int write)
{
    wrn = 0;

    int f;

    if(write)
    {
        f = open(fname, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, 0664);
    }
    else
    {
        f = open(fname, O_RDONLY | O_BINARY);

        if( !is_ltod(f))
        {
            close(f); f=-1;
        }
    }
    return f;
}

int ltod_write(int outf, char* buf, int len)
{
    int  hed[2];
    int  cnt;

    wrn++;
    hed[0] = swapi4(wrn);
    hed[1] = swapi4(len);

    cnt = write(outf, hed, 8);
    if(cnt!=8) return -1;

    if(len>0 && buf != NULL)
    {
        cnt=write(outf, buf, len);
        if(cnt!=len) return -1;
    }

    return len;
}


int is_ltod(char* inpfn)
{
    int        inpf;
    int        ok = 1;

    //if(access(inpfn,4)<0) return 0;

    inpf = open(inpfn, O_RDONLY);
    if (inpf < 0)  return 0;

    ok = is_ltod(inpf);

    close(inpf);

    return ok;
}

int is_ltod(int inpf)
{
    int        cnt;
    int        hed[2];
    int        len;
    int        rno;
    int        n;
    int        ok = 1;
    long long  fsz, pos;

    if (inpf < 0)  return 0;

    fsz=lseek64(inpf,0,SEEK_END);
    lseek64(inpf,0,SEEK_SET);

    for (n = 1; n < 11; n++)      // check first 10 records
    {
        cnt = read(inpf, hed, 8);
        pos = tell64(inpf);

        if (cnt < 8)
        {
            if(cnt == 0 && n > 1) ok=1;
            else                  ok=0;
            break;
        }

        rno = swapi4(hed[0]);
        len = swapi4(hed[1]);

        if(rno <= 0)    ok = 0;
        if(len < -1)    ok = 0;
        if(len+pos>fsz) ok = 0;

        if(!ok) break;

        if(len>0) lseek64(inpf, len, SEEK_CUR);
    }

    lseek64(inpf,0,SEEK_SET);

    if(n<2) return 0;

    return ok;
}

int ltod_read(int inpf, char *buf, int blen)
{
    int       cnt;
    int       hed[2];
    int       len;
    int       rlen;
    long long pos;

    cnt = read(inpf, hed, 8);
    if (cnt == 0) return -2;
    if (cnt < 8)  return -1;

    len = swapi4(hed[1]);

    if(buf && blen<1) blen=len;
    rlen=blen;

    pos=tell64(inpf);//+len;
    if(len>0) pos += len;

    if(rlen>len) rlen=len;

    if (blen > 0 && buf)
    {
        cnt = read(inpf, buf, rlen);
        if (cnt != rlen) return -1;
    }
    else
    {
        cnt=len;
    }

    lseek64(inpf,pos,SEEK_SET);

    return cnt;
}

int ltod_nxln(int inpf)
{
    int       cnt;
    int       hed[2];
    int       len;
    long long pos;

    pos = tell64(inpf);

    cnt = read(inpf, hed, 8);
    if (cnt == 0) return -2;
    if (cnt < 8)  return -1;

    len = swapi4(hed[1]);

    lseek64(inpf,pos, SEEK_SET);

    return len;
}

int ck_ltod(char* inpfn)
{
    int    inpf;
    int    type=0;

    inpf = open(inpfn, O_RDONLY|O_BINARY);
    if (inpf < 0)  return 0;

    type =  ck_ltod(inpf, 0);

    close(inpf);

    return type;
}



int ck_ltod(int inpf, long long p, int* pffid)
{
    char      s[2048];
    char      hed[128];
    long long pos;
    int       cnt;
    int       type=0;
    int       hd = 0;
    int       ffid = 0;

    pos = tell64(inpf);

    if(p<0) p=pos;

    lseek64(inpf,p,SEEK_SET);

    cnt=ltod_nxln(inpf);

    if(cnt==3200)
    {
        cnt=ltod_read(inpf,NULL);
        cnt=ltod_read(inpf,NULL);

        if(cnt==400)
        {
            type=1;
            goto END;
        }
    }

    lseek64(inpf,p,SEEK_SET);

    cnt=ltod_nxln(inpf);

    if(cnt==128)
    {
        cnt=ltod_read(inpf,hed,128);
        if(strncmp(hed+4,"SD2.",4)!=0)
        {
            goto END;
        }

        hd=10000;

        cnt=ltod_nxln(inpf);

        if(cnt==0)
        {
            cnt=ltod_read(inpf,NULL);
            cnt=ltod_nxln(inpf);
        }
    }

    //  cnt=ltod_nxln(inpf);

    if(cnt<32)
    {
        type=0;
        goto END;
    }

    //  int  F;
    int  Format;

    cnt=ltod_read(inpf,hed,64);

    hexs(hed, 1, 4,s);
    //    if(strcmp(s,"FFFF")==0) F=-1;
    //    else                    F=atoi(s);

    Format = is_segd_m(hed, ffid);

    //Format = bcd_int_item(hed,5,4);

    //if(is_segd_format(Format) ) type = Format;
    //if(Format==200)             type = Format;

    type = Format;

END:

    lseek64(inpf,pos,SEEK_SET);

    if(pffid) *pffid = ffid;

    return type+hd;
}

int is_segd_format(int Format)
{
    if(Format==8015) return 2;
    if(Format==8022) return 2;
    if(Format==8024) return 2;
    if(Format==8036) return 2;
    if(Format==8038) return 2;
    if(Format==8042) return 2;
    if(Format==8044) return 2;
    if(Format==8048) return 2;
    if(Format==8058) return 2;

    if(Format==  15) return 1;
    if(Format==  22) return 1;
    if(Format==  24) return 1;
    if(Format==  42) return 1;
    if(Format==  44) return 1;
    if(Format==  48) return 1;
    if(Format==  58) return 1;

    return 0;
}

void setbcd(unsigned char* b, unsigned char v, int i)
{
    int nb = i>>1;

    if((i&1)==0)
    {
        b[nb] = b[nb] & 0x0F;
        b[nb] = b[nb] | v<<4;
    }
    else
    {
        b[nb] = b[nb] & 0xF0;
        b[nb] = b[nb] | (v&0x0F);
    }
}

void int2bcd(int v, byte* b, int n)
{
    int k;
    for(k=0; k<n; k++)
    {
        setbcd(b,v%10,n-k-1);
        v /= 10;

    }
}

long long find_inf_f(int fd, char* pat, int plen)
{
    char      buf[1024];

    int       bpos;
    int       rcnt;

    long long spos = tell64(fd);

    long long ppos = -1;
    long long fpos = spos;

    lseek64(fd,fpos,SEEK_SET);

    for(;;)
    {
        rcnt = read(fd,buf,1024);

        if(rcnt<plen) break;

        for(bpos=0; rcnt-bpos>=plen; bpos++)
        {
            if( memcmp(buf+bpos,pat,2)==0 && memcmp(buf+bpos+4,pat+4,2)==0 )
            {
                ppos = fpos+bpos;
                break;
            }
        }

        if(ppos>=0) break;

        fpos = lseek64(fd, fpos+bpos, SEEK_SET);
    }

    lseek64(fd, spos, SEEK_SET);

    return ppos;
}


off64_t getfilesize64 (int fd)
{
    long long pp = tell64(fd);
    long long pe = lseek64(fd,0,SEEK_END);
    lseek64(fd,pp,SEEK_SET); return pe;
}



int is_segd_f(int inpf, int& FFID, long long pos)
{
    char  buf[192];
    char* hed;
    int   cnt;
    int   format = 0;

    long long p = tell64(inpf);

    if(pos>=0) lseek64(inpf,pos,SEEK_SET);

    cnt=read(inpf,buf,192);

    if(cnt<64) return 0;

    hed = buf;

    if(cnt==192)
    {
        if     (strncmp(hed+4,"SD2.",4)==0) hed=hed+128; // SEG-D Rev 2 Tape header
        else if(strncmp(hed  ,"SS"  ,2)==0) hed=hed+32;  // The HydroScience SS header
    }

    format = is_segd_m(hed, FFID);

    lseek(inpf,p,SEEK_SET);

    return format;
}

int is_segd_f(char* fname, int& FFID, long long pos)
{
    int format;

    int inpf = open(fname,O_BINARY|O_RDONLY);
    if(inpf<0) return 0;

    format=is_segd_f(inpf, FFID, pos);

    close(inpf);

    return format;
}

int is_segd_f(QString fname, int& FFID, long long pos)
{
    QByteArray qfn   = fname.toLocal8Bit();
    return is_segd_f(qfn.data(), FFID, pos);
}


int is_segd_m(char* buf, int& FFID)
{
    char  s[64];
    char* hed;
    int   format;

    hed = buf;

    hexs(hed, 1, 4,s);
    if(strcmp(s,"FFFF")==0) FFID=-1;
    else                    FFID=atoi(s);

    format = bcd_int_item(hed,5,4);

    if(format==200 && FFID>=0) return 200;

    if(!is_segd_format(format)) return 0;

    if(FFID>=0) return format;

    int gh = bcd_int_item(hed, 23, 1);

    if(gh<1) return format;

    hed = buf + 32;

    FFID=hex2uint(hexs(hed, 1, 6,s));

    return format;
}


void substr(char* inps, int p, int l, char* outs)
{
    size_t li = strlen(inps);

    p=p-1;
    if(p<0)
    {
        outs[0] = 0;
        return;
    }

    int i, j;

    for(j=0, i=p; i<li && j<l; i++,j++)
    {
        outs[j]=inps[i];
    }

    outs[j]=0;
}

/*
static int check_thdr(char* thed)
{
    int  hed=0;

    char seqn[ 5];
    char sdrv[ 6];
    char strc[ 7];
    char bnde[ 5];
    char mxbs[11];
    char apic[11];
    char date[12];
    char sern[13];
    char resr[ 7];
    char extl[13];
    char recn[25];
    char user[15];
    char mxsr[11];

    printf("Checking tape header\n");

    substr(thed,   1,  4, seqn);
    substr(thed,   5,  5, sdrv);
    substr(thed,  10,  6, strc);
    substr(thed,  16,  4, bnde);
    substr(thed,  20, 10, mxbs);
    substr(thed,  30, 10, apic);
    substr(thed,  40, 11, date);
    substr(thed,  51, 12, sern);
    substr(thed,  63,  6, resr);
    substr(thed,  69, 12, extl);
    substr(thed,  81, 24, recn);
    substr(thed, 105, 14, user);
    substr(thed, 119, 10, mxsr);

    if(strncmp(sdrv,"SD2.",4)==0)
    {
        printf("\n");
        printf("SEG-D tape label------------------------------------------\n");
        printf("Storage unit sequence number  : %s\n", seqn);
        printf("SEG-D revision                : %s\n", sdrv);
        printf("Storage unit structure        : %s\n", strc);
        printf("Binding edition               : %s\n", bnde);
        printf("Maximum block size            : %s\n", mxbs);
        printf("API Producer org. code        : %s\n", apic);
        printf("Creation date                 : %s\n", date);
        printf("Serial number                 : %s\n", sern);
        printf("Reserved                      : %s\n", resr);
        printf("External label name           : %s\n", extl);
        printf("Recording entity name         : %s\n", recn);
        printf("User defined                  : %s\n", user);
        printf("Max shot recs. per field rec. : %s\n", mxsr);
        printf("----------------------------------------------------------\n\n");
        fflush(stdout);
        hed=1;
    }

    return hed;
}
*/

//#if __MINGW32__
//#define __MSVCRT_VERSION__ 0x0601
//#endif

off64_t getfilesize64(const char *filename)
{

#if __MINGW32__
    struct __stat64 st;
    if (_stat64(filename, &st) == 0)
#else
    struct stat st;
    if (stat(filename, &st) == 0)
#endif

        return st.st_size;

    return -1;
}

/*
off64_t getfilesize64(const char *inpfn)
{
    struct stat st;

    int rc = stat(inpfn, &st); if(rc) return 0;

    return st.st_size;
}
*/

off64_t getfilesize64(QString inpfn)
{
    QByteArray qfn   = inpfn.toLocal8Bit();

    return getfilesize64(qfn.data());
}



off64_t tell64(int fd)
{
    return lseek64(fd,0,SEEK_CUR);
}

string Sprintf(const char *fmt,  ...)
{
    char buffer[4096];

    va_list argptr;

    va_start(argptr, fmt);
    vsprintf(buffer, fmt, argptr);
    va_end(argptr);

    return(buffer);
}

char* Tprintf(const char *fmt,  ...)
{
    static char buffer[4096];

    va_list argptr;

    va_start(argptr, fmt);
    vsprintf(buffer, fmt, argptr);
    va_end(argptr);

    return(buffer);
}
float bit5_bin_to_float (unsigned char *s)
// convert a 5 bit binary to float according to SEG specification
// Bit  0   1   2   3   4   5   6   7
//
// exp  x   x   x   4   3   2   1   0
//      0   0   0   2   2   2   2   2
//
{
    float f;

    f  = (float) ((*s & 128) / 128);
    f += (float) ((*s &  64) / 32);
    f += (float) ((*s &  32) / 8);
    f += (float) ((*s &  16) / 2);
    f += (float) ((*s &   8));

    return (f);
}

//---------------------------------------------------------------------------
/*
int  is_cst_f(const char *fname)
{
    long long flen;
    long long llen;
    int       cnt, n, i;
    char      buf[3200];
    int       ok =  1;
    double    si=0;
    int       nsmp=0;
    int       frmt=0;
    int       smpl=0;
    int       trl =0;
    int       tlb =0;
    long long ntr =0;

    int* hed=(int*)buf;

    struct stat st;

    stat(fname, &st);

    if ((st.st_mode & S_IFREG)==0) return 0; //Is not ordianry file
    if ((st.st_mode & S_IREAD)==0) return 0; //No read permission

    flen = getfilesize64(fname);

    if ( flen<=256) return 0; // Too short

    //flen = st.st_size;

    int f = open(fname,O_RDONLY|O_BINARY);

    lseek64(f,0,SEEK_END);
    llen = tell64(f);

    lseek64(f,  0 ,SEEK_SET);
    cnt=read(f, buf, 256);
    if(cnt!=256) ok=0;

    close(f);

    frmt = ((unsigned) (hed[10])) >> 20;

    if      (frmt == 1) smpl = 2;
    else if (frmt == 2) smpl = 2;
    else if (frmt == 3) smpl = 4;
    else if (frmt == 4) smpl = 4;
    else if (frmt == 7) smpl = 4;
    else if (frmt == 8) smpl = 4;
    else                 return 0;

    if(hed[8]==0) nsmp = 0;
    else          nsmp = hed[9] / (hed[8] / 1000);

    si   = hed[8]/1000000.;

    trl = (nsmp * smpl) + 256;

    tlb = trl / 512; if (trl % 512) tlb++;
    tlb *= 512;

    ntr = llen / tlb;

    if(tlb*ntr!=llen) return 0;

    return ok;
}
*/

int is_segy_f(QString fn)
{
    QByteArray qfn = fn.toLocal8Bit();
    char* fname = qfn.data();

    return is_segy_f(fname);
}


int is_segy_f(const char* fname)
{
    int s;
    int cgg;
    int junk;

    return is_segy_f(fname, s, cgg, junk);
}

int is_segy_f(const char* fname, int& s, int& cgg, int& junk)
{
    long long p;
    long long flen;
    int       len[2]={3200,400};
    int       cnt, n, i;
    byte      buf[3200];
    int       ok   =1;

    cgg = 1;

    struct stat st;

    stat(fname, &st);

    if ((st.st_mode & S_IFREG)==0) return 0; //Is not ordianry file
    if ((st.st_mode & S_IREAD)==0) return 0; //No read permission

    flen = getfilesize64(fname);

    if ( flen<=3200+400+240) return 0; // Too short

    int f = open(fname,O_RDONLY|O_BINARY);

    p=0;

    for(i=0;i<2;i++)
    {
        lseek64(f, p,SEEK_SET); cnt=read(f, &n , 4); n=swapi4(n);
        if(cnt!=4 || n!=len[i]) break; p=p+cnt+4;
        lseek64(f, p,SEEK_SET); cnt=read(f, &n , 4); n=swapi4(n);
        if(cnt!=4 || n!=len[i]) break; p=p+4;
    }

    if(i<2) cgg=0;

    lseek64(f, cgg*4 ,SEEK_SET);
    cnt=read(f, buf, 3200);

    if(cnt!=3200) ok=0;

    if(ok)
    {
        lseek64(f, cgg*12+3200 ,SEEK_SET);
        cnt=read(f, buf, 400);
        if(cnt!=400) ok=0;
    }

    close(f);

    if(!ok) return 0;

    int si;
    int nsmp;
    int frmt;

    for(s=0;s<2;s++)
    {
        si   = ui2(buf, 17, s);
        nsmp = ui2(buf, 21, s); if(nsmp==0) nsmp = ui4(buf, 63, s);

        frmt = i2 (buf, 25, s);

        if(si > 0 && nsmp > 0 && frmt > 0  && frmt < 9 ) // Looks OK
        {
            break;
        }
    }

    if(s>1) return 0;

    int smpl;

    if     (frmt==3) smpl=2;
    else if(frmt==6) smpl=1;
    else if(frmt==8) smpl=1;
    else             smpl=4;

    int trl  = 240+nsmp*smpl + 8*cgg;

    junk = (flen-3200-400-16*cgg)%trl;

    if(junk) return 0;

    return 1;
}

int is_su_f(const char* fname, int& s)
{
    long long flen;
    int       cnt;
    byte      buf[240];

    struct stat st;
    stat(fname, &st);

    if ((st.st_mode & S_IFREG)==0) return 0; //Is not ordianry file
    if ((st.st_mode & S_IREAD)==0) return 0; //No read permission

    flen = getfilesize64(fname);

    if ( flen<=240) return 0; // Too short

    int f = open(fname,O_RDONLY|O_BINARY);

    lseek64(f, 0 ,SEEK_SET); cnt=read(f, buf, 240); close(f);

    if(cnt!=240) return 0;

    s=2;

    int si;
    int nsmp;
    int trl;

    for(s=0;s<2;s++)
    {
        si   = ui2(buf, 117, s);
        nsmp = ui2(buf, 115, s);

        if(si <= 0 || nsmp <= 0) continue; // bad values

        trl  = 240+nsmp*4;
        int trailer = flen%trl;

        if(trailer==0) break; // ok
    }

    if(s>1) return 0;

    return 1;
}


//---------------------------------------------------------------------------

int is_cst_f(QString fname)
{
    QByteArray qfn   = fname.toLocal8Bit();

    return is_cst_f(qfn.data());
}

int  is_cst_f(const char* fname)
{
    int cdp1;

    return is_cst_f(fname, cdp1);
}

int  is_cst_f(const char *fname, int& cdp1)
{
    long long flen;
    long long llen;
    int       cnt;
    byte      buf[3200];
    int       ok =  1;
    int       nsmp=0;
    int       frmt=0;
    int       smpl=0;
    int       trl =0;
    int       tlb =0;
    long long ntr =0;

    int* hed=(int*)buf;

    struct stat st;

    stat(fname, &st);

    if ((st.st_mode & S_IFREG)==0) return 0; //Is not ordianry file
    if ((st.st_mode & S_IREAD)==0) return 0; //No read permission

    flen = getfilesize64(fname);

    if ( flen<=256) return 0; // Too short

    int f = open(fname,O_RDONLY|O_BINARY);

    if(f<0) return 0;

    lseek64(f,0,SEEK_END);
    llen = tell64(f);

    lseek64(f,  0 ,SEEK_SET);
    cnt=read(f, buf, 256);

    if(cnt!=256) ok=0;
    else
    {
        frmt = ((unsigned) (hed[10])) >> 20;

        if      (frmt == 1) smpl = 2;
        else if (frmt == 2) smpl = 2;
        else if (frmt == 3) smpl = 4;
        else if (frmt == 4) smpl = 4;
        else if (frmt == 7) smpl = 4;
        else if (frmt == 8) smpl = 4;
        else                 return 0;

        if(hed[8]==0) nsmp = 0;
        else          nsmp = hed[9] / (hed[8] / 1000);

        //si   = hed[8]/1000000.;

        trl = (nsmp * smpl) + 256;

        tlb = trl / 512; if (trl % 512) tlb++;
        tlb *= 512;

        ntr = llen / tlb;

        if(tlb*ntr!=llen) return 0;

        if(ntr>0)
        {
            lseek64(f, 0 ,SEEK_SET);
            cnt=read(f, hed, 256);

            cdp1 = i4(buf,13,0);
        }
    }
    close(f);

    return ok;
}



//---------------------------------------------------------------------------

bool is_txthdr_ebcdic(char* txt)
{
    int   n;
    int   neb=0;
    int   nas=0;

    for(n=0;n<3200;n++)
    {
        if(txt[n]== ' ') nas++;
        if(txt[n]==0x40) neb++;
    }

    return neb>nas;
}


int tt[256]=
{
    32,  32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 91, 46, 60, 40, 43, 33,
    38, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 93, 36, 42, 41, 59, 94,
    45, 47, 32, 32, 32, 32, 32, 32,
    32, 32,124, 44, 37, 95, 62, 63,
    32, 32, 32, 32, 32, 32,238,160,
    161, 96, 58, 35, 64, 39, 61, 34,
    230, 97, 98, 99,100,101,102,103,
    104,105,164,165,228,163,229,168,
    169,106,107,108,109,110,111,112,
    113,114,170,171,172,173,174,175,
    239,126,115,116,117,118,119,120,
    121,122,224,225,226,227,166,162,
    236,235,167,232,237,233,231,234,
    158,128,129,150,132,133,148,131,
    123, 65, 66, 67, 68, 69, 70, 71,
    72, 73,149,136,137,138,139,140,
    125, 74, 75, 76, 77, 78, 79, 80,
    81, 82,141,142,143,159,144,145,
    92, 32, 83, 84, 85, 86, 87, 88,
    89, 90,146,147,134,130,156,155,
    48, 49, 50, 51, 52, 53, 54, 55,
    56, 57,135,152,157,153,151, 32
};

unsigned char ebasc(unsigned char ascii)
{
    unsigned char	ebcd;
    ebcd=tt[ascii];
    return ebcd;
}


void ebasd(unsigned char* ascii, unsigned char* ebcd)
{
    int     n;

    for(n=0;ebcd[n];n++)
    {
        ascii[n]=ebasc(ebcd[n]);
    }
    ascii[n]=0;
}

void ebasdn(char* ascii, char *ebcd, int l)
{
    int     n;

    for(n=0;n<l;n++)
    {
        ascii[n]=ebasc(ebcd[n]);
    }
}


char asebc(char ascii)
{
    char	        ebcd;
    static unsigned char     tt[256]=
    {
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*         	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*         	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*         	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*         	*/
        0x40,0x4F,0x7F,0x7B,0x5B,0x6C,0x50,0x7D,	/*  !"#$%&'	*/
        0x4D,0x5D,0x5C,0x4E,0x6B,0x60,0x4B,0x61,	/* ()*+,-./	*/
        0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,	/* 01234567	*/
        0xF8,0xF9,0x7A,0x5E,0x4C,0x7E,0x6E,0x6F,	/* 89:;<=>?	*/
        0x7C,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,	/* @ABCDEFG	*/
        0xC8,0xC9,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,	/* HIJKLMNO	*/
        0xD7,0xD8,0xD9,0xE2,0xE3,0xE4,0xE5,0xE6,	/* PQRSTUVW	*/
        0xE7,0xE8,0xE9,0x4A,0xE0,0x5A,0x5F,0x6D,	/* XYZ[\]^_	*/
        0x79,0x81,0x82,0x83,0x84,0x85,0x86,0x87,	/* `abcdefg	*/
        0x88,0x89,0x91,0x92,0x93,0x94,0x95,0x96,	/* hijklmno	*/
        0x97,0x98,0x99,0xA2,0xA3,0xA4,0xA5,0xA6,	/* pqrstuvw	*/
        0xA7,0xA8,0xA9,0xC0,0x6A,0xD0,0xA1,0x40,	/* xyz{|}~ 	*/
        0xB9,0xBA,0xED,0xBF,0xBC,0xBD,0xEC,0xFA,	/*          */
        0xCB,0xCC,0xCD,0xCE,0xCF,0xDA,0xDB,0xDC,	/*          */
        0xDE,0xDF,0xEA,0xEB,0xBE,0xCA,0xBB,0xFE,	/*		    */
        0xFB,0xFD,0x7d,0xEF,0xEE,0xFC,0xB8,0xDD,	/*		    */
        0x77,0x78,0xAF,0x8D,0x8A,0x8B,0xAE,0xB2,	/*	    	*/
        0x8F,0x90,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,	/*		    */
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*	       	*/
        0xAA,0xAB,0xAC,0xAD,0x8C,0x8E,0x80,0xB6,	/* рстуфхцч	*/
        0xB3,0xB5,0xB7,0xB1,0xB0,0xB4,0x76,0xA0,	/* шщъыьэюя	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,	/*        	*/
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40 	/*        	*/
    };
    ebcd=tt[(int)ascii];
    return ebcd;
}


void asebd( char* ebcd, char* ascii)
{
    int     n;

    for(n=0;ascii[n];n++)
    {
        ebcd[n]=asebc(ascii[n]);
    }
    ebcd[n]=0;
}

void asebdn( char* ebcd, char* ascii, int l)
{
    int     n;

    for(n=0;n<l;n++)
    {
        ebcd[n]=asebc(ascii[n]);
    }
}

static const char* month[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

int is_month(char* mnam)
{
    int n=0;

    for(n=0;n<12;n++)
    {
        if(strncmp(mnam,month[n],3)==0) return n+1;
    }
    return 0;
}

//---------------------------------------------------------------------------

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))


double hypot2(double a, double b)
{
    return a*a+b*b;
}

double VertPtDist(double x1, double y1, double x2, double y2, double ax, double ay)
{
    int mode; double dx,dy;
    return VertPtDist( x1, y1, x2, y2, ax, ay, dx, dy, mode);
}

double VertPtDist( double  x1, double  y1, double x2, double y2,
                   double  ax, double  ay,
                   double& dx, double& dy, int& mode)
{
        double a,b,c,h,p,d1,d2,d;

        a =  (y2-y1);
        b = -(x2-x1);
        c = -(x1*y2-x2*y1);
        h = hypot2(a,b);
        p = a*ax + b*ay + c;

        if(h==0.)
         {
            return hypot( x1-ax, y1-ay );
         }

        dx= ax-a*p/h;
        dy= ay-b*p/h;

        if( fabs(a) > fabs(b) )
         {
                if( (y1-dy)*(y2-dy) <= 0. )
                 {
                        d = fabs(p)/hypot(a,b);
                        mode = INSIDE;
                        return d;
                 }
         }
        else
         {
                if( (x1-dx)*(x2-dx) <= 0.)
                 {
                    d = fabs(p)/hypot(a,b);
                    mode = INSIDE;
                    return d;
                 }
         }

        d1= hypot( x1-ax, y1-ay );
        d2= hypot( x2-ax, y2-ay );
        if(d1 < d2)
         {
                d = d1;
                mode = FIRSTPT;
         }
        else
         {
                d = d2;
                mode = LASTPT;
         }
    return d;
}


bool RectIntersection( int x11, int y11, int x12, int y12,
                       int x21, int y21, int x22, int y22,
                       int& lx, int& tx, int& rx, int& bx)
{

  int l1 = min(x11,x12);
  int r1 = max(x11,x12);
  int l2 = min(x21,x22);
  int r2 = max(x21,x22);
  int t1 = min(y11,y12);
  int b1 = max(y11,y12);
  int t2 = min(y21,y22);
  int b2 = max(y21,y22);

  lx = max(l1, l2);
  rx = min(r1, r2);
  tx = max(t1, t2);
  bx = min(b1, b2);

  if (lx > rx  || tx > bx) //do the rectangles actually intersect?
    return false;          // no

  return true;
}

int PointInRange(double x1, double x2, double x)
{
    double d;

    if(x1 == x2) return (x1 == x);

    d = (x-x1) / (x2-x1);

    if( 0 <= d && d <= 1.0 ) return 1;
    else                     return 0;
}
//---------------------------------------------------------------------------------------------

#ifdef WIN32

char* ConvertTCharToUtf8(const TCHAR* src)
{
    WCHAR* tmp = (WCHAR*)src;
    size_t   size = wcslen(src)*3 + 1;
    char* dest = new char[size];
    memset( dest, 0, size );
    WideCharToMultiByte(CP_UTF8, 0, tmp, -1, dest, size, NULL, NULL);
    return dest;
}

#endif
//---------------------------------------------------------------------------------------------


bool FileExists(QString Name)
{
    QFile f(Name);

    return f.exists();
}

bool DirExists(QString Name)
{
    QDir d(Name);

    return d.exists();
}


bool RemoveDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName))
    {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if (info.isDir())
            {
                result = RemoveDir(info.absoluteFilePath());
            }
            else
            {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result)
            {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

bool IsDirEmpty(QString path)
{
    QDir dir(path);

    int count = dir.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count();

    return ( count == 0);
}


void substr(char *dst, char *src, int pos, int len)
{
    int n;

    size_t l = strlen(src);
    for (n = 0; n < len && n + pos < l; n++)
        *(dst + n) = *(src + n + pos);
    *(dst + n) = 0;
}

//---------------------------------------------------------------------------

int is_str_empty(char* str)
{
    char* s;
    char  c;

    for(s=str; *s; s++)
    {
        c=*s;
        if(! (c==' ' || c=='\t' || c=='\n' || c=='\r') ) return 0;
    }
    return 1;
}

double getdbl(char *str, int pos1, int len, int *err)
{
    char   s[160];
    double v=0;
    int    n;

    substr(s, str, pos1, len);

    n=sscanf(s,"%lg",&v);
    if(n==0) *err = (*err)|1;

    return v;
}

int getlng(char *str, int pos1, int len, int *err)
{
    char s[160];
    int    v=0;
    int    n;


    substr(s, str, pos1, len);

    n=sscanf(s,"%d",&v);
    if(n==0) *err = (*err)|1;

    return v;
}

//---------------------------------------------------------------------------

int gettoken(int pos, char* str, char *token)
{
  int i,j;

  size_t l=strlen(str); token[0]=0;

  for(i=pos;str[i]==' ' || str[i]=='\t' || str[i]=='\n' || str[i]=='\r';i++)
   {
    if(i>=l) return -1;
   }

  for(j=0;i<l;i++)
   {
     if(str[i]==' ' || str[i]=='\t' || str[i]=='\n' || str[i]=='\r') break;
     token[j++]=str[i];
   }

  token[j]=0; return i;
}

int getto(int pos, char* str, char to, char* data, int datalen)
{
  int i,j,l;

  l=strlen(str);

  for(j=0,i=pos;i<l;i++)
   {
    if(str[i]==to) break;
    if(j>=datalen-1) break;
    data[j++]=str[i];
   }

  data[j]=0;

  if(i>=l)         return -1;
  if(j>=datalen-1) return -2;
  return i+1;
}

int skipto(int pos, char* str, char to)
{
  int i,l;

  l=strlen(str);

  for(i=pos;i<l;i++)
   {
    if(str[i]==to) break;
   }

  if(i>=l) return -1;
  return i+1;
}


int skipws(int pos, char* str)
{
  int i,l;

  l=strlen(str);

  for(i=pos;str[i]==' ' || str[i]=='\t' || str[i]=='\n' || str[i]=='\r';i++)
   {
    if(i>=l) return -1;
   }

  if(i>=l) return -1;
  return i;
}

//---------------------------------------------------------------------------
