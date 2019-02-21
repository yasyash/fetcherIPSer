#include <QCoreApplication>


#include "app.h"


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
