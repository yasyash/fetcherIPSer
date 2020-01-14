#ifndef LIGA_AC_H
#define LIGA_AC_H

#include "./soap/WSHttpBinding_USCOREIAutoChromWCFHost.nsmap"
#include "./soap/soapWSHttpBinding_USCOREIAutoChromWCFHostProxy.h"
#include "./soap/soapStub.h"

#include <QObject>

class liga_ac : public QObject
{
    Q_OBJECT

public:
    liga_ac(QObject *parent, QString *ip, quint16 *port);
    virtual ~liga_ac();

signals:
    void tcpPortActive(bool val);
    void connectionError(const QString &msg);
    void dataIsReady( bool *is_read, QMap<QString, float> *_measure, QMap<QString, int> *_sample  );


protected:

private slots:
    void readData();
    void displayError(const QString& strError);
    void writes();


private:
    struct soap *m_soap;
    WSHttpBinding_USCOREIAutoChromWCFHostProxy *liga;
    quint32 blockSize;
    QString *m_ip;
    int *m_port;

public:
    //enum _status {Idle, Waiting, Running};
    enum _command {RDMN, MSTATUS, MSTART, MSTOP, RMMEAS};
    QString model;
    QString status;
    bool is_read;
    QMap<QString, int> *sample_t;
    QMap<QString, float> *measure;
    enum _command last_command;
    bool connected = false;
};

#endif // LIGA_AC_H
