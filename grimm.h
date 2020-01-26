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


#include <QUdpSocket>
#include <QTcpSocket>
#include <QSerialPort>
#include <QTimer>

class Grimm : public QObject
{
    Q_OBJECT

public:
    Grimm(QObject *parent, QString *device,
                            int baud = 9600, int parity = 0, int data_bits = 8,
                            int stop_bits = 1);
    virtual ~Grimm();

    void sendData(QByteArray *data);
    void reOpen();

protected:

private slots:
    void readData();
    void displayError(QSerialPort::SerialPortError error);
    void writes();
    void rand();


signals:
    void errorOccurred(QSerialPort::SerialPortError error);
    void readyRead();
    void dataIsReady(bool *is_read, QMap<QString, float> *_measure, QMap<QString, int> *_sample );

private:

  QSerialPort  *m_rs232 = nullptr;
  QByteArray *_buffer;

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

};

#endif // GRIMM_H
