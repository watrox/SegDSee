#include "readerthread.h"

#include "util2.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#ifdef _WIN32
 #include <io.h>
#endif

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>

ReadThread::ReadThread(QObject *parent) :
    QThread(parent)
{

}

//---------------------------------------------------------------------------------------------

SgdReadThread::SgdReadThread(QObject *parent) :
    ReadThread(parent)
{
    stop=true;
}

void SgdReadThread::Read(QString Fname, QObject* win)
{
    stop=false;

    qfn = Fname.toLocal8Bit();
    fname = qfn.data();
    receiver = win;
    segd = new TSegD();
    start();
 }

void SgdReadThread::run()
{
    SgdReadEvent* e;

    int pos, cnt;
    int pers;
    int bsz = 1024*1024;

    segd->Clear();

    long long fsz = getfilesize64(fname);

    int inpf = open(fname,O_BINARY | O_RDONLY);

    char* buf = new char[fsz];

    if(inpf<0) { stop = true; goto NOP; }

    for(pos=0;;)
    {
        if(stop) break;

        bsz=fsz-pos;
        if(bsz>1024*1024) bsz=1024*1024;

        cnt = read(inpf, buf+pos, bsz);
        if(cnt<1) break;
        pos+=cnt;
        pers = (int)((100.*pos)/fsz);

        e = new SgdReadEvent (); e->pers=pers;
        QCoreApplication::postEvent(receiver, e);
    }

    ::close(inpf);

NOP:
    if(stop)
    {
        delete [] buf;
        goto END;
    }

    segd->Parse(buf,fsz,false,&stop);

    if(stop)
    {
        segd->Clear();
    }

END:
    e = new SgdReadEvent (); e->pers=-1;

    if(!stop) e->segd = segd;

    QCoreApplication::postEvent(receiver, e);
}

//---------------------------------------------------------------------------------------------

LtdReadThread::LtdReadThread(QObject *parent) :
    ReadThread(parent)
{
}

void LtdReadThread::Read(QString Fname, long long Pos, long long Len, QObject* win)
{
    stop=false;

    qfn = Fname.toLocal8Bit();
    fname = qfn.data();
    receiver = win;
    segd = new TSegD();
    lpos  = Pos;
    llen  = Len;
    start();
 }

void LtdReadThread::run()
{
    int pos, cnt;
    int pers;

    SgdReadEvent *e;

    char* buf = new char[llen];

    int inpf = open(fname,O_BINARY | O_RDONLY);
    if(inpf<0) { stop = true; goto NOP; }

    lseek64(inpf,lpos,SEEK_SET);

    for(pos=0;;)
    {
        if(stop) break;

        cnt=ltod_read(inpf,buf+pos, llen-pos);

        if(cnt==0 || cnt==-2) break;

        pos+=cnt;
        pers = (int)((100.*pos)/llen);

        if(pos>=llen) break;

        e = new SgdReadEvent (); e->pers=pers;
        QCoreApplication::postEvent(receiver, e);
    }

    ::close(inpf);

NOP:
    segd->Clear();

    if(stop)
    {
        delete [] buf;
        goto END;
    }

    segd->Parse(buf,llen,false,&stop);

    if(stop)
    {
        segd->Clear();
    }

END:
    e = new SgdReadEvent (); e->pers=-1;

    if(!stop) e->segd = segd;

    QCoreApplication::postEvent(receiver, e);
}

//---------------------------------------------------------------------------------------------


DirScanThread::DirScanThread(QObject *parent) :
    ReadThread(parent)
{
}

void DirScanThread::SendEvent(int pers)
{
    DirScanEvent   *e;

    e = new DirScanEvent (); e->pers=pers;
    QCoreApplication::postEvent(receiver, e);
}


void DirScanThread::run()
{
    DirItem   ditem;
    int       nr, n,l, pers, ppers;

    ppers = -1;

    QDir dir(dname);
    dlist.clear();

    QFileInfoList list = dir.entryInfoList();

    l = list.size();

    dlist.clear();

    for(nr=n=0; n < l; n++)
    {
        QFileInfo fileInfo = list.at(n);

        if(!fileInfo.isDir()       ) continue;
        if(fileInfo.fileName()==".") continue;

        ditem.ftype = 0;
        ditem.fname = fileInfo.fileName();

        if(fileInfo.fileName()=="..") dlist.prepend(ditem);
        else                          dlist.append (ditem);

        nr++;

        pers = (int)((100.*nr)/l);

        if(pers!=ppers)
        {
            ppers=pers;
            SendEvent(pers);
        }
    }

    for(n=0; n < l; n++)
    {
        if(stop) break;

        QFileInfo fileInfo = list.at(n);

        if(fileInfo.isDir()) continue;

        pers = (int)((100.*nr)/l);
        if(pers!=ppers)
        {
            ppers=pers;
            SendEvent(pers);
        }

        ditem.ftype = -1;
        ditem.fname = fileInfo.fileName();

        dlist.append(ditem); nr++;
    }

    if(stop)
    {
        dlist.clear();
        SendEvent(-2);
    }
    else
    {
        SendEvent(-1);
    }
}

void DirScanThread::Read(QString Dname, QObject* win)
{
    stop=false;

    dname = Dname;
    receiver = win;
    start();
}

//---------------------------------------------------------------------------------------------


