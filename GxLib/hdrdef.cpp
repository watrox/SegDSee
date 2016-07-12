#include "hdrdef.h"

/*
HdrDef::HdrDef(QObject *parent) :
    QObject(parent)
{
    _m = 0;
    _a = 0;
    _l = 0;
    _frmt = NULL;
    _pos  = NULL;
    _desc = NULL;
    _name = NULL;
//  _expr = NULL;

    Clear();
}

HdrDef::~HdrDef()
{
    Clear();
}

void HdrDef::AllocateMemory(int nitems)
{
    if(_a>=nitems) return;

    _a = nitems+100;

    int*        f = new int[_a];
    int*        p = new int[_a];
    string* n = new string[_a];
    string* s = new string[_a];
//  ExprEval*   e = new ExprEval  [_a];

    int i;

    for(i=0;i<_l;i++)
    {
        f[i]=_frmt[i];
        p[i]=_pos [i];
        n[i]=_desc[i];
        s[i]=_name[i];
//      e[i]=_expr[i];
    }

    if (_frmt) delete [] _frmt; _frmt=f;
    if (_pos ) delete [] _pos ; _pos =p;
    if (_desc) delete [] _desc; _desc=n;
    if (_name) delete [] _name; _name=s;
//  if (_expr) delete [] _expr; _expr=e;
}


void HdrDef::Add(int f, int p, string n, string s, string e)
{
    AllocateMemory(_l+1);

    _frmt[_l]=f;
    _pos [_l]=p;
    _name[_l]=s;
    _desc[_l]=n;
//  _expr[_l].Expression=e;

    _l++;
}


void HdrDef::Mod(int f, int p, string n, string s, string e ,int i)
{
    if(i<0 || i>=_l) return;

    _frmt[i]=f;
    _pos [i]=p;
    _name[i]=s;
    _desc[i]=n;
//  _expr[i].Expression=e;
}


void HdrDef::Del(int idx)
{
    if(_l<=0) return;

    if(idx<0 || idx>=_l) return;

    int i;

    for(i=idx;i<_l-1;i++)
    {
        _frmt[i]=_frmt[i+1];
        _pos [i]=_pos [i+1];
        _desc[i]=_desc[i+1];
        _name[i]=_name[i+1];
//      _expr[i]=_expr[i+1];
    }
    _l--;
}


void HdrDef::Clear()
{
  if (_frmt) delete [] _frmt;
  if (_pos ) delete [] _pos ;
  if (_desc) delete [] _desc;
  if (_name) delete [] _name;
//if (_expr) delete [] _expr;

  _a = 0;
  _l = 0;
  _m = 0;
  _frmt = NULL;
  _pos  = NULL;
  _desc = NULL;
  _name = NULL;
}


void HdrDef::setF(int idx, int val)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

    _frmt[idx] = val;
}

void HdrDef::setP(int idx, int val)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

    _pos [idx] = val;
}

void HdrDef::setN(int idx, string val)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

    _desc[idx] = val;
}

void HdrDef::setS(int idx, string val)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

    _name[idx] = val;
}

void HdrDef::setE(int idx, string val)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

//  _expr[idx].Expression = val;
}


int HdrDef::F(int idx)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

    return _frmt[idx];
}


string HdrDef::Fnam  (int idx)
{
    return FormatList::name(F(idx));
}


int HdrDef::P(int idx)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

    return _pos [idx];
}

string HdrDef::N(int idx)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

    return _desc[idx];
}

string HdrDef::S(int idx)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

    return _name[idx];
}

string HdrDef::E(int idx)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

//  return _expr[idx].Expression;
    return "";
}


ExprEval* HdrDef::getEval(int idx)
{
    if(idx<0 || idx>=_l)
        throw "HdrDef: index error";

    return &_expr[idx];
}

int HdrDef::FindByName(string name)
{
    int i;

    for(i=0;i<_l;i++)
    {
        if( name == _name[i] ) return i;
    }

    return -1;
}

void HdrDef::Init(int* fmt,int* pos, string* desc, string* name, int len)
{
  _m = len;

  _a = _m;
  _l = _m;

  _frmt = new int[_a];
  _pos  = new int[_a];

  _desc = new string[_a];
  _name = new string[_a];
//_expr = new ExprEval  [_a];

  int n;

  for(n=0;n<_l;n++)
  {
    _frmt[n] = fmt[n];
    _pos [n] = pos[n];
    _desc[n] = desc[n];
    _name[n] = name[n];
//  _expr[n].Expression = "";
  }
}

void HdrDef::Init(int* fmt,int* pos, char* desc[], char* name[], int len)
{
  _m = len;

  _a = _m;
  _l = _m;

  _frmt = new int[_a];
  _pos  = new int[_a];

  _desc = new string[_a];
  _name = new string[_a];
//_expr = new ExprEval  [_a];

  int n;

  for(n=0;n<_l;n++)
  {
    _frmt[n] = fmt[n];
    _pos [n] = pos[n];
    _desc[n] = desc[n];
    _name[n] = name[n];
//  _expr[n].Expression = "";
  }
}

*/
//---------------------------------------------------------------------------

Hdri::Hdri(QString n, QString d, int f, int p, QString /*expr*/)
{

    frmt = f;
    pos  = p;
    desc = d;
    name = n;
//  ExprEval*      _expr;

}

QString Hdri::bytesStr()
{
    QString s = "";

    if(frmt>0)
    {
        s.sprintf("%d-%d",pos,pos+frmt-1);
    }
    return s;
}
