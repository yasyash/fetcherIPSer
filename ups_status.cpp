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

#include "ups_status.h"
#include <QDebug>


ups_status::ups_status( QString *ip, quint16 *port, QString *ups_username)
{


    /*
     * Initialize the SNMP library
     */

    init_snmp("snmpapp");


    /*
     * Initialize a "session" that defines who we're going to talk to
     */
    addr = ip;
    u_port = port;
    user_name = ups_username;

    snmp_sess_init( &session );                   /* set up defaults */
    session.peername = strdup(ip->toLocal8Bit().data());


    /* set the SNMP version number */
    session.version = SNMP_VERSION_1;
    const  char *community  = ups_username->toStdString().c_str();

    session.community = (uchar*)community;
    session.community_len = strlen((char*)session.community);
    //session.rcvMsgMaxSize = MAX_NAME_LEN;

    /*
     * Open the session
     */

    ss = snmp_open(&session);                     /* establish the session */

    if (!ss) {
        //snmp_sess_perror("ack", &session);
        //SOCK_CLEANUP;
        //exit(1);
        qDebug() << "Error opening SNMP library...";
        return;
    }

    vars = get_data((char*)"SNMPv2-MIB::sysName.0");



    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
        print_variable(vars->name, vars->val_len, vars);
        //qDebug() << "Error: \n" << snmp_errstring(response->errstat);


    } else {
        /*
         * FAILURE: print what went wrong!
         */

        if (status == STAT_SUCCESS)
            qDebug() << "Error in packet\n Reason: \n" << snmp_errstring(response->errstat);
        else if (status == STAT_TIMEOUT)
            qDebug() << "Timeout: No response from \n" <<      session.peername;
        else
            qDebug() << "Somethig error: " << ss;

    }

    //  if (response)
    //     snmp_free_pdu(response);

    vars = get_data((char*)"SNMPv2-MIB::sysLocation.0");
    if (vars)
        print_variable(vars->name, vars->val_len, vars);

    vars = get_data((char*)"DISMAN-EVENT-MIB::sysUpTimeInstance");
    if (vars)
        print_variable(vars->name, vars->val_len, vars);

    vars = get_data((char*)"SNMPv2-SMI::mib-2.33.1.3.3.1.3.1"); //Input voltage MIB
    if (vars)
        print_variable(vars->name, vars->val_len, vars);

    if (response)
        snmp_free_pdu(response);

    sample_t = 0;
    measure = new  QMap<QString, int>;

    qDebug() << "UPS SNMP monitoring initialization complete.";
}

ups_status::~ups_status()
{
    snmp_close(ss);
    //SOCK_CLEANUP;
}


void ups_status::read_voltage()
{
    vars = get_data((char*)"SNMPv2-SMI::mib-2.33.1.3.3.1.3.1"); //Input voltage MIB
    if (vars)
    {
        voltage = int(*vars->val.integer);
        measure->insert("Напряжение", voltage);
    }

}

variable_list * ups_status::get_data(char * mib)
{
    anOID_len = MAX_OID_LEN;

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (!read_objid(mib, anOID, &anOID_len)) {
        //snmp_perror("read_objid");
        // exit(1);
        qDebug() << "Unexpected problem while getting object ID via SNMP...";
        return nullptr;
    }


    snmp_add_null_var(pdu, anOID, anOID_len);

    /*
     * Send the Request out.
     */

    status = snmp_synch_response(ss, pdu, &response);
    //print_variable(response->variables->name, response->variables->val_len, response->variables);



    return response->variables;
}

/*
 * simple printing of returned data
 */
int print_result (int status, struct snmp_session *sp, struct snmp_pdu *pdu)
{
    char buf[1024];
    struct variable_list *vp;
    int ix;
    struct timeval now;
    struct timezone tz;
    struct tm *tm;

    gettimeofday(&now, &tz);
    tm = localtime(&now.tv_sec);
    fprintf(stdout, "%.2d:%.2d:%.2d.%.6d ", tm->tm_hour, tm->tm_min, tm->tm_sec,
            now.tv_usec);
    switch (status) {
    case STAT_SUCCESS:
        vp = pdu->variables;
        if (pdu->errstat == SNMP_ERR_NOERROR) {
            while (vp) {
                snprint_variable(buf, sizeof(buf), vp->name, vp->name_length, vp);
                fprintf(stdout, "%s: %s\n", sp->peername, buf);
                vp = vp->next_variable;
            }
        }
        else {
            for (ix = 1; vp && ix != pdu->errindex; vp = vp->next_variable, ix++)
                ;
            if (vp) snprint_objid(buf, sizeof(buf), vp->name, vp->name_length);
            else strcpy(buf, "(none)");
            fprintf(stdout, "%s: %s: %s\n",
                    sp->peername, buf, snmp_errstring(pdu->errstat));
        }
        return 1;
    case STAT_TIMEOUT:
        fprintf(stdout, "%s: Timeout\n", sp->peername);
        return 0;
    case STAT_ERROR:
        snmp_perror(sp->peername);
        return 0;
    }
    return 0;
}

QString getStringFromUnsignedChar(unsigned char *str, size_t len)
{

    QString s;
    QString result = "";


    // Print String in Reverse order....
    for ( ulong i = 0; i < len; i++)
    {
        s = QString("%1").arg(str[i],0,16);

        if(s == "0"){
            s="00";
        }
        result.append(s);

    }
    return result;
}
