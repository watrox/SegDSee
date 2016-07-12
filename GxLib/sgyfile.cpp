#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include <unistd.h>

#include "sgyfile.h"
#include "util2.h"

#include "math.h"

#include <QDebug>

static int _ff [] = {
    4,4,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,4,2,2,0
};

static int _fp [] = {
    1,5,9,13,15,17,19,21,23,25,27,29,31,33,
    35,37,39,41,43,45,47,49,51,53,55,57,59,63,301,303,305,0
};

//static int _fe [] = {1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,-1,-1,1,1,0};


const char* _fs[] = {
    "  Job identification number",
    "* Line number",
    "* Reel number",
    "* # data traces per record",
    "* # aux  traces per record",
    "* Sample interval (microseconds) for reel",
    "  Sample interval (microseconds) for field",
    "* Number samples per data trace for reel",
    "  Number samples per data trace for field",
    "* Data sample format code",
    "* CDP fold",
    "* Trace sorting code",
    "  Vertical sum code",
    "  Sweep frequency at start",
    "  Sweep frequency at end",
    "  Sweep length (milliseconds)",
    "  Sweep type code",
    "  Trace number of sweep channel",
    "  Sweep trace taper length at start (ms)",
    "  Sweep trace taper length at end   (ms)",
    "  Taper type",
    "  Correlated data traces",
    "  Binary gain recovered",
    "  Amplitude recovery method",
    "  Measurement system (1-m / 2-feet)",
    "  Impulse signal",
    "  Vibratory polarity code",
    "- SSC Co SEG-Y samples number",
    "  SEG-Y Rev",
    "  Fixed trace flag",
    "  Number of Extended Textual Headers",
    ""
};

