#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//#include <unistd.h>

#if defined (__WIN32__)
#include <io.h>
#endif

#include "libSegD.h"
#include "util2.h"

#ifdef _MSC_VER
    #pragma warning( push )
    #pragma warning( disable : 4267 4100 4068 )
#endif


using namespace std;

//---------------------------------------------------------------------------
ptrAry::ptrAry()
{
    _nalloc=1;
    _data=new void*[1];
    _nitems=0;
}

ptrAry::ptrAry(ptrAry& src)
{
    _nitems = src._nitems;
    _nalloc = src._nalloc;

    _data=new void*[_nalloc];

    if(_nalloc)
        memcpy(_data,src._data,_nalloc*sizeof(void*));
}


ptrAry::~ptrAry()
{
    delete [] _data;
}

void*& ptrAry::operator[](int i)
{
    if(i>=_nitems)
        throw "ptrAry: index error";

    return _data[i];
}

ptrAry& ptrAry::operator= (const ptrAry &src)
{
    if (this ==&src)return *this;

    _nitems = src._nitems;
    _nalloc = src._nalloc;


    delete [] _data;
    _data=new void*[_nalloc];

    if(_nalloc)
        memcpy(_data,src._data,_nalloc*sizeof(void*));

    return *this;
}

int ptrAry::IndexOf(void* v)
{
    int n;
    for(n=0; n<_nitems; n++)
        if(_data[n]==v) return n;
    return -1;
}

void ptrAry::Del(void* v)
{
    Del(IndexOf(v));
}

void ptrAry::Del(int i)
{
    if(_nitems<1 || i<0 || i>=_nitems) return;

    int n;

    for(n=i+1; n<_nitems; n++) _data[n-1]=_data[n];

    _nitems--;
}



void ptrAry::Add(void* v)
{
    int n;

    if(_nitems>=_nalloc)
    {
        _nalloc+=1000;

        void** tmp = new void*[_nalloc];

        for(n=0; n<_nitems; n++) tmp[n]=_data[n];

        delete [] _data;
        _data=tmp;
    }

    _data[_nitems++]=v;
}

void ptrAry::Clear()
{
    delete [] _data;
    _nalloc=1000;
    _data=new void*[1000];
    _nitems=0;
}

int ptrAry::SetLength(int len)
{
    int n;
    int na;

    na=len;
    if(na<1) na=1;

    void** tmp = new void*[na];

    memset(tmp,0,sizeof(void*)*na);

    for(n=0; n<_nitems && n<len; n++) tmp[n]=_data[n];

    delete [] _data;
    _data=tmp;

    _nitems=len;
    _nalloc=na;

    return _nitems;
}
//---------------------------------------------------------------------------

byteAry::byteAry()
{
    _nitems=0;
    _data=NULL;
}

byteAry::byteAry(byteAry& src)
{
    _nitems = src._nitems;

    if(_nitems)
    {
        _data=new char[_nitems];
        memcpy(_data,src._data,_nitems);
    }
    else
    {
        _data=NULL;
    }
}


byteAry::~byteAry()
{
    if(_data) delete [] _data;
}

char& byteAry::operator[](int i)
{
    if(i>=_nitems || i<0)
        throw "byteAry: index error";

    return _data[i];
}

byteAry& byteAry::operator= (const byteAry &src)
{
    if (this == &src) return *this;

    _nitems = src._nitems;

    if(_nitems)
    {
        _data=new char[_nitems];
        memcpy(_data,src._data,_nitems);
    }
    else
    {
        _data=NULL;
    }

    return *this;
}

void byteAry::Clear()
{
    if(_data)delete [] _data;
    _nitems=0;
    _data=NULL;
}

int byteAry::SetLength(int len)
{
    if(len == _nitems) return _nitems;

    if(len<=0)
    {
        Clear();
        return 0;
    }

    char* tmp = new char[len];

    if(_nitems>len) _nitems=len;

    memset(tmp,0,len);

    if(_nitems) memcpy(tmp,_data,_nitems);

    delete [] _data;
    _data=tmp;

    _nitems=len;

    return _nitems;
}


//---------------------------------------------------------------------------


TSegD::TSegD()
{
    objs = new TMyObjList;
    csl  = new TCsList;

   _buf = NULL;
   _pos = 0;
   _len = 0;

    Clear();
}


void   TSegD::Clear()
{
   if(_buf) delete [] _buf;

   _buf = NULL;
   _pos = 0;
   _len = 0;

   Rev=0;

   K[0] = 0;

    Format= 0;
    R     = 0;
    ST    = 0;
    CS    = 0;
    SK    = 0;
    EC    = 0;
    EX    = 0;
    GH    = 0;
    I     = 0;
    Mc    = 0;
    Ms    = 0;
    P     = 0;
    THE   = 0;
    SKK   = 0;
    SP    = 0;
    XX    = 0;
    XF    = 0;

    R_ms  = 0;

    NavStr = "";

    Client = "";
    Contra = "";
    LineNm = "";

    ReelNo = 0;

    //len     = 0;
    hdr_len = 0;
    trc_cnt = 0;

    atn = 0;

    //pos = 0;

    objs->Clear();
    csl ->Clear();

    _err = "";
}


TSegD::~TSegD()
{
    delete objs;
    delete csl;
    if(_buf) delete [] _buf;
}

int  TSegD::get_nobj()
{
    return objs->getCount();
}


TCs*   TSegD::get_csi(int idx)
{
    return (TCs*)(csl->getObj(idx));
}

int   TSegD::find_csi(int csn)
{
    int i;
    for(i=0;i<get_ncs();i++)
    {
       TCs* Cs = get_csi(i);
       if(Cs->csn == csn) return i;
    }
    return -1;
}


TSegDObj*  TSegD::get_obj(int idx)
{
    return (TSegDObj*) (objs->getObj(idx));
}

int  TSegD::get_ncs()
{
    return csl->getCount();
}


int TSegD::Parse(char* buf, int len, bool HeadersOnly, bool keep_buf)

