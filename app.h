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
        bool res;							// признак успешности установки сигналов-слотов
        killTimer (ev->timerId ());			// остановка таймера
        res = QObject::connect (&_obj, SIGNAL (objectIsReady ()), this, SLOT (connectObject ()));
        Q_ASSERT_X (res, "connect", "connection is not established");	// установка связей с объектом
        const QStringList args =  QCoreApplication::arguments();
        _obj.starting (SIGNAL (finished ()), SLOT (terminate ()),  QThread::HighPriority,  args);					// запуск потока с высоким приоритетом
    }

private slots:
    void connectObject (void)		// установка связей с объектом
    {
        bool res;					// признак успешности установки сигналов-слотов
        res = QObject::connect (this, SIGNAL(finish()), _obj, SLOT(terminate()));
        Q_ASSERT_X (res, "connect", "connection is not established");	// закрытие этого объекта з акрывает объект в потоке
        res = QObject::connect (_obj, SIGNAL(finished()), this, SLOT(terminate()));
        Q_ASSERT_X (res, "connect", "connection is not established");	// конец операции завершает работу приложения

        emit(startAction());

    }
public slots:
    void terminate (void)			{ emit finish (); }		// завершение работы приложения

signals:
    void startAction (void);		// сигнал "запуск действия"
    void finish (void);

};

#endif // APP_H
