#include <QDebug>
#include <QApplication>
#include <QHeaderView>

#include "mystringtable.h"


MyStringTableModel::MyStringTableModel(MyStringTable *parent) :
    QAbstractTableModel((QObject*)parent)
{
    _tab = parent;
}

QVariant MyStringTableModel::data(const QModelIndex & index,int role) const
{
    QVariant result;
    QColor   color;

    if(!index.isValid())
    {
        return result;
    }

    int r = index.row();
    int c = index.column();

    // цвет фона ячейки
    if (role == Qt::BackgroundRole)
    {
        color = _tab->CellColor(r,c);
        if(color.isValid()) return color;

        color = _tab->_rows[r].color;
        if(color.isValid()) return color;
    }

    // Если необходимо отобразить картинку - ловим роль Qt::DecorationRole
    if (role == Qt::DecorationRole)
    {
        QString imgname = _tab->ImgName(r,c);
        if(imgname!="") return QIcon(imgname);
    }

    // Если хотим отобразить CheckBox, используем роль Qt::CheckStateRole
    // если текущий вызов не относится к роли отображения, завершаем

    if(role == Qt::CheckStateRole)
    {

        if(_tab->_cols[c].chkbx) return _tab->Check(r,c);
    }

    if (role == Qt::DisplayRole)
    {
        result = _tab->Cell(r,c);
        return result;
    }

    if (role == Qt::EditRole)
    {
        result = _tab->Cell(r,c);
        return result;
    }

    return result;
    //return QAbstractItemModel::data(index,role);
}


bool MyStringTableModel::setData( const QModelIndex &index, const QVariant &value, int role)
{
//    qDebug() << "setData" << index << value.toString() << role;

    if(!index.isValid()) return false;

    int r = index.row();
    int c = index.column();

    // нас интересует только роль, сообщающая об изменении
    if (role == Qt::EditRole)
    {
        if(_tab->_rows[r].cells[c].text != value.toString())
        {
           _tab->_rows[r].cells[c].text = value.toString();
           _tab->setTextChanged(true);
        }
        emit(dataChanged(index, index));
        // данные приняты
        return true;
    }


    if (role == Qt::DisplayRole)
    {
        //_tab->setCell(r,c,value.toString());
        _tab->_rows[r].cells[c].text = value.toString();
        emit(dataChanged(index, index));
        // данные приняты
        return true;
    }

    if (role == Qt::CheckStateRole)
    {
        //_tab->setCheck(r,c,value.toInt());
        _tab->_rows[r].cells[c].check = value.toInt();
        emit(dataChanged(index, index));
        // данные приняты
        return true;
    }

    return false;
}

Qt::ItemFlags MyStringTableModel::flags(const QModelIndex & index) const
{
    //int r = index.row();
    int c = index.column();

    if(_tab->ColEdit(c)) return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;

    return QAbstractTableModel::flags(index);
}


QVariant MyStringTableModel::headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const
{
    if(role == Qt::DisplayRole)
    {
        if     (orientation == Qt::Horizontal) return _tab->_cols[section].label;
        else if(orientation == Qt::Vertical  ) return _tab->_rows[section].label;
    }

    return QAbstractTableModel::headerData (section, orientation, role);
}

int MyStringTableModel::rowCount(const QModelIndex &) const
{
    int v = _tab->RowCount();
    return v;
}

int MyStringTableModel::columnCount(const QModelIndex &) const
{
    int v = _tab->ColCount();
    return v;
}


