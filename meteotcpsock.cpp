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
    measure->insert("RAIN_HOUR", 0);
    measure->insert("ET_DAILY", 0);
    //measure->insert("REMOTE_BATT", 0);

    is_read = false;
    status = "";
    sample_t = 0;

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

    //emit (dataReady(data));


    blockSize = 0;

    measure->insert("PRESSURE",  measure->value("PRESSURE") + ((float)(((uchar(data[9])<<8)+(uchar(data[8]))))/1000)*25.4f);//inchs Hg TO mm Hg Conversion Formula
    measure->insert("TEMP_IN", measure->value("TEMP_IN") + ((float)((uchar(data[11])<<8) + (uchar(data[10])))/10-32)*5/9); //Fahrenheit TO Celsius Conversion Formula
    measure->insert("HUM_IN", measure->value("HUM_IN") + (float)(uchar(data[12])));
    measure->insert("TEMP_OUT", measure->value("TEMP_OUT") + ((float)((uchar(data[14])<<8) + (uchar(data[13])))/10-32)*5/9); //Fahrenheit TO Celsius Conversion Formula
    measure->insert("WIND_SPEED", measure->value("WIND_SPEED") + (float)(uchar(data[15]))*1609.344f);//mile to meter convertion formula
    measure->insert("WIND_DIR",  measure->value("WIND_DIR") + (float)((uchar(data[18])<<8) + uchar(data[17])));
    measure->insert("DEW_POINT",  measure->value("DEW_POINT") + ((float)((uchar(data[32])<<8) + (uchar(data[31])))-32)*5/9); //Fahrenheit TO Celsius Conversion Formula
    measure->insert("HUM_OUT", measure->value("HUM_OUT") + (float)(uchar(data[34])));

    if ((uchar(data[37])==0) && uchar(data[36]) == 0xff)
        measure->insert("HEAT_IDX", measure->value("HEAT_IDX") +0);
    else{
        measure->insert("HEAT_IDX", measure->value("HEAT_IDX") + ((float)((uchar(data[37])<<8) + (uchar(data[36])))-32)*5/9); //Fahrenheit TO Celsius Conversion Formula
    }
    if ((uchar(data[39])==0) && uchar(data[38]) == 0xff)
        measure->insert("WIND_CHILL", measure->value("WIND_CHILL") +0);
    else {

        measure->insert("WIND_CHILL", measure->value("WIND_CHILL") + ((float)((uchar(data[39])<<8) + (uchar(data[38])))-32)*5/9); //Fahrenheit TO Celsius Conversion Formula
    }
    if ((uchar(data[41])==0) && uchar(data[40]) == 0xff)
        measure->insert("THSW_IDX", measure->value("THSW_IDX") +0);
    else {
        measure->insert("THSW_IDX", measure->value("THSW_IDX") + ((float)((uchar(data[41])<<8) + (uchar(data[40])))-32)*5/9); //Fahrenheit TO Celsius Conversion Formula
    }

    measure->insert("RAIN_RATE", measure->value("RAIN_RATE") + ((float)(uchar(data[43])<<8) + (uchar(data[42])))*0.2f);//mm per hour

    if (uchar(data[44]) == 0xff)
        measure->insert("UV_IDX", measure->value("UV_IDX") +0);
    else {
        measure->insert("UV_IDX", measure->value("UV_IDX") + (float)(uchar(data[44])));
    }

    if ((uchar(data[46])==0x7f) && uchar(data[45]) == 0xff)
        measure->insert("SOL_RAD", measure->value("SOL_RAD") +0);
    else {
        measure->insert("SOL_RAD",  measure->value("SOL_RAD") + ((float)(uchar(data[46])<<8) + (uchar(data[45]))));//unit in watt on m2
    }
    measure->insert("RAIN_DAILY",  measure->value("RAIN_DAILY") + ((float)(uchar(data[52])<<8) + (uchar(data[51])))*0.2f);//last day quantity
    measure->insert("RAIN_HOUR", measure->value("RAIN_DAILY") + ((float)(uchar(data[56])<<8) + (uchar(data[55])))*0.2f); //last hour quantity
    measure->insert("ET_DAILY",  measure->value("ET_DAILY") + ((float)(((uchar(data[69])<<8)+(uchar(data[68]))))/1000)*25.4f);//inchs  TO mm Conversion Evapotranspiration Formula
    //   measure->insert("REMOTE_BATT",  measure->value("REMOTE_BATT") + (float)(uchar(data[87])));//%

    sample_t++;
}

void MeteoTcpSock::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<   ("Meteostation handling error: The host was not found. Please check the "
                      "host name and port settings.");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<< ("Meteostation handling error: The connection was refused by the peer. "
                    "Make sure the fortune server is running, "
                    "and check that the host name and port "
                    "settings are correct.");
        break;
    default:
        qDebug()<< ("Meteostation handling error: ") << (m_sock->errorString());
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
