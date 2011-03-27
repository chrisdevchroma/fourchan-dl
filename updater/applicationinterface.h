#ifndef APPLICATIONINTERFACE_H
#define APPLICATIONINTERFACE_H

#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <iostream>
#include <QTimer>
#include "commands.h"
#include "types.h"

#define TIMEOUT_VALUE   1000       // 1s
#define PING_INTERVAL   500        // 500ms

class ApplicationInterface : public QObject
{
    Q_OBJECT
public:
    explicit ApplicationInterface(QObject *parent = 0);
    void sendCloseRequest();

private:
    QUdpSocket* udpSocket;
    QTextStream* output;
    QTimer* timer;
    QTimer* timeout;
    bool varConnected;
    bool pinging;
    bool closingApplication;
    FileUpdate fu;
    QList<FileUpdate> updateList;

    void p(QString);
signals:

private slots:
    void readPendingDatagrams(void);
    void processCommand(QByteArray);
    void timerTrigger(void);
    void timeoutOccured();

public slots:
    void sendMessage(QString);
    void sendError(QString);

signals:
    void connected(bool);
    void startUpdate(QList<FileUpdate>);
    void applicationClosed(bool);
    void executableChanged(QString);
    void close();
};

#endif // APPLICATIONINTERFACE_H
