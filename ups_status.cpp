#include "ups_status.h"
#include <QDebug>



ups_status::ups_status(int *port)
{

    try
    {
        // Connection
        m_client = new TcpClient("localhost", *port);
        m_device = new Device(m_client->getDevice("dummy"));

        m_model = new Variable(m_device->getVariable("device.model"));
        m_voltage = new Variable( m_device->getVariable("outlet.2.voltage"));
        qDebug() << "UPS initialization complete.";
    }
    catch(NutException& ex)
    {
        qDebug() << "Unexpected problem : " <<  QString::fromStdString(ex.str());
    }

}

ups_status::~ups_status()
{

}
