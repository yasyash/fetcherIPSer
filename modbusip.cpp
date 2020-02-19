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

#include "modbusip.h"
#include <stdlib.h>
#include <QDebug>
#include <QThread>

#ifdef MODBUSIP_H
ModbusIP::ModbusIP(QObject *parent , QString *ip, quint16 *port) : QObject (parent)

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

    measure = new  QMap<QString, int>;
    sample_t = new QMap<QString, int>;



    /*   measure->insert("CO", 0);
    measure->insert("NO2", 0);
    measure->insert("NO", 0);
    measure->insert("SO2", 0);
    measure->insert("H2S", 0);
    measure->insert("CH2O", 0);
    measure->insert("O3", 0);
    measure->insert("NH3", 0);

    sample_t->insert("CO", 0);
    sample_t->insert("NO2", 0);
    sample_t->insert("NO", 0);
    sample_t->insert("SO2", 0);
    sample_t->insert("H2S", 0);
    sample_t->insert("CH2O", 0);
    sample_t->insert("O3", 0);
    sample_t->insert("NH3", 0);*/

    is_read = false;
    status = "";
    connected = m_sock->state();

    qDebug() << "ModbusIP measure equipment handling has been initialized.\n\r";

}
#endif


ModbusIP::~ModbusIP()
{
    m_sock->disconnectFromHost();
}



void ModbusIP::changeInterface(const QString &address, quint16 portNbr)
{
    m_sock->connectToHost(address, portNbr);
}





void ModbusIP::on_cbEnabled_clicked(bool checked)
{
    if (checked)
    {
    }
    else {
        m_sock->disconnectFromHost();
    }
    //emit tcpPortActive(checked);
}


void ModbusIP::readData()
{

    QStringList list;
    int ind;
    int running;
    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");

    QByteArray data = m_sock->readAll();

    qDebug() << "ModbusIP measure equipment data: " << data << " lenght - " << data.length() << " \n\r";

    // if (is_read)
    //  {
    //    measure ->insert("SO2", 0);
    //     measure->insert("H2S", 0);
    //     sample_t->insert("SO2", 0);
    //      sample_t->insert("H2S", 0);
    //     is_read = false;
    //}

    int i = data.indexOf(':');
    int pool;


    while (i != -1)
    {
        QByteArray _tmp = QByteArray(1, data[i+3]) ;

        if (_tmp.toInt() == 0) //error detecting if modbus function
        {
            QString md, name, result;

            _tmp = QByteArray(1, data[i+1]) ;
            _tmp.append(data[i+2]);

            int addr = _tmp.toInt();

            //i+=6;
            _tmp = QByteArray(1, data[i+6]);
            pool = _tmp.toInt()*2;
            if (pool > 0){
                //i++;// first element 7

                _tmp = QByteArray(1, data[i+9]);
                _tmp.append(data[i+10]);
                int _mode = _tmp.toInt();

                _tmp = QByteArray(1, data[i+8]);
                int _type = _tmp.toInt();

                if (_type == 2){ name = "CO"; //detect type of a sensor HARDCODED for OPTEC's equipments
                    if (_mode == 2) md = "fault";
                    else
                    {md = (_mode ?  "off" :  "measuring");};
                }
                if (_type == 4){ name = "NO2"; //detect type of a sensor HARDCODED
                    if (_mode == 7) md = "change sensor";
                    else
                    {md = (_mode ?  "off" :  "measuring");};
                }

                if (_type == 6){ name = "SO2"; //detect type of a sensor HARDCODED
                    if (_mode == 7) md = "change sensor";
                    else
                    {md = (_mode ?  "off" :  "measuring");};
                }

                if ((_type == 2) && (addr > 29)){ name = "CH2O"; //hardcoded for the Fort measure equipment address = 30 (or OPTEC's equipments)
                    if (_mode == 2) md = "fault";
                    else
                    {md = (_mode ?  "off" :  "measuring");};
                }

                if (_type == 1){ name = "O3"; //detect type of a sensor HARDCODED for OPTEC's equipments
                    if (_mode == 2) md = "fault";
                    else
                    {md = (_mode ?  "off" :  "measuring");};
                }

                if (_type == 7){ name = "NO"; //detect type of sensor HARDCODED
                    if (_mode == 7) md = "change sensor";
                    else
                    {md = (_mode ?  "off" :  "measuring ");};
                }

                if (_type == 3){ name = "H2S"; //detect type of sensor HARDCODED
                    if (_mode == 7) md = "change sensor ";
                    else
                    {md = (_mode ?  "off" :  "measuring ");};
                }

                if ((_type == 1) && (addr > 2)) { name = "NH3"; //detect type of sensor HARDCODED
                    if (_mode == 7) md = "change sensor ";
                    else
                    {md = (_mode ?  "off" :  "measuring ");};
                }

                _tmp = QByteArray(1, data[i+11]) ;
                _tmp.append(data[i+12]);
                _tmp.append(data[i+13]);
                _tmp.append(data[i+14]);

                int _measure = _tmp.toInt();


                QTextStream(&result) << name << " : " << md << " : " << _measure;
                qDebug() << result;

                measure ->insert(name, measure->value(name) + _measure);
                sample_t->insert(name, sample_t->value(name) + 1);


                i = data.indexOf(':', pool+i); //detection of new chain position
            }
            else {
                i = data.indexOf(':', i+1);
            }
        } else {
            i = data.indexOf(':', i+1); //detection of new chain position - when bundle error

        }
        is_read = false;

    }
    if (!is_read)
        emit dataIsReady(&is_read, measure, sample_t);






}

