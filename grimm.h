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

#ifndef GRIMM_H
#define GRIMM_H


#include <QTcpSocket>
#include <QSerialPort>
#include <QTimer>

#include "qtelnet.h"

class Grimm : public QObject
{
    Q_OBJECT

public:
    Grimm(QObject *parent,  QString *ip, quint16 *port);
    virtual ~Grimm();
    void changeInterface(const QString &address, quint16 portNbr);
    void sendData(QByteArray *data);
    void reOpen(QString *ip, quint16 *port);
    void setCallbackFunc(void (*func) (bool *is_read, QMap<QString, float> *_measure, QMap<QString, int> *_sample ));
    void send_go();

protected:
    static void onDataRecv(const char *data, int size, void *bundle);

private slots:
    void readData();
    void displayError(QAbstractSocket::SocketError socketError);
    void writes();
    void rand();
    void detect();


signals:
    void readyRead();
    void dataIsReady(bool *is_read, QMap<QString, float> *_measure, QMap<QString, int> *_sample );

private:
    QTcpSocket *m_sock;
    void (*callbackFunc)(bool *is_read, QMap<QString, float> *_measure, QMap<QString, int> *_sample);

    QSerialPort  *m_rs232 = nullptr;
    QByteArray *_buffer;
    QString *m_ip;
    int *m_port;

public:
    //enum _status {Idle, Waiting, Running};
    enum _command {RDMN, MSTATUS, MSTART, MSTOP, RMMEAS};
    QString model;
    QString status;
    bool is_read;
    QMap<QString, int> *sample_t;
    QMap<QString, float> *measure;
    enum _command last_command;
    bool connected = false;
    QTimer * m_Timer;

    qtelnet *tracker;


};

#endif // GRIMM_H
