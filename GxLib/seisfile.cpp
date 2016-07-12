#include "gfxsrcfile.h"
#include "seisfile.h"

#include <QDebug>

//---------------------------------------------------------------------------

DataStorage::DataStorage()
{
    _nobj = 0;
    _obji = NULL;
    _obuf = NULL;
    _stri = NULL;
    _next = 0;
}


DataStorage::~DataStorage()
{
    if(_obji) delete [] _obji;
    if(_obuf) delete [] _obuf;
    if(_stri) delete [] _stri;
}

void DataStorage::Clear()
{
    if(_obji) delete [] _obji;
    if(_obuf) delete [] _obuf;
    if(_stri) delete [] _stri;
    _nobj = 0;
    _obji = NULL;
    _obuf = NULL;
    _stri = NULL;
    _next = 0;
}


void DataStorage::Init(int nobj, int olen, int strl)
{
    _nobj = nobj;
    _olen = olen;
    _strl = strl;

    _next = 0;

    if(_obji) delete [] _obji;
    if(_obuf) delete [] _obuf;
    if(_stri) delete [] _stri;

    _stri = new int  [_strl];
    _obji = new int  [_nobj];
    _obuf = new byte [_strl*_olen];

    memset(_obji,0xFF,_nobj*sizeof(int));
    memset(_stri,0xFF,_strl*sizeof(int));
}


void DataStorage::Reset()
{
    memset(_obji,0xFF,_nobj*sizeof(int));
    memset(_stri,0xFF,_strl*sizeof(int));
}


byte *DataStorage::GetObject(int n)
{
    //int n1;
    if(n>=_nobj)   return NULL;

    if(_obji[n]<0) return NULL;
    else           return _obuf+_obji[n]*_olen;
}

byte *DataStorage::PutObject(int n)
{
    if(n>=_nobj) return NULL;

    if(_obji[n]<0)
    {
        if(_stri[_next]>=0)
            _obji[_stri[_next]]= -1;

        _obji[n]=_next;
        _stri[_next]=n;
        _next++; if(_next>=_strl) _next=0;
    }
    return _obuf+_obji[n]*_olen;
}


DataStorage& DataStorage::Assign (const DataStorage &src)
{
    if (this ==&src)return *this;

    Clear();

    _nobj = src._nobj;
    _olen = src._olen;
    _strl = src._strl;

    _next = 0;

    if(_obji) delete [] _obji;
    if(_obuf) delete [] _obuf;
    if(_stri) delete [] _stri;

    _stri = new int  [_strl];
    _obji = new int  [_nobj];
    _obuf = new byte [_strl*_olen];

    memset(_obji,0xFF,_nobj*sizeof(int));
    memset(_stri,0xFF,_strl*sizeof(int));

    return *this;
}
//---------------------------------------------------------------------------

SeisFile::SeisFile(QObject *parent) :
    QObject(parent)
{
    _f    = -1;
    _hlen = 0;
    _swap = 0;

    _Fname  = "";
    _Active =  false;
    _Nt     =  0;
    _Ns     =  0;

    _Co     =  0;
    _Po     =  0;
    _Pm     =  1;

    _f       = -1;
    _hlen    =  0;
    _swap    =  0;
}

SeisFile::~SeisFile()
{
    int n;

    //qDebug() << "~SeisFile()";

    for(n=0; n<_links.count();n++)
    {
        GfxSrcFile* f = (GfxSrcFile*)_links[n];
        f->setSfile(NULL);
    }

    if(_f>=0) close(_f);

    if(_inptrc) delete [] _inptrc; _inptrc=NULL;
}


void SeisFile::RegisterLink  (QObject* v)
{
    _links.append(v);
//    update();
}

void SeisFile::UnRegisterLink(QObject* v)
{
    _links.removeAll(v);
//    update();
}

void SeisFile::Invalidate()
{
    for(int n=0; n<_links.count();n++)
    {
        GfxSrcFile* src = (GfxSrcFile*)_links[n];
        src->Invalidate();
    }
}


byte* SeisFile::Tb (long long ntr) // Get trace with header
{
  byte*  buf;

  if(ntr<0)      return NULL;
  if(ntr>=_Nt)  return NULL;

  //if(_slist.Count>0) ntr=_slist[ntr];

  buf  = _data.GetObject(ntr);

  if(buf) return buf;

  ReadTrace(ntr);

  buf = _data.GetObject(ntr);

  if(buf)
  {
    float* trace = (float*)(buf+_hlen);

    execProc(this,_Ns, _Si, trace);

    return buf;
  }

  else    return NULL;
}



double SeisFile::Th(long long tidx, int nitm)
{
    byte*  buf;
    double v;

    if(nitm<0 || nitm>=Hdrs.size()) return 0;

    buf = Tb(tidx);

    if(buf==NULL) return 0;

    Hdri hi = Hdrs[nitm];

    int        frmt = hi.frmt;
    int        hpos = hi.pos;
//  ExprEval*  e    = FTrHdrDef->Expr[nitm];

/*
    if(e->Expression!="")
    {
        e->TraceHdr = buf;
        e->TraceIdx = tidx+1;//GetRawTraceNumber(ntr)+1;
        e->TraceCnt = FNt;
        v = e->Eval();
    }
*/
    if(frmt==0)
        v = tidx+1;
    else
        v = iFmt(frmt,buf,hpos,_swap);

    return v;
}

double SeisFile::Th(long long tidx, QString name)
{
    int n;

    if(Hidx.contains(name)) n=Hidx[name];
    else                    n=0;

    return Th(tidx,n);
}


#ifdef _MSC_VER
    #pragma warning( push )
    #pragma warning( disable : 4100 ) // implicit conversion, possible loss of data
#else
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

bool SeisFile::AddTrcHeader(QString n, QString d, int f, int p, QString expr)
{
    return false;
}

#ifdef _MSC_VER
    #pragma warning( pop )
#else
    #pragma GCC diagnostic warning "-Wunused-parameter"
#endif

Hdri* SeisFile::Hd(int nitm)
{
    if(nitm<0 || nitm>=Hdrs.size()) return NULL;

    return &(Hdrs[nitm]);
}

int SeisFile::Hn()
{
    return Hdrs.size();
}
