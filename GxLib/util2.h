//---------------------------------------------------------------------------

#ifndef Util2H
#define Util2H

#include <stdarg.h>

#include <string>

#include <QString>

using namespace std;

typedef unsigned char byte;
typedef unsigned int  uns;

#ifdef WIN32

#include "windows.h"

    char* ConvertTCharToUtf8(const TCHAR* src);
#endif



#ifdef _MSC_VER

   #include <io.h>
   typedef __int64 off64_t;
   #define lseek64 _lseeki64

   #define _USE_MATH_DEFINES
   #include <cmath>

// #include <vld.h>

#else

 #include <unistd.h>

#ifndef O_BINARY
    #define O_BINARY 0x8000
#endif


#endif


#define swapb(x,y) {byte tmp; tmp=(x); (x)=(y); (y)=tmp;}

int isnumeric(char *str);

char* strmem(char* str, char* buf, int blen);

char  hexc (char* buf, int n);
char* hexs(char* buf, int n, int count, char* str);

int            i4 (byte* buf, int nbyte, int swap);
unsigned int   ui4(byte* buf, int nbyte, int swap);
int            i3 (byte* buf, int nbyte, int swap);
short          i2 (byte* buf, int nbyte, int swap);
unsigned short ui2(byte* buf, int nbyte, int swap);
char           i1 (byte* buf, int nbyte);

double iFmt(int frmt, byte* buf, int nbyte, int swap);
void   sFmt(int frmt, byte *buf, int nbyte, double v, int swap);

char* strncpy1(char* d, char* s, int n);
char* memstr(char* mem, int maxl, char* str);

byte               hexc2byte (char hex);
unsigned int       hex2uint  (char* hex);
unsigned long long hex2uint64(char* hex);
long long          hex2sint64(char* hex);
char*              hex2ascii (char* hex);
float              hex2flt   (char* hex);
double             hex2dbl   (char* hex);

int   uns_item    (char* buf, int pos, int count);
int   int_item    (char* buf, int pos, int count);
float flt_item    (char* buf, int pos, int count);
char* bcd_str_item(char* buf, int pos, int count, char* str=NULL);
int   bcd_int_item(char* buf, int pos, int count);
char* asc_item    (char* buf, int pos, int count, char* str);

const char* pritem(const char *descr, int type, char* buf, int n, int count, int p2=0, int eol=1);


const char* polnum2nam(int num);
const char* mcnum2nam (int num);
const char* fmtnum2nam(int num);
const char* typnum2nam(int num);

void swap4(void* x);
void swap2(void* x);

float hexep32_to_ieee(void* x);
float ieee_to_ieee   (float x);

float ibm2ieee(float x, int swap);

void segd8015_to_ieee(void* inp, float* out, int ns);

void B0015_to_float (void* inp, float* out, int len);

void F8015_to_float (void* inp, float* out, int len);
void F0015_to_float (void* inp, float* out, int len);
void F8022_to_float (void* inp, float* out, int len);
void F8024_to_float (void* inp, float* out, int len);
void F8036_to_float (void* inp, float* out, int len);
void F8038_to_float (void* inp, float* out, int len);
void F8042_to_float (void* inp, float* out, int len);
void F8044_to_float (void* inp, float* out, int len);
void F8048_to_float (void* inp, float* out, int len);
void F8058_to_float (void* inp, float* out, int len);

float bit5_bin_to_float (unsigned char *s);

void mult_fa (float m, float* inp, int len);

int   swapi4(int   x, int swap = 1);
short swapi2(short x, int swap = 1);

int ltod_open(char* fname,int write);

int is_ltod(char* inpfn);
int is_ltod(int   inpf);

int ck_ltod(char* inpfn);
int ck_ltod(int inpf, long long p=-1, int *pffid=NULL);

int ltod_read (int inpf, char *buf, int len=0);
int ltod_write(int outf, char* buf, int len);
int ltod_nxln (int inpf);

int is_segd_format(int Format);

int is_segd_f(QString fname, int& FFID, long long pos = -1);
int is_segd_f(char*   fname, int& FFID, long long pos = -1);
int is_segd_f(int     inpf,  int& FFID, long long pos = -1);

int is_segd_m(char* buf  , int& FFID);

int is_segy_f(const char *fname, int &s, int &cgg, int &junk);
int is_segy_f(const char* fname);
int is_segy_f(QString fname);

int is_su_f  (const char* fname, int &s);

int  is_cst_f(QString fname);
int  is_cst_f(const char* fname);
int  is_cst_f(const char *fname, int& cdp1);


bool is_txthdr_ebcdic(char* txt);

void int2bcd(int v, char* b, int n);

void substr(char* inps, int p, int l, char* outs);

long long find_inf_f(int fd, char* pat, int plen);

off64_t getfilesize64(int fd);
off64_t getfilesize64(QString     fname);
off64_t getfilesize64(const char* fname);
off64_t tell64(int fd);

string Sprintf(const char *fmt,  ...);

char*  Tprintf(const char *fmt,  ...);


unsigned char ebasc(unsigned char ascii);
void          ebasd(unsigned char* ascii, unsigned char* ebcd);
void          ebasdn(char *ascii, char* ebcd, int l);
char          asebc(char ascii);
void          asebd( char* ebcd, char* ascii);
void          asebdn( char* ebcd, char* ascii, int l);

int is_month(char* mnam);

//---------------------------------------------------------------------------

#define INSIDE  0
#define FIRSTPT 1
#define LASTPT  2

double hypot2(double a, double b);

double VertPtDist(double x1, double y1, double x2, double y2, double ax, double ay);

double VertPtDist( double  x1, double  y1, double x2, double y2,
                   double  ax, double  ay,
                   double& dx, double& dy, int& mode);

bool RectIntersection( int x11, int y11, int x12, int y12,
                       int x21, int y21, int x22, int y22,
                       int& lx, int& tx, int& rx, int& bx);

int PointInRange(double x1, double x2, double x);

//---------------------------------------------------------------------------

bool FileExists(QString Name);
bool DirExists (QString Name);

bool RemoveDir(const QString &dirName);

bool IsDirEmpty(QString path);

//---------------------------------------------------------------------------

void trim(char *s);

void substr(char *dst, char *src, int pos, int len);

double getdbl(char *str, int pos1, int len, int *err);
int    getlng(char *str, int pos1, int len, int *err);

int is_str_empty(char* str);

//---------------------------------------------------------------------------

int gettoken(int pos, char* str, char *token);
int skipws  (int pos, char* str);
int skipto  (int pos, char* str, char to);
int getto   (int pos, char* str, char to, char* data, int datalen);

//---------------------------------------------------------------------------


#endif
