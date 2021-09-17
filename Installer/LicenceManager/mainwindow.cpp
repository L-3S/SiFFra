#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageAuthenticationCode>
#include <QSettings>
#include <QCloseEvent>
#include <QStringList>
#include <QStandardItemModel>
#include <QDate>
#include <QFileInfo>
#include <QDebug>
#include <QListWidgetItem>

#include "LicenseManager.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    saved(false)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMinimumDate(QDate::currentDate());
    ui->Duration->setRange(0,500);
    // Add features to selection list
    QStringList FeaturesList;
    FeaturesList << "LogicEditor";

    ui->Features->setRowCount( FeaturesList.count() );
    for (int i = 0; i < FeaturesList.count(); i++)
    {
        QTableWidgetItem *item2 = new QTableWidgetItem();
        item2->setText(FeaturesList[i]);
        item2->setCheckState(Qt::Unchecked);
        ui->Features->setItem(i, 0, item2);
        QSpinBox *FeaturesBox = new QSpinBox( ui->Features );
        FeaturesBox->setRange(0,500);

        ui->Features->setCellWidget ( i, 1, FeaturesBox);
    }
    ui->Features->setColumnWidth(0, ui->Features->width() - 60);
    ui->Features->setColumnWidth(1, 55);
    // Set default expiration date
    ui->DateExpiration->setText("permanent");// default
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MainWindow::~MainWindow()
{
    delete ui;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::keygen()
{
    QString hostid;

    if (ui->mac1->text().isEmpty())
        hostid="ANY";
    else
        hostid  =   ui->mac1->text()+":"
                +   ui->mac2->text()+":"
                +   ui->mac3->text()+":"
                +   ui->mac4->text()+":"
                +   ui->mac5->text()+":"
                +   ui->mac6->text();

    LicenseManager lm;
    lm.setHostid(hostid);

    lm.settings.setValue("HOSTID", hostid);
    lm.settings.setValue("DATE", ui->DateExpiration->text());
    lm.settings.setValue("SIGN", QString(lm.gencode(ui->DateExpiration->text())));

    //~~~~~~~~~~~~~~~~~~ set FEATURES keys ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (int i=0;i<ui->Features->rowCount();i++)
    {
        if (ui->Features->item(i,0)->checkState()== Qt::Checked)
        {           
            double val = static_cast<QDoubleSpinBox*>(ui->Features->cellWidget(i,1))->value();
            QString expire;
            if (val==0)
                expire="permanent";
            else
                expire=QDate::currentDate().addDays(val).toString("d-M-yyyy");

            QString sign=QString(lm.gencode(ui->Features->item(i,0)->text(),expire));
            lm.settings.beginGroup(ui->Features->item(i,0)->text());
            lm.settings.setValue(  "DATE", expire.toStdString().c_str());
            lm.settings.setValue(  "SIGN", sign);
            lm.settings.endGroup();
        }
    }
    saved=true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::on_mac1_textChanged(const QString &arg1)
{
    saved=false;
    if (arg1.length()==2) ui->mac2->setFocus();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::on_mac2_textChanged(const QString &arg1)
{
    saved=false;
    if (arg1.length()==2) ui->mac3->setFocus();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::on_mac3_textChanged(const QString &arg1)
{
    saved=false;
    if (arg1.length()==2) ui->mac4->setFocus();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::on_mac4_textChanged(const QString &arg1)
{
    saved=false;
    if (arg1.length()==2) ui->mac5->setFocus();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::on_mac5_textChanged(const QString &arg1)
{
    saved=false;
    if (arg1.length()==2) ui->mac6->setFocus();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::on_Duration_valueChanged(int value)
{
    saved=false;
    // update dateEdit ui
    ui->dateEdit->setDate(QDate::currentDate().addDays(value));
    setDuration(value);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::on_dateEdit_userDateChanged(const QDate &date)
{
    saved=false;
    int days=QDate::currentDate().daysTo(date); // key duration
    // update duration ui
    ui->Duration->setValue(days);
    setDuration(days);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::setDuration(int days)
{
    // set default features duration to main key duration
    for (int i = 0; i < ui->Features->rowCount(); i++)
    {
        QSpinBox* item=((QSpinBox *)ui->Features->cellWidget ( 0, 1));
        item->setValue(days);
        item->setMaximum(days);
    }
    QString expire=QDate::currentDate().addDays(days).toString("d-M-yyyy");
    ui->DateExpiration->setText(expire);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (saved==false) keygen();
    event->accept();
}
