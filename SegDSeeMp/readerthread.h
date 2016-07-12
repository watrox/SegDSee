#ifndef SgdReadThread_H
#define SgdReadThread_H

#include <QThread>
#include <QEvent>
#include <QCoreApplication>

#include "libSegD.h"

class DirItem
{
public:
    QString    fname;
    int        ftype;  // 0 - dir, 1 - Sgd, 2 - ltod, 3 - file in ltod
    int        ffid;
    int        type;   // Sgd type
    qint64     lpos;   // position in ltod
    qint64     llen;   // file length in ltod
};

//---------------------------------------------------------------------------------------------

class ReadThread : public QThread
{
    Q_OBJECT
public:

    QList<DirItem> dlist;
    QString        sstr;

    explicit ReadThread(QObject *parent = 0);

    virtual void Stop() { stop = true; }

protected:

    QObject*   receiver;
    bool       stop;

    virtual void run() {}
};

//---------------------------------------------------------------------------------------------

class SgdReadEvent : public QEvent
{
public:

    int pers;

    TSegD* segd;

    SgdReadEvent ()
        : QEvent((QEvent::Type)(QEvent::User + 1))
    {
        segd=NULL;
    }

};


class SgdReadThread : public ReadThread
{
    Q_OBJECT
public:
    explicit SgdReadThread(QObject *parent = 0);

    void Read(QString Fname, QObject *win);

protected:

    TSegD*     segd;
    QByteArray qfn;
    char*      fname;

    virtual void run();

};

//---------------------------------------------------------------------------------------------

class LtdReadThread : public ReadThread
{
    Q_OBJECT
public:
    explicit LtdReadThread(QObject *parent = 0);

    void Read(QString Fname, long long Pos, long long Len, QObject *win);

protected:

    TSegD*     segd;
    QByteArray qfn;
    char*      fname;
    long long  lpos;
    long long  llen;

    virtual void run();
};




class DirScanEvent : public QEvent
{
public:

    int pers;

    DirScanEvent ()
        : QEvent((QEvent::Type)(QEvent::User + 2))
    {
    }

};

class FileCheckEvent : public QEvent
{
public:

    int     pers;
    int     nrow;
    DirItem di;

    FileCheckEvent ()
        : QEvent((QEvent::Type)(QEvent::User + 4))
    {
    }

};

//---------------------------------------------------------------------------------------------

DirItem FileTypeCheck(QString Dname, QString Fname);

class FileCheckThread : public ReadThread
{
    Q_OBJECT
public:

    explicit FileCheckThread(QObject *parent = 0);

    void Read(QString Dname, QList<DirItem> Dlist, QObject *win);

protected:

    QString        dname;
    QList<DirItem> dlist;

    virtual void run();

    virtual void SendEvent(int row, DirItem di, int pers);
};

//---------------------------------------------------------------------------------------------


class DirScanThread : public ReadThread
{
    Q_OBJECT
public:

    explicit DirScanThread(QObject *parent = 0);

    void Read(QString Dname, QObject *win);

protected:

    QString    dname;

    virtual void run();

    virtual void SendEvent(int pers);
};

//---------------------------------------------------------------------------------------------

class LtdScanEvent : public QEvent
{
public:

    int pers;

    LtdScanEvent ()
        : QEvent((QEvent::Type)(QEvent::User + 3))
    {
    }

};


class LtdScanThread : public ReadThread
{
    Q_OBJECT
public:

    explicit LtdScanThread(QObject *parent = 0);

    void Read(QString Fname, QObject *win);

    void Stop()
    {
        stop = true;
    }

protected:

    char*      fname;
    QByteArray qfn;

    virtual void run();

    virtual void SendEvent(int pers);
};




#endif // SgdReadThread_H
