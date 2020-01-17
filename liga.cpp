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
    m_soap = new soap();

    struct SOAP_ENV__Header header;
    struct wsa5__EndpointReferenceType replyTo;

    soap_init(m_soap);

    soap_default_SOAP_ENV__Header(m_soap, &header);

    soap_default_wsa5__EndpointReferenceType(m_soap, &replyTo);

    replyTo.Address = "http://www.w3.org/2005/08/addressing/anonymous";


    header.wsa5__ReplyTo = &replyTo;
    char *str_to = "http://";

    strcat(str_to, ip->toUtf8());
    strcat(str_to, ":");
    strcat(str_to,  QString::number( *port).toUtf8());
    strcat( str_to, "/liga.AutoChrom");
    header.wsa5__To = str_to;

    strcat(str_to, "");
    strcat(str_to, _action);
    strcat(str_to, "GetSystemConfigurations");

    header.wsa5__Action = str_to;

    m_soap->header = &header;


    liga_proxy = new  WSHttpBinding_USCOREIAutoChromWCFHostProxy(m_soap);
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
    std::string _str = *sysConfResp.GetSystemConfigurationsResult->Aux2Device_USCOREConfiguration->PortNumber;

    is_read = false;
    status = "";
    connected = (_str.size() > 0) ? true : false;
    if (connected){

        QMap<QString, float>::iterator it = measure->begin();

        qDebug() << "ACA-Liga measure equipment handling has been initialized.";

        getLastResult();

        qDebug() <<   "Last sampling at: " << sample_t->value(0);

        while (it != measure->end())
        {
            qDebug() << "ACA-Liga measure: " << it.key() << " =  " << it.value();
        }
        is_read = true;

    } else {
        {
            qDebug() << "ACA-Liga measure equipment handling hasn't been initialized. Not respond.";

        }
    }
}

Liga::~Liga()
{

}

void Liga::getLastResult()
{
    char *str_to = NULL;
    _ns2__GetLastResult getLastRes;
    _ns2__GetLastResultResponse getLastResResp;


    strcat(str_to, "");
    strcat(str_to, _action);
    strcat(str_to, "GetLastResult");

    liga_proxy->soap->header->wsa5__Action = str_to;

    int i = liga_proxy->GetLastResult(&getLastRes, getLastResResp);

    if (getLastResResp.GetLastResultResult->EndSamplingDateTime != nullptr)
    {

        sample_t->insert(1, QDateTime::fromTime_t(*getLastResResp.GetLastResultResult->EndSamplingDateTime)); //insert last sample result

        if(sample_t->value(1) != sample_t->value(0))
        {
            sample_t->insert(0, sample_t->value(1)); //insert datetime of last sample into previous node
        }

        for (int j = 0; j < getLastResResp.GetLastResultResult->Results->EResult.size(); j++)
        {
            measure->insert(QString::fromStdString(*getLastResResp.GetLastResultResult->Results->EResult[j]->ComponentName),
                            QString::fromStdString(*getLastResResp.GetLastResultResult->Results->EResult[j]->Value).toFloat());
        }
        is_read = false;

    }
}
#endif
