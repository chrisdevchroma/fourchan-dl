#ifndef COMMANDS_H
#define COMMANDS_H

#define APPLICATION_PORT    60001
#define UPDATER_PORT        60000
#define HOST_ADDRESS        QHostAddress::LocalHost

#include <QByteArray>
#include <QString>

enum COMMANDS { PING=0, PONG, CLEAR, SET_URI, SET_TARGET,
                ADD_SET, SET_EXE, START, STOP, CLOSE_REQUEST,
                DISPLAY_MSG, ERROR};

QByteArray createCommand(int c, QByteArray a);
QByteArray createCommand(int c, QString s);
QByteArray createCommand(int c);
#endif // COMMANDS_H
