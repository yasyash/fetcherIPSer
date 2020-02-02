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

#include <QRandomGenerator>
#include <QDebug>

Grimm::Grimm(QObject *parent,  QString *ip, quint16 *port ) : QObject (parent)

{

    tracker = nullptr;
    //m_sock = new QTcpSocket(this);
    //changeInterface(*ip, *port);
    //m_sock->setSocketOption(QAbstractSocket::LowDelayOption, 0);
    //m_sock->setSocketOption(QAbstractSocket::TypeOfServiceOption, 64);

    //connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));

    //connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this, SIGNAL(dataReady(QByteArray&)), this, SLOT(setData(QByteArray&)) );



    measure = new  QMap<QString, float>;
    sample_t = new QMap<QString, int>;
    _buffer = new QByteArray();

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
    // connected = m_rs232->isOpen();
   // connected = m_sock->state();



     m_Timer = new QTimer( this );
     connect( m_Timer, SIGNAL(timeout()), this, SLOT(detect()));
     m_Timer->start(20000);

    // Create new tracker for new connection
    tracker = new qtelnet;
    // Add (optional) callback for receiving data from server
    tracker->set_data_recv_callback(&(Grimm::onDataRecv));
    tracker->set_data_recv_bundle((void*) this);

    QString _port = QString::number(ushort(*port));

    if (tracker) {
        int r = qtelnet::telnet_connect(*tracker,
                                        ip->toStdString().c_str(),
                                        _port.toStdString().c_str());
        if (r == 0) {
            // Connection success
            connected = true;
        //    QObject::connect(this, SIGNAL(Grimm::dataIsReady(bool*, QMap<QString, float>*, QMap<QString, int>*)), parent, SLOT(parent->fillSensorData(bool*, QMap<QString, float>*, QMap<QString, int>*)));
            qDebug() << "Dust measure equipment handling via Telnet 4711 port has been initialized.\n\r";

        } else {
            // When fail, clean up
            qtelnet::telnet_disconnect(*tracker);
            delete tracker;
            tracker = nullptr;
            qDebug() << "Dust measure equipment handling via Telnet 4711 port has not been initialized. Connection error...\n\r";

        }
    } else {
        qtelnet::telnet_disconnect(*tracker);
        delete tracker;
        tracker = nullptr;
        qDebug() << "Dust measure equipment handling via Telnet 4711 port has not been initialized. Handling error...\n\r";

    }



}

Grimm::~Grimm()
{
    if (m_sock->isOpen())
        m_sock->close();
    //m_rs232->~QSerialPort();



}

void Grimm::changeInterface(const QString &address, quint16 portNbr)
{
    m_sock->connectToHost(address, portNbr);
}

void Grimm::reOpen(QString *ip, quint16 *port)
{
    /*   if(!m_rs232->isOpen())
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
    }*/

   // if (connected)
   // {
        //QByteArray _byte;
        //_byte.resize(1);
        //_byte[0] = 82; //"R" command for start measure
       // sendData( &_byte);

        qtelnet::telnet_disconnect(*tracker);
        delete tracker;
        tracker = nullptr;


  //  }

    measure->insert("PM", 0.0f);
    measure->insert("PM10", 0.0f);
    measure->insert("PM2.5", 0.0f);
    measure->insert("PM1", 0.0f);

    sample_t->insert("PM", 0);
    sample_t->insert("PM10", 0);
    sample_t->insert("PM2.5", 0);
    sample_t->insert("PM1", 0);

    _buffer->clear();

    is_read = false;

    QString _port = QString::number(ushort(*port));

    tracker = new qtelnet;
    // Add (optional) callback for receiving data from server
    tracker->set_data_recv_callback(&(Grimm::onDataRecv));
    tracker->set_data_recv_bundle((void*) this);

    int r = qtelnet::telnet_connect(*tracker,
                                    ip->toStdString().c_str(),
                                    _port.toStdString().c_str());
    if (r == 0) {
        // Connection success
        connected = true;
    //    QObject::connect(this, SIGNAL(Grimm::dataIsReady(bool*, QMap<QString, float>*, QMap<QString, int>*)), parent, SLOT(parent->fillSensorData(bool*, QMap<QString, float>*, QMap<QString, int>*)));
        qDebug() << "Dust measure equipment handling via Telnet 4711 port has been initialized.\n\r";
         send_go();

    } else {
        // When fail, clean up
        qtelnet::telnet_disconnect(*tracker);
        //delete tracker;
        //tracker = nullptr;
        qDebug() << "Dust measure equipment handling via Telnet 4711 port has not been initialized. Connection error...\n\r";

    }


}


