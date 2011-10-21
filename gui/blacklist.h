#ifndef BLACKLIST_H
#define BLACKLIST_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>
#include <QFile>

class BlackList : public QObject
{
    Q_OBJECT
public:
    explicit BlackList(QObject *parent = 0);
    ~BlackList();
    void add(QString);
    void remove(QString);
    bool contains(QString);

private:
    QStringList list;
    QNetworkAccessManager* manager;
    QTimer* timer;
    QFile* blackListFile;
    QSettings* settings;
    bool useBlackList;
    int interval;

    void loadBlackList();
    void saveBlackList();

private slots:
    void replyFinished(QNetworkReply*);
    void cleanUp();

signals:

public slots:
    void loadSettings();

};

#endif // BLACKLIST_H
