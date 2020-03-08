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

#include "serinus.h"
#include <math.h>

#include <QDebug>

#ifdef SERINUS_H
Serinus::Serinus(QObject *parent , QString *ip, quint16 *port) : QObject (parent)

{

    m_sock = new QTcpSocket(this);

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
    // connect(m_sock, SIGNAL(bytesWritten(qint64)), this, SLOT(writes()));

    connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this, SIGNAL(dataIsReady(const QString)), this, SLOT(writes()) );

    changeInterface(*ip, *port);
    m_sock->setSocketOption(QAbstractSocket::LowDelayOption, 0);
    //qDebug() << "Socket " << m_sock->socketOption(QAbstractSocket::SendBufferSizeSocketOption);
    // m_sock->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 1024);
    // qDebug() << "Socket next " << m_sock->socketOption(QAbstractSocket::SendBufferSizeSocketOption);

    m_sock->setSocketOption(QAbstractSocket::TypeOfServiceOption, 64);

    measure = new  QMap<QString, float>;
    sample_t = new QMap<QString, int>;

    measure->insert("SO2", 0);
    measure->insert("H2S", 0);

    sample_t->insert("SO2", 0 );
    sample_t->insert("H2S", 0 );

    is_read = false;
    status = "";
    connected = m_sock->state();

    qDebug() << "Serinus measure equipment handling has been initialized.\n\r";

}
#endif


Serinus::~Serinus()
{
    m_sock->disconnectFromHost();
}



void Serinus::changeInterface(const QString &address, quint16 portNbr)
{
    m_sock->connectToHost(address, portNbr);
}





void Serinus::on_cbEnabled_clicked(bool checked)
{
    if (checked)
    {
    }
    else {
        m_sock->disconnectFromHost();
    }
    //emit tcpPortActive(checked);
}


void Serinus::readData()
{

    QStringList list;
    int ind;
    int running;
    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");

    QByteArray _data = m_sock->readAll();

    qDebug() << "Serinus buffer --- data: " << _data << " lenght - " << _data.length() << " \n\r";

    if (is_read)
    {
        measure ->insert("SO2", 0);
        measure->insert("H2S", 0);
        sample_t->insert("SO2", 0);
        sample_t->insert("H2S", 0);
        is_read = false;
    }

    char sign;
    int expo;
    uint32_t mantissa;
    uint32_t ieee;
    int i, j;
    float result;

    data.append( _data); //copy to main buffer

    if (data.length() > 16)  //data buffer detection on fullness
    {
        if (( int(data[2]) == 1 ) && (int(data[4]) == 10)) //detect right response for pri. and sec. gas request
        {
            for (j = 0; j < 2; j++) {



                sign = data [5*j + 6] >> 7;
                ieee = uint32_t((uchar(data [5*j + 6]) << 24 ) | (uchar(data [5*j + 7]) << 16) | (uchar(data[5*j + 8]) << 8) | uchar(data[5*j + 9]));
                expo = (ieee & 0x7F800000) >> 23;
                expo -= 127;

                mantissa = (ieee & 0x7FFFFF);
                float dec = 1.0f;
                for(i=0; i<=22; i++) {
                    if(((mantissa >> (22 - i)) & 1) != 0) {
                        dec += float(pow(2, -i-1));
                    }
                }

                result = float( pow(2,expo));
                if(sign)
                    result = -1*result * dec;
                else
                    result = result * dec;

                if (int(data[5*j + 5]) == 50){
                    sample_t->insert("SO2", sample_t->value("SO2")+1);
                    measure->insert("SO2",  measure->value("SO2") + result);
                }

                if (int(data[5*j + 5]) == 51){
                    sample_t->insert("H2S", sample_t->value("H2S")+1);
                    measure->insert("H2S", measure->value("H2S") + result);
                }

            }
            is_read = false;
            qDebug() << "Serinus measure equipment data: " << data << " lenght - " << data.length() << " \n\r";
            data.clear();
            emit dataIsReady(&is_read, measure, sample_t);

        }

    }


    blockSize = 0;

}

void Serinus::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<   ("Serinus measure equipment handling error: The host was not found. Please check the "
                      "host name and port settings.\n\r");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<< ("Serinus measure equipment handling error: The connection was refused by the peer. "
                    "Make sure the fortune server is running, "
                    "and check that the host name and port "
                    "settings are correct.\n\r");
        break;
    default:
        qDebug()<< ("Serinus measure equipment handling error: ") << (m_sock->errorString()) << "\n\r";
    }

    if (m_sock->isOpen())
        m_sock->close();
    connected = m_sock->state();

}

void Serinus::sendData(int command, QByteArray *data)
{
    int checksum =  0; //id = 0
    checksum = checksum ^ command ^ data->length() ^ data->at(0) ^ data->at(1);

    QString _msg = QString(0x02) + QLatin1Char(0) + QLatin1Char(command) + QLatin1Char(0x03) + QString(data->length()) + QString(data->at(0)) + QString(data->at(1)) + QString(checksum) + QLatin1Char(0x04);
    //strcat(str,  QLatin1Char(51));
    //strcat(str,  "\r");
    qint64 lnt = _msg.size();//qint64(strlen(str));

    lnt = m_sock->write(_msg.toLatin1(), lnt);
    lnt = m_sock->flush();

    qDebug()<< "\n\rSerinus command: " << _msg <<"\n\r" ;
}

void Serinus::writes()
{
    qDebug()<< "written " ;
}