{
    int       n,i;
    long long tpos   = 0;
    int       scnlen = 0;
    int       lenok;
    char*     hed;

    Clear();

    TSegDObj* obj = NULL;

    hdr_len = 0;
    trc_cnt = 0;

    _buf = buf;
    _len = len;

    new TSegdInfo(this);

    hed = read(128);

    if(hed)
    {
        seek(0,SEEK_SET);
        if(strncmp(hed+4,"SD2.",4)==0)
        {
            new TTapeHdr(this); seek(128,SEEK_SET);
        }

        if(strncmp(hed,"SS",2)==0)
        {
            int ffid;
            if(is_segd_m(hed+32, ffid))
            {
              new TSsHdr(this); seek(32,SEEK_SET);
            }
        }

        if(len==128)
        {
            Type=21;
            goto END;
        }
    }


    obj = new TGenHdr1(this);
    if(obj->get_err()!="")
    {
        objs->Del(obj);
        obj=NULL;
        goto END;
    }

    if(Format==200)
    {

        //  SEG-B parsing

        objs->Del(obj); obj=NULL; seek(0,SEEK_SET);
        obj = new TGenHdrB(this);

        int ntrs = ((TGenHdrB*)obj)->NTRS;
        int naux = ((TGenHdrB*)obj)->NAUX;

        trc_cnt = ntrs+naux;

        obj = new TCsB(this,0,naux);
        obj = new TCsB(this,1,ntrs);

        obj = new TMux(this);
        if(obj->get_err()!="") goto END;


        goto END;
    }

    //  SEG-D parsing

    if(GH>0)
    {
        obj = new TGenHdr2(this);
        if(obj->get_err()!="") goto END;
    }

    if(GH>1)
    {
        obj = new TGenHdr3(this);
        if(obj->get_err()!="") goto END;
    }

    for(n=2; n<GH; n++)
    {
        obj = new TGenHdrN(this, n+2);
        if(obj->get_err()!="") goto END;
    }

    for(i=0; i<ST; i++)
    {
        for(n=0; n<CS; n++)
        {
            obj = new TCsD(this);
            if(obj->get_err()!="") goto END;
        }

        if(SK>0)
        {
            obj = new TSkew(this);
            if(obj->get_err()!="") goto END;
        }
    }

    if(EC>0)
    {
        obj = new TEcHdr(this);
        if(obj->get_err()!="") goto END;
    }

    if(EX>0)
    {
        obj = new TExHdr(this);
        if(obj->get_err()!="") goto END;
    }

    if(XX>0)
    {
        obj = new TXxHdr(this);
        if(obj->get_err()!="") goto END;
    }

    if(XF && csl->getCount()==1)
    {
        TCs* cs = get_csi(0);
       cs->nch = XF;
    }

    for(n=0; n<csl->getCount(); n++)
    {
        TCs* cs = get_csi(n);
        trc_cnt = trc_cnt + cs->nch;
    }

    hdr_len = 32 * ( ST*(CS+SK) + 1 + GH + EC + EX  );

    if(HeadersOnly) goto END;

    if(Si>0 && R_ms>0) TrcSmpNum = (int)(R_ms / Si)+1;
    else               TrcSmpNum = 0;
    if(Format/100) // demultiplexed
    {
        for(;;)
        {
            int tpc = tell();
            int tp  = find_trcx(tpc, FFFF, F); if(tp<0) tp=_len;

            if(tp!=tpc)
            {
                new TJunk(this,tp-tpc);
                seek(tp,SEEK_SET);
            }

            TTrInf ti = chck_trcx(tp, FFFF,F);
            TTrInf tn;

            if(ti.stat==0)
            {
                new TSegdEof(this, "End of file!");
                break;
            }
            if(ti.stat<0)
            {
                new TTr(this,ti);
                break;
            }

/*
            if(ti.chn==1 && ti.csn==17)
            {
                int x =1;
            }
*/
            lenok=0;

            //printf("stat=%d stn=%d csn=%d chn=%d\n", ti.stat, ti.stn,ti.csn,ti.chn); fflush(stdout);

            int  csidx = find_csi(ti.csn);
            TCs* Cs = get_csi(csidx);

            int Ns;
            int Nso  = (int)(Cs->tml / Si);
            int Nss = Cs->nsscan;
            int Hl  = 20 + 32*ti.the;
            int Ln  = 0;
            int Lj  = 0;

            if(Cs->ns>0)
            {
                Ln  = Cs->ln;
                Ns  = Cs->ns;
                tn = chck_trcx(tp+Ln, FFFF,F);
                if(tn.stat>=0) lenok=1;
            }

            if(!lenok && ti.exns>0)
            {
                //printf("exns=%d\n", ti.exns); fflush(stdout);
                Ns = ti.exns;
                Ln = Hl + Ns * SmplB;
                tn = chck_trcx(tp+Ln, FFFF,F);
                if(tn.stat>=0) lenok=1;
            }

            if(!lenok)
            {
                Ns = Nso * Nss + 1;
                Ln = Hl + Ns * SmplB;
                tn = chck_trcx(tp+Ln, FFFF,F);
                //printf("next1=%d stn=%d csn=%d chn=%d len=%d\n", tn.stat, tn.stn,tn.csn,tn.chn,Ln); fflush(stdout);
                if(tn.stat>=0) lenok=1;
            }

            if(!lenok)
            {
                Ns = Nso * Nss;
                Ln = Hl + Ns * SmplB;
                tn = chck_trcx(tp+Ln, FFFF,F);
                //printf("next1=%d stn=%d csn=%d chn=%d len=%d\n", tn.stat, tn.stn,tn.csn,tn.chn,Ln); fflush(stdout);
                if(tn.stat>=0) lenok=1;
            }

            if(!lenok)
            {
                Nss = 1;
                Ns  = Nso;
                Ln = Hl + Ns * SmplB;
                tn = chck_trcx(tp+Ln, FFFF,F);
                //printf("next2=%d stn=%d csn=%d chn=%d len=%d\n", tn.stat, tn.stn,tn.csn,tn.chn,Ln); fflush(stdout);
                if(tn.stat>=0) lenok=1;
            }

            if(!lenok)
            {
                Nss = 1;
                Ns  = Nso+1;
                Ln = Hl + (Ns+1) * SmplB;
                tn = chck_trcx(tp+Ln, FFFF,F);
                //printf("next2=%d stn=%d csn=%d chn=%d len=%d\n", tn.stat, tn.stn,tn.csn,tn.chn,Ln); fflush(stdout);
                if(tn.stat>=0) lenok=1;
            }

            if(!lenok)
            {
                // После текущей трассы не найдена следующая или конец файла
                // Пытаемся определить длину трассы
                // ориентируясь на конец файла

                Nss = Cs->nsscan;

                int ln = Hl + Nso * SmplB; // minimal possible length
                int pos = find_trcx(tp+ln, FFFF, F);

                if(pos<0) pos=_len;

                ln = pos-tp;

                if(ti.exns)
                {
                    Ns = ti.exns;
                }
                else if(ln >= Hl + Nso*Nss+1 * SmplB)
                {
                    Ns = Nso*Nss+1;
                }
                else if(ln >= Hl + Nso*Nss * SmplB)
                {
                    Ns = Nso*Nss;
                }
                else if(ln >= Hl + (Nso+1) * SmplB)
                {
                    Ns  = Nso+1;
                    Nss = 1;
                }
                else if(ln >= Hl + Nso * SmplB)
                {
                    Ns = Nso;
                    Nss = 1;
                }

                Ln = Hl + Ns * SmplB;

                if(ln>=Ln)
                {
                    //Lj = ln - Ln;
                    lenok=1;
                }

                //printf("fixl=%d junk=%d\n",Ln, Lj); fflush(stdout);
                //nextok=1;
            }

            //printf("this=%d stn=%d csn=%d chn=%d len=%d junk=%d\n", ti.stat, ti.stn,ti.csn,ti.chn,Ln,Lj); fflush(stdout);
            //printf("next=%d stn=%d csn=%d chn=%d len=%d junk=%d\n", tn.stat, tn.stn,tn.csn,tn.chn,Ln,Lj); fflush(stdout);

            if(lenok)
            {
                if(Cs->ns<1)
                {
                    Cs->ns     = Ns;
                    Cs->ln     = Ln;
                    Cs->nsscan = Nss;
                    Cs->si     = Si/(Nss);
                }

                TTr* tr = new TTr(this,ti);
                if(tr->get_err()!="")
                {
                    break;
                }

                if(Lj>0)
                {
                    seek(Ln+tp,SEEK_SET);
                    new TJunk(this,Lj);
                }

                seek(Ln+Lj+tp,SEEK_SET);
            }
            else
            {
                break;
            }
        }

        int csi;

        for(ntrs=csi=0; csi<csl->getCount(); csi++)
        {
            TCs* cs = get_csi(csi);
            ntrs += cs->nch;
        }

        if(atn<ntrs )
        {
            new TSegdWarn(this, "Unexpected end of traces!");
        }        

    }
    else // multiplexed
    {
        scnlen = 0;
        for(n=0; n<csl->getCount(); n++)
        {
            TCs* cs = get_csi(n);

            int nss = cs->nsscan;
            int nch = cs->nch;
            scnlen += nch*nss;
        }

        scnlen = (int) (scnlen * Smpl / 8.)+8;

        obj = new TMux(this);
        if(obj->get_err()!="") goto END;
    }

END:

    if(obj) _err = obj->get_err();
    else    _err = "Unknown data";

    if(_err!="" || tpos<0) return -1;

    if(Format!=200)
    {

        if(Si>0 && R_ms>0) TrcSmpNum = (int)(R_ms / Si)+1;
        else               TrcSmpNum = 0;

        int samples_len = (TrcSmpNum * Smpl) / 8;
        int trheadr_len = 20 + 32 * THE;
        int fheader_len = 32 * ( ST*(CS+SK) + 1 +
                                 GH + EC + EX + XX );

        int trace_len   = trheadr_len+samples_len;

        int trblock_len = trc_cnt * (trace_len);

        hdr_len=fheader_len;
        trc_len=trblock_len;
    }

    if(!keep_buf)
    {
      delete [] _buf; _buf=NULL;
    }

    return 1;
}
//---------------------------------------------------------------------------

int TSegDObj::xprintf(int out, const char *fmt,  ...)
{
    char buffer[1024];
    va_list argptr;
    int cnt;

    va_start(argptr, fmt);
    cnt = vsprintf(buffer, fmt, argptr);
    va_end(argptr);

    if(out==0) _inf = _inf + buffer;
    else       _err = _err + buffer;

    return cnt;
}

//---------------------------------------------------------------------------

void TSegDObj::displayMemory(int out, char *address, int length)
{
    char *ao = hdr;

    int i = 0;              //used to keep track of line lengths
    int j;


    for(i=0;i<length && i < 16000 ;i+=16)
    {
        xprintf(out,"%08d | ", address-ao+_pos+i);

        for(j=0;j<16;j++)
        {
            if(i+j<length)
                xprintf(out,"%02X ", (unsigned char)(address[i+j]));
            else
                xprintf(out,"   ");
        }

        xprintf(out," | ");

        for(j=0;j<16 && i+j<length;j++)
        {
            unsigned char c =  (unsigned char)(address[i+j]);

            if(c<33 || c>126) c=0x2e;

            xprintf(out,"%c",c);
        }

        xprintf(out,"\r\n");
    }

    if(i<length) xprintf(out,"...\r\n");
    xprintf(out,"\r\n");
}

//---------------------------------------------------------------------------

string  TGenHdr1::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    char   s[2048];

    if(_segd->Format==200)
    {
        xprintf(0,pritem("File Number                   F  ",0,hdr, 1, 4));
        xprintf(0,"SEG-B Header Block #1\r\r\n\r\r\n");
    }
    else
    {
        xprintf(0,"General Header Block #1\r\n\r\n");
    }
    displayMemory(0, hdr, len);

    if(_segd->Format==200) return _inf;

    xprintf(0,pritem("File Number                   F  ",0,hdr, 1, 4));
    xprintf(0,pritem("Format Code                   Y  ",0,hdr, 5, 4, 0, 0));
    xprintf(0,"[%s]\r\n", fmtnum2nam(_segd->Format));
    xprintf(0,pritem("General Constants             K  ",0,hdr, 9,12));
    xprintf(0,pritem("Year                          YR ",0,hdr,21, 2));
    xprintf(0,pritem("Num add bocks in  gen header  GH ",0,hdr,23, 1));
    xprintf(0,pritem("Julian day                    DY ",0,hdr,24, 3));
    xprintf(0,pritem("Hour of day                   H  ",0,hdr,27, 2));
    xprintf(0,pritem("Minute of hour                MI ",0,hdr,29, 2));
    xprintf(0,pritem("Second of minute              SE ",0,hdr,31, 2));

    xprintf(0,pritem("Manufacturer's code           Mc ",0,hdr,33, 2, 0, 0));
    xprintf(0,"[%s]\r\n", mcnum2nam(_segd->Mc));

    xprintf(0,pritem("Manufacturer's serial nb      Ms ",0,hdr,35, 4));
    xprintf(0,pritem("Bytes per scan                B  ",0,hdr,39, 6));
    xprintf(0,pritem("Base scan interval            I  ",1,hdr,45, 2, 0, 0));
    xprintf(0,"[%g ms]\r\n",_segd->Si);
    xprintf(0,pritem("Polarity                      P  ",0,hdr,47, 1, 0, 0));
    xprintf(0,"[%s]\r\n", polnum2nam(_segd->P));
    xprintf(0,pritem("S/B exponent                  SB ",1,hdr,48, 3));
    xprintf(0,pritem("Record type                   Z  ",0,hdr,51, 1));
    xprintf(0,pritem("Record length                 R  ",0,hdr,52, 3, 0, 0));
    hexs(hdr, 52, 3,s);
    if(_segd->R<0) xprintf(0,"\r\n");
    else    xprintf(0,"[%g ms]\r\n",_segd->R_ms);

    xprintf(0,pritem("Scan types per record         STR",0,hdr,55, 2));
    xprintf(0,pritem("# of chan sets per scan type  CS ",0,hdr,57, 2));
    xprintf(0,pritem("# of sample skew 32 byte ext  SK ",0,hdr,59, 2));
    xprintf(0,pritem("Extended header length        EC ",0,hdr,61, 2));
    xprintf(0,pritem("External header length        EX ",0,hdr,63, 2));

    return _inf;
}



