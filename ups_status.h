#ifndef UPS_STATUS_H
#define UPS_STATUS_H

#include<QObject>
#include "nutclient.h"

using namespace nut;

class ups_status
{

public:
    explicit ups_status(int *port);
    ~ups_status();

    Device *m_device;
    Variable *m_model;
    Variable *m_voltage;

private:
    Client * m_client;

signals:



};

#endif // UPS_STATUS_H
