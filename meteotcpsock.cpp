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

#include "meteotcpsock.h"

#include <QDebug>

#ifdef DUSTTCP_H
MeteoTcpSock::MeteoTcpSock(QObject *parent , QString *ip, quint16 *port) : QObject (parent)

{


    m_sock = new QTcpSocket(this);

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_sock, SIGNAL(bytesWritten(qint64)), this, SLOT(writes()));

    connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this, SIGNAL(dataReady(QByteArray&)), this, SLOT(setData(QByteArray&)) );

    changeInterface(*ip, *port);
    m_sock->setSocketOption(QAbstractSocket::LowDelayOption, 0);
     //qDebug() << "Socket " << m_sock->socketOption(QAbstractSocket::SendBufferSizeSocketOption);
   // m_sock->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 1024);
   // qDebug() << "Socket next " << m_sock->socketOption(QAbstractSocket::SendBufferSizeSocketOption);

    m_sock->setSocketOption(QAbstractSocket::TypeOfServiceOption, 64);

    measure = new  QMap<QString, float>;
    measure->insert("PRESSURE", 0);
    measure->insert("TEMP_IN", 0);
    measure->insert("HUM_IN", 0);
    measure->insert("TEMP_OUT", 0);
    measure->insert("WIND_SPEED", 0);
    measure->insert("WIND_DIR", 0);
    measure->insert("DEW_POINT", 0);
    measure->insert("HUM_OUT", 0);
    measure->insert("HEAT_IDX", 0);
    measure->insert("WIND_CHILL", 0);
    measure->insert("THSW_IDX", 0);
    measure->insert("RAIN_RATE", 0);
    measure->insert("UV_IDX", 0);
    measure->insert("SOL_RAD", 0);
    measure->insert("RAIN_DAILY", 0);
    measure->insert("RAIN_15MIN", 0);
    measure->insert("RAIN_HOUR", 0);



















    is_read = false;
    status = "";
    qDebug() << "Meteostation handling has been initialized.";

}
#endif



MeteoTcpSock::~MeteoTcpSock()
{
    m_sock->disconnectFromHost();
}



void MeteoTcpSock::changeInterface(const QString &address, quint16 portNbr)
{
    m_sock->connectToHost(address, portNbr);
}





void MeteoTcpSock::on_cbEnabled_clicked(bool checked)
{
    if (checked)
    {
    }
    else {
        m_sock->disconnectFromHost();
    }
    //emit tcpPortActive(checked);
}


void MeteoTcpSock::readData()
{

    QStringList list;
    int ind;
    int running;
    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");

    QByteArray data = m_sock->readAll();

        qDebug() << "Meteostation sent data: " << data << " lenght - " << data.length() << " \n";

        this->is_read = true;

        emit (dataReady(data));


        blockSize = 0;

    }

    void MeteoTcpSock::displayError(QAbstractSocket::SocketError socketError)
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

    void MeteoTcpSock::sendData( char *data)
    {

        char *str = (char*)(malloc(strlen(data) * sizeof(char) + 1));
        *str = '\0';
        strcat(str, data);
        strcat(str,  "\n");
        qint64 lnt = qint64(strlen(str));

        lnt = m_sock->write(str, lnt);
        // lnt = m_sock->flush();

        qDebug()<< "Meteostation command: " << data ;
    }

    void MeteoTcpSock::writes()
    {
        qDebug()<< "written " ;
    }
