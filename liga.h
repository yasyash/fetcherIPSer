#ifndef LIGA_H
#define LIGA_H


#include "WSHttpBinding_USCOREIAutoChromWCFHost.nsmap"
#include "soapWSHttpBinding_USCOREIAutoChromWCFHostProxy.h"
#include "soapStub.h"
#include <QString>
#include <QDateTime>

class Liga
{

public:
    Liga( QString *ip, quint16 *port);
    virtual ~Liga();

    void sendData(int command, QByteArray *data);



protected:
   //void changeInterface(const QString& address, quint16 portNbr);

    //void on_cbEnabled_clicked(bool checked);
   // void readData();
   // void writes();


private:
    //QTcpSocket *m_sock;
    quint32 blockSize;
    QString *m_ip;
    int *m_port;
    soap *m_soap;

public:
    //enum _status {Idle, Waiting, Running};
    enum _command {RDMN, MSTATUS, MSTART, MSTOP, RMMEAS};
    QString model;
    QString status;
    bool is_read;
    QMap<int, QDateTime> *sample_t;
    QMap<QString, float> *measure;
    enum _command last_command;
    bool connected = false;
    // QDataStream *in_stream;
};


#endif // LIGA_H
