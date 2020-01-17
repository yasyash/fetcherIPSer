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

#ifndef UPS_STATUS_H
#define UPS_STATUS_H

#include <QString>
#include <QMap>
#include <qglobal.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/session_api.h>
#include <net-snmp/library/snmp_api.h>
//#include <QtNetSNMP/qsnmpmanager.h>
//#include <QtNetSNMP/qsnmpobject.h>
//#include "QtNetSNMP/qsnmpoid.h"

//using namespace QtNetSNMP;


//using namespace nut;
//#undef DEMO_USE_SNMP_VERSION_3

class ups_status
{

public:
    explicit ups_status( QString *ip, quint16 *port, QString *ups_username);
    ~ups_status();
    void read_voltage();

public:
    struct snmp_session session, *ss;
    struct snmp_pdu *pdu;
    struct snmp_pdu *response ;

    oid anOID[MAX_OID_LEN];
    size_t anOID_len = MAX_OID_LEN;

    struct variable_list *vars;
    int status;
    int err_count = 0;

    QString *addr;
    quint16 *u_port;
    QString *user_name;

    QString *model;
    QString *location;
    int *uptime;
    int voltage;

    bool is_read;
    uint sample_t;
    QMap<QString, int> *measure;


    //Device *m_device;
    //Variable *m_model;
    //Variable *m_voltage;
private:
    //Client * m_client;
    variable_list * get_data(char *mib);




};

#endif // UPS_STATUS_H