FileCheckThread::FileCheckThread(QObject *parent) :
    ReadThread(parent)
{
}

void FileCheckThread::SendEvent(int row, DirItem di, int pers)
{
    FileCheckEvent   *e;

    e = new FileCheckEvent ();
    e->pers=pers;
    e->nrow=row;
    e->di  =di;
    QCoreApplication::postEvent(receiver, e);
}



DirItem FileTypeCheck(QString Dname, QString Fname)
{
    int     type;
    DirItem ditem;

    QString    fn = Dname + "/" + Fname;
    QByteArray qfn = fn.toLocal8Bit();
    char* fname = qfn.data();

    QFileInfo fileInfo(fname);

    ditem.fname = Fname;

    if(fileInfo.isDir())
    {
        ditem.ftype=0;
        return ditem;
    }

    int inpf = open(fname,O_BINARY|O_RDONLY);
    if(inpf>0)
    {
        if(is_ltod(inpf))
        {
            type = ck_ltod(inpf,0);
            type = ck_ltod(inpf,0);

            if(type>9999) type=type-10000;

            ditem.ftype = 2;
            ditem.type  = type;
        }
        else
        {
            int ffid;
            int frmt = is_segd_f(inpf, ffid,0);

            if(frmt)
            {
                ditem.ftype = 1;
                ditem.type  = frmt;
                ditem.ffid  = ffid;
            }
            else
            {
                ditem.ftype = 4;
                ditem.type  = 0;
            }
        }
        close(inpf);
    }
    else
    {
        if(errno==ENOENT)
        {
            ditem.ftype = 5;
            ditem.type  = 0;
        }
        else
        {
            ditem.ftype = 6;
            ditem.type  = 0;
        }

    }

    return ditem;
}



void FileCheckThread::run()
{
    int       nr, pers;
    DirItem   ditem;

    int ni = dlist.count();

    sstr = "Checking file types...";

    for(nr=0;nr<ni;nr++)
    {

        if(stop) break;

        DirItem di = dlist[nr];
        ditem = di;

        pers = (int)((100.*nr)/ni);

        if(di.ftype==-1)
        {
            ditem = FileTypeCheck(dname, di.fname);
        }

        SendEvent(nr,ditem,pers);
    }

    sstr = "";

    if(stop)
    {
        SendEvent(-2,ditem,0);
    }
    else
    {
        SendEvent(-1,ditem,0);
    }
}

void FileCheckThread::Read(QString Dname, QList<DirItem> Dlist, QObject* win)
{
    stop=false;

    dname = Dname;
    dlist  = Dlist;
    receiver = win;
    start();
}

//---------------------------------------------------------------------------------------------

LtdScanThread::LtdScanThread(QObject *parent) :
    ReadThread(parent)
{
}

void LtdScanThread::SendEvent(int pers)
{
    LtdScanEvent   *e;

    e = new LtdScanEvent (); e->pers=pers;
    QCoreApplication::postEvent(receiver, e);
}


void LtdScanThread::run()
{
    DirItem   ditem;

    long long fsz;
    long long pos;
    long long rpos;
    long long len;

    int       ffid = 0;

    int       nr  = 0;
    int       nf;
    int       cnt;
    int       eof = 0;
    int       pers, ppers;

    int t1 = time(NULL);
    int t2;

    dlist.clear();

    nr=1;

    int inpf = open(fname,O_RDONLY|O_BINARY);
    if(inpf<0) return;

    ditem.ftype = 0;
    ditem.fname = ".";

    dlist.append(ditem);

    SendEvent(0);
    SendEvent(-10);

    fsz = lseek64(inpf,0,SEEK_END);
    lseek64(inpf,0,SEEK_SET);

    pos=0; ppers=-1;

    int type;

    sstr = "Scanning LTOD image...";

    for(len=0,nf=1,nr=0;;)
    {
        if(stop) break;

        cnt=ltod_read(inpf,NULL);

        if(cnt==0 || cnt==-2)
        {
            rpos=tell64(inpf);
            pers = (100.*rpos)/fsz;

            if(pers!=ppers)
            {
                ppers=pers;
                SendEvent(pers);
            }

            if(eof)
            {
                break;
            }

            eof=1;
            nr=0;

            type=ck_ltod(inpf, pos, &ffid);

            if(type>9999) type=type-10000;

            ditem.ftype = 3;
            ditem.fname = Tprintf("%d",nf);
            ditem.type  = type;
            ditem.ffid  = ffid;
            ditem.llen  = len;
            ditem.lpos  = pos;

            dlist.append(ditem);

            int ni = dlist.length();

            t2 = time(NULL);

            if(ni && t2-t1>2)
            {
                SendEvent(-11);
                t1 = t2;
            }

            pos=tell64(inpf);

            nf++;
            len=0;
        }
        else
        {
            eof=0;
            nr++;
            len+=cnt;
        }
    }

    close(inpf);

    sstr = "";

    if(stop)
    {
        dlist.clear();
        SendEvent(-2);
    }
    else
    {
        SendEvent(-1);
    }
}


//---------------------------------------------------------------------------------------------

void LtdScanThread::Read(QString Fname, QObject* win)
{
    stop=false;

    qfn = Fname.toLocal8Bit();
    fname = qfn.data();
    receiver = win;
    start();
}
