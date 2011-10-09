#include "applicationinterface.h"

ApplicationInterface::ApplicationInterface(QObject *parent) :
    QObject(parent)
{
    output = new QTextStream(stdout);
    udpSocket = new QUdpSocket(this);
    timer = new QTimer();
    timer->setSingleShot(true);
    timeout = new QTimer();
    timeout->setSingleShot(true);
    _connected = false;
    _updateFinished = true;

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTrigger()));
    connect(timeout, SIGNAL(timeout()), this, SLOT(timeoutOccured()));

    if (!(udpSocket->bind(QHostAddress(HOST_ADDRESS), UPDATER_PORT)))
        p(QString("Could not bind to %1:%2").arg(HOST_ADDRESS).arg(UPDATER_PORT));
    else {
        p(QString("Listening on %1:%2").arg(HOST_ADDRESS).arg(UPDATER_PORT));

        timer->start(1000);
    }
}

void ApplicationInterface::timerTrigger(void) {
    udpSocket->writeDatagram(createCommand(PING), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);
    pinging = true;
    timeout->start(TIMEOUT_VALUE);
}

void ApplicationInterface::p(QString msg) {
    *output << "ApplicationInterface: " << msg << endl;
    output->flush();
}

void ApplicationInterface::timeoutOccured() {
    if (_connected) p("Not connected");

    _connected = false;

    timer->start(PING_INTERVAL);

    emit applicationClosed(true);
}

void ApplicationInterface::readPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;

        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        processCommand(datagram);
    }
}

void ApplicationInterface::processCommand(QByteArray a) {
    int command;
    QByteArray payload;

    command = a.at(0);
    payload = a.mid(1);

    if (command != PONG)
        p("Received: "+QString::number(command)+":"+QString(payload));

    switch (command) {
    case PING:
        udpSocket->writeDatagram(createCommand(PONG), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);
        break;

    case PONG:
        if (pinging) {
            if (!_connected) {
                p("Connected");
                _connected = true;

                sendFailedFiles();
            }

            if (_updateFinished) {
                udpSocket->writeDatagram(createCommand(UPDATE_FINISHED), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);
            }

            pinging = false;
            timeout->stop();
            timer->start(PING_INTERVAL);

            emit applicationClosed(false);
        }
        else {
            p("Received unwanted PONG");
        }
        break;

    case CLEAR:
        if (_connected) {
            fu.filename = "";
            fu.uri = "";
            updateList.clear();
        }
        break;

    case ADD_SET:
        if (_connected) {
            updateList.append(fu);
            fu.filename = "";
            fu.uri = "";
        }
        break;

    case SET_TARGET:
        if (_connected) {
            fu.filename = QString(payload);
        }
        break;

    case SET_URI:
        if (_connected) {
            fu.uri = QString(payload);
        }
        break;

    case SET_EXE:
        if (_connected) {
            emit executableChanged(QString(payload));
        }
        break;

    case START:
        if (_connected) {
            emit startUpdate(updateList);
        }
        break;

    case CLOSE_REQUEST:
        if (_connected) {
            p("Received close request");
            emit close();
        }
        break;

    case DISPLAY_MSG:
        p(QString(payload));
        break;

    case GET_VERSION:
        udpSocket->writeDatagram(createCommand(VERSION, QString(PROGRAM_VERSION)), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);
        break;

    case STOP:
    default:
        p("Received unknown/unhandled command");
    }
}

void ApplicationInterface::sendCloseRequest() {
    udpSocket->writeDatagram(createCommand(CLOSE_REQUEST), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);

    closingApplication = true;

    p("Requesting Application close");
}

void ApplicationInterface::sendMessage(QString s) {
    udpSocket->writeDatagram(createCommand(DISPLAY_MSG, s), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);
}

void ApplicationInterface::sendError(QString s) {
    udpSocket->writeDatagram(createCommand(ERROR, s), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);
}

void ApplicationInterface::setFailedFiles(QList<FileUpdate> l) {
    failedFiles = l;

    if (_connected) sendFailedFiles();

    if (failedFiles.count() > 0)
        p("Some files failed");
}

void ApplicationInterface::sendFailedFiles() {
    if (_connected && (failedFiles.count() > 0)) {
        udpSocket->writeDatagram(createCommand(CLEAR), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);

        for (int i=0; i<failedFiles.count(); i++) {
            udpSocket->writeDatagram(createCommand(SET_URI, failedFiles.at(i).tmpFilename), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);
            udpSocket->writeDatagram(createCommand(SET_TARGET, failedFiles.at(i).filename), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);
            udpSocket->writeDatagram(createCommand(ADD_SET), QHostAddress(HOST_ADDRESS), APPLICATION_PORT);
        }
        failedFiles.clear();

        p("Transmitted failed files to main application");
    }
}

void ApplicationInterface::setUpdateFinished(bool b) {
    _updateFinished = b;
}
