#include "gfxsrcfile.h"

GfxSrcFile::GfxSrcFile(QObject *parent) :
    GfxSrcSeis(parent)
{
//  qDebug() << "GfxSrcFile()";
    m_Nt   =  0;
    m_Sfile = NULL;
}
