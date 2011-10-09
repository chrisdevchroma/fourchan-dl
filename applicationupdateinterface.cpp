#include "applicationupdateinterface.h"

ApplicationUpdateInterface::ApplicationUpdateInterface(QObject *parent) :
    QObject(parent)
{
    udpSocket = new QUdpSocket();
    pinging = false;
    connected = false;
    answerPing = false;
    startRequest = false;

    fileToMoveFrom = "";
    fileToMoveTo = "";
    filesToMove.clear();

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

            if (!connected) {
                connected = true;
                emit connectionEstablished();
            }
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

    case CLEAR:
        fileToMoveFrom = "";
        fileToMoveTo = "";
        filesToMove.clear();
        break;

    case SET_URI:
        fileToMoveFrom = QString(payload);
        qDebug() << "from" << fileToMoveFrom;
        break;

    case SET_TARGET:
        fileToMoveTo = QString(payload);
        qDebug() << "to" << fileToMoveTo;
        break;

    case ADD_SET:
        filesToMove.append(QString("%1->%2").arg(fileToMoveFrom).arg(fileToMoveTo));
        qDebug() << "add set";
        break;

    case UPDATE_FINISHED:
        emit updateFinished();
        break;

    case VERSION:
        emit updaterVersionSent(QString(payload));
        break;

    default:
        qDebug() << "Don't know what to do with command " << QString::number(command);
        break;
    }
}

void ApplicationUpdateInterface::startUpdate() {
    writeCommand(SET_EXE, QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(APP_NAME));
    writeCommand(START);
}

void ApplicationUpdateInterface::init() {
#ifdef USE_UPDATER
    if (connected) {
            writeCommand(CLEAR);
            writeCommand(GET_VERSION);  // Always poll version
    }
    else {
        qDebug() << "I am not connected to the update executable!";
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

void ApplicationUpdateInterface::addFile(QString url, QString loc) {
    if (connected) {
        writeCommand(SET_URI, url);
        writeCommand(SET_TARGET, loc);
        writeCommand(ADD_SET);
    }
}

void ApplicationUpdateInterface::addFiles(QStringList list) {
    foreach (QString set, list) {
        QStringList sl;

        sl = set.split("->");
        addFile(sl.at(0), sl.at(1));
    }
}

void ApplicationUpdateInterface::exchangeFiles() {
    QStringList sl;
    QString from, to;
    QFile source;
    QFile target;
    bool success;


    for(int i=0; i<filesToMove.count(); i++) {
        sl = filesToMove.at(i).split("->");

        from = sl.at(0);
        to = sl.at(1);

        source.setFileName(from);
        target.setFileName(to);

        qDebug() << "Moving" << source.fileName() << "to" << target.fileName();
        success = false;

        while (!target.remove());

//        if (target.remove()) {
            if (source.rename(to)) {
                success = true;
            }
            else {
                qDebug() << "rename failed";
            }
//        }

        if (!success) {
            qDebug() << " Failed";
        }
        else {
            qDebug() << " Success!";
        }
    }
}

void ApplicationUpdateInterface::closeUpdaterExe() {
    writeCommand(CLOSE_REQUEST);
}
