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

#include "surgard.h"

#include <QDebug>

surgard::surgard(QObject *parent) : QObject (parent)
{

}

surgard::~surgard()
{
}

void surgard::setData(QByteArray &data)
{

    QByteArray chnl_in = data.mid(1, 2); //surgard message format decoding
    QByteArray line = data.mid(3, 1);
    QByteArray equip_id = data.mid(7, 4);
    QByteArray  flag = data.mid(11, 1);
    QByteArray event = data.mid(12, 3);
    QByteArray group = data.mid(15, 2);
    QByteArray chain = data.mid(17, 3);

    qDebug() << "Fire alarm channel: " << (chnl_in);
    qDebug() << "Fire alarm line: " << (line);
    qDebug() << "Fire alarm equipment code: " << (equip_id);
    qDebug() << "Fire alarm flag: " << (flag);
    qDebug() << "Fire alarm event: " << (event);
    qDebug() << "Fire alarm group: " << (group);
    qDebug() << "Fire alarm chain: " << (chain);


}
