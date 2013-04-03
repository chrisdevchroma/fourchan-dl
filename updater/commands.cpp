#include "commands.h"

QByteArray createCommand(int c, QByteArray a) {
    QByteArray ba;

    ba[0] = c;
    ba.append(a);

    return ba;
}

QByteArray createCommand(int c, QString s) {
    return createCommand(c, s.toLatin1());
}

QByteArray createCommand(int c) {
    return createCommand(c, QByteArray());
}

