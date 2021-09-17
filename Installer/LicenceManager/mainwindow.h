#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    bool    saved;
    void setDuration(int days);

public slots:
   void keygen();

private slots:
   void on_mac1_textChanged(const QString &arg1);
   void on_mac2_textChanged(const QString &arg1);
   void on_mac3_textChanged(const QString &arg1);
   void on_mac4_textChanged(const QString &arg1);
   void on_mac5_textChanged(const QString &arg1);
   void on_Duration_valueChanged(int value);

   void on_dateEdit_userDateChanged(const QDate &date);

protected:

    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
