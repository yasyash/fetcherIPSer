#include <QCoreApplication>


#include "app.h"


_App * globalApp = NULL;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    _App app;                    // объект
    bool res;							// признак успешности операции
    globalApp = &app;
    app.startTimer(0);



    res = QObject::connect (&app, SIGNAL (finish ()), &a, SLOT (quit ()));
    Q_ASSERT_X (res, "connect", "connection is not established");	// окончание работы объекта закрывает приложение
    res = QObject::connect (&a, SIGNAL (aboutToQuit ()), &app, SLOT (terminate ()));
    Q_ASSERT_X (res, "connect", "connection is not established");	// окончание работы приложения закрывает объект


    return a.exec();
}