TGenHdr1::TGenHdr1(TSegD* segd) : TSegDObj(segd)
{
    int       v;
    char      s[2048];

    name = "General #1";

    char* dat = _segd->read(32);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

    hexs(dat, 1, 4,s);
    if(strcmp(s,"FFFF")==0) _segd->F=-1;
    else                    _segd->F=atoi(s);

    memcpy(&(_segd->FFFF),dat,2);

    _segd->Format = bcd_int_item(dat,5,4);

    _segd->Smpl  = 0;

    if(_segd->Format==8015) _segd->Smpl=20;
    if(_segd->Format==8022) _segd->Smpl=8;
    if(_segd->Format==8024) _segd->Smpl=16;
    if(_segd->Format==8036) _segd->Smpl=24;
    if(_segd->Format==8038) _segd->Smpl=32;
    if(_segd->Format==8042) _segd->Smpl=2;
    if(_segd->Format==8044) _segd->Smpl=16;
    if(_segd->Format==8048) _segd->Smpl=32;
    if(_segd->Format==8058) _segd->Smpl=32;

    if(_segd->Format==  15)  segd->Smpl=20;
    if(_segd->Format==  22) _segd->Smpl=8;
    if(_segd->Format==  24) _segd->Smpl=16;
    if(_segd->Format==  36) _segd->Smpl=24;
    if(_segd->Format==  38) _segd->Smpl=32;
    if(_segd->Format==  42) _segd->Smpl=2;
    if(_segd->Format==  44) _segd->Smpl=16;
    if(_segd->Format==  48) _segd->Smpl=32;
    if(_segd->Format==  58) _segd->Smpl=32;

    _segd->SmplB = _segd->Smpl/8.;

    if(_segd->Format==200)
    {
        _segd->Type=2;
        return;
    }

    if(_segd->Smpl==0)
    {
        hexs(dat, 5, 4,s);
        xprintf(1, "Unknown format %s\r\n", s);
        return;
    }

    _segd->Type=3;

    bcd_str_item(dat, 9, 12, _segd->K);

    _segd->GH=bcd_int_item(dat, 23, 1);

    _segd->Mc=bcd_int_item(dat, 33, 2);
    _segd->Ms=bcd_int_item(dat, 35, 4);

    v=uns_item(dat, 45, 2);
    _segd->Si=v/16.;

    _segd->P=bcd_int_item(dat,47, 1);

    hexs(dat, 52, 3,s);
    if(strcmp(s,"FFF")==0)
    {
        _segd->R=-1;
    }
    else
    {
        _segd->R=atoi(s);
        _segd->R_ms = _segd->R*0.1024*1000;
    }

    hexs(dat, 55, 2,s);
    _segd->ST=atoi(s);

    hexs(dat, 57, 2,s);
    if(strcmp(s,"FF")==0) _segd->CS=-1;
    else                  _segd->CS=atoi(s);

    hexs(dat, 59, 2,s);
    _segd->SK=atoi(s);

    hexs(dat, 61, 2,s);
    if(strcmp(s,"FF")==0) _segd->EC=-1;
    else                  _segd->EC=atoi(s);

    hexs(dat, 63, 2,s);
    if(strcmp(s,"FF")==0) _segd->EX=-1;
    else                  _segd->EX=atoi(s);

    set_len(32); memcpy(hdr,dat,32);

    type=1;
}

string  TGenHdr2::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"General Header Block #2\r\n\r\n");

    displayMemory(0, hdr, len);

    xprintf(0,pritem("Extended file number           ", 1,hdr, 1, 6));
    xprintf(0,pritem("Extended Ch Sets/Scan Types n/u", 1,hdr, 7, 4));
    xprintf(0,pritem("Extended hdr blocks         n/u", 1,hdr,11, 4));
    xprintf(0,pritem("External header blocks         ", 1,hdr,15, 4));
    xprintf(0,pritem("Not used                       ", 0,hdr,19, 2));
    xprintf(0,pritem("SEG-D Revision Number          ", 1,hdr,21, 2));
    xprintf(0,pritem(".                              ", 1,hdr,23, 2));
    xprintf(0,pritem("Num Blocks of General Trailer  ", 0,hdr,25, 4));
    xprintf(0,pritem("Extended Record Length ms      ", 1,hdr,29, 6));
    xprintf(0,pritem("Not used                       ", 0,hdr,35, 2));
    xprintf(0,pritem("General Header Block Number    ", 1,hdr,37, 2));
    xprintf(0,pritem("Not used                       ", 0,hdr,39,13));

    return _inf;
}


//---------------------------------------------------------------------------


TGenHdr2::TGenHdr2(TSegD* segd) : TSegDObj(segd)
{
    int CS;
    int EX;
    int R;
    int F;

    int  v;
    char   s[2048];

    name = "General #2";

    char* dat = _segd->read(32);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

    v=hex2uint(hexs(dat, 1,  6,s)); F =v;
    v=hex2uint(hexs(dat, 7,  4,s)); CS=v;
    v=hex2uint(hexs(dat, 11, 4,s)); EC=v;
    v=hex2uint(hexs(dat, 15, 4,s)); EX=v;
    v=hex2uint(hexs(dat, 29, 6,s)); R=v;

    v=hex2uint(hexs(dat, 21, 4,s)); _segd->Rev=v;

    if(_segd->F <0)    _segd->F    = F;
    if(_segd->CS<0)    _segd->CS   = CS;
    if(_segd->EC<0)    _segd->EC   = EC;
    if(_segd->EX<0)    _segd->EX   = EX;
    if(_segd->R_ms<0)  _segd->R_ms = R;

    set_len(32); memcpy(hdr,dat,32);
    type=1;
}


string  TGenHdr3::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0, "General Header Block #3\r\n\r\n");

    displayMemory(0, hdr, len);

    xprintf(0,pritem("Expanded file number           ", 1,hdr, 1, 6));
    xprintf(0,pritem("Source Line Number             ", 1,hdr, 7, 6));
    xprintf(0,pritem(".                              ", 1,hdr,13, 2));
    xprintf(0,pritem("Source Point Number            ", 1,hdr,17, 6));
    xprintf(0,pritem(".                              ", 1,hdr,23, 2));
    xprintf(0,pritem("Source Point Index             ", 1,hdr,27, 2));
    xprintf(0,pritem("Phase Control (not recorded)   ", 1,hdr,29, 2));
    xprintf(0,pritem("Vibrator Type (not recorded)   ", 1,hdr,31, 2));
    xprintf(0,pritem("Phase angle (not recorded)     ", 1,hdr,33, 4));
    xprintf(0,pritem("General Header Block Number    ", 1,hdr,37, 2));
    xprintf(0,pritem("Source Set Number              ", 1,hdr,39, 2));
    xprintf(0,pritem("Not used                       ", 0,hdr,42,12));

    return _inf;
}


TGenHdr3::TGenHdr3(TSegD* segd) : TSegDObj(segd)
{
    name = "General #3";

    char* dat = _segd->read(32);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

    _segd->SP = int_item(dat, 17, 6);

    set_len(32); memcpy(hdr,dat,32);
    type=1;
}



string  TGenHdrB::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"SEG-B Header Block #1\r\r\n\r\r\n");

    displayMemory(0, hdr, len);

    xprintf(0,pritem("File Number                   F  ",0,hdr, 1, 4));
    xprintf(0,pritem("Format Code                   Y  ",0,hdr, 5, 4, 0, 0));
    xprintf(0,"[%s]\r\n", "SEG-B 20 bit binary multiplexed");
    xprintf(0,pritem("General Constants             K  ",0,hdr, 9,12));

    int B = bcd_int_item(hdr, 21, 3);
    xprintf(0,pritem("# Bytes per Data Scan         B  ",0,hdr,21, 3));

    xprintf(0,pritem("# Sample interval (ms)        B  ",0,hdr,24, 1));
    xprintf(0,pritem("Manufacturer's code           Mc ",0,hdr,25, 2, 0, 0));
    xprintf(0,"[%s]\r\n", mcnum2nam(_segd->Mc));
    xprintf(0,pritem("Manufacturer's serial nb      Ms ",0,hdr,27, 6));
    xprintf(0,pritem("Record length (s)             R  ",0,hdr,33, 2));

    //int J = bcd_int_item(hdr, 35, 1);
    xprintf(0,pritem("Amplifier gain control mode   J  ",0,hdr,35, 1));

    int Z = bcd_int_item(hdr, 36, 1);
    xprintf(0,pritem("Type of record                Z  ",0,hdr,36, 1, 0, 0));

    if      (Z==8) xprintf(0,"[%s]\r\n", "shot");
    else if (Z==4) xprintf(0,"[%s]\r\n", "shot bridle");
    else if (Z==2) xprintf(0,"[%s]\r\n", "test");
    else if (Z==1) xprintf(0,"[%s]\r\n", "other");
    else           xprintf(0,"[%s]\r\n", "unknown");

    xprintf(0,pritem("Low cut filter setting        Lc ",0,hdr,37, 2));
    xprintf(0,pritem("Low cut filter slope          Ls ",0,hdr,39, 1));
    xprintf(0,pritem("High cut filter setting       Hc ",0,hdr,41, 3));
    xprintf(0,pritem("High cut filter slope         Hs ",0,hdr,44, 1));
    xprintf(0,pritem("Special filter setting        S  ",0,hdr,45, 2));
    xprintf(0,pritem("Alias filter setting          A  ",0,hdr,47, 1));

    int NAUX = 5;
    int ntr = ((B - 4) / 2 - NAUX) / NAUX * 4;

    xprintf(0,"\r\n");

    int n;

    const char* chnam[8] =
    {
        "Unused channel",          //0
        "Seismic channel",         //1
        "Time-break channel",      //2
        "Uphole channel",          //3
        "Water-break channel",     //4
        "Time counter",            //5
        "Gain channel",            //6
        "Other"                    //7;
    };

    int ntrfix = (_segd->hdr_len-24) / 2 - 5;

    if(ntrfix < ntr) ntr=ntrfix;

    for(n=0;n<ntr+5;n++)
    {
        char           s[1024];
        unsigned char* h = (unsigned char*) hdr;
        int nb = 24+n*2;
        unsigned char b1 = h[nb  ];
//      unsigned char b2 = h[nb+1];

        int ty = b1>>5;

//      int g1 = b1&0x1f;

        if(n<ntr) sprintf(s,"Seis channel #%2d Type / Gain     ",n+1);
        else      sprintf(s,"Aux  channel #%2d Type / Gain     ",n+1-ntr);
        xprintf(0,pritem (s,0,hdr,nb*2+1,4,0,0),ntr);
        if(ty>=0 && ty<=7) xprintf(0," [%s]",chnam[ty]);

        xprintf(0,"\r\n");

    }
    return _inf;
}


