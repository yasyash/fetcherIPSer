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

#ifndef SURGARDWIDGET_H
#define SURGARDWIDGET_H

#include <QByteArray>
#include <QObject>
#include <QDateTime>



class surgard : public QObject
{
    Q_OBJECT

public:
     surgard(QObject *parent);
    ~surgard();

private slots:
    void setData(QByteArray &data);

public:
    QMap<QString, QString>   * m_table; //assosiative array of polling data <key, meaning>
    QMap<QDateTime, QString>   * m_event; //assosiative array of polling data <datetime, meaning>
    QMap<QDateTime, QString>   * m_event_code; //assosiative array of polling code data <datetime, code>




};

#endif // SURGARDWIDGET_H
