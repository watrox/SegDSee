#ifndef HDRDEF_H
#define HDRDEF_H

using namespace std;

#include <string>

#include <QObject>

static string _fname [] =
{
  "Integer 1",
  "Integer 2",
  "Integer 4",
  "IEEE Float 4",
  "IEEE Float 8",
  "IBM  Float 4"
};

static int _fcode [] =
{
  1,  // Integer 1
  2,  // Integer 2
  4,  // Integer 4
  14, // IEEE Float 4
  18, // IEEE Float 8
  24  // IBM  Float 4
};

class FormatList
{
public:

  static int code(int idx)
  {
      if(idx<0 || idx>= length()) return -1;
      return _fcode[idx];
  }

  static string* names()
  {
    return _fname;
  }

  static string name(int code)
  {
    int n;

    for(n=0;n<length();n++)
        if(_fcode[n]==code) return _fname[n];
      return "Unknown";
  }

  static int code(string name)
  {
    int n;

    for(n=0;n<length();n++)
        if(_fname[n]==name) return _fcode[n];
    return -1;
  }


  static int length()
  {
      return  sizeof(_fcode)/sizeof(int);
  }
};


class Hdri
{
public:

    int           frmt; // format array
    int           pos;  // position
    QString       desc; // description
    QString       name; // short name
//  ExprEval*      _expr;

public:

    QString bytesStr();

//  explicit Hdri(int f, int p, QString d, QString n);
    explicit Hdri(QString n, QString d, int f, int p, QString expr = "");
    explicit Hdri(QString n, QString d, QString expr);
};

/*
class HdrDef : public QObject
{
    Q_OBJECT

protected:

    int            _m;    // # number of standard headers

    int            _a;    // # of allocated items
    int            _l;    // # of used items
    int*           _frmt; // format array
    int*           _pos;  // position
    string*        _desc; // description
    string*        _name; // short name
//  ExprEval*      _expr;

    void setF(int idx, int val);
    void setP(int idx, int val);

    void setN(int idx, string val);
    void setS(int idx, string val);
    void setE(int idx, string val);

    string  Fnam  (int idx);

    int    F    (int idx);
    int    P    (int idx);

    string N(int idx);
    string S(int idx);
    string E(int idx);

//  ExprEval* getEval(int idx);

    double error(char *s);

    void AllocateMemory(int nitems);


public:
    explicit HdrDef(QObject *parent = 0);

    ~HdrDef();

    void Clear();
    void Init(int* fmt,int* pos, char* desc[],     char*       name[], int len);
    void Init(int* fmt,int* pos, string* desc, string* name,   int len);

    bool SaveToRegistry  ();
    bool LoadFromRegistry();
    int  FindByName(string name);

    void Mod(int f, int p, string n, string s, string e ,int i);
    void Add(int f, int p, string n, string s, string e);
    void Del(int idx);


signals:

public slots:

};
*/
#endif // HDRDEF_H
