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

#include "liga.h"


#ifdef LIGA_H

#include <QDebug>

Liga::Liga(QString *ip, quint16 *port)

{
    struct soap _soap;

    struct SOAP_ENV__Header header;
    struct wsa5__EndpointReferenceType replyTo;

    soap_init(&_soap);
    //m_soap = &_soap;

    soap_default_SOAP_ENV__Header(&_soap, &header);

    soap_default_wsa5__EndpointReferenceType(&_soap, &replyTo);

    replyTo.Address = "http://www.w3.org/2005/08/addressing/anonymous";


    header.wsa5__ReplyTo = &replyTo;
    char str_to[255] = "http://";

    strcat(str_to, ip->toUtf8());
    strcat(str_to, ":");
    strcat(str_to,  QString::number( *port).toUtf8());
    strcat( str_to, "/liga.AutoChrom");
    header.wsa5__To = str_to;

    char action[255] = "";
    strcat(action, _action);
    strcat(action, "GetSystemConfigurations");

    header.wsa5__Action = action;

    _soap.header = &header;

    liga_proxy = new  WSHttpBinding_USCOREIAutoChromWCFHostProxy(&_soap);
    m_soap = &_soap;

    _ns2__GetSystemConfigurations sysConf;
    _ns2__GetSystemConfigurationsResponse sysConfResp;

    measure = new  QMap<QString, float>;
    sample_t = new QMap<int, QDateTime>;

    measure->insert("бензол", 0);
    measure->insert("толуол", 0);
    measure->insert("этилбензол", 0);
    measure->insert("м,п-ксилол", 0);
    measure->insert("о-ксилол", 0);
    measure->insert("хлорбензол", 0);
    measure->insert("стирол", 0);
    measure->insert("фенол", 0);

    QDateTime dt = QDateTime::fromString("1970-01-01 00:00:00", Qt::ISODate);

    sample_t->insert(0, dt);
    sample_t->insert(1, dt);

    int i = liga_proxy->GetSystemConfigurations(&sysConf, sysConfResp);
    if (!i){
        std::string _str = *sysConfResp.GetSystemConfigurationsResult->Aux2Device_USCOREConfiguration->PortNumber;

        is_read = false;
        status = "";
        connected = (_str.size() > 0) ? true : false;


        QMap<QString, float>::iterator it = measure->begin();

        qDebug() << "ACA-Liga measure equipment handling has been initialized.\n\r";

        getLastResult();

        qDebug() <<   "Last sampling at: " << sample_t->value(0);

        while (it != measure->end())
        {
            qDebug() << "ACA-Liga measure: " << it.key() << " =  " << it.value();
        }
        is_read = true;
        error= false;
    } else {
        {
            qDebug() << "ACA-Liga measure equipment handling has been initialized with issues. Not respond.\n\r";

        }
        error = true;
        status = "ACA-Liga measure equipment handling has been initialized with issues. Not respond.\n\r";
    }
    const int j = 7120;
    m_ip = ip;
    //m_port = &j;
}

Liga::~Liga()
{

}

void Liga::getLastResult()
{
    error = false;
    struct soap _soap;

    struct SOAP_ENV__Header header;
    struct wsa5__EndpointReferenceType replyTo;

    soap_init(&_soap);
    //m_soap = &_soap;

    soap_default_SOAP_ENV__Header(&_soap, &header);

    soap_default_wsa5__EndpointReferenceType(&_soap, &replyTo);

    replyTo.Address = "http://www.w3.org/2005/08/addressing/anonymous";


    header.wsa5__ReplyTo = &replyTo;
    char str_to[255] = "http://";

    strcat(str_to, m_ip->toUtf8());
    strcat(str_to, ":");
    strcat(str_to,  "7120");
    strcat( str_to, "/liga.AutoChrom");
    header.wsa5__To = str_to;

    char action[255] = "";
    strcat(action, _action);
    strcat(action, "GetLastResult");

    header.wsa5__Action = action;

    _soap.header = &header;

    _ns2__GetLastResult getLastRes;
    _ns2__GetLastResultResponse getLastResResp;


    liga_proxy->soap = &_soap;

    int i = liga_proxy->GetLastResult(&getLastRes, getLastResResp);
    if (!i){
        if (getLastResResp.GetLastResultResult->EndSamplingDateTime != nullptr)
        {

            sample_t->insert(1, QDateTime::fromTime_t(*getLastResResp.GetLastResultResult->EndSamplingDateTime)); //insert last sample result

            if(sample_t->value(1) != sample_t->value(0))
            {
                sample_t->insert(0, sample_t->value(1)); //insert datetime of last sample into previous node
            }

            for (int j = 0; j < getLastResResp.GetLastResultResult->Results->EResult.size(); j++)
            {
                switch( j )
                {
                case 0:
                    measure->insert("бензол", QString::fromStdString(*getLastResResp.GetLastResultResult->Results->EResult[j]->Value).toFloat());
                    break;
                case 1:
                    measure->insert("толуол", QString::fromStdString(*getLastResResp.GetLastResultResult->Results->EResult[j]->Value).toFloat());
                    break;
                case 2:
                    measure->insert("этилбензол", QString::fromStdString(*getLastResResp.GetLastResultResult->Results->EResult[j]->Value).toFloat());
                    break;
                case 3:
                    measure->insert("м,п-ксилол", QString::fromStdString(*getLastResResp.GetLastResultResult->Results->EResult[j]->Value).toFloat());
                    break;
                case 4:
                    measure->insert("о-ксилол", QString::fromStdString(*getLastResResp.GetLastResultResult->Results->EResult[j]->Value).toFloat());
                    break;
                case 5:
                    measure->insert("хлорбензол", QString::fromStdString(*getLastResResp.GetLastResultResult->Results->EResult[j]->Value).toFloat());
                    break;
                case 6:
                    measure->insert("стирол", QString::fromStdString(*getLastResResp.GetLastResultResult->Results->EResult[j]->Value).toFloat());
                    break;
                case 7:
                    measure->insert("фенол", QString::fromStdString(*getLastResResp.GetLastResultResult->Results->EResult[j]->Value).toFloat());
                    break;

                default:
                    break;
                }
                is_read = false;
                error = false;
                status = "";
            }

        }
    }
    else {
        error = true;
        if (i == SOAP_TCP_ERROR)
            status = "Connection TCP error...";
    }

}
#endif
