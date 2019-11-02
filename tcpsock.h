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

#ifndef TCPSOCKSWIDGET_H
#define TCPSOCKSWIDGET_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>

#include "surgard.h"


class TcpSock : public QObject
{
    Q_OBJECT

public:
    TcpSock(QObject *parent, QString *ip, quint16 *port);
    virtual ~TcpSock();
    // IModbus interface


protected:
    void changeInterface(const QString& address, quint16 portNbr);

public slots:
    // void on_cbEnabled_clicked(bool checked);
    //void start();
    void acceptConnection();
    void readData();
    void displayError(QAbstractSocket::SocketError socketError);

signals:
    void tcpPortActive(bool val);
    void connectionError(const QString &msg);
    void dataReady(QByteArray &str);

private:
    QTcpServer *m_srv = nullptr;
    QTcpSocket *m_sock = nullptr;
    quint32 blockSize;
    QString *m_ip;
    int     *m_port;

public:
    QAbstractSocket::SocketState connected = QAbstractSocket::UnconnectedState;
    surgard *surgardI;

    // QDataStream *in_stream;
};

#endif // TCPSOCKSWIDGET_H