MyStringTable::MyStringTable(QWidget *parent) :
    QTableView(parent)
{
    _changed = false;
    _model = new MyStringTableModel(this);
    setModel(_model);
    verticalHeader  ()->setDefaultSectionSize(18);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void MyStringTable::setRowCount(int v, bool reset)
{
    int n;

    _model->beginResetModel();

    _rows.resize(v);

    for(n=0;n<_rows.count();n++) _rows[n].cells.resize(ColCount());

    if(reset) _model->endResetModel();

}

void MyStringTable::setColCount(int v, bool reset)
{
    int n;

    _model->beginResetModel();

    for(n=0;n<_rows.count();n++) _rows[n].cells.resize(v);
    _cols.resize(v); update();

    if(reset) _model->endResetModel();

}


QSize MyStringTable::sizeHint() const
{
    return QSize(200,100);
}


int MyStringTable::RowCount()
{
    return _rows.count();
}

int MyStringTable::ColCount()
{
    return _cols.count();
}


QString MyStringTable::Cell(int r, int c)
{
    if(r<0 || r>=RowCount()) return "";
    if(c<0 || c>=ColCount()) return "";


    int rc = receivers(SIGNAL(cellDataRequest(int, int, QString&)));
    if(rc<1) return _rows[r].cells[c].text;

    QString v;

    emit cellDataRequest(r,c,v);

    return  v;
}

void MyStringTable::setCell(int r, int c, QString v)
{
    if(r<0 || r>=RowCount()) return;
    if(c<0 || c>=ColCount()) return;

    //_rows[r].cells[c].text = v;  //_model->updateRow(r);
    _model->setData(_model->index(r,c),v,Qt::EditRole);
}

QString MyStringTable::ImgName(int r, int c)
{
    if(r<0 || r>=RowCount()) return "";
    if(c<0 || c>=ColCount()) return "";

    return _rows[r].cells[c].imgnm;
}

void MyStringTable::setImgName(int r, int c, QString v)
{
    if(r<0 || r>=RowCount()) return;
    if(c<0 || c>=ColCount()) return;

    _rows[r].cells[c].imgnm = v; //_model->updateRow(r);
}

bool MyStringTable::CellEdit(int r, int c)
{
    if(r<0 || r>=RowCount()) return false;
    if(c<0 || c>=ColCount()) return false;

    return _rows[r].cells[c].edit;
}


QColor MyStringTable::CellColor(int r, int c)
{
    if(r<0 || r>=RowCount()) return QColor();
    if(c<0 || c>=ColCount()) return QColor();

    return _rows[r].cells[c].color;
}


void MyStringTable::setCellColor(int r, int c, QColor v)
{
    if(r<0 || r>=RowCount()) return;
    if(c<0 || c>=ColCount()) return;

    _rows[r].cells[c].color = v; //_model->updateRow(r);
}


void MyStringTable::setColCheck(int c, int v)
{
    if(c<0 || c>=ColCount()) return;

    int r;

    for(r=0;r<RowCount();r++)
    {
    //_rows[r].cells[c].check = v; //_model->updateRow(r);
    _model->setData(_model->index(r,c),v,Qt::CheckStateRole);
    }
}



void MyStringTable::setCheck(int r, int c, int v)
{
    if(r<0 || r>=RowCount()) return;
    if(c<0 || c>=ColCount()) return;

    //_rows[r].cells[c].check = v; //_model->updateRow(r);
    _model->setData(_model->index(r,c),v,Qt::CheckStateRole);
}


int MyStringTable::Check(int r, int c)
{
    if(r<0 || r>=RowCount()) return 4;
    if(c<0 || c>=ColCount()) return 4;

    return _rows[r].cells[c].check;
}


void MyStringTable::setTag(int r, int c, int v)
{
    if(r<0 || r>=RowCount()) return;
    if(c<0 || c>=ColCount()) return;

    _rows[r].cells[c].tag = v;
}

int MyStringTable::Tag(int r, int c)
{
    if(r<0 || r>=RowCount()) return -1;
    if(c<0 || c>=ColCount()) return -1;

    return _rows[r].cells[c].tag;
}


void MyStringTable::setColor(QColor v)
{
    _color=v; update();
}

void MyStringTable::setTextc(QColor v)
{
    _textc=v; update();
}


void MyStringTable::setRowColor(int r, QColor v)
{
    if(r<0 || r>=RowCount()) return;

    _rows[r].color=v; //_model->updateRow(r);
}

void MyStringTable::setRowTextc(int r, QColor v)
{
    if(r<0 || r>=RowCount()) return;

    _rows[r].textc=v; //_model->updateRow(r);

}

void MyStringTable::setRowLabel(int r, QString v)
{
    if(r<0 || r>=RowCount()) return;

    _rows[r].label=v; update();
}

void MyStringTable::setColChkbx(int c, bool v)
{
    if(c<0 || c>=ColCount()) return;

    _cols[c].chkbx=v;  update();
}

bool MyStringTable::ColChkbx(int c)
{
    if(c<0 || c>=ColCount()) return false;

    return _cols[c].chkbx;
}

void MyStringTable::setColWidth(int c, int v)
{
    if(c<0 || c>=ColCount()) return;

    _cols[c].width=v;  update();
}

int MyStringTable::ColWidth(int c)
{
    if(c<0 || c>=ColCount()) return false;

    return _cols[c].width;
}


void MyStringTable::setColLabel(int c, QString v)
{
    if(c<0 || c>=ColCount()) return;

    _cols[c].label=v;  update();
}

QString MyStringTable::ColLabel(int c)
{
    if(c<0 || c>=ColCount()) return "";

    return _cols[c].label;
}

void MyStringTable::resizeEvent(QResizeEvent * event)
{
    int w = event->size().width();

    if(ColCount()>0)
    {
        int ws = w / ColCount();
        int ns;
        int ww = 0;
        int nn = 0;

        for(ns=0;ns<ColCount();ns++)
        {
            ws = ColWidth(ns);
            if(ws>0) ww += ws;
            else     nn++;
        }

        if(nn>0)
        {
            ww = (w - ww) / nn; if(ww<10) ww=10;
        }

        for(ns=0;ns<ColCount();ns++)
        {
            ws = ColWidth(ns);
            if(ws<1) ws = ww;
            horizontalHeader()->resizeSection(ns,ws);
        }
    }
    QTableView::resizeEvent(event);
}

void MyStringTable::keyPressEvent(QKeyEvent*    event)
{
    QTableView::keyPressEvent(event);

    QItemSelectionModel *sel = selectionModel();

    if(!sel->hasSelection()) return;

//    QModelIndexList list = sel->selectedRows();
//    if(list.count()<1) return ;

//    int row = list.at(0).row();

    int key = event->key();

    if(key==Qt::Key_Enter || key==Qt::Key_Return)
    {
        int r = currentIndex().row();
        int c = currentIndex().column();

        if(ColEdit(c))
        {

            if(this->state()!=QAbstractItemView::EditingState)
            {
                edit(currentIndex());
            }
        }
        else
        {
            emit rowEvent(r, 2);
        }
    }
    else if(key==Qt::Key_Home)
    {
        QApplication::beep();
        QModelIndex index = _model->index(0,0, QModelIndex());

        sel->clearSelection();
        setCurrentIndex(index);
    }
    else if(key==Qt::Key_End)
    {
        QModelIndex index = _model->index(RowCount()-1,0, QModelIndex());

        sel->clearSelection();
        setCurrentIndex(index);
    }
}




void MyStringTable::mousePressEvent (QMouseEvent * event)
{
    QTableView::mousePressEvent(event);

    QItemSelectionModel *sel = selectionModel();

    if(!sel->hasSelection()) return;

    QModelIndexList list = sel->selectedRows();

    //int row = list.at(0).row();

    QModelIndex idx = currentIndex();

    int c = idx.column();
    int r = idx.row();

    int ck = Check(r,c);

    if(ck) ck=0;
    else   ck=2;

    if(ColChkbx(c))
    {
        setCheck(r,c,ck);
        emit rowEvent(r, 4);
    }

}

void MyStringTable::mouseDoubleClickEvent(QMouseEvent * event)
{
    QTableView::mouseDoubleClickEvent(event);

    QItemSelectionModel *sel = selectionModel();

    if(!sel->hasSelection()) return;

    QModelIndexList list = sel->selectedRows();

    if(list.count()<1) return;

    int row = list.at(0).row();

    emit rowEvent(row, 2);
}

void MyStringTable::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    QAbstractItemView::selectionChanged(selected,deselected);

    QItemSelectionModel *sel = selectionModel();

    if(!sel->hasSelection()) return;

    QModelIndexList list = sel->selectedRows();

    if(list.count()<1) return;

    int row = list.at(0).row();

    emit rowEvent(row, 1);
}