TGenHdrB::TGenHdrB(TSegD* segd) : TSegDObj(segd)
{
    char      s[2048];

    segd->seek(0,SEEK_SET);

    name = "SEG-B Header";

    char* dat = _segd->read(32);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

    hexs(dat, 1, 4,s);
    if(strcmp(s,"FFFF")==0) _segd->F=-1;
    else                    _segd->F=atoi(s);

    memcpy(&(_segd->FFFF),dat,2);

    _segd->Format = bcd_int_item(dat,5,4);

    _segd->Smpl = 0;

    if(_segd->Format==200)
    {
        _segd->Type=2;
        _segd->Smpl=20;
    }

    if(_segd->Smpl==0)
    {
        hexs(dat, 5, 4,s);
        xprintf(1, "Unknown format %s\r\n", s);
        return;
    }

    _segd->SmplB=_segd->Smpl/8.;

    bcd_str_item(dat, 9, 12, _segd->K);

    _segd->GH=0;

    _segd->Mc=bcd_int_item(dat, 25, 2);
    _segd->Ms=bcd_int_item(dat, 27, 4);

    _segd->Si=bcd_int_item(dat, 24, 1);

    _segd->R=bcd_int_item(dat, 33, 2);
    _segd->R_ms=_segd->R*1000;

    _segd->CS=2;

    int B = bcd_int_item(dat, 21, 3); // # Bytes per scan

//  B=314;

    _segd->B=B;

    NAUX = 5;

    NTRS = ((B - 4) / 2 - NAUX) / NAUX * 4;

    int HDRL = NTRS * 2 + NAUX * 2 + 10 + 16;

    _segd->seek(0,SEEK_SET);
    dat = _segd->read(HDRL);

    set_len(HDRL); memcpy(hdr,dat,HDRL);

    // Check Real Header Len --------------

    int i;

    for(i=24;i<HDRL;i+=2)
    {
        int b1 = hdr[i];
        int b2 = hdr[i+1];
        if(b1==-1 && b2==-1) break;
 //     if(b1==0x03 && b2==0x03) break;
    }

    // ------------------------------------

    //printf("HDRL=%d i=%d\n", HDRL, i); fflush(stdout);

    if(i<HDRL)
    {
        HDRL=i;
        _segd->seek(0,SEEK_SET);
        dat = _segd->read(HDRL);
    }

    _segd->hdr_len=HDRL;

    int DTL = _segd->getlen()-_segd->hdr_len;
    int NS  = DTL/B;

    int NSo = _segd->R_ms/_segd->Si;

    if(NS>NSo) NS=NSo;

//    NS=180;

    _segd->TrcSmpNum=NS;

    type=1;
}


string  TGenHdrN::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0, "General Header Block #%d\r\n",ghn);

    displayMemory(0, hdr, len);

    return _inf;
}


TGenHdrN::TGenHdrN(TSegD* segd, int num) : TSegDObj(segd)
{
//    long long pos;
    int  NH;
    char s[1024];

    char* dat = _segd->read(32);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

    name = "General #?";

    NH = int_item(dat, 37, 2);

    ghn = NH; if(NH<1) NH=num;

    sprintf(s,"General #%d",NH);
    name = s;

    set_len(32); memcpy(hdr,dat,32);
    type=1;
}
//---------------------------------------------------------------------------

TCs*  TCsList::get_cs(int csn)
{
    TCs* cs;
    int  n;

    for(n=0; n<getCount(); n++)
    {
        cs = (TCs*)getObj(n);
        if(cs->csn==csn) return cs;
    }

    return NULL;
}

TCs*  TCsList::get_csi(int idx)
{
    return (TCs*)getObj(idx);
}

TCs*  TCsList::find(int stn, int csn)
{
    TCs* cs;
    int  n;

    for(n=0; n<getCount(); n++)
    {
        cs = (TCs*)getObj(n);
        if(cs->csn==csn && cs->stn==stn) return cs;
    }

    return NULL;
}


string  TCsB::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"%s\r\n\r\n", name.c_str());

    int ntr = get_ntr();

    xprintf(0,"%d traces", ntr);

    return _inf;
}

TCsB::TCsB(TSegD* segd, int no, int ntr) : TCs(segd)
{
    if(no==0)
    {
        name = "Aux  Chan Set";
        strcpy(typnm,"aux");
    }
    else
    {
        name = "Seis Chan Set";
        strcpy(typnm,"seis");
    }

    csn=no;
    stn=0;

    nch=ntr;
    to = 0;
    tn = _segd->TrcSmpNum*_segd->Si/2;

    typn = no;
    type=2;
}


string  TCsD::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"%s\r\n\r\n", name.c_str());

    displayMemory(0, hdr, len);

    xprintf(0,pritem("Scan Type #                    ", 0,hdr, 1, 2));
    xprintf(0,pritem("Channel Set Number             ", 0,hdr, 3, 2));
    xprintf(0,pritem("Channel Set Starting Time (2ms)", 1,hdr, 5, 4));
    xprintf(0,       "                                  %d ms\r\n", to*2);
    xprintf(0,pritem("Channel Set End Time           ", 1,hdr, 9, 4));
    xprintf(0,       "                                  %d ms\r\n", tn*2);
//  xprintf(0,pritem("Descale Multiplier             ", 2,hdr,13, 4));
    xprintf(0,"Descale Multiplier                %g\r\n", mp);
    xprintf(0,pritem("Num of channnels in this set   ", 0,hdr,17, 4));
    xprintf(0,pritem("Channel Type Identification    ", 0,hdr,21, 1));
    xprintf(0,"                                  %s\r\n", typnm);

    xprintf(0,pritem("Not used                       ", 0,hdr,22, 1));
    xprintf(0,pritem("Number of samples exponent     ", 0,hdr,23, 1));
    xprintf(0,"                                : %d subscan(s)\r\n", nsscan);
    xprintf(0,pritem("Channel gain control method    ", 0,hdr,24, 1));
    xprintf(0,pritem("Alias filter freq at - 3dB     ", 0,hdr,25, 1));
    xprintf(0,pritem("Alias filter slope             ", 0,hdr,29, 4));
    xprintf(0,pritem("Low-cut filter frequency       ", 0,hdr,33, 4));
    xprintf(0,pritem("First Notch Frequency          ", 0,hdr,41, 4));
    xprintf(0,pritem("Second Notch Frequency         ", 0,hdr,45, 4));
    xprintf(0,pritem("Third Notch Frequency          ", 0,hdr,49, 4));
    xprintf(0,pritem("Extended channel set number    ", 0,hdr,53, 4));
    xprintf(0,pritem("Extended header flag           ", 0,hdr,57, 1));
    xprintf(0,pritem("Trace Header Extensions        ", 1,hdr,58, 1));
    xprintf(0,pritem("Vertical Stack                 ", 1,hdr,59, 2));
    xprintf(0,pritem("Streamer cable number          ", 0,hdr,62, 2));
    xprintf(0,pritem("Array forming                  ", 0,hdr,63, 1));

    return _inf;
}



TCsD::TCsD(TSegD* segd) : TCs(segd)
{
    mp = 0;

    char* dat = _segd->read(32);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

    char s[2048];

    hexs(dat, 3, 2,s);
    if(strcmp(s,"FF")==0) csn=-1;
    else                  csn=atoi(s);

    hexs(dat, 1, 2,s);
    stn=atoi(s);
    hexs(dat,17, 4,s);
    nch=atoi(s);

    to = hex2uint(hexs(dat, 5, 4,s));
    tn = hex2uint(hexs(dat, 9, 4,s));

    //if(to==tn) nch=0;

    hexs(dat, 23, 1,s);
    csse=atoi(s);

    if(csse<1) nsscan=1;
    else       nsscan=1<<csse;

    nsso = nsscan;

    if(csn<=0)
    {
        hexs(dat, 53, 4,s);
        csn=atoi(s);
    }

    sprintf(s,"Chan Set # %d:%d",stn, csn);
    name = s;

    hexs(dat, 21, 1,s);
    typn = atoi(s);
    strcpy(typnm,typnum2nam(typn));
    tml=(tn-to)*2; // *(1<<csse);

    segd->R_ms=tml;

    unsigned char imp0 = dat[6];
    unsigned char imp1 = dat[7];

    int z   = imp1 & 0x80;
    if(z) z =  -1;
    else  z =   1;

    int m   = (imp1 & 0x7F)<<8 | imp0;

    mp = z * m / 1024.;

    set_len(32); memcpy(hdr,dat,32);
    type=2;
}


TCs::TCs(TSegD* segd) : TSegDObj(segd)
{
    _trc = new TTrList();
    name = "Chan Set #?";
    nsscan   =  1;
    nsso     =  1;
    ns       = -1;
    ln       =  0;
    lj       =  0;

    _segd->csl->Add(this);
}

TCs::~TCs()
{
    delete _trc;
}

//---------------------------------------------------------------------------

