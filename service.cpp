
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
