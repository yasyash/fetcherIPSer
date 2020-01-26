


#ifndef LIGA_H
#define LIGA_H

#include <QString>
#include <QDateTime>
#include <QMap>


#include "soapWSHttpBinding_USCOREIAutoChromWCFHostProxy.h"
#include "soapStub.h"

class Liga
{

public:
    Liga( QString *ip, quint16 *port);
    virtual ~Liga();

    void getLastResult();



protected:
   //void changeInterface(const QString& address, quint16 portNbr);

    //void on_cbEnabled_clicked(bool checked);
   // void readData();
   // void writes();


private:
    //QTcpSocket *m_sock;
    char *_action = "http://liga.AutoChrom.WCFHost/IAutoChromWCFHost/";

    quint32 blockSize;
    const QString *m_ip;
    const int *m_port;
    struct soap *m_soap;
    WSHttpBinding_USCOREIAutoChromWCFHostProxy *liga_proxy;

public:
    //enum _status {Idle, Waiting, Running};
    enum _command {RDMN, MSTATUS, MSTART, MSTOP, RMMEAS};
    QString model;
    QString status;
    bool error;
    bool is_read;
    QMap<int, QDateTime> *sample_t;
    QMap<QString, float> *measure;
    enum _command last_command;
    bool connected = false;
    // QDataStream *in_stream;
};


#endif // LIGA_H