/*
static int _tf [] = {0,
                     4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  4,  4,  2,
                     2,  4,  4,  4,  4,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 12,  2,
                     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                     2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  2,  2
                    };

static int _tp[] = {0,
                    1,  5,  9, 13, 17, 21, 25, 29, 31, 33, 35, 37, 41, 45, 49, 53, 57, 61, 65, 69,
                    71, 73, 77, 81, 85, 89, 91, 93, 95, 97, 99,101,103,105,107,109,111,113,115,117,
                    119,121,123,125,127,129,131,133,135,137,139,141,143,149,151,153,155,157,159,161,
                    163,165,167,169,171,173,175,177,179,181,185,189,193,197,201,203
                   };

static const char* _tn[] = {
    "  Trace index",
    "* Trace sequence number within line",
    "  Trace sequence number within reel",
    "* FFID - Original field record number ",
    "* Trace number within field record",
    "  SP - Energy source point number",
    "  CDP ensemble  number ",
    "  Trace  number",
    "* Trace identification code",
    "  Number of vertically summed traces",
    "  Number of horizontally stacked traces",
    "  Data use ( 1-production, 2-test )",
    "  Distance from source point to receiv grp",
    "  Receiver group elevation",
    "  Surface elevation at source",
    "  Source depth below surface",
    "  Datum elevation at receiver group",
    "  Datum elevation at source",
    "  Water depth at source",
    "  Water depth at group",
    "  Scaler to all elevations & depths",
    "  Scaler to all coordinates",
    "  Source X coordinate",
    "  Source Y coordinate",
    "  Group  X coordinate",
    "  Group  Y coordinate",
    "  Coordinate units (1-lenm/ft 2-secarc)",
    "  Weathering velocity",
    "  Subweathering velocity",
    "  Uphole time at source",
    "  Uphole time at group",
    "  Source static correction",
    "  Group  static correction",
    "  Total static applied",
    "  Lag time A",
    "  Lag time B",
    "  Delay Recording time",
    "  Mute time start",
    "  Mute time end",
    "* Number of samples in this trace",
    "* Sample interval in ms for this trace",
    "  Gain type of field instruments",
    "  Instrument gain",
    "  Instrument gain constant",
    "  Correlated (1-yes / 2-no)",
    "  Sweep frequency at start",
    "  Sweep frequency at end",
    "  Sweep lenth in ms",
    "  Sweep type 1-lin,2-parabol,2-exp,4-ohter",
    "  Sweep trace taper length at start in ms",
    "  Sweep trace taper length at end   in ms",
    "  Taper type 1-lin,2-cos2,3-other",
    "  Alias filter frequency, if used",
    "  Alias filter slope",
    "  Low cut frequency,  if used ",
    "  High cut frequency, if used ",
    "  Low cut slope",
    "  High cut slope",
    "  Year data recorded",
    "  Day of year",
    "  Hour of day",
    "  Minute of hour",
    "  Second of minute",
    "  Time basis code 1-local,2-GMT,3-other",
    "  Trace weighting factor",
    "  Geophone group number of roll sw pos 1",
    "  Geophone group number of trace # 1",
    "  Geophone group number of last trace",
    "  Gap size (total # of groups dropped)",
    "  Overtravel assoc w taper of beg/end line",
    "+ CDP X",
    "+ CDP Y",
    "+ Inline Number",
    "+ Clossline Number",
    "+ Shot Point Number",
    "+ Shot Point Scalar",
    "+ Trace value measurement unit"
};

static const char* _ts[] = {
    "TRCIDX",
    "SEQWL",
    "SEQWR",
    "FFID",
    "TRCFLD",
    "SP",
    "CDP",
    "TRCNUM",
    "TRCID",
    "NVST",
    "NHST",
    "DU",
    "DSREG",
    "RGE",
    "SES",
    "SDBS",
    "DERG",
    "DES",
    "WDS",
    "WGD",
    "SAED",
    "SAC",
    "SRCX",
    "SRCY",
    "GRPX",
    "GRPY",
    "UNITS",
    "WVEL",
    "SVEL",
    "UTSRC",
    "UTGRP",
    "SECSCOR",
    "GRPSCOR",
    "TSA",
    "LAGTA",
    "LAGTB",
    "DELRECT",
    "MTSTART",
    "MTEND",
    "NSMP",
    "SI",
    "GTFI",
    "IG",
    "IGC",
    "CORREL",
    "SFSTART",
    "SFEND",
    "SLEN",
    "STYP",
    "SSTRLS",
    "SSTLE",
    "TTYP",
    "AFF",
    "AFS",
    "LOCF",
    "HOCF",
    "LOCS",
    "HICS",
    "YEAR ",
    "DAY",
    "HOUR",
    "MINUTE",
    "SCE",
    "TMBS",
    "TWF",
    "GGNSW",
    "GGN1ST",
    "GGNLST",
    "GAPSZ",
    "OAWT",
    "CDP-X",
    "CDP-Y",
    "INLINE#",
    "XLINE#",
    "SP#",
    "SPS",
    "TVMU"
};
*/


SgyFile::SgyFile(QObject *parent) :
    SeisFile(parent)
{
    _inptrc=NULL;
    _inpsmp=NULL;
    _hlen  =240;
    SetDefaultHdrDefs();
}

SgyFile::~SgyFile()
{
//  qDebug() << "~SgyFile()";

    Close();
}

//---------------------------------------------------------------------------
bool SgyFile::ReadTxtHed()
{
    memset(_txthed,' ',3200); _txtcode=0;

    if(_f<0) return false;

    char txthed[3200];

    lseek64(_f,0,SEEK_SET);

    int cnt  = read(_f, txthed , 3200); if(cnt!=3200) return false;

    _txtcode=is_txthdr_ebcdic(txthed);

    if(_txtcode) ebasdn(txthed,txthed,3200);

    memcpy(_txthed,txthed,3200);

    return true;
}

bool SgyFile::ReadBinHed()
{
    memset(_binhed,0,400);

    if(_f<0) return false;

    byte buf[400];

    lseek64(_f,3200,SEEK_SET);

    int cnt  = read(_f, buf , 400); if(cnt!=400) return false;

    memcpy(_binhed,buf,400);

    int si   = ui2(buf, 17, _swap);
    int nsmp = ui2(buf, 21, _swap);
    int frmt = i2 (buf, 25, _swap);

    int smpl;

    if     (frmt==3) smpl=2;
    else if(frmt==6) smpl=1;
    else if(frmt==8) smpl=1;
    else             smpl=4;

    int trl  = 240+nsmp*smpl;

    _Nt  = (_flen-3600)/trl;
    _Format = frmt;
    _smpl  = smpl;
    _trl   = trl;
    _Ns    = nsmp;
    _Si   = si/1000000.;

    return true;
}


