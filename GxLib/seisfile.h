#ifndef SEISFILE_H
#define SEISFILE_H

#include <string>

#include <QObject>
#include <QMap>
#include <QDebug>


#include "util2.h"
#include "hdrdef.h"

using namespace std;

class DataStorage
{
private:
    int  _nobj;  //number of objects;
    int  _strl;  //number of real items in storage;
    int  _olen;  //object length

    int*  _obji;  //object index -1 (not in storage)
    int*  _stri;  //index of object of this storage item
    byte* _obuf;  //storage memory

    int   _next;

public:
    DataStorage& Assign (const DataStorage &src);

    DataStorage();
    DataStorage(DataStorage& src) { Assign(src); }

    ~DataStorage();

    DataStorage& operator= (const DataStorage &src) { return Assign(src); }

    void Init(int nobj, int olen, int strl);
    void Clear();
    void Reset();

    byte* GetObject(int n);
    byte* PutObject(int n);
};


class SeisFile : public QObject
{
    Q_OBJECT

protected:

    DataStorage _data;

    QList<QObject*> _links;

    QList<Hdri>       Hdrs;
    QMap<QString,int> Hidx;

    double _Co; // CDP on first trace;
    double _Po;
    double _Pm;

    double _Si;
    double _Ti;
    int    _Nt;
    int    _Ns;
    int    _Format;

    bool   _Active;

    string _Fname;

    int    _smpl;
    int    _trl;
    int    _tlb;
    int    _rd_only;

    float* _inpsmp;
    char*  _inptrc;
    int    _f;
    int    _hlen;  //length of trace header (bytes)
    int    _swap;
    int    _cgg;
    int    _su;


    int    _txtcode; // 0 - ascii, 1 - ebcdic

    off64_t _flen;

    char   _txthed[3200];
    byte   _binhed [400];

    virtual void  Open(string /*fn*/) {}
    virtual void  Close()             {}

    virtual QString getType() { return "BaseType"; }

//  virtual void Close() { qDebug() << "SeisFile::Close()"; }
    void Invalidate();

public:
    explicit SeisFile(QObject *parent = 0);

    virtual ~SeisFile();

    void RegisterLink  (QObject* v);
    void UnRegisterLink(QObject* v);

    bool AddTrcHeader(QString n, QString d, int f, int p, QString expr);

    virtual QString BinHedValue(int/* idx*/) {return 0;}
    virtual QString BinHedName (int/*  idx*/) {return 0;}
    virtual QString BinHedBytes(int/*  idx*/) {return 0;}
    virtual int     BinHedCount()        {return 0;}

    //   virtual QString TrcHedValue(int idx) {return 0;}
    //    virtual QString TrcHedName (int idx) {return 0;}
    //    virtual QString TrcHedSnam (int idx) {return 0;}
    //    virtual QString TrcHedBytes(int idx) {return 0;}
    //    virtual int     TrcHedCount()        {return 0;}

    QString TxtHed()
    {
        int n;
        QString hed;
        char    lin[81];

        lin[80]=0;

        for(n=0;n<40;n++)
        {
            memcpy(lin,_txthed+n*80,80);
            hed = hed + lin + QString("\n");
        }

        return hed;
    }

    void Reset()
    {
        _data.Reset(); Invalidate();
    }


    void setFname(string fname)
    {
        _Fname = fname;
    }

    void setFname(QString fname)
    {
        QByteArray qfn = fname.toLocal8Bit();
        _Fname = qfn.data();
    }

    string Fname()
    {
        return _Fname;
    }

    int Swap()
    {
        return _swap;
    }

    inline bool  Active ()
    {
        return  _Active;
    }

    virtual void  setActive (bool v)
    {
        if(!v) Close();
        else   Open(_Fname);
        Invalidate();
    }

    virtual int  Nt()
    {
        return _Nt;
    }

    virtual int  Ns()
    {
        return _Ns;
    }

    virtual double  Si()
    {
        return _Si;
    }

    virtual double  Ti()
    {
        return _Ti;
    }

    virtual double  Co()
    {
        return _Co;
    }

    virtual double  Po()
    {
        return _Po;
    }

    virtual void  setPo (double v)
    {
        _Po=v; Invalidate();
    }

    virtual double  Pm()
    {
        return _Pm;
    }

    virtual void  setPm (double v)
    {
        if(v==0) v=1;
        _Pm=v; Invalidate();
    }

    virtual double  Tp (long long tidx)
    {
        return tidx*_Pm+_Po;
    }

    virtual double  Tmin ()
    {
        return 0;
    }

    virtual double  Tmax ()
    {
        return _Si*_Ns;
    }

    virtual void   ReadTrace(long long /*ntr*/)
    {
    }

    virtual int ReadRawTrace(byte* &buf, long long/* ntr*/)
    {
        buf = NULL; return 0;
    }

    virtual int WriteRawTrace(byte* /*buf*/, long long/* ntr*/)
    {
        return 0;
    }


    virtual bool ReadTxtHed()
    {
        memset(_txthed,' ',3200);
        _txtcode=0;
        return true;
    }

    virtual bool ReadBinHed()
    {
        memset(_binhed,0,400);
        return true;
    }

    virtual float*  Rs(long long /*ntr*/)
    {
        return NULL;
    }

    virtual float*  Ts (long long /*tidx*/)
    {
        return NULL;
    }

    virtual double Tx(double p)
    {
        double ip = (p-_Po)/_Pm;

        if(ip <= 0     ) return 0;
        if(ip >  _Nt-1 ) return _Nt-1;
        return ip;
    }

    virtual double Th(long long tidx, int nitm);

    double Th(long long tidx, QString name);

    virtual Hdri*  Hd(int nitm);
    virtual int    Hn();

    byte* Tb (long long ntr); // Get trace with header

signals:
    void execProc(SeisFile* sf, int ns, double si, float* smp);

public slots:

};

#endif // SEISFILE_H
