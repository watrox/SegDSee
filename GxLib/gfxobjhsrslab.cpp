#include "gfxobjhsrslab.h"

GfxObjHsrsLab::GfxObjHsrsLab(QObject *parent) :
    GfxObj(parent)
{

}

GfxObjHsrsLab::~GfxObjHsrsLab()
{

}


void GfxObjHsrsLab::setHdrs(QList<QString> v)
{
    m_hdrs = v;

    Preset();
}

void   GfxObjHsrsLab::DoDraw()
{
   if(m_view==NULL) return;

   int xa = 0;
   int xb = m_view->width()-1;

   int ya = 0;
   int yb = m_view->height()-1;

   int ht  = gfx->GetTextHeight();

   int nh, y;

   for(nh=0;nh<m_hdrs.count();nh++)
   {
     y = yb - (nh + 1) * (ht + 2) -2 ;

     gfx->DrawText (2,y+ht-4,m_hdrs[nh]);
   }

   gfx->DrawRect(xa,ya,xb,yb,0);
   //gfx->DrawLine(xa,ya,xb,yb,0);
}

