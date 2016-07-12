//---------------------------------------------------------------------------

#ifndef libSegDH
#define libSegDH

#include <iostream>
#include <vector>
#include <string>

/*
#if defined (__WIN32__)
#include <io.h>
#include "diskio64.h"
#endif
*/
#include "util2.h"

using namespace std;

//---------------------------------------------------------------------------

class ptrAry
{
private:

    void** _data;
    int    _nitems;
    int    _nalloc;

public:

    int getCount ()
    {
        return _nitems;
    };

    ptrAry();
    ptrAry(ptrAry& src);
    ~ptrAry();

    void*& operator[](int i);
    ptrAry& operator= (const ptrAry &src);

    int IndexOf(void* v);

    void Del(void* v);
    void Del(int   i);

    void Add(void* v);
    void Clear();
    int  SetLength(int len);
};
//---------------------------------------------------------------------------

class byteAry
{
private:

    char* _data;
    int  _nitems;

public:

    int   get_count()
    {
        return _nitems;
    }
    char* get_data ()
    {
        return _data;
    }

    byteAry();
    byteAry(byteAry& src);
    ~byteAry();

    char& operator[](int i);
    byteAry& operator= (const byteAry &src);

    void Clear();
    int SetLength(int len);
};

//---------------------------------------------------------------------------

class TMyObj
{
public:
    virtual ~TMyObj()
    {
        //int x=1;
    }
};

class TMyObjList
{
private:

    bool     OwnsObjects;

    ptrAry _obj;

public:

    int getCount()
    {
        return _obj.getCount();
    }

    void Add(TMyObj* pobj)
    {
        _obj.Add(pobj);
    }

    void Del(TMyObj* pobj)
    {
        int n;

        int nobj = getCount();
        for(n=0; n<nobj; n++)
        {
            if((TMyObj*)_obj[n] == pobj)
            {
                if(OwnsObjects) delete pobj;
                _obj.Del(n);
            }
        }
    }

    void Clear()
    {
        int n;

        if(OwnsObjects)
        {
            int nobj = getCount();
            for(n=0; n<nobj; n++)
            {
                TMyObj* obj = (TMyObj*)_obj[n];
                if(obj) delete obj;
            }
        }

        _obj.Clear();
    }


    TMyObj* getObj(int n)
    {
        if(n<0 || n>=getCount()) return NULL;

        return (TMyObj*)_obj[n];
    }

    TMyObjList()
    {
        OwnsObjects=true;
    }

    TMyObjList(bool AOwnsObjects)
    {
        OwnsObjects=AOwnsObjects;
    }

    ~TMyObjList()
    {
        Clear();
    }
};

//---------------------------------------------------------------------------

class TTrInf
{
public:
    int         the;
    int         csn;
    int         stn;
    int         chn;
    int         exns;
    int         stat;
    int         pos;
};

/*
class TTri
{
public:
//  int   csn;
//  int   stn;
    int   csi;
    int   chn;
    int   pos;

//  TTri(int cs, int st, int ch)
    TTri(int ci, int ch)
    {
//      csn  = cs;
//      stn  = st;
        csi  = ci;
        chn  = ch;
    }

    TTri()
    {
//      csn  =  0;
//      stn  =  0;
        csi  = -1;
        chn  =  0;
        pos  = -1;
    }
};
*/


class TCs;
class TCsList;
class TSegDObj;
class TSegD;

class TSegD
{
protected:

    string  _err;
public:

//    TTri        ltrs [10000]; // Much enough
    int         ntrs;

    int         hdr_len;
    int         trc_len;
    int         trc_cnt;

    TMyObjList* objs;
    TCsList*    csl;

    char* _buf;
    int   _len;
    int   _pos;

    char* read  (int len);
    int   open  (char* fname);
    int   create(int len);
    void  close ();
    int   seek  (int pos, int from);
    int   tell  ();
    char* getbuf() { return _buf; }
    int   getlen() { return _len; }

    int find_trcx(int ppos,  short ffff, int  fn);

    TTrInf  chck_trcx(int ppos,  short ffff, int fn);

