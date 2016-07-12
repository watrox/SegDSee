#ifndef MYPTRLIST_H
#define MYPTRLIST_H

#include <QObject>
#include <QVector>

class MyObjPtrList : public QObject
{
    Q_OBJECT
protected:

    QVector<QObject*>items;

public:
    explicit MyObjPtrList(QObject *parent = 0);

    ~MyObjPtrList();


    int count() { return items.count(); }

    QObject* operator[](int i)
    {
        return items.value(i,NULL);
    }

    void append(QObject* v)
    {
        items.append(v);
    }

    void clear()
    {
        int n;

        for(n=0;n<items.count();n++)
        {
            if(items[n]) delete items[n];
        }

        items.clear();
    }

    
signals:
    
public slots:
    
};

#endif // MYPTRLIST_H
