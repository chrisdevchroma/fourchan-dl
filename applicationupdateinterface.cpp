#include "applicationupdateinterface.h"

ApplicationUpdateInterface::ApplicationUpdateInterface(QObject *parent) :
    QObject(parent)
{
    udpSocket = new QUdpSocket();
    pinging = false;
    connected = false;
    answerPing = false;
    startRequest = false;

    if (!udpSocket->bind(APPLICATION_PORT))
        qDebug() << "Could not create socket (" << udpSocket->errorString() << ")";

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    connect(this, SIGNAL(connectionEstablished()), this, SLOT(init()));
}

void ApplicationUpdateInterface::readPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        processCommand(datagram);
    }
}

void ApplicationUpdateInterface::processCommand(QByteArray a) {
    int command;
    QByteArray payload;

    command = a.at(0);
    payload = a.mid(1);

    switch (command) {
    case PING:
            writeCommand(PONG);
            if (!connected)
                emit connectionEstablished();
            connected = true;
        break;
    case PONG:
        if (pinging)
            pinging=false;
        else
            writeCommand(ERROR);
    case CLOSE_REQUEST:
        exit(0);
        break;

    case DISPLAY_MSG:
        QMessageBox::information(0, "Updater Message", QString(payload));
        break;

    case ERROR:
        QMessageBox::critical(0, "Update Error", QString(payload));
        break;
    default:
        qDebug() << "Don't know what to do with command "<<QString::number(command);
        break;
    }
}

void ApplicationUpdateInterface::startUpdate(QString v) {
    version = v;

    startRequest = true;
}

void ApplicationUpdateInterface::init() {
#ifdef USE_UPDATER
    if (startRequest) {
        startRequest = false;

        if (version != "") {
            QDir dir;            
            writeCommand(SET_EXE, QString("%1/%2").arg(dir.absolutePath()).arg(APP_NAME));
            writeCommand(SET_URI, QString("http://sourceforge.net/projects/fourchan-dl/files/v%1/%2/download").arg(version).arg(APP_NAME));
            writeCommand(SET_TARGET, QString(APP_NAME));
            writeCommand(ADD_SET);
            writeCommand(START);
        }
        else {
            qDebug() << "Error: No version information given.";
        }
    }
    else {
        // Updater was running because of previous update
        writeCommand(CLOSE_REQUEST);
    }
#endif
}

void ApplicationUpdateInterface::writeCommand(int c, QByteArray a) {
    qint64 bytesSent;

    bytesSent = udpSocket->writeDatagram(
            createCommand(c, a),
            QHostAddress(HOST_ADDRESS),
            UPDATER_PORT
            );
}

void ApplicationUpdateInterface::writeCommand(int c, QString s) {
    writeCommand(c, s.toAscii());
}

void ApplicationUpdateInterface::writeCommand(int c) {
    writeCommand(c, QByteArray());
}

