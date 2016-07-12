#ifndef PROCPARMDIALOG_H
#define PROCPARMDIALOG_H

#include <QDialog>

class ProcParm
{
public:

};

namespace Ui {
class ProcParmDialog;
}

class ProcParmDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ProcParmDialog(QWidget *parent = 0);
    ~ProcParmDialog();

    double f1,f2,f3,f4;
    double agcw;
    bool   filt;
    bool   agc;
    bool   norm;

    void show();
    
private slots:
    void on_closeButton_pressed();

    void on_okButton_pressed();

    void on_applyButton_pressed();

    void on_ckAgc_toggled(bool checked);

    void on_ckNorm_toggled(bool checked);

private:
    Ui::ProcParmDialog *ui;

    void FillForm();
    bool ReadForm();


signals:
    void changedEvent();

};

#endif // PROCPARMDIALOG_H
