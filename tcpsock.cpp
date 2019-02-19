#include "tcpsock.h"
#include "modbus-tcp.h"

#include <QDebug>

TcpSock::TcpSock(QObject *parent , QString *ip, quint16 *port) : QObject (parent)

{


    m_sock = new QTcpSocket(this);
    surgardI = new surgard(this);

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(dataReady(QByteArray&)), surgardI, SLOT(setData(QByteArray&)) );

    changeInterface(*ip, *port);
    qDebug() << "Fire Alarm handling has been initialized.";

}

TcpSock::~TcpSock()
{
    m_sock->disconnectFromHost();
}



void TcpSock::changeInterface(const QString &address, quint16 portNbr)
{
    m_sock->connectToHost(address, portNbr);
}





void TcpSock::on_cbEnabled_clicked(bool checked)
{
    if (checked)
    {
    }
    else {
        m_sock->disconnectFromHost();
    }
    //emit tcpPortActive(checked);
}


void TcpSock::readData()
{
    // QDataStream in_stream(m_sock);

    /* if (blockSize == 0) {
        // Relies on the fact that QDataStream serializes a quint32 into
        // sizeof(quint32) bytes
        if (m_sock->bytesAvailable() < int(sizeof(quint32)))
            return;
        in_stream >> blockSize;
    }

    if (m_sock->bytesAvailable() < 34 )
        return;
*/

    // int i = m_sock->bytesAvailable();
    // char data[i];
    //in_stream.readRawData( data, i);

    QByteArray data = m_sock->readAll();
    qDebug() << "Alarm data: " << data << " lenght - " << data.length() << " \n";

    emit (dataReady(data));


    blockSize = 0;

}

void TcpSock::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<   ("Fire Alarm handling error: The host was not found. Please check the "
                      "host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<< ("Fire Alarm handling error: The connection was refused by the peer. "
                    "Make sure the fortune server is running, "
                    "and check that the host name and port "
                    "settings are correct.");
        break;
    default:
        qDebug()<< ("Fire Alarm handling error: ") << (m_sock->errorString());
    }

}
