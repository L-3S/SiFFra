#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QQuickItem>
#include <QMessageBox>
#include <QString>

class FbsfMessageBox : public QQuickItem
{
    Q_OBJECT

public:
    explicit FbsfMessageBox(QQuickItem *parent = 0);

    Q_PROPERTY(QString title
               WRITE setTitle)
    Q_PROPERTY(QMessageBox::Icon icon
               WRITE setIcon)
    Q_PROPERTY(QString text
               WRITE setText)
    Q_PROPERTY(QString info
               WRITE setInfo)
    Q_PROPERTY(QMessageBox::StandardButtons buttons
               WRITE setButtons)
    Q_PROPERTY(QMessageBox::StandardButtons defaultButton
               WRITE setDefaultButton)
    Q_INVOKABLE int show();

signals:

public slots:
    void setTitle(const QString& title) {msgBox.setWindowTitle(title); }
    void setIcon(const QMessageBox::Icon& icon) {msgBox.setIcon(icon); }
    void setText(const QString& text) {msgBox.setText(text); }
    void setInfo(const QString& info) { msgBox.setInformativeText(info); }
    void setButtons(const QMessageBox::StandardButtons& buttons) { msgBox.setStandardButtons(buttons); }
    void setDefaultButton(const QMessageBox::StandardButtons& defaultButton)
                {Q_UNUSED(defaultButton);/*msgBox.setDefaultButton(defaultButton);*/}
private :
    QMessageBox msgBox;
};

#endif // MESSAGEBOX_H
