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

#include "grimm.h"

#include <QDebug>

Grimm::Grimm(QObject *parent, QString *device,
             int baud , int parity , int data_bits ,
             int stop_bits ) : QObject (parent), m_rs232( new QSerialPort(this))

{


    m_rs232->setPortName(*device);
    m_rs232->setBaudRate(baud);
    m_rs232->setDataBits(QSerialPort::DataBits(data_bits));
    m_rs232->setParity(QSerialPort::Parity(parity));
    m_rs232->setStopBits(QSerialPort::StopBits (stop_bits));
    m_rs232->setFlowControl(QSerialPort::SoftwareControl);
    if (m_rs232->open(QIODevice::ReadWrite)) {
        connected = m_rs232->isOpen();
    } else {
        displayError(m_rs232->error());

    }

    connect(m_rs232, SIGNAL(readyRead()), this, SLOT(readData()));

    connect(m_rs232, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(displayError(QSerialPort::SerialPortError)));
    //connect(this, SIGNAL(dataReady(QByteArray&)), this, SLOT(setData(QByteArray&)) );



    measure = new  QMap<QString, float>;
    sample_t = new QMap<QString, int>;

    //measure->insert("PM4", 0f);
    measure->insert("PM", 0.0f);
    measure->insert("PM10", 0.0f);
    measure->insert("PM2.5", 0.0f);
    measure->insert("PM1", 0.0f);

    sample_t->insert("PM", 0);
    sample_t->insert("PM10", 0);
    sample_t->insert("PM2.5", 0);
    sample_t->insert("PM1", 0);

    is_read = false;
    status = "";
    connected = m_rs232->isOpen();

    _buffer = new QByteArray();

    //Grimm command for acqusition
    if (connected)
    {
        QByteArray _byte;
        _byte.resize(1);
        _byte[0] = 82; //"R" command for start measure
        sendData( &_byte);

    }

    qDebug() << "Dust measure equipment handling via USB port has been initialized.";

}

Grimm::~Grimm()
{
    if (m_rs232->isOpen())
        m_rs232->close();
    //m_rs232->~QSerialPort();



}

void Grimm::reOpen()
{
    if(!m_rs232->isOpen())
    {
        m_rs232->setPortName(m_rs232->portName());
        m_rs232->setBaudRate(m_rs232->baudRate());
        m_rs232->setDataBits(m_rs232->dataBits());
        m_rs232->setParity(m_rs232->parity());
        m_rs232->setStopBits(m_rs232->stopBits());
        m_rs232->setFlowControl(QSerialPort::SoftwareControl);

        if (m_rs232->open(QIODevice::ReadWrite)) {
            connected = m_rs232->isOpen();
        } else {
            displayError(m_rs232->error());

        }
    }

    if (connected)
    {
        QByteArray _byte;
        _byte.resize(1);
        _byte[0] = 82; //"R" command for start measure
        sendData( &_byte);

    }
    _buffer->clear();
    is_read = false;
}


void Grimm::readData()
{

    QStringList list;
    int ind;
    int running;
    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");

    QByteArray data = m_rs232->readAll();

    if (is_read)
    {
        measure->insert("PM", 0.0f);
        measure->insert("PM10", 0.0f);
        measure->insert("PM2.5", 0.0f);
        measure->insert("PM1", 0.0f);

        sample_t->insert("PM", 0);
        sample_t->insert("PM10", 0);
        sample_t->insert("PM2.5", 0);
        sample_t->insert("PM1", 0);

        is_read = false;
    }

    _buffer->append(data); //filling all data fragments
    //_buffer->replace(QByteArray("\x0"), QByteArray("\x20"));//replace /0 character

    list = QString(*_buffer).simplified().split(QRegExp("[\x20{\x20}]"));

    int _ind = list.indexOf("N_");
    //qDebug() << "Dust measure USB equipment data: " << data << " lenght - " << data.length() << " \n";

    //qDebug() << "Dust measure USB equipment buffer: " << *_buffer << " lenght - " << _buffer->length() << " \n";
    //qDebug() << "Index of buffer: " << _ind << " AND lenght - " << _buffer->length() << " \n";

    this->is_read = false;

    if ((_ind > 0 ) && (list.length() > _ind + 5)) { //detecting that data is not fragmented on read cycles may be without total count of particles
        float result = list.value(_ind + 1).toFloat()/1000; //TSP
        if (result > 0) {
            measure->insert("PM",  measure->value("PM") + result);
            sample_t->insert("PM", sample_t->value("PM") + 1);
        }

        result = list.value(_ind + 2).toFloat()/1000; //PM10
        if (result > 0) {
            measure->insert("PM10",  measure->value("PM10") + result);
            sample_t->insert("PM10", sample_t->value("PM10") + 1);
        }

        result = list.value(_ind + 3).toFloat()/1000; //PM2.5
        if (result > 0) {
            measure->insert("PM2.5",  measure->value("PM2.5") + result);
            sample_t->insert("PM2.5", sample_t->value("PM2.5") + 1);
        }

        result = list.value(_ind + 4).toFloat()/1000; //PM1
        if (result > 0) {
            measure->insert("PM1",  measure->value("PM1") + result);
            sample_t->insert("PM1", sample_t->value("PM1") + 1);
        }

        qDebug() << "Dust measure USB equipment data: " << list << " \n";

        _buffer->clear();


        emit (dataIsReady(&is_read, measure, sample_t));

    }


    //blockSize = 0;

}

void Grimm::displayError(QSerialPort::SerialPortError error)
{

    if (error == QSerialPort::ResourceError) {
        qDebug()<< ("Dust measure equipment USB port error: ") << m_rs232->errorString();

        if (m_rs232->isOpen())
        {
            m_rs232->close();
            connected = m_rs232->isOpen();
        }

    }


}

void Grimm::sendData( QByteArray *data)
{

    QString _msg = QString(data->at(0)) + QLatin1Char(10) +  QLatin1Char(13);


    qint64    lnt = m_rs232->write(_msg.toLatin1(), _msg.length());
    lnt = m_rs232->flush();

    qDebug()<< "Dust command: " << *data ;
}

void Grimm::writes()
{
    qDebug()<< "written " ;
}
