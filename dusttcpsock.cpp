#include "dusttcpsock.h"

#include <QDebug>

DustTcpSock::DustTcpSock(QObject *parent , QString *ip, quint16 *port) : QObject (parent)

{


    m_sock = new QTcpSocket(this);

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this, SIGNAL(dataReady(QByteArray&)), this, SLOT(setData(QByteArray&)) );

    changeInterface(*ip, *port);
    qDebug() << "Dust measure equipment handling has been initialized.";

}

DustTcpSock::~DustTcpSock()
{
    m_sock->disconnectFromHost();
}



void DustTcpSock::changeInterface(const QString &address, quint16 portNbr)
{
    m_sock->connectToHost(address, portNbr);
}





void DustTcpSock::on_cbEnabled_clicked(bool checked)
{
    if (checked)
    {
    }
    else {
        m_sock->disconnectFromHost();
    }
    //emit tcpPortActive(checked);
}


void DustTcpSock::readData()
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
    qDebug() << "Dust measure equipment data: " << data << " lenght - " << data.length() << " \n";

    emit (dataReady(data));


    blockSize = 0;

}

void DustTcpSock::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<   ("Dust measure equipment handling error: The host was not found. Please check the "
                      "host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<< ("Dust measure equipment handling error: The connection was refused by the peer. "
                    "Make sure the fortune server is running, "
                    "and check that the host name and port "
                    "settings are correct.");
        break;
    default:
        qDebug()<< ("Dust measure equipment handling error: ") << (m_sock->errorString());
    }

}
