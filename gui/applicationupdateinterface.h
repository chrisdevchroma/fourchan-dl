#ifndef APPLICATIONUPDATEINTERFACE_H
#define APPLICATIONUPDATEINTERFACE_H

#include <QObject>
#include <QCoreApplication>
#include <QByteArray>
#include <QtNetwork/QUdpSocket>
#include <QDir>
#include <QSettings>
#include <QUuid>

#ifdef __GUI__
#include <QMessageBox>
#endif

#include "../updater/commands.h"
#include "defines.h"
#include "QsLog.h"

class ApplicationUpdateInterface : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationUpdateInterface(QObject *parent = 0);
    void startUpdate();
    void addFile(QString url, QString loc);
    void addFiles(QStringList list);
    void closeUpdaterExe();
    void exchangeFiles();
private:
    QUdpSocket* udpSocket;
    bool pinging;
    bool answerPing;
    bool connected;
    bool startRequest;
    QString version;
    QString fileToMoveFrom, fileToMoveTo;
    QHash<QString,QString> filesToMove;
    QSettings* settings;

    void processCommand(QByteArray a);
    void writeCommand(int c, QByteArray a);
    void writeCommand(int c, QString s);
    void writeCommand(int c);

private slots:
    void readPendingDatagrams(void);
    void init();

signals:
    void connectionEstablished();
    void updateFinished();
    void updaterVersionSent(QString);

public slots:

};

#endif // APPLICATIONUPDATEINTERFACE_H