string  TTr::get_inf()
{
    if(_inf!="") return _inf;

    int  n;

    char* hed = hdr;

    xprintf(0,"%s - (%d) - len = %d bytes\r\n", name.c_str(), atn, fln);

    if(junk)
        xprintf(0,"%d junk bytes before trace\r\n",junk);
    else
        xprintf(0,"\r\n");

    if(hdr==NULL)
    {
        xprintf(0, "No data are available");
        return _inf;
    }

    if(_err!="") xprintf(0, "Error: %s\r\n", _err.c_str());

    xprintf(0,"Trace Header\r\n\r\n");
    displayMemory(0, hed, 20);

    xprintf(0,pritem("File Number                    ", 0,hed, 1, 4));
    xprintf(0,pritem("Scan Type Number               ", 0,hed, 5, 2));
    xprintf(0,pritem("Channel Set Number             ", 0,hed, 7, 2));
    xprintf(0,pritem("Trace Number                   ", 0,hed, 9, 4));
    xprintf(0,pritem("First Timing Word              ", 0,hed,13, 6));
    xprintf(0,pritem("Trace Header Extensions        ", 1,hed,19, 2));
    xprintf(0,pritem("Sample Skew                    ", 1,hed,21, 2));
    xprintf(0,pritem("Trace edit                     ", 1,hed,23, 2));
    xprintf(0,pritem("Time break window              ", 1,hed,25, 6));
    xprintf(0,pritem("Extended Channel Set Number    ", 1,hed,33,2));
    xprintf(0,pritem("Extended File Number           ", 1,hed,35,6));

    hed += 20;

    for(n=0; n<the; n++)
    {
        xprintf(0,"\r\nExtended Trace Header # %d\r\n\r\n",n+1);
        if(n==0)
        {
            displayMemory(0, hed, 32);
            xprintf(0,pritem("Receiver Line Number         ", 2,hed, 1, 6));
            xprintf(0,pritem("Receiver Point Number        ", 2,hed, 7, 6));
            xprintf(0,pritem("Rec Point Index              ", 2,hed,13, 2));
            xprintf(0,pritem("Ext # of Samples per Trace   ", 2,hed,15, 6));
            xprintf(0,pritem("Ext Receiver Line Number     ",12,hed,21,10, -16));
            xprintf(0,pritem("Ext Receiver Point Number    ",12,hed,31,10, -16));

            xprintf(0,pritem("Sensor Type                  ", 2,hed,41, 2));

        }
        else
        {
            displayMemory(0, hed, 32);
        }
        hed += 32;
    }

    xprintf(0,"\r\n...\r\n");

    return _inf;
}



TTr::TTr(TSegD* segd,  TTrInf ti) : TSegDObj(segd)
{
    char  s[2048];
    int   n;

    name = "Trace ??:??";
    smps=NULL;

    exns = 0;
    junk = 0;

    segd->seek(ti.pos,SEEK_SET);

    char* dat = _segd->read(20);
    char* thdr = dat;

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

    len=0; len=len+20;

    hexs(dat, 9, 4,s);
    tn=atoi(s);

    the=hex2uint(hexs(dat, 19, 2,s));
    skk=hex2uint(hexs(dat, 21, 2,s));

    hexs(dat, 5, 2,s);
    stn=atoi(s);

    hexs(dat, 7, 2,s);
    if(s[0]==0 || strcmp(s,"FF")) csn=atoi(s);
    else             csn=hex2uint(hexs(dat, 31, 4,s));

    sprintf(s,"Trace %02d:%02d",csn,tn);
    name = s;

    int  csidx = segd->find_csi(csn);

    TCs* Cs = segd->get_csi(csidx);
    csi = csidx;

    ns  = 0;

//  Extended Trace Headers

    for(n=0; n<the; n++)
    {
        dat = segd->read(32);
        if(!dat)
        {
            xprintf(1, "%s is too short\r\n", name.c_str());
            return;
        }

        len+=32;

        if(n==0)
        {
            exns = int_item(dat,15, 6);
        }
    }


    _segd->THE=the;
    _segd->atn++; atn=segd->atn;

    char* smpdat = NULL;

    if(Cs)
    {
        Cs->get_trc()->Add(this);
        ns  = Cs->ns;
        fln = Cs->ln;

        int sampleslen = (int) ((ns * _segd->Smpl) / 8.);
        if(sampleslen>0)
        {
            smpdat = segd->read(sampleslen);
          //len = len + sampleslen;

            if(!smpdat)
            {
                xprintf(1, "%s is too short\r\n", name.c_str());
                return;
            }
        }
    }
    else
    {
        xprintf(1,"Unknown chan set: %d\r\n", csn);
    }

    set_len(len); memcpy(hdr,thdr,len);
    type=3;

    if(ns>0)
    {
        smps = new float[ns+4];
        memset(smps,0,ns*sizeof(float));
        cnv_smps(smpdat, smps);
    }
}

TTr::TTr(TSegD *segd, int nsmp,  int chn, int ci) : TSegDObj()
{
    _segd = segd;

    csi = ci;

    TCs* cs = _segd->get_csi(csi);
    exns = 0;
    junk = 0;

    tn   = chn;
    csn=cs->csn;
    stn=cs->stn;

    name = "Demux Trace";

    if(nsmp<1) smps = NULL;
    else       smps = new float[nsmp];

    ns=nsmp;
    type=3;
}

TTr::~TTr()
{
    if(smps) delete [] smps;
}


void  TTr::get_smps(float* osmp)
{
    if(!smps) return;

    float* fsmps = (float*)smps;

    memcpy(osmp,fsmps,ns*sizeof(float));
}

int  TTr::cnv_smps(char* ismp, float* osmp)
{
    TCs* cs = _segd->get_csi(csi);

    float fmp = pow(2,cs->mp);

    if(_segd->Format==8058)
    {
        F8058_to_float(ismp, osmp, ns);
        mult_fa(fmp, osmp, ns);
    }

    else if(_segd->Format==8048)
    {
        F8048_to_float(ismp, osmp, ns);
        mult_fa(fmp, osmp, ns);
    }

    else if(_segd->Format==8036)
    {
        F8036_to_float(ismp, osmp, ns);
        mult_fa(fmp, osmp, ns);
    }

    else if(_segd->Format==8038)
    {
        F8038_to_float(ismp, osmp, ns);
        mult_fa(fmp, osmp, ns);
    }

    else if(_segd->Format==8015)
    {
        F8015_to_float(ismp, osmp, ns);
        mult_fa(fmp, osmp, ns);
    }
    else
    {
        return 0;
    }

    return ns;
}



//---------------------------------------------------------------------------


string  TEcHdr::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"Extended Header Block len = %d bytes\r\n\r\n", len);

    displayMemory(0, hdr, len);

    char   s[2048];

    if(_segd->Mc==20 && _segd->Rev==0x0106)
    {
        xprintf(0,"FairField Rev 0x0106 Extended Header\r\n\r\n");
        xprintf(0,"Number of record in file     %d\r\n", _segd->XF);
    }

    if(_segd->dev_type == 2 || _segd->Mc==34)
    {
        xprintf(0,"Syntrak Extended Header\r\n\r\n");

        hexs(hdr, 3, 1,s);
        xprintf(0,"Tape Unit                    %s\r\n", s            );

        hexs(hdr, 4, 1,s);
        xprintf(0,"Buffer                       %s\r\n", s            );

        xprintf(0,"\r\n");

        xprintf(0,"Line Name                    %s\r\n", _segd->LineNm.c_str());
        xprintf(0,"Reel Number                  %d\r\n", _segd->ReelNo        );
        xprintf(0,"Client                       %s\r\n", _segd->Client.c_str());
        xprintf(0,"Contractor                   %s\r\n", _segd->Contra.c_str());
        xprintf(0,"Extra 32 byte blocks         %d\r\n", _segd->XX            );
    }

    if(_segd->Mc == 13 && len>=1024 && is_month(hdr+563)) //Seal
    {
        xprintf(0,"Seal Extended Header\r\n\r\n");
        xprintf(0,pritem("Acquisition length             ", 1,hdr,   1, 8));
        xprintf(0,pritem("Sample rate                    ", 1,hdr,   9, 8));
        xprintf(0,pritem("Total number of traces         ", 1,hdr,  17, 8));
        xprintf(0,pritem("Number of Auxes                ", 1,hdr,  25, 8));
        xprintf(0,pritem("Number of Seis traces          ", 1,hdr,  33, 8));
        xprintf(0,pritem("Number of dead Seis traces     ", 1,hdr,  41, 8));
        xprintf(0,pritem("Number of live Seis traces     ", 1,hdr,  49, 8));
        xprintf(0,pritem("Type of source                 ", 1,hdr,  57, 8));
        xprintf(0,pritem("Number of samples in trace     ", 1,hdr,  65, 8));
        xprintf(0,pritem("Shot number                    ", 1,hdr,  75, 8));
        xprintf(0,pritem("TB window                      ", 3,hdr,  80, 8));
        xprintf(0,pritem("Test record type               ", 1,hdr,  89, 8));
        xprintf(0,pritem("Swath first line               ", 1,hdr,  97, 8));
        xprintf(0,pritem("Swath first number             ", 1,hdr, 105, 8));
        xprintf(0,pritem("Spread number                  ", 1,hdr, 113, 8));
        xprintf(0,pritem("Spread type                    ", 1,hdr, 121, 8));
        xprintf(0,pritem("Timebreak                      ", 1,hdr, 129, 8));
        xprintf(0,pritem("Uphole time                    ", 1,hdr, 137, 8));
        xprintf(0,pritem("Blaster id                     ", 1,hdr, 145, 8));
        xprintf(0,pritem("Blaster status                 ", 1,hdr, 153, 8));
        xprintf(0,pritem("Refraction delay               ", 1,hdr, 163, 8));
        xprintf(0,pritem("Tb to T0 time                  ", 3,hdr, 169, 8));
        xprintf(0,pritem("Internal time break            ", 1,hdr, 177, 8));
        xprintf(0,pritem("Prestack within field units    ", 1,hdr, 185, 8));
        xprintf(0,pritem("Noise elimination type         ", 1,hdr, 193, 8));
        xprintf(0,pritem("Low trace percentage           ", 1,hdr, 201, 8));
        xprintf(0,pritem("Low trace value                ", 1,hdr, 209, 8));
        xprintf(0,pritem("Number of windows              ", 1,hdr, 217, 8));
        xprintf(0,pritem("Historic editing type          ", 1,hdr, 225, 8));
        xprintf(0,pritem("Noisy trace percentage         ", 1,hdr, 233, 8));
        xprintf(0,pritem("Historic range                 ", 1,hdr, 241, 8));
        xprintf(0,pritem("Threshold Hold/Var             ", 1,hdr, 257, 8));
        xprintf(0,pritem("Historic threshold Init value  ", 1,hdr, 265, 8));
        xprintf(0,pritem("Historic zeroing length        ", 1,hdr, 273, 8));
        xprintf(0,pritem("Type of process                ", 1,hdr, 281, 8));
        xprintf(0,pritem("Acquisition type table #1      ", 1,hdr, 289, 8));
        xprintf(0,pritem("Threshold type table   #1      ", 1,hdr, 545, 8));
        xprintf(0,pritem("Stacking fold                  ", 1,hdr, 801, 8));
        xprintf(0,pritem("Aux correlation description    ", 5,hdr, 809,160));
        xprintf(0,pritem("Investigation length           ", 1,hdr, 869, 8));
        xprintf(0,pritem("Autocorrelation peak time      ", 1,hdr, 877, 8));
        xprintf(0,pritem("FFT exponent                   ", 1,hdr, 885, 8));
        xprintf(0,pritem("Source aux nb                  ", 1,hdr, 893, 8));
        xprintf(0,pritem("Pilot length                   ", 1,hdr,1001, 8));
        xprintf(0,pritem("Sweep length                   ", 1,hdr,1009, 8));
        xprintf(0,pritem("Acquisition number             ", 1,hdr,1017, 8));
        xprintf(0,pritem("Max of max, Aux                ", 3,hdr,1025, 8));
        xprintf(0,pritem("Max of max, Seis               ", 3,hdr,1033, 8));
        xprintf(0,pritem("Dump stacking fold             ", 1,hdr,1041, 8));
        xprintf(0,pritem("Tape label                     ", 5,hdr,1049,32));
        xprintf(0,pritem("Tape number                    ", 1,hdr,1081, 8));
        xprintf(0,pritem("Software version               ", 5,hdr,1089,32));
        xprintf(0,pritem("Date                           ", 5,hdr,1121,24));
        xprintf(0,pritem("Source easting                 ", 4,hdr,1145,16));
        xprintf(0,pritem("Source northing                ", 1,hdr,1161,16));
        xprintf(0,pritem("Source elevation               ", 3,hdr,1177, 8));
        xprintf(0,pritem("Slip sweep mode used           ", 1,hdr,1185, 8));
        xprintf(0,pritem("Files per tape                 ", 1,hdr,1193, 8));
        xprintf(0,pritem("File count                     ", 1,hdr,1201, 8));
        xprintf(0,pritem("Acquisition error description  ", 5,hdr,1209,160));
        xprintf(0,pritem("Filter type                    ", 1,hdr,1529, 8));
        xprintf(0,pritem("Profile number                 ", 1,hdr,1537, 8));
        xprintf(0,pritem("Not used                       ", 1,hdr,1545, 2));
        xprintf(0,pritem("T0 mode                        ", 1,hdr,1561, 8));
        xprintf(0,pritem("Source Line Name               ", 5,hdr,1569,32));
        xprintf(0,pritem("Sequence Number                ", 1,hdr,1601, 8));
        xprintf(0,pritem("Nav. status                    ", 1,hdr,1609, 8));
        xprintf(0,pritem("Shots to go                    ", 1,hdr,1617, 8));
        xprintf(0,pritem("Shots before on line           ", 1,hdr,1625, 8));
        xprintf(0,pritem("Total num. of editions         ", 1,hdr,1633, 8));
        xprintf(0,pritem("Aux digital low-cut filter     ", 3,hdr,1641, 8));
        xprintf(0,pritem("Seis digital low-cut filter    ", 3,hdr,1649, 8));
        xprintf(0,pritem("-3dB seis low-cut filter       ", 3,hdr,1657, 8));
        xprintf(0,pritem("Num. of chars from nav         ", 1,hdr,1665, 8));
        xprintf(0,0,"Other Not used\r\n");
    }

    char* ss;
    ss=strmem((char*)"$1",    (char*)hdr,len);

    if(ss)
    {
        int p = ss-hdr;
        if(p+36<len && memcmp(ss+33,"UTC",3)==0) DumpNavData(ss);
    }

    ss=strmem((char*)"*GCS90",(char*)hdr,len);
    if(ss) DumpGunData(ss);

    xprintf(0,"--------------------------------\r\n\r\n");

    return _inf;
}

