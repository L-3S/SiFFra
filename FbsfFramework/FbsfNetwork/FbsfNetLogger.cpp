#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include "FbsfNetLogger.h"

namespace FbsfNetLogger
{
    void FbsfLog_trace(const QString &module, const QString &message)
    {
        FbsfLog_(module, TraceLevel, message);
    }

    void FbsfLog_Debug(const QString &module, const QString &message)
    {
        FbsfLog_(module, DebugLevel, message);
    }

    void FbsfLog_Info(const QString &module, const QString &message)
    {
        FbsfLog_(module, InfoLevel, message);
    }

    void FbsfLog_Warning(const QString &module, const QString &message)
    {
        FbsfLog_(module, WarnLevel, message);
    }

    void FbsfLog_Error(const QString &module, const QString &message)
    {
        FbsfLog_(module, ErrorLevel, message);
    }

    void FbsfLog_Fatal(const QString &module, const QString &message)
    {
        FbsfLog_(module, FatalLevel, message);
    }

    void FbsfLog_(const QString &module, LogLevel level, const QString &message)
    {
        FbsfNetLoggerManager *manager = FbsfNetLoggerManager::getInstance();

        QMutexLocker(&manager->mutex);

        FbsfNetLoggerWriter *logWriter = manager->getLogWriter(module);

        if (logWriter && logWriter->getLevel() <= level)
                logWriter->write(module,message);
    }

    //FbsfNetLoggerManager
    FbsfNetLoggerManager * FbsfNetLoggerManager::INSTANCE = NULL;

    FbsfNetLoggerManager::FbsfNetLoggerManager() : QThread(), mutex(QMutex::Recursive)
    {
        start();
    }

    FbsfNetLoggerManager * FbsfNetLoggerManager::getInstance()
    {
        if (!INSTANCE)
            INSTANCE = new FbsfNetLoggerManager();

        return INSTANCE;
    }

    QString FbsfNetLoggerManager::levelToText(const LogLevel &level)
    {
        switch (level)
        {
            case TraceLevel: return "Trace";
            case DebugLevel: return "Debug";
            case InfoLevel:  return "Info";
            case WarnLevel:  return "Warning";
            case ErrorLevel: return "Error";
            case FatalLevel: return "Fatal";
            default: return QString();
        }
    }

    bool FbsfNetLoggerManager::addDestination(const QString &fileDest, const QStringList &modules, LogLevel level)
    {

        foreach (QString module, modules)
        {
            FbsfNetLoggerWriter *log;
            log = new FbsfNetLoggerWriter(fileDest,level);
            if (!moduleDest.contains(module))
            {
                moduleDest.insert(module, log);
                return true;
            }
        }
        return false;
    }

    void FbsfNetLoggerManager::closeLogger()
    {
        deleteLater();
    }

    FbsfNetLoggerWriter::FbsfNetLoggerWriter(const QString &fileDestination, LogLevel level)
    {
        m_fileDestination = fileDestination;
        m_level = level;
    }

    void FbsfNetLoggerWriter::write(const QString &module, const QString &message)
    {
        QString _fileName = m_fileDestination;

        int MAX_SIZE = 1024 * 1024;

        QDir dir(QDir::currentPath());
        if (!dir.exists("logs")) dir.mkdir("logs");

        QFile file(_fileName);
        QString toRemove = _fileName.section('.',-1);
        QString fileNameAux = _fileName.left(_fileName.size() - toRemove.size()-1);
        bool renamed = false;
        QString newName = fileNameAux + "_%1__%2.log";

        if (file.size() >= MAX_SIZE)
        {
            QDateTime currentTime = QDateTime::currentDateTime();
            newName = newName.arg(currentTime.date().toString("dd_MM_yy")).arg(currentTime.time().toString("hh_mm_ss"));
            renamed = file.rename(_fileName, newName);

        }

        file.setFileName(_fileName);
        if (file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
        {
            QTextStream out(&file);
            QString dtFormat = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz");

            if (renamed)
                out << QString("%1 - Previous log %2\n").arg(dtFormat).arg(newName);

            QString logLevel = FbsfNetLoggerManager::levelToText(m_level);
            QString text = QString("[%1] [%2] {%3} %4\n").arg(dtFormat).arg(logLevel).arg(module).arg(message);
            out << text;
            file.close();
        }
    }
}