    int   dev_type;  // 1 - Seal, 2-syntrak

    int   ltod;

    string  warn;

    char K [13];

    unsigned short  FFFF;
    int    Type;
    int    F;
    int    Format;
    int    G;
    int    R;
    int    ST;
    int    CS;
    int    SK;
    int    EC;
    int    EX;
    int    GH;
    int    I;
    int    Mc;
    int    Ms;
    int    P;
    int    THE;
    int    SKK;
    int    SP;
    int    B;          // SEG-B Scan Length

    int    XX;         // number 32 block aftrer SEG-D header (Syntrak peculiarity)

    int    XF;         // Number of records (shots or time slices) in this file
                       // (FairField peculiarity)

    int    Rev;        // SEG-D Revision Number

    int    Smpl;       // sample length
    float  SmplB;      // sample length in bytes;
    int    TrcSmpNum;  // number samples in trace

    double Si;         // sample interval
    double R_ms;

    int     ReelNo;
    string  LineNm;
    string  Client;
    string  Contra;

    string  NavStr;

    int     atn; // sequential trace number

    int  get_nobj();
    int  get_ncs ();

    string  get_err()
    {
        return _err;
    }

    TCs*  get_csi (int idx);
    int find_csi(int csn);

    TSegDObj*  get_obj(int idx);

     TSegD();
    ~TSegD();

    void  Clear();
    int   Parse(char* buf, int len, bool HeadersOnly, bool keep_buf=false);
};


class TSegDObj : public TMyObj
{
protected:

    string  _inf;
    string  _err;

    TSegD*      _segd;

    int  xprintf(int out, const char *fmt,  ...); // 0 - log, 1 - get_err()
    void displayMemory(int out, char *address, int length);

    void        DumpNavData(char* s);
    void        DumpGunData(char* s);

    byteAry    _hdr;
    char*      _dat;
    int        _pos;

public:

    int        len; // length in memory (for demux traces only headrs)
    int        fln; // length in file for demux traces;
    char*      hdr;

    string      name;
    int         type;  // 0 - unk, 1 - hdr, 2 - cs, 3 - trc, 4 - mux

    TSegDObj()
    {
        _segd = NULL;
        _dat  = NULL;
        _pos  = 0;

        hdr   = NULL;
        len   = 0;
        fln   = 0;
        type  = 0;
    }

    TSegDObj(TSegD* segd)
    {
        _segd =  segd;
        _pos  = _segd->tell();
        _dat  =  _segd->getbuf()+_pos;

        len  = 0;
        fln  = 0;
        type = 0;

        _segd->objs->Add(this);
    }

    ~TSegDObj()
    {
    }

    void set_len(int l)
    {
        _hdr.SetLength(l);
        hdr=_hdr.get_data();
        len=l;
    }

    char* get_buf()
    {
        return _segd->getbuf()+_pos;
    }

    virtual string  get_inf()
    {
        return _inf;
    }
    virtual string  get_err()
    {
        return _err;
    }

};

class TSegdInfo : public TSegDObj
{
private:

    string  get_inf();

public:
    TSegdInfo(TSegD* segd);
};


class TSegdWarn : public TSegDObj
{
private:

    string  _wrn;
    string  get_inf();

public:

    TSegdWarn(TSegD* segd, string warn);
};

class TSegdEof : public TSegDObj
{
private:

    string  _wrn;
    string  get_inf();

public:

    TSegdEof(TSegD* segd, string warn);
};




class TGenHdr1 : public TSegDObj
{
private:    // User declarations
    virtual string  get_inf();

public:

    TGenHdr1(TSegD* segd);

};


class TGenHdr2 : public TSegDObj
{
private:

    string  get_inf();

public:
    TGenHdr2(TSegD* segd);

    int       R;
    int       CS;
    int       EC;
    int       EX;

    double    R_ms;
};


class TGenHdr3 : public TSegDObj
{
private:

    string  get_inf();

public:
    TGenHdr3(TSegD* segd);
};


class TGenHdrB : public TSegDObj
{
private:

    string  get_inf();

public:

    int NTRS;
    int NAUX;

    TGenHdrB(TSegD* segd);
};



class TGenHdrN : public TSegDObj
{

private:

    string  get_inf();

    int ghn;

public:
    TGenHdrN(TSegD* segd, int num);
};


class TEcHdr : public TSegDObj
{
private:

    int _mod;

    string  get_inf();

public:
    TEcHdr(TSegD* segd);
};


class TExHdr : public TSegDObj
{
private:

    string      get_inf();

public:
    TExHdr(TSegD* segd);
};

class TSsHdr : public TSegDObj
{
private:

    string  get_inf();

public:
    TSsHdr(TSegD* segd);
};


class TXxHdr : public TSegDObj
{
private:

    string  get_inf();

public:
    TXxHdr(TSegD* segd);
};

class TSkew : public TSegDObj
{
private:

    string get_inf();

public:
    TSkew(TSegD* segd);
};


class TMux : public TSegDObj
{
private:

    TMyObjList* traces;

    string get_inf();

public:

    TMux(TSegD* segd);
    ~TMux();
};



class TJunk : public TSegDObj
{
private:

    string  get_inf();

public:
    TJunk(TSegD* segd, int len);
};


class TTapeHdr : public TSegDObj
{
private:

    string  get_inf();

public:
    TTapeHdr(TSegD* segd);
};


class TTr : public TSegDObj
{
private:

public:

    int         csi;

    int         atn; // seguential trace number in file
    int         tn;
    int         the;
    int         skk;
    int         csn;
    int         stn;
    int         ns;
    int         exns;
    int         junk;

    float*       smps;

    void  get_smps(float *smps);

    string  get_inf();

    float  get_smp (int ns);
    int    cnv_smps(char* ismp, float* osmp);

    TTr(TSegD* segd, int nsmp,  int chn, int csi);
    TTr(TSegD* segd,  TTrInf ti);

   ~TTr();
};

class TTrList : public TMyObjList
{
private:


public:

    TTrList() : TMyObjList(false) {}

    TTr*  get_tr(int trn)
    {
        return  (TTr*)getObj(trn);
    }

};

class TCs : public TSegDObj
{
protected:

    TTrList*    _trc;
    int         _ntr;

public:

    char        typnm[1024];
    int         typn;
    int         stn;
    int         csn;
    int         to;
    int         tn;
    float       si;
    int         nsscan;     // fixed
    int         nsso;       // original value from headers
    float       mp;
    int         tml;        // channel length in ms
    int         ns;         // nsamples
    int         ln;         // trace length in bytes
    int         lj;         // junk length at end of trace
    int         hl;         // header length
    int         csse;
    int         nch;

    TCs(TSegD* segd);
    ~TCs();

    int       get_ntr()
    {
        return _trc->getCount();
    }

    void      set_ntr(int v)
    {
        _ntr = v;
    }
    TTrList*  get_trc()
    {
        return _trc;
    }
};


class TCsD : public TCs
{

public:

    TCsD(TSegD* segd);

    virtual string  get_inf();
};

class TCsB : public TCs
{
public:
    TCsB(TSegD* segd, int no, int ntr); // no: 0 - aux, 1 - data

    virtual string  get_inf();
};




class TCsList : public TMyObjList
{
private:

public:

    TCsList() : TMyObjList(false) {}

    TCs*  find(int stn, int csn);

    TCs*  get_cs (int csn);
    TCs*  get_csi(int idx);
};

//---------------------------------------------------------------------------

class TLtod : public TMyObjList
{
private:

public:

    string     fname;

    TLtod() : TMyObjList(true) {}
};



class TFileInLtod : public TMyObj
{
private:

public:
    TLtod*     ltod;
    int        fno;
    long long  pos;
    int        len;
    int        type;
    int        ffid;

    TFileInLtod() : TMyObj()
    {
        ltod =  NULL;
        fno  =  0;
        pos  =  0;
        len  =  0;
        type =  0;
        ffid = -1;
    }
};




//---------------------------------------------------------------------------


#endif