TEcHdr::TEcHdr(TSegD* segd) : TSegDObj(segd)
{
    int   n;
    char  s[2048];
    char* hed;
    int   EC = _segd->EC;

    int    l = _segd->EC*32;

    name = "Extended Hdr";

    char* dat = segd->read(32*EC);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

    if(segd->dev_type == 2 || segd->Mc==34)
    {
        hed = dat;
        for(n=0; n<EC; n++)
        {
            if(n==0)
            {
                if(segd->Mc==34)
                {
                    hexs(dat, 59, 4,s);
                    segd->XX=atoi(s);
                }
            }
            else if(n==1)
            {
                strncpy1(s, hed, 8);
                trim(s);
                segd->LineNm=s;
            }
            if(n==2)
            {
                strncpy1(s, hed+16, 16);
                segd->ReelNo = atoi(s);
            }
            else if(n==3)
            {
                strncpy1(s, hed,    16);
                trim(s);
                segd->Client=s;
                strncpy1(s, hed+16, 16);
                trim(s);
                segd->Contra=s;
            }
            hed+=32;
        }
    }
    else if(segd->Mc==20 && EC>1 && segd->Rev==0x0106)
    {
        char   s[2048];
        int    v = hex2uint(hexs(dat, 101, 4,s));

        segd->XF=v;
    }

    set_len(l); memcpy(hdr,dat,l);
    type=1;    
}

//---------------------------------------------------------------------------

void  TSegDObj::DumpNavData(char* s)
{
    char  v[1024];
    int   l;
    int   rev;

    xprintf(0,"Navigation (Spectra) Data - ACSII\r\n");
    strncpy1(v,s, 2);
    xprintf(0,"Header ID           : %s\r\n", v);
    s+= 2;
    strncpy1(v,s, 4);
    xprintf(0,"Header length       : %s\r\n", v);
    s+= 4;

    strncpy1(v,s, 4);
    xprintf(0,"Program revision    : %s\r\n", v);
    s+= 4;
    rev=atoi(v);

    strncpy1(v,s, 2);
    xprintf(0,"Line status         : %s\r\n", v);
    s+= 2;

    if(s[6]=='.') l=13;
    else          l=6;

    strncpy1(v,s,l);
    xprintf(0,"Shot time           : %s\r\n", v);
    s+=l;

    strncpy1(v,s,4);
    xprintf(0,"Year                : %s\r\n", v);
    s+=4;
    strncpy1(v,s,2);
    xprintf(0,"Month               : %s\r\n", v);
    s+=2;
    strncpy1(v,s,2);
    xprintf(0,"Day                 : %s\r\n", v);
    s+=2;
    strncpy1(v,s, 3);
    xprintf(0,"Time Ref            : %s\r\n", v);
    s+= 3;

    strncpy1(v,s, 6);
    xprintf(0,"Shot number         : %s\r\n", v);
    s+= 6;
    strncpy1(v,s,16);
    xprintf(0,"LineName            : %s\r\n", v);
    s+=16;
    strncpy1(v,s,11);
    xprintf(0,"Master latitude     : %s\r\n", v);
    s+=11;
    strncpy1(v,s,11);
    xprintf(0,"Master longitude    : %s\r\n", v);
    s+=11;
    strncpy1(v,s, 6);
    xprintf(0,"Water depth         : %s\r\n", v);
    s+= 6;
    strncpy1(v,s,11);
    xprintf(0,"Source latitude     : %s\r\n", v);
    s+=11;
    strncpy1(v,s,11);
    xprintf(0,"Source longitude    : %s\r\n", v);
    s+=11;
    strncpy1(v,s, 5);
    xprintf(0,"Master Gyro         : %s\r\n", v);
    s+= 5;
    strncpy1(v,s, 5);
    xprintf(0,"Master GMG          : %s\r\n", v);
    s+= 5;
    strncpy1(v,s, 4);
    xprintf(0,"Master Speed        : %s\r\n", v);
    s+= 4;

    if(rev>3)
    {
        strncpy1(v,s, 4);
        xprintf(0,"Sequence number     : %s\r\n", v);
        s+= 4;
    }

    if(rev==3 || rev==6 || rev==7)
    {
        strncpy1(v,s, 3);
        xprintf(0,"Vessel ID           : %s\r\n", v);
        s+= 3;
    }

    if(! (s[0]=='*' || s[0]=='>'))
    {
        strncpy1(v,s,11);
        xprintf(0,"Master easting      : %s\r\n", v);
        s+=11;
        strncpy1(v,s,11);
        xprintf(0,"Master northing     : %s\r\n", v);
        s+=11;
        strncpy1(v,s, 7);
        xprintf(0,"Source delta east   : %s\r\n", v);
        s+= 7;
        strncpy1(v,s, 7);
        xprintf(0,"Source delta north  : %s\r\n", v);
        s+= 7;
        strncpy1(v,s, 5);
        xprintf(0,"Line bearing        : %s\r\n", v);
        s+= 5;
        strncpy1(v,s, 3);
        xprintf(0,"Julian day          : %s\r\n", v);
    }

    xprintf(0,"\r\n-------------------------------------------\r\n\r\n");
}

