#ifndef DUSTTCPSOCK_H
#define DUSTTCPSOCK_H

#include <QTcpSocket>


class DustTcpSock : public QObject
{
Q_OBJECT

public:
    DustTcpSock(QObject *parent, QString *ip, quint16 *port);
    virtual ~DustTcpSock();

    void sendData(char *data);


protected:
    void changeInterface(const QString& address, quint16 portNbr);

private slots:
    void on_cbEnabled_clicked(bool checked);
    void readData();
    void displayError(QAbstractSocket::SocketError socketError);
    void writes();

signals:
    void tcpPortActive(bool val);
    void connectionError(const QString &msg);
    void dataReady(QByteArray &str);

private:
    QTcpSocket *m_sock;
    quint32 blockSize;
    QString *m_ip;
    int     *m_port;

public:
    //enum _status {Idle, Waiting, Running};
    enum _command {RDMN, MSTATUS, MSTART, MSTOP, RMMEAS};
    QString model;
    QString status;
    bool is_read;
    uint sample_t;
    QMap<QString, int> *measure;
    enum _command last_command;

   // QDataStream *in_stream;
};

#endif // DUSTTCPSOCK_H
