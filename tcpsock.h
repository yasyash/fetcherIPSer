#ifndef TCPSOCKSWIDGET_H
#define TCPSOCKSWIDGET_H

#include <QTcpSocket>
#include "surgard.h"


class TcpSock : public QObject
{
Q_OBJECT

public:
    TcpSock(QObject *parent, QString *ip, quint16 *port);
    virtual ~TcpSock();
    // IModbus interface


protected:
    void changeInterface(const QString& address, quint16 portNbr);

private slots:
    void on_cbEnabled_clicked(bool checked);
    void readData();
    void displayError(QAbstractSocket::SocketError socketError);

signals:
    void tcpPortActive(bool val);
    void connectionError(const QString &msg);
    void dataReady(QByteArray &str);

private:
    QTcpSocket *m_sock;
    quint32 blockSize;
    surgard *surgardI;
    QString *m_ip;
    int     *m_port;

   // QDataStream *in_stream;
};

#endif // TCPSOCKSWIDGET_H