void  SgyFile::Open(string fn)
{
    if(_f>0) Close();

    const char* fname = fn.c_str();

    int cgg, junk, swap;

    if(!is_segy_f(fname,swap,cgg,junk))
    {
        _Active=false;
        return;
    }

    _swap=swap;
    _rd_only=1;


    //_Nt=_ntr;
    //_Ns=_nsmp;
    //_Si=_si/1000000.;
    //_Ti=25;

    //    _slist.Clear();

    _f = open(fname,O_RDWR|O_BINARY);
    if(_f<0) //try read only
    {
        _f = open(fname,O_RDONLY|O_BINARY);
        _rd_only=1;
    }

    if(_f<0)
    {
        _Active=false;
        return;
    }

    _flen = getfilesize64 (_f);

    bool errb = !ReadBinHed();
    bool errt = !ReadTxtHed();

    if(errb || errt) goto ERR;

    if(_Nt<2048)
        _data.Init(_Nt, 240+(_Ns)*4, _Nt);
    else
        _data.Init(_Nt, 240+(_Ns)*4, 2048);

    //    ReadRawHeader(0,       _trhedo, false);
    //    ReadRawHeader((_ntr-1),_trhedn, false);

    lseek(_f,3600,SEEK_SET);

    if(_inptrc) delete [] _inptrc;
    if(_inpsmp) delete [] _inpsmp;
    //    _inptrc =   new char[_trl+_dlymax];
    _inptrc =   new char [_trl];
    _inpsmp =   new float[_Ns];

    //    if(!_rd_only)  rc|=1;
    //    if(_cgg)       rc|=2;
    //    if(!getswap()) rc|=4;

    _Active=true;

    return;

ERR:
    _data.Clear(); _Nt=0; _Active=false;

    return;
}

void  SgyFile::Close()
{
//    qDebug() << "SgyFile::Close()";

    if(_f>0) close(_f); _f=-1;

    _data.Clear();

    if(_inptrc) delete [] _inptrc; _inptrc=NULL;
    if(_inpsmp) delete [] _inpsmp; _inpsmp=NULL;

    _Ns=_Nt=0; _Active=false;
}


void  SgyFile::ReadTrace(long long ntr)
{
    if(!_f) return;

    int  n;

    lseek64(_f,3600+ntr*_trl,SEEK_SET);
    n = read(_f, _inptrc, _trl);

    byte*  buf = _data.PutObject(ntr);
    if(buf==NULL) return;

    //    int delay = 0;

    //    if(_recdly) delay=i2(_inptrc,109)/(_si*1000)+0.5;
    //    if(_recdly) delay=i2(_inptrc,109)/(_si*1000)+0.5;

    float* sample = (float*)(buf+240);

    //    if(_recdly) memset(sample,0,_nsmp*4);
    //    for(n=0;n<_nsmp-_dlymax;n++) sample[n+delay] = GetSample(n, _inptrc);
    for(n=0;n<_Ns;n++) sample[n] = GetSample(n, _inptrc);

    //    DoProcessing(sample);

    memcpy(buf,_inptrc,240);
}


int SgyFile::ReadRawTrace(byte* &buf, long long ntr)
{
    if(!_f) return -3;

    if(ntr<0 || ntr>=_Nt) return -2;

    lseek64(_f,3600+ntr*_trl,SEEK_SET);

    buf = new byte[_trl];

    int cnt = read(_f, buf, _trl);

    if(cnt!=_trl)
    {
        delete [] buf; buf=NULL; return -1;
    }

    return _trl;
}


int  SgyFile::WriteRawTrace(byte* buf, long long ntr)
{
    if(!_f) return -1;

    if(ntr<0 || ntr>=_Nt) return -1;

    lseek64(_f,3600+ntr*_trl,SEEK_SET);

    int cnt = write(_f, buf, _trl);

    if(cnt!=_trl) return -1;

    return 0;
}