void  TSegDObj::DumpGunData(char* s)
{
    char  v[1024];
    char  x[5],y[5];

    xprintf(0,"Gun data\r\n");
    strncpy1(v,s, 6);
    xprintf(0,"Format Identifier   : %s\r\n", v);
    s+= 6;
    strncpy1(v,s, 4);
    xprintf(0,"Header length       : %s\r\n", v);
    s+= 4;
    //s=s+10;
    strncpy1(v,s, 8);
    xprintf(0,"Line number         : %s\r\n", v);
    s+= 8;
    strncpy1(v,s,10);
    xprintf(0,"Shot point number   : %s\r\n", v);
    s+=10;
    strncpy1(v,s, 2);
    xprintf(0,"Active string mask  : %s\r\n", v);
    s+= 2;
    strncpy1(v,s, 1);
    xprintf(0,"Trigger mode        : %s\r\n", v);
    s+= 1;
    strncpy1(v,s, 8);
    xprintf(0,"Shot time YY/MM/DD  : %s\r\n", v);
    s+= 8;
    strncpy1(v,s, 9);
    xprintf(0,"Shot time:HH/MM/SS  : %s\r\n", v);
    s+= 9;
    strncpy1(v,s, 1);
    xprintf(0,"Sequence number     : %s\r\n", v);
    s+= 1;
    strncpy1(x,s, 1);
    xprintf(0,"Number of strings   : %s\r\n", x);
    s+= 1;
    strncpy1(y,s, 2);
    xprintf(0,"# of guns total     : %s\r\n", y);
    s+= 2;
    strncpy1(v,s, 2);
    xprintf(0,"# of guns fired     : %s\r\n", v);
    s+= 2;
    strncpy1(v,s, 2);
    xprintf(0,"# of guns fire err  : %s\r\n", v);
    s+= 2;
    strncpy1(v,s, 2);
    xprintf(0,"# of guns autofired : %s\r\n", v);
    s+= 2;
    strncpy1(v,s, 2);
    xprintf(0,"# of guns mis-fired : %s\r\n", v);
    s+= 2;
    strncpy1(v,s, 3);
    xprintf(0,"Total err spread    : %s\r\n", v);
    s+= 3;
    strncpy1(v,s, 5);
    xprintf(0,"Volume fired cu.in  : %s\r\n", v);
    s+= 5;
    strncpy1(v,s, 5);
    xprintf(0,"Aver. abs. error ms : %s\r\n", v);
    s+= 5;
    strncpy1(v,s, 5);
    xprintf(0,"Stand deviat.err ms : %s\r\n", v);
    s+= 5;
    strncpy1(v,s, 4);
    xprintf(0,"Spare               : %s\r\n", v);
    s+= 4;
    strncpy1(v,s, 4);
    xprintf(0,"Mainfold pressure   : %s\r\n", v);
    s+= 4;
    strncpy1(v,s, 4);
    xprintf(0,"Not used            : %s\r\n", v);
    s+= 4;

    xprintf(0,"\r\n");

    int n;
    int ns = atoi(x);

    for(n=0; n<ns; n++)
    {
        strncpy1(v,s, 4);
        xprintf(0,"String # %d pressure: %s\r\n", n+1,v);
        s+= 4;
    }

    int ng = atoi(y);

    xprintf(0,"\r\n");

    xprintf(0,"Gun #  Gun   Detect  Seq #  Auto   Static  Gun      Fire  Error  Gun\r\n");
    xprintf(0,"       mode  mode           fire   Offset  delay    time         Depth\r\n");

    char gn[10];
    char gm[10];
    char dm[10];
    char sn[10];
    char af[10];
    char bb[10];
    char so[10];
    char dl[10];
    char ft[10];
    char er[10];
    char dp[10];

    for(n=0; n<ng; n++)
    {
        strncpy1(gn, s, 2);
        s+= 2;
        strncpy1(gm, s, 1);
        s+= 1;
        strncpy1(dm, s, 1);
        s+= 1;
        strncpy1(sn, s, 1);
        s+= 1;
        strncpy1(af, s, 1);
        s+= 1;
        strncpy1(bb, s, 1);
        s+= 1;
        strncpy1(so, s, 3);
        s+= 3;
        strncpy1(dl, s, 3);
        s+= 3;
        strncpy1(ft, s, 3);
        s+= 3;
        strncpy1(er, s, 3);
        s+= 3;
        strncpy1(dp, s, 3);
        s+= 3;

        xprintf(0,"%s     %s     %s       %s      %s      %s     %s      %s    %s   %s\r\n",
                gn, gm, dm, sn, af, so, dl, ft, er, dp);
    }

    xprintf(0,"\r\n-------------------------------------------\r\n");
}


string  TExHdr::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"External Header Block len = %d bytes \r\n\r\n",len);

    displayMemory(0, hdr, len);

    char* ss;
    ss=strmem((char*)"$1",    (char*)hdr,len);

    if(ss)
    {
        int p = ss-hdr;
        if(p+36<len && memcmp(ss+33,"UTC",3)==0) DumpNavData(ss);
    }

    ss=strmem((char*)"*GCS90",(char*)hdr,len);
    if(ss) DumpGunData(ss);

    xprintf(0,"\r\n-------------------------------------------\r\n\r\n");

    return _inf;
}

TExHdr::TExHdr(TSegD* segd) : TSegDObj(segd)
{
    char*  s;
    char*  navs;
    char*  tmps;
    int    EX = _segd->EX;

    unsigned int l = EX*32;

    name = "External Hdr";

    char* dat = segd->read(l);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

    len=l;

    set_len(l); memcpy(hdr,dat,l);
    type=1;

    tmps = new char[len+1];
    navs = new char[len+1];

    strncpy1(tmps,dat,len);
    tmps[len]=0;

    s=strstr(tmps,"$1");
    if(!s) goto END;

    strncpy1(navs,s+2,4);
    if(strlen(navs)<4) goto END;

    l=atoi(navs);
    if(l<1) goto END;

    l=l+2; if(l>strlen(s)) l=strlen(s);

    strncpy1(navs,s,l);
    if(strlen(navs)!=l) goto END;

    _segd->NavStr=navs;

END:

    delete [] tmps;
    delete [] navs;
}


string  TSsHdr::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"HydroScience SS header\r\n\r\n");

    displayMemory(0, hdr, len);

    return _inf;
}

TSsHdr::TSsHdr(TSegD* segd) : TSegDObj(segd)
{
    name = "SS Hdr";

    char* dat = segd->read(32);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

   set_len(32); memcpy(hdr,dat,32);
   type=1;
}


string  TXxHdr::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"Syntrak Extra Header Blocks %d\r\n\r\n",_segd->XX);

    displayMemory(0, hdr, len);

    return _inf;
}

TXxHdr::TXxHdr(TSegD* segd) : TSegDObj(segd)
{
    int    l = _segd->XX*32;

    name = "Extra Hdr";

    char* dat = segd->read(l);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

   set_len(l); memcpy(hdr,dat,l);
    type=1;
}


string  TJunk::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"%d bytes of unknown data\r\n\r\n",len);
    displayMemory(0, hdr, len);

    return _inf;
}

TJunk::TJunk(TSegD* segd, int l) : TSegDObj(segd)
{
    name = "Unknown data";
    len = l;

    char* dat = segd->read(l);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

   set_len(l); memcpy(hdr,dat,l);
    
}

string  TTapeHdr::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

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

    substr(hdr,   1,  4, seqn);
    substr(hdr,   5,  5, sdrv);
    substr(hdr,  10,  6, strc);
    substr(hdr,  16,  4, bnde);
    substr(hdr,  20, 10, mxbs);
    substr(hdr,  30, 10, apic);
    substr(hdr,  40, 11, date);
    substr(hdr,  51, 12, sern);
    substr(hdr,  63,  6, resr);
    substr(hdr,  69, 12, extl);
    substr(hdr,  81, 24, recn);
    substr(hdr, 105, 14, user);
    substr(hdr, 119, 10, mxsr);

    xprintf(0,"\n");
    xprintf(0,"SEG-D tape label------------------------------------------\n");
    xprintf(0,"Storage unit sequence number  : %s\n", seqn);
    xprintf(0,"SEG-D revision                : %s\n", sdrv);
    xprintf(0,"Storage unit structure        : %s\n", strc);
    xprintf(0,"Binding edition               : %s\n", bnde);
    xprintf(0,"Maximum block size            : %s\n", mxbs);
    xprintf(0,"API Producer org. code        : %s\n", apic);
    xprintf(0,"Creation date                 : %s\n", date);
    xprintf(0,"Serial number                 : %s\n", sern);
    xprintf(0,"Reserved                      : %s\n", resr);
    xprintf(0,"External label name           : %s\n", extl);
    xprintf(0,"Recording entity name         : %s\n", recn);
    xprintf(0,"User defined                  : %s\n", user);
    xprintf(0,"Max shot recs. per field rec. : %s\n", mxsr);
    xprintf(0,"----------------------------------------------------------\n\n");

    displayMemory(0, hdr, len);

    return _inf;
}

TTapeHdr::TTapeHdr(TSegD* segd) : TSegDObj(segd)
{
    char* dat = segd->read(128);

    name = "SEG-D Tape Header";
    type = 21;

    set_len(128); memcpy(hdr,dat,128);
}

string  TSkew::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"Sample Skew Header %d*32 \r\n\r\n",_segd->SK);

    displayMemory(0, hdr, len);

    return _inf;
}

TSkew::TSkew(TSegD* segd) : TSegDObj(segd)
{
//    long long pos;
    int    l = _segd->SK*32;

    name = "Skew Hdr";

    char* dat = segd->read(l);

    if(!dat)
    {
        xprintf(1, "%s is too short\r\n", name.c_str());
        return;
    }

   set_len(l); memcpy(hdr,dat,l);
}

//---------------------------------------------------------------------------

string  TMux::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    xprintf(0,"Multilplexed Data %d bytes\r\n\r\n",len);

    return _inf;
}

