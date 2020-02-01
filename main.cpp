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

#include <QCoreApplication>
#include "WSHttpBinding_USCOREIAutoChromWCFHost.nsmap"


#include "app.h"

//Use next command line perameters
//-slaveid 1 3 11 16 -port /dev/ttyr00 -baud 9600 -data 8 -stop 1 -parity none -db database -user username -pwd password -dustip 192.168.1.150 -dustport 3602 -alarmip 192.168.1.110 -alarmport 5555 -upsport 3493 -polltime 20
//Also you would to use verbose mode with '-verbose' option

_App * globalApp = NULL;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    _App app;                    // application object

    bool res;							// success ops flag
    globalApp = &app;
    app.startTimer(0);



    res = QObject::connect (&app, SIGNAL (finish ()), &a, SLOT (quit ()));
    Q_ASSERT_X (res, "connect", "connection is not established");	// closing this object is close app
    res = QObject::connect (&a, SIGNAL (aboutToQuit ()), &app, SLOT (terminate ()));
    Q_ASSERT_X (res, "connect", "connection is not established");	// closing app is close this object


    return a.exec();
}
