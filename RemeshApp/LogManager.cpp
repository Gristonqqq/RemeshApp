#include "LogManager.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QThread>
#include <QMutexLocker>
#include <QCoreApplication>

// Helper to convert QtMsgType to string
static const char* levelToString(QtMsgType t)
{
    switch (t) {
    case QtDebugMsg:    return "DEBUG";
    case QtInfoMsg:     return "INFO";
    case QtWarningMsg:  return "WARN";
    case QtCriticalMsg: return "ERROR";
    case QtFatalMsg:    return "FATAL";
    }
    return "?";
}

// Singleton instance
LogManager& LogManager::instance()
{
    static LogManager inst;
    return inst;
}

// Constructor
LogManager::LogManager(QObject* parent)
    : QObject(parent)
{
}

// Get log file path
QString LogManager::logFilePath() const
{
    QMutexLocker lock(&m_mutex);
    return m_filePath;
}

// Get log entries
QVector<LogManager::Entry> LogManager::entries() const
{
    QMutexLocker lock(&m_mutex);
    return m_entries;
}

// Initialize log manager
void LogManager::init(const QString& fileName)
{
    QMutexLocker lock(&m_mutex);
    if (m_inited)
        return;

    const QString finalName = fileName.isEmpty() ? QStringLiteral("RemeshApp.log") : fileName;

    auto tryOpenAt = [&](const QString& baseDir) -> bool
        {
            QDir dir(baseDir);
            if (!dir.exists("logs"))
                dir.mkpath("logs");

            m_filePath = dir.filePath(QStringLiteral("logs/") + finalName);

            m_file.setFileName(m_filePath);
            return m_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        };

    const QString exeDir = QCoreApplication::applicationDirPath();
    bool ok = tryOpenAt(exeDir);

    if (!ok) {
        const QString base = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        ok = tryOpenAt(base);
        if (!ok) {
            m_filePath.clear();
        }
    }

    m_prevHandler = qInstallMessageHandler(&LogManager::messageHandler);

    if (m_file.isOpen()) {
        const QString header =
            QStringLiteral("==== RemeshApp log started: %1 ====\n")
            .arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs));
        m_file.write(header.toUtf8());
        m_file.flush();
    }

    m_inited = true;
}

// Message handler
void LogManager::messageHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
    LogManager::instance().append(type, ctx, msg);

    if (auto prev = LogManager::instance().m_prevHandler) {
        prev(type, ctx, msg);
    }

    if (type == QtFatalMsg) {
        abort();
    }
}

// Append log entry
void LogManager::append(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
    const QDateTime ts = QDateTime::currentDateTime();

    const QString category = ctx.category ? QString::fromUtf8(ctx.category) : QString();
    const QString file = ctx.file ? QString::fromUtf8(ctx.file) : QString();
    const QString function = ctx.function ? QString::fromUtf8(ctx.function) : QString();

    QString line = ts.toString(Qt::ISODateWithMs);
    line += " [";
    line += levelToString(type);
    line += "]";

    if (!category.isEmpty()) {
        line += " [";
        line += category;
        line += "]";
    }

    line += " ";
    line += msg;

    if (!file.isEmpty()) {
        line += " (";
        line += QFileInfo(file).fileName();
        if (ctx.line > 0) {
            line += ":";
            line += QString::number(ctx.line);
        }
        line += ")";
    }

    Entry e;
    e.timestamp = ts;
    e.type = type;
    e.category = category;
    e.message = msg;
    e.file = file;
    e.line = ctx.line;
    e.function = function;
    e.formatted = line;

    {
        QMutexLocker lock(&m_mutex);

        if (m_file.isOpen()) {
            m_file.write((line + "\n").toUtf8());
            m_file.flush();
        }

        m_entries.push_back(e);
        if (m_entries.size() > m_maxEntries) {
            const int overflow = m_entries.size() - m_maxEntries;
            m_entries.erase(m_entries.begin(), m_entries.begin() + overflow);
        }
    }

    emit entryAdded(e);
}