float* SgyFile::Rs(long long ntr)
{
    int  n;

    if(!_f) return NULL;

    lseek64(_f,3600+ntr*_trl,SEEK_SET);
    n = read(_f, _inptrc, _trl);

    for(n=0;n<_Ns;n++) _inpsmp[n] = GetSample(n, _inptrc);

    return _inpsmp;
}


float  SgyFile::GetSample(int idx, char* inptrc)
{
    float   smp;
    int*    xmp = (int*)&smp;
    float*  dat = (float*)(inptrc+240);

    short*       int2ptr = (short*)       dat;
    int*         int4ptr = (int*)         dat;
    signed char* int1ptr = (signed char*) dat;

    if(idx<0 && idx> _Ns) return 0;

    if(_Format==1)
    {
        smp =  dat[idx];
        smp = ibm2ieee(smp, _swap);
    }
    else if(_Format==2)
    {
        smp = swapi4(int4ptr[idx],_swap);
    }
    else if(_Format==3)
    {
        smp = swapi2(int2ptr[idx],_swap);
    }
    else if(_Format==4)
    {
        smp = dat[idx];
    }
    else if(_Format==5)
    {
        *xmp = swapi4(int4ptr[idx],_swap);
    }
    else if(_Format==6 || _Format==8)
    {
        smp = int1ptr[idx];
    }
    else
        smp = 0;

    int wgti = i2((byte*)_inptrc,169, _swap);

    if(wgti==0) return smp;

    double wgt = pow (2.,wgti);

    return smp/wgt;
}


float*  SgyFile::Ts (long long ntr)
{
    byte*  buf =   Tb(ntr);
    if(buf) return (float*)(buf+240);
    else    return NULL;
}