void MyStringTable::setCurCell(int r, int c)
{
    QModelIndex index = _model->index(r,c, QModelIndex());
    setCurrentIndex(index);
}

int MyStringTable::curRow()
{
    //QModelIndex index = _model->index(r,c, QModelIndex());
    QModelIndex index = currentIndex();
    return index.row();
}


int MyStringTable::curCol()
{
    //QModelIndex index = _model->index(r,c, QModelIndex());
    QModelIndex index = currentIndex();
    return index.column();
}

void MyStringTable::editorDestroyed ( QObject * /*editor*/ )
{
}

void MyStringTable::setColEdit(int c, bool v)
{
    if(c<0 || c>=ColCount()) return;
    _cols[c].edit = v;
}


bool MyStringTable::ColEdit(int c)
{
    if(c<0 || c>=ColCount()) return false;
    return _cols[c].edit;
}



void MyStringTable::setData(int r, int c, QString v)
{
    if(r<0 || r>=RowCount()) return;
    if(c<0 || c>=ColCount()) return;

    _rows[r].cells[c].data = v;
}

QString MyStringTable::Data(int r, int c)
{
    if(r<0 || r>=RowCount()) return QString();
    if(c<0 || c>=ColCount()) return QString();

    return _rows[r].cells[c].data;
}




void MyStringTable::setTextChanged(bool v)
{
    _changed = v;

    if(v) emit textChanged();
}



