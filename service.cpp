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

#include "service.h"

void ThreadedObjectBase::initObject (QObject *obj)
{
    bool res;
    if (_finished_signal)		// set of signal work termination
    {	res = connect (obj, _finished_signal, this, SLOT (quit ()));
        Q_ASSERT_X (res, "connect", "connection is not established");	}	//at the end of an object life a flow will be ended
    {	res = connect (this, SIGNAL (finished ()), obj, _terminate_slot);
        Q_ASSERT_X (res, "connect", "connection is not established");	}	// before stopping a thread, the "stop work" object slot will be called
    if (_to_delete_later_object && _finished_signal)	// установить отложенное удаление объекта?
    {	res = connect (obj, _finished_signal, obj, SLOT (deleteLater ()));
        Q_ASSERT_X (res, "connect", "connection is not established");	}	// at the end of an object working will be set pending delete
    emit objectIsReady ();		// ready to work signal
}


 /*template  <class T>
void ThreadedObject<T>::starting (const char *FinishedSignal, const char *TerminateSlot, QThread::Priority Priority, bool ToDeleteLaterThread, bool ToDeleteLaterObject )		// new thread creating
    {
        _finished_signal = FinishedSignal;		//  remembering a name of a terminate signal
        _terminate_slot = TerminateSlot;		// remembering a name of a termination slot
        _to_delete_later_object = ToDeleteLaterObject;	// remembering pending object deletion flag
        start (Priority);	// create of an object
    }*/
