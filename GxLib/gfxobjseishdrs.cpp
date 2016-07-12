#include <stdio.h>

#include "gfxobjseishdrs.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

GfxObjSeisHdrs::GfxObjSeisHdrs(QObject* parent) :
    GfxObjSeis(parent)
{
}

void GfxObjSeisHdrs::setHdrs(QList<QString> v)
{
    m_hdrs = v;

    Preset();
}

void GfxObjSeisHdrs::DoDraw()
{
    if(m_view==NULL) return;

    double X1 = min(m_X1,m_X2);
    double X2 = max(m_X1,m_X2);

    //double Y1 = min(m_Y1,m_Y2);
    //double Y2 = max(m_Y1,m_Y2);

    int x1 = x2pix(X1);
    int x2 = x2pix(X2);

    int y1 = 0;        //   y2pix(Y1);
    int y2 = gfx->H(); //   y2pix(Y2);

    gfx->DrawRect(x1,y1,x2,y2-1,0); //QColor(255,0,255).rgb());
    //gfx->DrawLine(x1,y1,x2,y2,0 );

    //  return;

    if(!s_src || s_src->Nt()<1) return;

    int ht  = gfx->GetTextHeight();
    int wto = gfx->GetTextWidth ("123456");
    int wtc;

    double xp,xc;

    double pl,pr;
    int    nl,nr;

    int    nc, n1, n2;

    pl = pix2x(L);
    pr = pix2x(R);

    nl = s_src->Tx(pl);
    nr = s_src->Tx(pr);
    n1 = s_src->Tx(m_X1);
    n2 = s_src->Tx(m_X2);

    if(nl>nr) { int tmp = nr; nr=nl; nl=tmp; }
    if(n1>n2) { int tmp = n2; n2=n1; n1=tmp; }

    if(nl<0)                  nl=0;
    if(nr<0)                  nr=s_src->Nt()-1;
    if(n1<0)                  n1=0;
    if(n2<0)                  n2=s_src->Nt()-1;

     nc=nl-10;

     xp = x2fpix(s_src->Tp(n1));
     xc = x2fpix(s_src->Tp(n1+1));

     int step = 4 / fabs(xp-xc);

     if(step<1) step=1;

     int na = nl/step-10;
     int nb = nr/step+10;

     na = na*step;
     nb = nb*step;

     char   lab[1024];
     double v;

     int nh;

     for(nc=na;nc<nb;nc+=step)
      {
        if(nc<0 || nc>=s_src->Nt()) continue;

        xc = x2pix(s_src->Tp(nc));
        v  = s_src->Th(nc,0); sprintf(lab,"%g",v);

        gfx->DrawVLine(xc,y2-4,y2,0,0);
      }

     xp = x2fpix(s_src->Tp(n1));
     xc = x2fpix(s_src->Tp(n1+1));

     step = wto / fabs(xp-xc);

     if(step<1) step=1;

     na = nl/step-10;
     nb = nr/step+10;

     na = na*step;
     nb = nb*step;


     int y;

     for(nc=na;nc<nb;nc+=step)
     {
       if(nc<0 || nc>=s_src->Nt()) continue;

       for(nh=0;nh<m_hdrs.count();nh++)
       {
         xc = x2pix(s_src->Tp(nc));
//       v  = s_src->Th(nc,m_hdrs[nh]); sprintf(lab,"%g",v);
         v  = s_src->Th(nc,nh); sprintf(lab,"%g",v);
         wtc = gfx->GetTextWidth(lab)/2;

         y = y2 - (nh + 1) * (ht + 2) -2 ;

         gfx->DrawText (xc-wtc,y+ht-4,lab);
         gfx->DrawVLine(xc,y+ht-2,y+ht ,0,0);
       }
     }
}

