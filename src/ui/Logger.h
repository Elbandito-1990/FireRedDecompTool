#pragma once
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

inline void logMessage(const QString &msg) {
    QFile logFile("firedecomp_log.txt");
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "] "
            << msg << "\n";
        logFile.close();
    }
}