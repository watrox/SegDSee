#ifndef SGYFILE_H
#define SGYFILE_H

#include "seisfile.h"

class SgyFile : public SeisFile
{
    Q_OBJECT

protected:

    virtual void  Open (string fn);
    virtual void  Close();

    virtual void   SetDefaultHdrDefs();

    virtual float*  Rs(long long ntr);

    virtual bool ReadTxtHed();

    virtual bool   ReadBinHed();

    virtual void   ReadTrace(long long ntr);

    virtual float   GetSample(int idx, char* inptrc);
public:
    explicit SgyFile(QObject *parent = 0);
    virtual ~SgyFile();

    virtual float*  Ts (long long ntr);

    virtual QString getType() { return "SEG-Y"; }

    int ReadRawTrace(byte* &buf, long long ntr);
    int WriteRawTrace(byte* buf, long long ntr);

    virtual QString BinHedValue(int idx);
    virtual QString BinHedName (int idx);
    virtual QString BinHedBytes(int idx);
    virtual int     BinHedCount();

signals:

public slots:

};

class SuFile : public SgyFile
{
    Q_OBJECT

protected:

    virtual void  Open (string fn);
    virtual void  Close();

    virtual float*  Rs(long long ntr);

    virtual bool   ReadBinHed();

    virtual void   ReadTrace(long long ntr);

public:
    explicit SuFile(QObject *parent = 0);
    virtual ~SuFile();

    int ReadRawTrace(byte* &buf, long long ntr);
    int WriteRawTrace(byte* buf, long long ntr);

    virtual QString getType() { return "SU"; }

signals:

public slots:

};


class CstFile : public SgyFile
{
    Q_OBJECT

protected:

    virtual void  Open (string fn);
    virtual void  Close();

    virtual float*  Rs(long long ntr);

    virtual bool   ReadBinHed();

    virtual void   ReadTrace(long long ntr);

    virtual void   SetDefaultHdrDefs();

public:

    explicit CstFile(QObject *parent = 0);
    virtual ~CstFile();

    virtual QString getType() { return "CST"; }

    int ReadRawTrace(byte* &buf, long long ntr);
    int WriteRawTrace(byte* buf, long long ntr);

    virtual float   GetSample(int idx, char* inptrc);

signals:

public slots:

};




#endif // SGYFILE_H
