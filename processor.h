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

#ifndef PROCESSOR_H
#define PROCESSOR_H


#include <QTimer>
#include <QMap>
#include <QSqlDatabase>
#include <QMutex>
#include <QUuid>
#include <QVector>

#include "modbus.h"
#include "modbus-private.h"
#include "ups_status.h"
#include "tcpsock.h"
#include "dusttcpsock.h"
#include "meteotcpsock.h"
#include "serinus.h"
#include "grimm.h"
#include "liga.h"

class processor : public QObject
{
    Q_OBJECT
public:
    processor(QObject *_parent = 0,  QStringList *cmdline = 0 );
    ~processor();



    static void stBusMonitorAddItem( modbus_t * modbus,
                                     uint8_t isOut, uint8_t slave, uint8_t func, uint16_t addr,
                                     uint16_t nb, uint16_t expectedCRC, uint16_t actualCRC );
    static void stBusMonitorRawData( modbus_t * modbus, uint8_t * data,
                                     uint8_t dataLen, uint8_t addNewline );
    void busMonitorRawData( uint8_t * data, uint8_t dataLen, bool addNewline );
    void cover_modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest, int *result);

    void releaseModbus(void);
    virtual modbus_t*  modbus() { return m_serialModbus; }
public:
    static QMap<QString, int>   * ms_data; //assosiative array of polling data
    static QMap<QString, int>   * ms_measure; //assosiative array of measurement quantities
    static QMap<QString, int>   * ms_range; //assosiative array of measurement equipments range

signals:
    void AsciiPortActive(bool active);
    void finished ();						// signal finished

public slots:
    void terminate ()			//termination
    {
        emit finished ();
    }
    void fillSensorData( bool *_is_read, QMap<QString, float> *_measure, QMap<QString, int> *_sample); //sensor equipment type or name
    void fillSensorData( bool *_is_read, QMap<QString, float> *_measure); //polymorphic method for slow measuring
    static void static_fillSensorData(  bool *_is_read, QMap<QString, float> *_measure, QMap<QString, int> *_sample);

private slots:
    void sendModbusRequest( void ); //update data view
    void onSendButtonPress( void );
    void pollForDataOnBus( void );
    void onRtuPortActive(bool active);
    void onAsciiPortActive(bool active);
    void onTcpPortActive(bool active);
    void resetStatus( void );
    void setStatusError(const QString &msg);
    void renovateSlaveID( void );
    void startTransactTimer(QSqlDatabase *conn); //start by signal dbForm
    void transactionDB   (void);   //transaction timer event
    void readSocketStatus(void);

private:

    modbus_t * m_serialModbus  = nullptr;

    modbus_t * m_modbus = NULL;
    QTimer * m_pollTimer;
    QTimer * m_statusTimer;
    QTimer * m_renovateTimer; //timer for  renovate of all modbus slave id polling
    QTimer * m_transactTimer; //timer for trunsaction
    QMap<QString, int>   * m_data; //assosiative array of polling data
    QMap<QString, int>   * m_measure; //assosiative array of measurement quantities
    QMap<QString, int>   * m_range; //assosiative array of measurement equipments range

    QMap<QString, QUuid>   * m_uuid; //assosiative array of sensors uuid
    //QList<int> *m_pool;
    QMap<int, int> *m_pool; //assosiative array of polling slave address - number of registers

    bool m_tcpActive;
    bool m_poll;
    uint8_t * q_poll;
    QSqlDatabase * m_conn;
    QUuid * m_uuidStation;
    //QMutex * m_mutex;
    QVector<bool> *slaveID;

    int funcModbus;
    int addrModbus;
    int numCoils;

    bool verbose; //verbose mode flag

    ups_status *m_ups = nullptr;   //member for UPS status
    QString m_ups_ip;
    quint16 m_ups_port;
    QString m_ups_username;

    TcpSock    *m_fire = nullptr; //member for Fire alarm status
    QString m_alarmip;
    quint16 m_alarmport;

    DustTcpSock    *m_dust = nullptr; //member for Dust Measure
    QString m_dust_ip;
    quint16 m_dust_port;

    MeteoTcpSock *m_meteo = nullptr; //member for Meteostation
    QString m_meteo_ip;
    quint16 m_meteo_port;

    Serinus *m_serinus = nullptr; //member for Serinus
    QString m_serinus_ip;
    quint16 m_serinus_port;

    Grimm *m_grimm = nullptr; //member for Grimm
    QString m_grimm_ip;
    quint16 m_grimmport;

    Liga     *m_liga = nullptr; //member for ACA-Liga
    QString m_liga_ip;
    quint16 m_liga_port;


private:
    void squeezeAlarmMsg();


};

#endif // PROCESSOR_H
