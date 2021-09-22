#ifndef FBSFFILEIO_H
#define FBSFFILEIO_H

#include <QQuickItem>

class FbsfFileIO : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(FbsfFileIO)

public:
    explicit FbsfFileIO(QQuickItem *parent = 0);
    ~FbsfFileIO();

    Q_PROPERTY(QString source
               READ source
               WRITE setSource
               NOTIFY sourceChanged)
    Q_INVOKABLE bool    fileExists(QString aPath);
    Q_INVOKABLE bool    open(QString);
    Q_INVOKABLE bool    openRead(QString);
    Q_INVOKABLE bool    backup(QString);
    Q_INVOKABLE void    close();
    Q_INVOKABLE QString read();
    Q_INVOKABLE QString readline();
    Q_INVOKABLE bool    write(const QString& data);

    QString source() { return mSource; }

public slots:
    void setSource(const QString& source) { mSource = source; }

signals:
    void sourceChanged(const QString& source);
    void error(const QString& msg);

private:
    QString             mSource;
    QFile               mFile;
    QTextStream         inStream;


};

#endif // FBSFFILEIO_H

