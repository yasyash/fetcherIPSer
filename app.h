#ifndef APP_H
#define APP_H

#include <QObject>

#include "service.h"
#include "processor.h"

class _App : public QObject
{
    Q_OBJECT
public:

    ThreadedObject <processor>	_obj;



protected:
    void timerEvent (QTimerEvent *ev)
    {
        bool res;							// success ops flag
        killTimer (ev->timerId ());			// stop timer
        res = QObject::connect (&_obj, SIGNAL (objectIsReady ()), this, SLOT (connectObject ()));
        Q_ASSERT_X (res, "connect", "connection is not established");	// connection with object
        const QStringList args =  QCoreApplication::arguments();
        _obj.starting (SIGNAL (finished ()), SLOT (terminate ()),  QThread::HighPriority,  args);					// starting of a thread with high priority
    }

private slots:
    void connectObject (void)		// connection with object
    {
        bool res;					// seccess ops flag
        res = QObject::connect (this, SIGNAL(finish()), _obj, SLOT(terminate()));
        Q_ASSERT_X (res, "connect", "connection is not established");	// closing this object is close oblect in thread
        res = QObject::connect (_obj, SIGNAL(finished()), this, SLOT(terminate()));
        Q_ASSERT_X (res, "connect", "connection is not established");	// ending of ops is terminate work of application

        emit(startAction());

    }
public slots:
    void terminate (void)			{ emit finish (); }		//terminate work of application

signals:
    void startAction (void);		// signal start working
    void finish (void);

};

#endif // APP_H