void  SgyFile::SetDefaultHdrDefs()
{
    int n;

    static int fmt [] = {0,
                         4,  4,  4,  4,  4,  4,  4,  2,  2,  2,  2,  4,  4,  4,  4,  4,  4,  4,  4,  2,
                         2,  4,  4,  4,  4,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 12,  2,
                         2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                         2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  4,  4,  2,  2
                        };

    static int pos[] = {0,
                        1,  5,  9, 13, 17, 21, 25, 29, 31, 33, 35, 37, 41, 45, 49, 53, 57, 61, 65, 69,
                       71, 73, 77, 81, 85, 89, 91, 93, 95, 97, 99,101,103,105,107,109,111,113,115,117,
                      119,121,123,125,127,129,131,133,135,137,139,141,143,149,151,153,155,157,159,161,
                      163,165,167,169,171,173,175,177,179,181,185,189,193,197,201,203
                       };

    static const char* desc[] = {
        "  Trace index",
        "* Trace sequence number within line",
        "  Trace sequence number within reel",
        "* FFID - Original field record number ",
        "* Trace number within field record",
        "  SP - Energy source point number",
        "  CDP ensemble  number ",
        "  Trace  number",
        "* Trace identification code",
        "  Number of vertically summed traces",
        "  Number of horizontally stacked traces",
        "  Data use ( 1-production, 2-test )",
        "  Distance from source point to receiv grp",
        "  Receiver group elevation",
        "  Surface elevation at source",
        "  Source depth below surface",
        "  Datum elevation at receiver group",
        "  Datum elevation at source",
        "  Water depth at source",
        "  Water depth at group",
        "  Scaler to all elevations & depths",
        "  Scaler to all coordinates",
        "  Source X coordinate",
        "  Source Y coordinate",
        "  Group  X coordinate",
        "  Group  Y coordinate",
        "  Coordinate units (1-lenm/ft 2-secarc)",
        "  Weathering velocity",
        "  Subweathering velocity",
        "  Uphole time at source",
        "  Uphole time at group",
        "  Source static correction",
        "  Group  static correction",
        "  Total static applied",
        "  Lag time A",
        "  Lag time B",
        "  Delay Recording time",
        "  Mute time start",
        "  Mute time end",
        "* Number of samples in this trace",
        "* Sample interval in ms for this trace",
        "  Gain type of field instruments",
        "  Instrument gain",
        "  Instrument gain constant",
        "  Correlated (1-yes / 2-no)",
        "  Sweep frequency at start",
        "  Sweep frequency at end",
        "  Sweep lenth in ms",
        "  Sweep type 1-lin,2-parabol,2-exp,4-ohter",
        "  Sweep trace taper length at start in ms",
        "  Sweep trace taper length at end   in ms",
        "  Taper type 1-lin,2-cos2,3-other",
        "  Alias filter frequency, if used",
        "  Alias filter slope",
        "  Low cut frequency,  if used ",
        "  High cut frequency, if used ",
        "  Low cut slope",
        "  High cut slope",
        "  Year data recorded",
        "  Day of year",
        "  Hour of day",
        "  Minute of hour",
        "  Second of minute",
        "  Time basis code 1-local,2-GMT,3-other",
        "  Trace weighting factor",
        "  Geophone group number of roll sw pos 1",
        "  Geophone group number of trace # 1",
        "  Geophone group number of last trace",
        "  Gap size (total # of groups dropped)",
        "  Overtravel assoc w taper of beg/end line",
        "+ CDP X",
        "+ CDP Y",
        "+ Inline Number",
        "+ Clossline Number",
        "+ Shot Point Number",
        "+ Shot Point Scalar",
        "+ Trace value measurement unit"
    };

    static const char* name[] = {
        "TRCIDX",
        "SEQWL",
        "SEQWR",
        "FFID",
        "TRCFLD",
        "SP",
        "CDP",
        "TRCNUM",
        "TRCID",
        "NVST",
        "NHST",
        "DU",
        "DSREG",
        "RGE",
        "SES",
        "SDBS",
        "DERG",
        "DES",
        "WDS",
        "WGD",
        "SAED",
        "SAC",
        "SRCX",
        "SRCY",
        "GRPX",
        "GRPY",
        "UNITS",
        "WVEL",
        "SVEL",
        "UTSRC",
        "UTGRP",
        "SECSCOR",
        "GRPSCOR",
        "TSA",
        "LAGTA",
        "LAGTB",
        "DELRECT",
        "MTSTART",
        "MTEND",
        "NSMP",
        "SI",
        "GTFI",
        "IG",
        "IGC",
        "CORREL",
        "SFSTART",
        "SFEND",
        "SLEN",
        "STYP",
        "SSTRLS",
        "SSTLE",
        "TTYP",
        "AFF",
        "AFS",
        "LOCF",
        "HOCF",
        "LOCS",
        "HICS",
        "YEAR ",
        "DAY",
        "HOUR",
        "MINUTE",
        "SCE",
        "TMBS",
        "TWF",
        "GGNSW",
        "GGN1ST",
        "GGNLST",
        "GAPSZ",
        "OAWT",
        "CDP-X",
        "CDP-Y",
        "INLINE#",
        "XLINE#",
        "SP#",
        "SPS",
        "TVMU"
    };

    int len = sizeof(fmt)/sizeof(int);

//    if( sizeof(pos )/sizeof(int)  != len) "#1";
//    if( sizeof(desc)/sizeof(char*)!= len) "#2";
//    if( sizeof(name)/sizeof(char*)!= len) "#3";

    // _TrHdrDef.Init(fmt,pos,desc,name,len);

    Hdrs.clear();

    for(n=0;n<len;n++)
    {
        Hdrs.append(Hdri(name[n],desc[n],fmt[n],pos[n]));
    }

    for(n=0;n<len;n++) Hidx[Hdrs[n].name]=n;
}

QString SgyFile::BinHedValue(int idx)
{
    if(idx<0 || idx>=BinHedCount()) return 0;

    QString vs;
    int     v;

    int p = _fp[idx];
    int f = _fp[idx];

    if     (f==4)                       v=i4 (_binhed,p, _swap);
    else if(idx==5 || idx==6 || idx==7) v=ui2(_binhed,p, _swap);
    else                                v=i2 (_binhed,p, _swap);

    if(p==301)
    {
        int maj = _binhed[300];
        int min = _binhed[301];
        vs.sprintf("%d.%d",maj,min);
    }
    else
    {
        vs.sprintf("%d",v);
    }

    return vs;
}

QString SgyFile::BinHedName (int idx)
{
    if(idx<0 || idx>=BinHedCount()) return "";
    return _fs[idx];
}

