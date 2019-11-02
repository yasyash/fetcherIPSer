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

#ifndef DUSTTCPSOCK_H
#define DUSTTCPSOCK_H


#include <QUdpSocket>
#include <QTcpSocket>


class DustTcpSock : public QObject
{
    Q_OBJECT

public:
    DustTcpSock(QObject *parent, QString *ip, quint16 *port);
    virtual ~DustTcpSock();

    void sendData(char *data);


protected:
    void changeInterface(const QString& address, quint16 portNbr);

private slots:
    void on_cbEnabled_clicked(bool checked);
    void readData();
    void displayError(QAbstractSocket::SocketError socketError);
    void writes();

signals:
    void tcpPortActive(bool val);
    void connectionError(const QString &msg);
    void dataReady(QByteArray &str);

private:

#ifdef DUSTTCP_H
    QTcpSocket *m_sock;
#endif

#ifdef DUSTUDP_H
    QUdpSocket *m_sock;
#endif

    quint32 blockSize;
    QString *m_ip;
    int     *m_port;

public:
    //enum _status {Idle, Waiting, Running};
    enum _command {RDMN, MSTATUS, MSTART, MSTOP, RMMEAS};
    QString model;
    QString status;
    bool is_read;
    uint sample_t;
    QMap<QString, int> *measure;
    enum _command last_command;
    QAbstractSocket::SocketState connected = QAbstractSocket::UnconnectedState;
    // QDataStream *in_stream;
};

#endif // DUSTTCPSOCK_H