TMux::TMux(TSegD* segd) : TSegDObj(segd)
{

    traces = new TMyObjList;

    int nscnsmp = 0;
    int scnlen=0;
    int n, nc;
    int ns;

    ns=0;

    if(_segd->Si>0) ns = _segd->TrcSmpNum;

    if(_segd->Format!=200) // SEG-D
    {

        for(n=0; n<_segd->csl->getCount(); n++)
        {
            TCs* cs = _segd->get_csi(n);

            int nss = cs->nsscan;
            int nch = cs->nch;
            nscnsmp += nch*nss;

            cs->si = _segd->Si/(cs->nsscan);

            for(nc=0; nc<nch; nc++)
            {
                TTr* tr = new TTr(segd, ns*nss,nc,n);
                traces->Add(tr);
                cs->get_trc()->Add(tr);
            }
        }

        scnlen = (int) (nscnsmp * _segd->Smpl / 8.)+8;

        int l = ns * scnlen;

        name = "Multplexed data";

        hdr=segd->read(l);

        if(!hdr)
        {
            xprintf(1, "%s is too short\r\n", name.c_str());
            return;
        }

        char*  scan;
        float* fsmp;
        int    is,iss;
        int    nt;

        len=l;

        float* osmp = new float[nscnsmp];
        //    float  fmp  = 1;

        for(is=0; is<ns; is++)
        {
            scan = hdr + is*scnlen+8;


            if(_segd->Format==15)
            {
                F0015_to_float(scan, osmp, nscnsmp);

                nt=0;

                for(n=0; n<_segd->csl->getCount(); n++)
                {
                    TCs* cs = _segd->get_csi(n);

                    float fmp = pow(2,cs->mp);

                    int nss = cs->nsscan;
                    int nch = cs->nch;

                    for(nc=0; nc<nch; nc++)
                    {
                        TTr* tr = cs->get_trc()->get_tr(nc);
                        int  smpi = is*nss;
                        fsmp = (float*)tr->smps;
                        for(iss=0; iss<nss; iss++)
                            fsmp[smpi+iss] = osmp[nt]*fmp;
                        nt++;
                    }
                }
            }
        }

        delete [] osmp;
    }
    else // SEG-B
    {
        for(n=0; n<_segd->csl->getCount(); n++)
        {
            TCs* cs = _segd->get_csi(n);

            int nch = cs->nch;
            nscnsmp += nch;

            cs->si = _segd->Si;

            for(nc=0; nc<nch; nc++)
            {
                TTr* tr = new TTr(segd, ns,nc,n);
                traces->Add(tr);
                cs->get_trc()->Add(tr);
            }
        }

        scnlen = _segd->B;

        int l = ns * scnlen;

        name = "Multplexed data";

        hdr=segd->read(l);

        if(!hdr)
        {
            xprintf(1, "%s is too short\r\n", name.c_str());
            return;
        }

        len=l;

        unsigned char*  scan;
        float* fsmp;
        int    is;
        int    nt;

        float* osmp = new float[nscnsmp];

        memset(osmp,0,nscnsmp*sizeof(float));

        for(is=0; is<ns; is++)
        {
            scan = (unsigned char*)hdr + is*scnlen;

            //int nn;

            for(nt=0;nt<5;nt++)
            {
              short b1 = scan[4+nt*2  ];
              short b2 = scan[4+nt*2+1];

              if(nt==0) b1 = b1 >> 1;

              b2 = b2 >> 1;

              short bb = (( b2 | (b1<<7) ) <<1); bb=bb>>1;

              osmp[nt]=bb;

              if(nt==0)
              {
                //printf("is = %6d  b1 = %2x  - b2 = %2x  bb = %4x - %d\n", is, b1,b2,bb,bb); fflush(stdout);
              }
            }

            B0015_to_float(scan+14, osmp+5, nscnsmp-5);

            nt=0;

            for(n=0; n<_segd->csl->getCount(); n++)
            {
                TCs* cs = _segd->get_csi(n);

                int nch = cs->nch;

                for(nc=0; nc<nch; nc++)
                {
                    TTr* tr = cs->get_trc()->get_tr(nc);
                    fsmp = (float*)tr->smps;
                    fsmp[is] = osmp[nt];
                    nt++;
                }
            }
        }

        delete [] osmp;
    }

    type=4;

}

TMux::~TMux()
{
    delete traces;
}

//---------------------------------------------------------------------------



string  TSegdInfo::get_inf()
{
    if(_err!="") return _err;
    if(_inf!="") return _inf;

    int n;

    xprintf(0, "Common Information\r\n\r\n");

    if(_segd->Type==0)
    {
        //xprintf(0, "Unknown data: %s\r\n",_err);
    }
    else if(_segd->Type==1)
    {
        //xprintf(0, "SEG-Y data: %s\r\n",_err);
    }
    else if(_segd->Type==2) // SEG-B
    {
        xprintf(0, "FFID            : %d\r\n",        _segd->F);
        xprintf(0, "Data Format     : %04d - %s\r\n", _segd->Format, "SEG-B 20 bit binary multiplexed");
        xprintf(0, "Sample Interval : %g ms\r\n",     _segd->Si);
        xprintf(0, "Total traces #       : %d\r\n",   _segd->trc_cnt);

        xprintf(0, "\r\nChannel Sets (%d):\r\n\r\n",    _segd->get_ncs());

        xprintf(0, "   #  type                     #chan #samples    to(ms) tn(ms)    si\r\n");

        for(n=0; n<_segd->get_ncs(); n++)
        {
            TCs*  cs = _segd->get_csi(n);
            float si = _segd->Si/(cs->nsscan);
            TTr*  tr = cs->get_trc()->get_tr(0);

            xprintf(0, "%4d  ",  cs->csn);
            xprintf(0, "%-25s ", cs->typnm);
            xprintf(0, "%-5d  ", cs->nch);
            if(tr==NULL) xprintf(0, "%-9s  ", "N/A");
            else         xprintf(0, "%-9d  ", tr->ns);
            xprintf(0, "%-5d  ", cs->to*2);
            xprintf(0, "%-5d  ", cs->tn*2);
            xprintf(0, "  %-g", si);
            xprintf(0,"\r\n");
        }

        xprintf(0,"\r\n");
    }
    else
    {
        if(_segd->objs->getCount()==2)
        {
         TSegDObj* obj = (TSegDObj*)(_segd->objs->getObj(1));
         if(obj->type==21)

          xprintf(0, "SEG-D tape header\r\n");

          return _inf;
        }

        xprintf(0, "FFID            : %d\r\n",      _segd->F);
        xprintf(0, "SP              : %d\r\n",      _segd->SP);

        xprintf(0, "Data Format          : %04d - %s\r\n", _segd->Format, fmtnum2nam(_segd->Format));
        xprintf(0, "Base Sample Interval : %g ms\r\n",   _segd->Si);
        xprintf(0, "Total traces #       : %d\r\n",      _segd->trc_cnt);

        xprintf(0, "\r\nChannel Sets (%d):\r\n\r\n",    _segd->get_ncs());

        xprintf(0, "   #  type                     #chan  #samples   to(ms) tn(ms)    si\r\n");

        for(n=0; n<_segd->get_ncs(); n++)
        {
            TCs*  cs = _segd->get_csi(n);
            float si = _segd->Si/(cs->nsscan);
            TTr*  tr = cs->get_trc()->get_tr(0);

            xprintf(0, "%4d  ",  cs->csn);
            xprintf(0, "%-25s ", cs->typnm);
            xprintf(0, "%-5d  ", cs->nch);
            if(tr==NULL) xprintf(0, "%-9s  ", "N/A");
            else         xprintf(0, "%-9d  ", tr->ns);
            xprintf(0, "%-5d  ", cs->to*2);
            xprintf(0, "%-5d  ", cs->tn*2);
            xprintf(0, "  %-g", si);
            xprintf(0,"\r\n");
        }
    }

    xprintf(0,"\r\n");


    if(_segd->hdr_len) xprintf(0, "File Header Len : %d bytes\r\n", _segd->hdr_len);
    xprintf(0, "File Length     : %d bytes\r\n", _segd->getlen()    );

    xprintf(0,"\r\n");
    xprintf(0,"%s\r\n",_segd->warn.c_str());


    return _inf;
}


TSegdInfo::TSegdInfo(TSegD* segd) : TSegDObj(segd)
{
    name = "Common Info";
}

//---------------------------------------------------------------------------



string  TSegdWarn::get_inf()
{
    return _inf;
}


TSegdWarn::TSegdWarn(TSegD* segd, string warn) : TSegDObj(segd)
{
    name = "Warning";
    _wrn = warn;
    _inf = warn;
    segd->warn = segd->warn + warn + "\r\n";
}


TSegdEof::TSegdEof(TSegD* segd, string warn) : TSegDObj(segd)
{
    name = "End of File";
    _wrn = warn;
    _inf = warn;
}

string  TSegdEof::get_inf()
{
    return _inf;
}


//---------------------------------------------------------------------------

char* TSegD::read(int len)
{
    char* dat;

    if(_pos+len>_len) len=_len-_pos;

    dat = _buf + _pos;

    if(len>0) _pos += len;
    else
    {
       len=0;
       dat=NULL;
    }

    return dat;
}

void TSegD::close()
{
    if(_len>0)
    {
        delete [] _buf;
        _len=0;
    }
}

int TSegD::open(char*)
{
    close();

    int len=0;

    if(len>0)
    {

    _buf = new char[len];
    _len = len;
    _pos = 0;
    }

    return 0;
}

int TSegD::create(int len)
{
    close();

    if(len>0)
    {
    _buf = new char[len];
    _len = len;
    _pos = 0;
    }

    return 0;
}


int TSegD::seek(int pos, int from)
{
    if(from==SEEK_END) pos=_len-pos;

    if(from==SEEK_CUR) pos=_pos+pos;


    if(pos <  0    ) pos=0;
    if(pos > _len-1) pos=_len;

    _pos = pos;

    return pos;
}

int TSegD::tell()
{
    return _pos;
}


int TSegD::find_trcx(int ppos, short ffff, int fn)
{
    int   pos;
    int   plen=20;

    for(pos=ppos; pos+plen<_len; pos++)
    {
        TTrInf ti = chck_trcx (pos, ffff, fn);

       if(ti.stat>=0)
       {
           return pos;
       }
    }

    return -1;
}

TTrInf TSegD::chck_trcx (int ppos, short ffff, int fn)
{
    TTrInf ti;

    ti.stat = 0; // EOF
    ti.pos  = ppos;

    if(ppos ==_len)
    {
        return ti;
    }

    ti.stat = -2;

    if(ppos+20 >= _len) return ti;

    char *dat = _buf+ppos;

    char      s  [1024];

    int       FN;
    int       CS;
    int       ST;
    int       CH;
    int       THE=0;
    int       EXNS=0;

    int       n;
    int       v;

    ti.stat = -3;

    if(memcmp(dat,&ffff,2)!=0) return ti;

    hexs(dat, 1, 4,s);
    if(strcmp(s,"FFFF"))
    {
        n = sscanf(s,"%d",&v);
        if(n==1) FN=v;
        else     return ti;
//      FN=atoi(s);
    }
    else                 FN=hex2uint(hexs(dat, 35, 6,s));

    ti.stat = -4;
    if(FN!=fn) return ti;

    hexs(dat, 5, 2,s);
    ST = atoi(s);

    hexs(dat, 7, 2,s);
    if(strcmp(s,"FF")) CS=atoi(s);
    else               CS=hex2uint(hexs(dat, 33, 2,s));

    TCs* Cs = NULL;
    int  csidx= find_csi(CS);

    if(csidx>=0)
    {
        Cs = get_csi(csidx);
    }

    ti.stat = -5;

    if(Cs==NULL)  return ti;
    if(Cs->nch<1) return ti;

    hexs(dat, 9, 4,s);
    CH = atoi(s);

    THE=hex2uint(hexs(dat, 19, 2,s));

    if(THE>0)
    {
       ti.stat = -6;
       if(ppos+20+32*THE > _len) return ti;
       EXNS = int_item(dat+20,15, 6);
    }

    ti.stat  = 1;
    ti.chn   = CH;
    ti.csn   = CS;
    ti.stn   = ST;
    ti.exns  = EXNS;
    ti.the   = THE;
    ti.pos   = ppos;

    return ti;
}



