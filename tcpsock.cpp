/*
 * Copyright © 2018-2019 Yaroslav Shkliar <mail@ilit.ru>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Research Laboratory of IT
 * www.ilit.ru on e-mail: mail@ilit.ru
 */

#include "tcpsock.h"
#include "modbus-tcp.h"
#include <QDebug>

TcpSock::TcpSock(QObject *parent , QString *ip, quint16 *port) : QObject (parent)

{


    m_srv = new QTcpServer();
    if (m_srv){
        m_srv->listen(QHostAddress (*ip), *port);
        //m_sock = new QUdpSocket(this);

        surgardI = new surgard(this);

        // connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
        //connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
        //connect(this, SIGNAL(dataReady(QByteArray&)), surgardI, SLOT(setData(QByteArray&)) );

        connect(m_srv, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
        connect(this, SIGNAL(dataReady(QByteArray&)), surgardI, SLOT(setData(QByteArray&)) );

        //connect(&tcpClient, &QIODevice::bytesWritten, this, &(Dialog::updateClientProgress);
        connect(m_srv, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

        //changeInterface(*ip, *port);
        qDebug() << "Fire Alarm handling has been initialized." << "\n\r";
    } else {
        qDebug() << "Fire Alarm handling initialization error."<< "\n\r";

    }

}

TcpSock::~TcpSock()
{
    m_srv->close();
}



void TcpSock::changeInterface(const QString &address, quint16 portNbr)
{
    //m_srv->connectToHost(address, portNbr);
}


void TcpSock::acceptConnection()
{
    m_sock = m_srv->nextPendingConnection();
    if (!m_sock) {
        qDebug() << (tr("Error: got invalid pending connection from Kros-gate!"))<< "\n\r";
        return;
    }

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(m_sock, SIGNAL(disconnected()), m_sock,  SLOT(deleteLater()));

    connected = m_sock->state();

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

     //int i = m_sock->bytesAvailable();
    // char data[i];
    //in_stream.readRawData( data, i);

   // QByteArray data = m_sock->readAll();
    QByteArray data = m_sock->readAll();

    qDebug() << "Alarm raw data: " << data << " lenght - " << data.length() << "\n\r";
if (m_sock->atEnd())
    m_sock->write("\6");
//data="";
//data = m_sock->readAll();

emit (dataReady(data));

   // m_sock->close();
    blockSize = 0;

}

void TcpSock::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug()<<   ("Fire Alarm handling error: Remote host closed connection.");
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<   ("Fire Alarm handling error: The host was not found. Please check the "
                      "host name and port settings.")<< "\n\r";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<< ("Fire Alarm handling error: The connection was refused by the peer. "
                    "Make sure the fortune kros-gate daemon is running, "
                    "and check that the host name and port "
                    "settings are correct.")<< "\n\r";
        break;
    default:
        qDebug()<< ("Fire Alarm handling error: ") << (m_sock->errorString())<< "\n\r";
    }
    if (m_sock->isOpen())
        m_sock->close();
    connected = m_sock->state();


}
