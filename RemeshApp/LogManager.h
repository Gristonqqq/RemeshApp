#pragma once

#include <QObject>
#include <QDateTime>
#include <QMutex>
#include <QString>
#include <QVector>
#include <QFile>
#include <QtGlobal>
#include <QMessageLogContext>

class LogManager final : public QObject
{
    Q_OBJECT
public:
    struct Entry
    {
        QDateTime timestamp;
        QtMsgType type = QtInfoMsg;
        QString category;
        QString message;

        QString file;
        int line = 0;
        QString function;

        QString formatted;
    };

    static LogManager& instance();

    void init(const QString& fileName = QString());

    QString logFilePath() const;

    QVector<Entry> entries() const;

signals:
    void entryAdded(const LogManager::Entry& entry);

private:
    explicit LogManager(QObject* parent = nullptr);

    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    static void messageHandler(QtMsgType type,
        const QMessageLogContext& ctx,
        const QString& msg);

    void append(QtMsgType type, const QMessageLogContext& ctx, const QString& msg);

private:
    mutable QMutex m_mutex;
    QString m_filePath;
    QFile m_file;
    QVector<Entry> m_entries;
    int m_maxEntries = 20000;
    bool m_inited = false;
    QtMessageHandler m_prevHandler = nullptr;
};
