#ifndef GFXSRCSEISMEMSEGD_H
#define GFXSRCSEISMEMSEGD_H

#include "gfxsrcseismem.h"

class GfxSrcSeisMemSegd : public GfxSrcSeisMem
{
    Q_OBJECT

protected:

    char*  _ti;  // trace is read
    int*   _cs;
    int*   _ch;


   virtual void  setSmp(int nt, int ns, float v);

   virtual float getSmp(int nt, int ns);

   virtual float* Ts(int tidx);

public:

   void  Reset(); // set all traces as unread

   virtual void  setNt(int nt);

   void  Set(int tidx, int ncs, int nch)
   {
        if(tidx<0 || tidx>=m_Nt) return;

        _ti[tidx] = 0;
        _ch[tidx] = nch;
        _cs[tidx] = ncs;
   }


     GfxSrcSeisMemSegd(QObject *parent=0);
    ~GfxSrcSeisMemSegd();

signals:
  void smpRequest(int ncs, int nch, float* smp, char& rc);

};

#endif // GFXSRCSEISMEMSEGD_H