void Grimm::readData()
{

    QStringList list;
    int ind;
    int running;
    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");

    QByteArray data =m_sock->readAll();

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
        float result = list.value(_ind + 1).toFloat()/1000; //TSP assignment
        if (result > 0) {
            measure->insert("PM",  measure->value("PM") + result);
            sample_t->insert("PM", sample_t->value("PM") + 1);
        }

        result = list.value(_ind + 2).toFloat()/1000; //PM10 assignment
        if (result > 0) {
            measure->insert("PM10",  measure->value("PM10") + result);
            sample_t->insert("PM10", sample_t->value("PM10") + 1);
        }

        result = list.value(_ind + 3).toFloat()/1000; //PM2.5 assignment
        if (result > 0) {
            measure->insert("PM2.5",  measure->value("PM2.5") + result);
            sample_t->insert("PM2.5", sample_t->value("PM2.5") + 1);
        }

        result = list.value(_ind + 4).toFloat()/1000; //PM1 assignment
        if (result > 0) {
            measure->insert("PM1",  measure->value("PM1") + result);
            sample_t->insert("PM1", sample_t->value("PM1") + 1);
        }

        qDebug() << "Dust measure Telnet equipment data: " << list << " \n\r";

        _buffer->clear();


        emit (dataIsReady(&is_read, measure, sample_t));

    }


    //blockSize = 0;

}



void Grimm::sendData( QByteArray *data)
{

    QString _msg =  QString(data->at(0)) +  QLatin1Char(13)  + QLatin1Char(10);


    qint64    lnt = m_sock->write(_msg.toLatin1(), _msg.length());
    //lnt = m_sock->flush();

    qDebug()<< "Dust command: " << *data << "\n\r";
}

void Grimm::writes()
{
    qDebug()<< "written " ;
}


void Grimm::rand()
{
    QRandomGenerator generator;
    float sum = 0;

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

    for (int i = 0; i < 3; ++i)
    {
        double value = generator.generateDouble();
        // qDebug()<< "Random : " << value ;


        if (i == 0) {
            measure->insert("PM10",  measure->value("PM10") + value/100);
            sample_t->insert("PM10", sample_t->value("PM10") + 1);
        }

        if (i == 1) {
            measure->insert("PM2.5",  measure->value("PM2.5") + value/100);
            sample_t->insert("PM2.5", sample_t->value("PM2.5") + 1);
        }

        if (i == 2) {
            measure->insert("PM1",  measure->value("PM1") + value/100);
            sample_t->insert("PM1", sample_t->value("PM1") + 1);

        }

        sum +=value;
    }


    measure->insert("PM",  measure->value("PM") + sum/100);
    sample_t->insert("PM", sample_t->value("PM") + 1);


    emit (dataIsReady(&is_read, measure, sample_t));


}

void Grimm::detect()
{
if (!tracker->worker_running)
{
connected  = false;
}

}

void Grimm::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<   ("Dust measure equipment handling error: The host was not found. Please check the "
                      "host name and port settings.\n\r");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<< ("Dust measure equipment handling error: The connection was refused by the peer. "
                    "Make sure the fortune server is running, "
                    "and check that the host name and port "
                    "settings are correct.\n\r");
        break;
    default:
        qDebug()<< ("Dust measure equipment handling error: ") << (m_sock->errorString()) << "\n\r";
    }

    if (m_sock->isOpen())
        m_sock->close();
    connected = m_sock->state();

}

