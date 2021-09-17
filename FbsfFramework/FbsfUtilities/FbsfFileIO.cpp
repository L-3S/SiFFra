#include "FbsfFileIO.h"
#include <QFileInfo>
#include <QDomDocument>
#include <QtWidgets/QMessageBox>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfFileIO::FbsfFileIO(QQuickItem *parent):
    QQuickItem(parent)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfFileIO::~FbsfFileIO()
{
    mFile.close();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfFileIO::fileExists(QString aPath)
{
    QUrl url(aPath);
    QFileInfo check_file(url.toLocalFile());
    // check if path exists and it is a file and no directory
    return check_file.exists() && check_file.isFile();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfFileIO::open(QString aFileName)
{
    QUrl url(aFileName);
    mFile.setFileName(url.toLocalFile());
    if (!mFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        qDebug() << "Error openning " << aFileName << mFile.errorString();
        QMessageBox::information( nullptr, "[Error]", mFile.errorString());
        return false;
    }

    return true;
}//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfFileIO::openRead(QString aFileName)
{
    QUrl url(aFileName);
    mFile.setFileName(url.toLocalFile());
    if (!mFile.open(QFile::ReadOnly))
    {
        qDebug() << "Error openning " << aFileName << mFile.errorString();
        return false;
    }
    inStream.setDevice(&mFile );
    inStream.setCodec("UTF-8");

    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfFileIO::backup(QString aFileName)
{
    QUrl current(aFileName);
    // create a backup copy if already exist
    if (!QFile::exists(current.toLocalFile())) return true;

    QString backupname(current.toLocalFile()+".1");
    QFile backupfile(backupname);
    if (backupfile.exists())
    {
        if (!backupfile.remove())
            qDebug() << "remove Backup failed" << backupname
                     << backupfile.errorString();;
    }

    if (!QFile::copy(current.toLocalFile(),backupname))
        qDebug() << "Backup failed" << backupname
                 << backupfile.errorString();

    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfFileIO::close()
{
    mFile.close();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfFileIO::read()
{
    if (mSource.isEmpty())
    {
        emit error("source is empty");
        return QString();
    }

    QString fileContent;
    QString line;
    QTextStream inStream( &mFile );
    inStream.setCodec("UTF-8");

    do {
        line = inStream.readLine();
        fileContent += line;
    } while (!line.isNull());
    return fileContent;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString FbsfFileIO::readline()
{
    QString line;
    line = inStream.readLine();
    return line;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool FbsfFileIO::write(const QString& data)
{
    if (mSource.isEmpty())  return false;

    QTextStream out(&mFile);
	out.setCodec("UTF-8");
    out << data << endl;

    return true;
}