void ModbusIP::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<   ("ModbusIP  equipment handling error: The host was not found. Please check the "
                      "host name and port settings.\n\r");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<< ("ModbusIP equipment handling error: The connection was refused by the peer. "
                    "Make sure the fortune server is running, "
                    "and check that the host name and port "
                    "settings are correct.\n\r");
        break;
    default:
        qDebug()<< ("ModbusIP equipment handling error: ") << (m_sock->errorString()) << "\n\r";
    }

    if (m_sock->isOpen())
        m_sock->close();
    connected = m_sock->state();

}

void ModbusIP::sendData(int address, int registers)
{
    /*  int checksum =  0; //id = 0
    checksum = checksum ^ command ^ data->length() ^ data->at(0) ^ data->at(1);

    QString _msg = QString(0x02) + QLatin1Char(0) + QLatin1Char(command) + QLatin1Char(0x03) + QString(data->length()) + QString(data->at(0)) + QString(data->at(1)) + QString(checksum) + QLatin1Char(0x04);
    //strcat(str,  QLatin1Char(51));
    //strcat(str,  "\r");*/

    QByteArray ba, _ba;
    QString _str, _str1;
    QChar _ch;
    int hi;
    int low;
    int sum = 0;

    for (int i = 1; i < registers; i+=2)
    {
        hi = int(address / 10);
        low = address - hi;

        _ch =  QChar(toAscii(&hi));

        ba.resize(17);
        ba[0] = 58; //:
        ba[1] = _ch.toLatin1() ; //0 hi

        _ch =  QChar(toAscii(&low));

        ba[2] = _ch.toLatin1();//3 addr low
        ba[3] = 48;//0
        ba[4] = 51;//3 func
        ba[5] = 48;//0
        ba[6] = 48;//0
        ba[7] = 48;//0

        _ch =  QChar(toAscii(&i));

        ba[8] = _ch.toLatin1();//from reg
        ba[9] = 48;
        ba[10] = 48;
        ba[11] = 48;
        ba[12] = 50;//2 -  amount of registers

        sum = 256 - (hi +low + 3 + i + 2); //LRC code

        hi = int(sum / 16);
        low = sum - hi*16;

        _ch =  QChar(toAscii(&hi));

        ba[13] = _ch.toLatin1();

        _ch =  QChar(toAscii(&low));

        ba[14] = _ch.toLatin1();

        ba[15] = 13;
        ba[16] = 10;





        qint64 lnt = ba.size();//qint64(strlen(str));

        lnt = m_sock->write(ba, lnt);
        lnt = m_sock->flush();
        QThread::msleep(300);
        qDebug()<< "\n\ModbusIP command: " << ba <<"\n\r" ;
    }

}

int ModbusIP::toAscii(int *_ch)
{
    if (*_ch == 0)
        return 48;

    if (*_ch == 1)
        return 49;

    if (*_ch == 2)
        return 50;

    if (*_ch == 3)
        return 51;

    if (*_ch == 4)
        return 52;

    if (*_ch == 5)
        return 53;

    if (*_ch == 6)
        return 54;

    if (*_ch == 7)
        return 55;

    if (*_ch == 8)
        return 56;

    if (*_ch == 9)
        return 57;

    if (*_ch == 10)
        return 65;

    if (*_ch == 11)
        return 66;

    if (*_ch == 12)
        return 67;

    if (*_ch == 13)
        return 68;

    if (*_ch == 14)
        return 69;

    if (*_ch == 15)
        return 70;


}

void ModbusIP::writes()
{
    qDebug()<< "written " ;
}
