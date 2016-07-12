#include "myobjptrlist.h"

MyObjPtrList::MyObjPtrList(QObject *parent) :
    QObject(parent)
{
}

MyObjPtrList::~MyObjPtrList()
{
    clear();
}
