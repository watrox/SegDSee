#ifndef MYSTRINGTABLE_H
#define MYSTRINGTABLE_H

#include <QColor>
#include <QTableView>
#include <QAbstractTableModel>
#include <QVector>
#include <QHeaderView>
#include <QResizeEvent>

class MyStringTableCell
{
public:
    QString text;
    QString imgnm;
    int     check;
    int     tag;
    QColor  color;  // background color
    QColor  textc;  // text color
    bool    edit;   // editable
    QString data;

    explicit MyStringTableCell()
    {
        check  = 0;
        tag    = 0;
        edit   = false;
    }
};

class MyStringTableRow
{
public:
    bool    sel;
    QColor  color;
    QColor  textc;  // text color
    QString label;

    QVector<MyStringTableCell> cells;

    explicit MyStringTableRow()
    {
        sel = false;
    }
};

class MyStringTableCol
{
public:
    bool    chkbx;   // show check
    QString label;
    QString imgnm;
    int     width;
    bool    edit;

    explicit MyStringTableCol()
    {
        chkbx    = false;
        width    = 0;
        edit     = false;
    }
};


class MyStringTable;
class MyStringTableModel;


class MyStringTableModel : public QAbstractTableModel
{
    Q_OBJECT

friend class MyStringTable;

protected:

    MyStringTable* _tab;

public:
    explicit MyStringTableModel(MyStringTable* parent);

    virtual int      rowCount   (const QModelIndex &) const;
    virtual int      columnCount(const QModelIndex &) const;

    virtual QVariant data       (const QModelIndex &,int) const;

    virtual bool     setData    (const QModelIndex &index, const QVariant &value, int role);

    virtual Qt::ItemFlags flags(const QModelIndex & /*index*/) const;

    virtual QVariant headerData( int section, Qt::Orientation orientation, int role) const;


signals:


public slots:

};


class MyStringTable : public QTableView
{
    Q_OBJECT

friend class MyStringTableModel;

protected:

    QVector<MyStringTableRow> _rows;
    QVector<MyStringTableCol> _cols;

    QColor  _color;  // background color
    QColor  _textc;  // text color

    bool    _changed;

    virtual void resizeEvent  (QResizeEvent* event);
    virtual void keyPressEvent(QKeyEvent*    event);

    virtual void mouseDoubleClickEvent(QMouseEvent * event);
//  virtual void mouseMoveEvent       (QMouseEvent * event);
    virtual void mousePressEvent      (QMouseEvent * event);
//  virtual void mouseReleaseEvent    (QMouseEvent * event);

    virtual void editorDestroyed ( QObject * editor );

    virtual void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

    virtual QSize	sizeHint() const;

    void setColumnWidth(int c, int w) { QTableView::setColumnWidth(c,w); }

public:

    MyStringTableModel*       _model;

    explicit MyStringTable(QWidget *parent=0);

    int     RowCount();
    void    setRowCount(int v, bool reset=true);

    int     ColCount();
    void    setColCount(int v, bool reset=true);

    QColor  RowColor(int r);
    void    setRowColor(int r, QColor v);

    QColor  RowTextc(int r);
    void    setRowTextc(int r, QColor v);

    QString RowLabel(int r);
    void    setRowLabel(int r, QString v);

    bool    ColChkbx(int c);
    void    setColChkbx(int c, bool);

    void    setColCheck(int c, int v);

    void    setColEdit (int c, bool v);
    bool       ColEdit (int c);

    int     ColWidth(int c);
    void    setColWidth(int c, int);

    QString ColLabel(int c);
    void    setColLabel(int c, QString);

    QColor  Color() { return _color; }
    void    setColor(QColor);

    QColor  Textc() { return _textc; }
    void    setTextc(QColor);

    QString  Cell     (int r, int c);
    void     setCell  (int r, int c, QString v);

    QString  ImgName   (int r, int c);
    void     setImgName(int r, int c, QString v);

    int      Check(int r, int c);
    void     setCheck(int r, int c, int v);

    int      Tag(int r, int c);
    void     setTag(int r, int c, int v);

    QString  Data(int r, int c);
    void     setData(int r, int c, QString v);

    QColor   CellColor(int r, int c);
    void     setCellColor(int r, int c, QColor v);

    bool     CellEdit(int r, int c);

    void     setCurCell(int r, int c=0);

    int      curRow();
    int      curCol();

    void     setTextChanged(bool v);

signals:
    void rowEvent(int row, int mode);

    void cellDataRequest(int row, int col, QString& data);

    void textChanged();


public slots:

};


#endif // MYSTRINGTABLE_H
