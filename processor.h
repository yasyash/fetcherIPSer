#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QTimer>
#include <QMap>
#include <QSqlDatabase>
#include <QMutex>
#include <QUuid>
#include <QMap>
#include <QVector>

#include "modbus.h"
#include "modbus-private.h"
#include "ups_status.h"
#include "tcpsock.h"

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

signals:
    void AsciiPortActive(bool active);
    void finished ();						// сигнал "остановка работы"

public slots:
    void terminate ()			// досрочная остановка
    {
        emit finished ();			// сигнал завергения работы
    }

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

private:

    modbus_t * m_serialModbus;

    modbus_t * m_modbus;
    QTimer * m_pollTimer;
    QTimer * m_statusTimer;
    QTimer * m_renovateTimer; //timer for  renovate of all modbus slave id polling
    QTimer * m_transactTimer; //timer for trunsaction
    QMap<QString, int>   * m_data; //assosiative array of polling data
    QMap<QString, int>   * m_measure; //assosiative array of measurement quantities
    QMap<QString, QUuid>   * m_uuid; //assosiative array of sensors uuid
    QList<int> *m_pool;

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

    ups_status *m_ups;
    TcpSock    *m_fire;

};

#endif // PROCESSOR_H