QString SgyFile::BinHedBytes(int idx)
{
    if(idx<0 || idx>=BinHedCount()) return 0;

    QString s; s.sprintf("%d-%d",_fp[idx],_fp[idx]+_ff[idx]-1);

    return s;
}

int SgyFile::BinHedCount()
{
    int n = sizeof(_ff) / sizeof(int);
    return n;
}


//---------------------------------------------------------------------------

SuFile::SuFile(QObject *parent) :
    SgyFile(parent)
{
}

SuFile::~SuFile()
{
//  qDebug() << "~SgyFile()";

    Close();
}

void  SuFile::Close()
{
    if(_f>0) close(_f); _f=-1;

    _data.Clear();

    if(_inptrc) delete [] _inptrc; _inptrc=NULL;
    if(_inpsmp) delete [] _inpsmp; _inpsmp=NULL;

    _Ns=_Nt=0; _Active=false;
}


void  SuFile::Open(string fn)
{
    if(_f>0) Close();

    const char* fname = fn.c_str();

    int swap;

    if(!is_su_f(fname,swap))
    {
        _Active=false;
        return;
    }

    _swap=swap;
    _rd_only=1;

    _f = open(fname,O_RDWR|O_BINARY);
    if(_f<0) //try read only
    {
        _f = open(fname,O_RDONLY|O_BINARY);
        _rd_only=1;
    }

    if(_f<0)
    {
        _Active=false;
        return;
    }

    _flen = getfilesize64 (_f);

    bool errb = !ReadBinHed();

    if(errb) goto ERR;

    if(_Nt<2048)
        _data.Init(_Nt, 240+(_Ns)*4, _Nt);
    else
        _data.Init(_Nt, 240+(_Ns)*4, 2048);

    //    ReadRawHeader(0,       _trhedo, false);
    //    ReadRawHeader((_ntr-1),_trhedn, false);

    lseek(_f,0,SEEK_SET);

    if(_inptrc) delete [] _inptrc;
    if(_inpsmp) delete [] _inpsmp;

    _inptrc =   new char [_trl];
    _inpsmp =   new float[_Ns];

    _Active=true;

    return;

ERR:
    _data.Clear(); _Nt=0; _Active=false;

    return;
}


bool SuFile::ReadBinHed()
{
// SU file desn't have Bin Header, get data from the first trace
    memset(_binhed,0,400);

    if(_f<0) return false;

    byte buf[240];

    lseek64(_f,0,SEEK_SET);

    int cnt  = read(_f, buf , 240); if(cnt!=240) return false;

//  memcpy(_binhed,buf,400);

    int si   = ui2(buf, 117, _swap);
    int nsmp = ui2(buf, 115, _swap);

    int smpl = 4;

    int trl  = 240+nsmp*smpl;

    _Nt  = (_flen)/trl;
    _Format = 5;
    _smpl  = smpl;
    _trl   = trl;
    _Ns    = nsmp;
    _Si   = si/1000000.;

    return true;
}

void  SuFile::ReadTrace(long long ntr)
{
    if(!_f) return;

    int  n;

    lseek64(_f,ntr*_trl,SEEK_SET);
    n = read(_f, _inptrc, _trl);

    byte*  buf = _data.PutObject(ntr);
    if(buf==NULL) return;

    float* sample = (float*)(buf+240);

    for(n=0;n<_Ns;n++) sample[n] = GetSample(n, _inptrc);

    memcpy(buf,_inptrc,240);
}



int SuFile::ReadRawTrace(byte* &buf, long long ntr)
{
    if(!_f) return -3;

    if(ntr<0 || ntr>=_Nt) return -2;

    lseek64(_f,ntr*_trl,SEEK_SET);

    buf = new byte[_trl];

    int cnt = read(_f, buf, _trl);

    if(cnt!=_trl)
    {
        delete [] buf; buf=NULL; return -1;
    }

    return _trl;
}


int  SuFile::WriteRawTrace(byte* buf, long long ntr)
{
    if(!_f) return -1;

    if(ntr<0 || ntr>=_Nt) return -1;

    lseek64(_f,ntr*_trl,SEEK_SET);

    int cnt = write(_f, buf, _trl);

    if(cnt!=_trl) return -1;

    return 0;
}