void Grimm::onDataRecv(const char *data, int size, void *bundle)
{
    char buf[size + 1];
    strncpy(buf, data, size);
    buf[size] = 0;
    Grimm *_grimm = (Grimm*) bundle;
    //MainWindow *window = (MainWindow*) bundle;
    //window->consoleText.append(QString(buf));

    QStringList list;
    int ind;
    int running;
    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");


    if (_grimm->is_read)
    {
        _grimm->measure->insert("PM", 0.0f);
        _grimm->measure->insert("PM10", 0.0f);
        _grimm->measure->insert("PM2.5", 0.0f);
        _grimm->measure->insert("PM1", 0.0f);

        _grimm->sample_t->insert("PM", 0);
        _grimm->sample_t->insert("PM10", 0);
        _grimm->sample_t->insert("PM2.5", 0);
        _grimm->sample_t->insert("PM1", 0);

        _grimm->is_read = false;
    }

    _grimm->_buffer->append(data); //filling all data fragments
    //_buffer->replace(QByteArray("\x0"), QByteArray("\x20"));//replace /0 character

    list = QString(*_grimm->_buffer).simplified().split(QRegExp("[\x20{\x20}]"));

    QStringList _filterN = list.filter("N");
    //qDebug() << "Dust measure USB equipment data: " << data << " lenght - " << data.length() << " \n";

    //qDebug() << "Dust measure USB equipment buffer: " << *_buffer << " lenght - " << _buffer->length() << " \n";
    //qDebug() << "Index of buffer: " << _ind << " AND lenght - " << _buffer->length() << " \n";

    _grimm->is_read = false;

    if ((_filterN.length() > 0 ) && (list.length() >  5)) { //detecting that data is not fragmented on read cycles may be without total count of particles
        int  _ind = list.indexOf(_filterN[0]);

        float result = list.value(_ind + 1).toFloat()/1000; //TSP
        if (result > 0) {
            _grimm->measure->insert("PM",  _grimm->measure->value("PM") + result);
            _grimm->sample_t->insert("PM", _grimm->sample_t->value("PM") + 1);
        }

        result = list.value(_ind + 2).toFloat()/1000; //PM10
        if (result > 0) {
            _grimm->measure->insert("PM10",  _grimm->measure->value("PM10") + result);
            _grimm->sample_t->insert("PM10", _grimm->sample_t->value("PM10") + 1);
        }

        result = list.value(_ind + 3).toFloat()/1000; //PM2.5
        if (result > 0) {
            _grimm->measure->insert("PM2.5",  _grimm->measure->value("PM2.5") + result);
            _grimm->sample_t->insert("PM2.5", _grimm->sample_t->value("PM2.5") + 1);
        }

        result = list.value(_ind + 4).toFloat()/1000; //PM1
        if (result > 0) {
            _grimm->measure->insert("PM1",  _grimm->measure->value("PM1") + result);
            _grimm->sample_t->insert("PM1", _grimm->sample_t->value("PM1") + 1);
        }

        qDebug() << "Dust measure Telnet equipment data: " << list << "\n\r";

        _grimm->_buffer->clear();


        //emit (_grimm->dataIsReady(&_grimm->is_read, _grimm->measure, _grimm->sample_t));

        _grimm->callbackFunc(&_grimm->is_read, _grimm->measure, _grimm->sample_t );

    }

}

void Grimm::setCallbackFunc(void (*func)(bool *is_read, QMap<QString, float> *_measure, QMap<QString, int> *_sample))
{
callbackFunc = func;
}

void Grimm::send_go()
{
    QByteArray _go =  QByteArray("v");

    //Grimm command for acqusition
    qtelnet::send_text(*tracker,
                       _go.toStdString().c_str(),
                       _go.length());
}
