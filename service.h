#ifndef SERVICE_H
#define SERVICE_H

#include <QtCore>


// **  Base class to  thread creating

class ThreadedObjectBase: public QThread
{
    Q_OBJECT
public:
protected:
    const char *_finished_signal; //name of work termination signal
    const char *_terminate_slot; //name of work termination slot
    bool _to_delete_later_object; //set pending object deletion flag
    QStringList *_param;
    int _s;


    void initObject (QObject *obj); //setup

public:

    ThreadedObjectBase (QObject *parent = 0): QThread (parent){}
    ~ThreadedObjectBase  (void)	{  }

signals:
    void objectIsReady (void);				// object ready signal

};

// end of class ThreadedObjectBase



//* thread creating template class

template <class T>
class ThreadedObject: public ThreadedObjectBase
{

protected:
    T*	_obj;						// object in a new tread

public:
    int _mem;
    T* ptr (void)	{ return reinterpret_cast <T*> (_obj); } //pointer to an object T
    const T* cptr (void) const { return reinterpret_cast <const T*> (_obj); } //pointer to an constant object T

    // driving
    ThreadedObject (QObject *parent = 0): ThreadedObjectBase (parent), _obj (0)	{}
    ~ThreadedObject  (void)	{  }

    // state
    void starting (const char *FinishedSignal, const char *TerminateSlot, QThread::Priority Priority,  QStringList param ,  bool ToDeleteLaterThread = true, bool ToDeleteLaterObject = true )		// new thread creating
    {

        _finished_signal = FinishedSignal;		//  remembering a name of a terminate signal
        _terminate_slot = TerminateSlot;		// remembering a name of a termination slot
        _to_delete_later_object = ToDeleteLaterObject;	// remembering pending object deletion flag
        _param = &param;
        _s = 1;
        _mem = 2;
        int i =sizeof((_param));
        qDebug()<< *_param;
        start (Priority);	// create of an object
    }
    void run (void)
    {
        int tmp = _mem;
        QStringList arg= QCoreApplication::arguments();
        initObject (_obj = new T(nullptr, &arg));
        exec ();
    } // T obj init
    //state
    bool objectIsCreated (void) const	{ return _obj != 0; } //is ready to work



    // reloading
    operator T* (void)					{ return ptr (); }		// pointer to an object
    T* operator -> (void)				{ return ptr (); }		// pointer to an object
    operator const T* (void) const		{ return cptr (); }		// pointer to const object
    const T* operator -> (void) const	{ return cptr (); }		// pointer to const object


}; //end of class ThreadedObject

#endif // SERVICE_H