float* SuFile::Rs(long long ntr)
{
    int  n;

    if(!_f) return NULL;

    lseek64(_f,ntr*_trl,SEEK_SET);
    n = read(_f, _inptrc, _trl);

    for(n=0;n<_Ns;n++) _inpsmp[n] = GetSample(n, _inptrc);

    return _inpsmp;
}


//---------------------------------------------------------------------------

CstFile::CstFile(QObject *parent) :
    SgyFile(parent)
{
    _hlen  = 256;
    SetDefaultHdrDefs();
}

CstFile::~CstFile()
{
//  qDebug() << "~SgyFile()";

    Close();
}

void  CstFile::Close()
{
    if(_f>0) close(_f); _f=-1;

    _data.Clear();

    if(_inptrc) delete [] _inptrc; _inptrc=NULL;
    if(_inpsmp) delete [] _inpsmp; _inpsmp=NULL;

    _Ns=_Nt=0; _Active=false;
}


void  CstFile::Open(string fn)
{
    if(_f>0) Close();

    const char* fname = fn.c_str();

    if(!is_cst_f(fname))
    {
        _Active=false;
        return;
    }

    _swap=0;
    _rd_only=1;

    _f = open(fname,O_RDWR|O_BINARY);
    if(_f<0) //try read only
    {
        _f = open(fname,O_RDONLY|O_BINARY);
        _rd_only=1;
    }

    if(_f<0)
    {
        _Active=false;
        return;
    }

    _flen = getfilesize64 (_f);

    bool errb = !ReadBinHed();

    if(errb) goto ERR;

    if(_Nt<2048)
        _data.Init(_Nt, 240+(_Ns)*4, _Nt);
    else
        _data.Init(_Nt, 240+(_Ns)*4, 2048);

    //    ReadRawHeader(0,       _trhedo, false);
    //    ReadRawHeader((_ntr-1),_trhedn, false);

    lseek(_f,0,SEEK_SET);

    if(_inptrc) delete [] _inptrc;
    if(_inpsmp) delete [] _inpsmp;

    _inptrc =   new char [_trl];
    _inpsmp =   new float[_Ns];

    _Active=true;

    return;

ERR:
    _data.Clear(); _Nt=0; _Active=false;

    return;
}


bool CstFile::ReadBinHed()
{
// CST file desn't have Bin Header, get data from the first trace
/*
    memset(_binhed,0,400);

    if(_f<0) return false;

    byte buf[240];

    lseek64(_f,0,SEEK_SET);

    int cnt  = read(_f, buf , 240); if(cnt!=240) return false;

//  memcpy(_binhed,buf,400);

    int si   = ui2(buf, 117, _swap);
    int nsmp = ui2(buf, 115, _swap);

    int smpl = 4;

    int trl  = 240+nsmp*smpl;

    _Nt  = (_flen)/trl;
    _Format = 5;
    _smpl  = smpl;
    _trl   = trl;
    _Ns    = nsmp;
    _Si   = si/1000000.;
*/

    memset(_binhed,0,400);

    if(_f<0) return false;

    byte buf[400];
    int* hed=(int*)buf;

    lseek64(_f,0,SEEK_END);
    long long l = tell64(_f);

    lseek64(_f,0,SEEK_SET);

    int cnt  = read(_f, buf , 256); if(cnt!=256) return false;

    int frmt = ((unsigned) (hed[10])) >> 20;

    int smpl;

    if      (frmt == 1)  smpl = 2;
    else if (frmt == 2)  smpl = 2;
    else if (frmt == 3)  smpl = 4;
    else if (frmt == 4)  smpl = 4;
    else if (frmt == 7)  smpl = 4;
    else if (frmt == 8)  smpl = 4;
    else
    {
      //printf("Data fomat is unknown\n");
      return false;
    }

    int    nsmp = hed[9] / (hed[8] / 1000);
    double si   = hed[8]/1000000.;

    int trl = (nsmp * smpl) + 256;
    int tlb = trl / 512; if (trl % 512) tlb++; tlb *= 512;

    _Nt = l / tlb;

    _Format = frmt;
    _smpl   = smpl;
    _trl   = trl;
    _tlb   = tlb;
    _Ns    = nsmp;
    _Si    = si;


    return true;
}

