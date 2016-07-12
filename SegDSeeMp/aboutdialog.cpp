#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "mainwindow.h"
#include "util2.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QString txt = Tprintf("<html><head/><body><p align=\"center\"><span style=\" font-size:12pt;\">Rev: %s (build: %s) </span></p></body></html>", VERSION,__DATE__);

    ui->label_rev->setText(txt);

}

AboutDialog::~AboutDialog()
{
    delete ui;
}
