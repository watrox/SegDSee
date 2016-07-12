#include "gfxobjseissect.h"
#include "gfx.h"
#include "gfxutil.h"
#include "util2.h"

GfxObjSeisSect::GfxObjSeisSect(QObject *parent) :
    GfxObjSeis(parent)
{
    m_SelTr   = -1;

    _si=NULL;
    _sj=NULL;

    m_Mode = 0;

}

GfxObjSeisSect::~GfxObjSeisSect()
{
    if(_si) delete [] _si;  _si=NULL;
    if(_sj) delete [] _sj;  _sj=NULL;
}

void   GfxObjSeisSect::DoDraw()
{
    double p;

    int xa = x2pix(m_X1);
    int xb = x2pix(m_X2);

    int ya = y2pix(m_Y1);
    int yb = y2pix(m_Y2);

    if(!s_src || s_src->Nt()<1)
    {
        gfx->DrawRect(xa,ya,xb,yb,0);
        return;
    }

    gfx->SetPalette(m_Pal);

    int    o;

    double xp,xc;

    double pl,pr;
    int    nl,nr;

    int    np, nc, n1, n2, clp;

//  int xl = gfx->Xo();
//  int xr = xl+gfx->W()-1;

    xp = 0;

    pl = pix2x(L);
    pr = pix2x(R);

//    printf("L=%d R=%d\n",xl,xr); fflush(stdout);

    nl = s_src->Tx(pl);
    nr = s_src->Tx(pr);
    n1 = s_src->Tx(m_X1);
    n2 = s_src->Tx(m_X2);

    //printf("n1=%d n2=%d\n",n1,n2); fflush(stdout);

    if(nl>nr) { int tmp = nr; nr=nl; nl=tmp; }
    if(n1>n2) { int tmp = n2; n2=n1; n1=tmp; }

    if(nl<0)                  nl=0;
    if(nr<0)                  nr=s_src->Nt()-1;
    if(n1<0)                  n1=0;
    if(n2<0)                  n2=s_src->Nt()-1;

    int lx = 0;
    int rx = R-L+1;

    int tw = m_Tw*m_Xs*xpmm/2;

//    Drawing in Color
    if(m_DispCol)
    {
      if(s_src->Nt()==1)
      {
        xc = x2pix(s_src->Tp(0));
        float* sc = s_src->Ts(0);

        if( sc && PointInRange(n1,n2,0))
              gfx->Draw2ColorTraceTB
                 (  xc-tw, _si, _sj, _nj, sc,
                    xc+tw, _si, _sj, _nj, sc,
                    lx, rx,
                    m_Gc,1);
      }
      else
      {
        int nn=0;

        //printf("nl=%d nr=%d\n",nl,nr); fflush(stdout);

        for(o=1,nc=nl-2;nc<nr+2;nc++)
         {
          p = s_src->Tp(nc);
          xc = x2pix(p);


          if(o)
          {
              o=0; xp=xc; np=nc; continue;
          }

          if(xc!=xp && PointInRange(n1,n2,nc))
          {
           float* sc = s_src->Ts(nc);
           float* sp = s_src->Ts(np);

           //printf("a=%d\n",nn); fflush(stdout);

           if( sc &&  sp && PointInRange(n1,n2,nc))
           {
              //printf("p=%g\n",p); fflush(stdout);
              gfx->Draw2ColorTraceTB
                 (  xp, _si, _sj, _nj, sp,
                    xc, _si, _sj, _nj, sc,
                    lx, rx,
                    m_Gc,1);
              //printf("b=%d\n",nn); fflush(stdout);
           }

          nn++;

          xp=xc; np=nc;
          }
        }
      }
    }

//    Drawing Wiggle

    double wg = m_Gw*m_Tw*m_Xs*xpmm;
    clp=m_Tw*m_Xs*xpmm*10;

    int _fil=m_WFill;

    int _fc=m_WFcolor;
    int _wc=m_WLcolor;

    xp = x2fpix(s_src->Tp(n1));
    xc = x2fpix(s_src->Tp(n1+1));

    int step = 4 / fabs(xp-xc);
    if(step<1) step=1;

    if(m_DispWig)
    {
     nc=nl-10;

     int na = nl/step-10;
     int nb = nr/step+10;

     na = na*step;
     nb = nb*step;

      for(o=1,nc=na;nc<nb;nc+=step)
      {
         xc = x2pix(s_src->Tp(nc));

         float* sc = s_src->Ts(nc);


         if( sc && PointInRange(n1,n2,nc))
              gfx->DrawWiggleTraceTB
               (xc, _si, _sj, _nj, sc, _fil, wg*step, clp*step,_fc,_wc);
      }
    }

    if(m_SelTr>=0 && m_SelTr<s_src->Nt())
    {
     int  nc    = m_SelTr;
     int  color = m_SLcolor;

     float* sc = s_src->Ts(nc);
     xc = x2pix(s_src->Tp (nc));

     if( PointInRange(xa,xb,xc) )
              gfx->DrawWiggleTraceTB
               (xc, _si, _sj, _nj, sc, 0, wg*step, clp*step,_fc,color);
    }

}

//---------------------------------------------------------------------------

void   GfxObjSeisSect::DoPreset()
{
    int i, j, ns1,ns2,nso,nsn;

    if(!s_src) return;

    double to = s_src->To();
    double si = s_src->Si();
    int    ns = s_src->Ns();

    if(ns<=0 || si<=0)
    {
        ns=0; _nj=0; return;
    }

    nso = ns1 = (m_Y1-to)/si;
    nsn = ns2 = (m_Y2-to)/si;

    if(nso<0) nso=0; if(nso>ns) nso=ns;
    if(nsn<0) nsn=0; if(nsn>ns) nsn=ns;

    if(nso==nsn)
    {
        _ns=0; _nj=0; return;
    }

    double ta=(nso*si)+to;
    double tb=(nsn*si)+to;

//    double s = si*m_Ys*ypmm;

    int so = y2pix(ta); //(to-m_Y1)*m_Ys*ypmm;   ;//nso*s;   //(to-_t1)*_ss;
    int sn = y2pix(tb); //*m_Ys*ypmm;   // int sn = nsn*s;   //(tn-_t1)*_ss;

    if(_si) delete [] _si;  _si=NULL;
    if(_sj) delete [] _sj;  _sj=NULL;

    _ns=abs(nsn-nso)+1;

    if(_ns<=0) return;

    _si = new int [_ns]; _sj = new int [_ns];

    Bresenham(nso, so, nsn, sn, _si, _sj);

    int sjp;

    for(i=j=0;i<_ns;i++)
    {
        if(i==0 || _sj[i]!=sjp)
        {
            sjp=_sj[j]=_sj[i]; _si[j]=_si[i]; j++;
        }
    }

    _nj=j;
    _preset=false;
}

//---------------------------------------------------------------------------

float  GfxObjSeisSect::getSelMaxAmp()
{
    float a, amax = 0;
    int    n;

    if(m_SelTr<0 || s_src->Nt()<1) return 0;

    int  nc    = m_SelTr;
    float* sc = s_src->Ts(nc); if(sc==NULL) return 0;

    for(n=0;n<s_src->Ns();n++)
    {
        a=fabs(sc[n]);
        if(amax < a) amax=a;
    }

    return amax;
}