void  CstFile::ReadTrace(long long ntr)
{
    int  n;

    if(!_f) return;

    lseek64(_f,ntr*_tlb,SEEK_SET);
    n = read(_f, _inptrc, _trl);

    byte*  buf = _data.PutObject(ntr);
    if(buf==NULL) return;

    float* sample = (float*)(buf+256);

    for(n=0;n<_Ns;n++) sample[n] = GetSample(n, _inptrc);

    memcpy(buf,_inptrc,256);

}

float  CstFile::GetSample(int idx, char* inptrc)
{
    int*    hed = (int*)inptrc;
    float   smp;
  //int*    xmp = (int*)&smp;
    float*  dat = (float*)(inptrc+256);

    short*       int2ptr = (short*)       dat;
//  int*         int4ptr = (int*)         dat;
    signed char* int1ptr = (signed char*) dat;

    int shift = hed[12];

    if(idx<0 && idx> _Ns) return 0;

    if(_Format==1)
     {
        smp = int2ptr[idx] * pow(2,shift);
     }
    else if(_Format==3)
     {
        smp =  ibm2ieee(dat[idx],0);
        //ibm2ieee(&smp,0);
        smp = smp * pow(2,shift);
     }
    else if(_Format==8)
     {
        smp =  dat[idx];
     }
    else if(_Format==5)
    {
        smp = int1ptr[idx] * pow(2,shift);
    }
    else if(_Format==2)
    {
        unsigned short v = int2ptr[idx];

        short d = v & 0xFFF0; d = d >> 4;
        short g = v & 0xF;

        try
        {
            smp = d * pow(2,15-g+shift);// * pow(2,shift);
        }
        catch (...)
        {
        }
    }
    else
        smp = 0;

    return smp;
}

int CstFile::ReadRawTrace(byte* &buf, long long ntr)
{
    if(!_f) return -3;

    if(ntr<0 || ntr>=_Nt) return -2;

    lseek64(_f,ntr*_tlb,SEEK_SET);

    buf = new byte[_trl];

    int cnt = read(_f, buf, _trl);

    if(cnt!=_trl)
    {
        delete [] buf; buf=NULL; return -1;
    }

    return _trl;
}


int  CstFile::WriteRawTrace(byte* buf, long long ntr)
{
    if(!_f) return -1;

    if(ntr<0 || ntr>=_Nt) return -1;

    lseek64(_f,ntr*_tlb,SEEK_SET);

    int cnt = write(_f, buf, _trl);

    if(cnt!=_trl) return -1;

    return 0;
}

float* CstFile::Rs(long long ntr)
{
    int  n;

    if(!_f) return NULL;

    lseek64(_f,ntr*_tlb,SEEK_SET);
    n = read(_f, _inptrc, _trl);

    for(n=0;n<_Ns;n++) _inpsmp[n] = GetSample(n, _inptrc);

    return _inpsmp;
}

void  CstFile::SetDefaultHdrDefs()
{
    int n;

    char s[2048];

    static int fmt [66];
    static int pos[66];

    static QString desc[66];
    static QString name[66];

    fmt [0]=0;
    pos [0]=0;
    desc[0] = "Trace Index";
    name[0] = "TRCIDX";

    fmt [1]=2;
    pos [1]=5;
    desc[1] = "Shot Point Number";
    name[1] = "SP";

    fmt [2]=4;
    pos [2]=13;
    desc[2] = "CDP Number";
    name[2] = "CDP";

    for(n=3;n<66;n++)
    {
        int i = n-3;
        fmt[n]   = 4;
        pos[n]    = (i)*4+1;
        sprintf(s, "W-%02d",i+1);
        desc[n]   = s;
        name[n]   = s;
    }

    Hdrs.clear();

    for(n=0;n<66;n++)
    {
        Hdrs.append(Hdri(name[n], desc[n], fmt[n],pos[n]));
    }

    for(n=0;n<66;n++) Hidx[Hdrs[n].name]=n;
}
