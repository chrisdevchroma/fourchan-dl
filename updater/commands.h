#ifndef COMMANDS_H
#define COMMANDS_H

#define APPLICATION_PORT    60001
#define UPDATER_PORT        60000
#define HOST_ADDRESS        QHostAddress::LocalHost

#include <QByteArray>
#include <QString>

enum COMMANDS { PING=0,
                PONG,
                CLEAR,          // Clear all settings / file information
                SET_URI,        // Set URI to file
                SET_TARGET,     // Set local filename
                ADD_SET,        // Add current URI/TARGET set to update list
                SET_EXE,        // Set executable file to start after update process
                START,          // Start the update
                STOP,           // Stop the update
                CLOSE_REQUEST,  // Request the other program to close
                DISPLAY_MSG,    // Display a message
                ERROR,          // Indicate an error
                UPDATE_FINISHED, //
                GET_VERSION,
                VERSION
              };

QByteArray createCommand(int c, QByteArray a);
QByteArray createCommand(int c, QString s);
QByteArray createCommand(int c);
#endif // COMMANDS_H
