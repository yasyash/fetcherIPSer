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

// -slaveid 1 3 11 12 15 -port /dev/ttyr00 -baud 9600 -data 8 -stop 1 -parity none -db weather -user weather -pwd 31415 -dustip 192.168.1.3 -dustport 3602 -alarmip 192.168.1.110 -alarmport 5555 -upsip 192.168.1.120 -upsport 3493 -upsuser liebert -meteoip 192.168.1.200 -meteoport 22222 -polltime 10 -verbose

#include "processor.h"
#include "app.h"
#include <QDebug>
#include <QSqlQuery>
#include <QRegExp>
#include <QDateTime>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>

#include <errno.h>

extern _App	*globalApp;



processor::processor(QObject *_parent,    QStringList *cmdline) : QObject (_parent),
    m_modbus( NULL ),
    m_tcpActive(false),
    m_poll(false), // not working initial state
    funcModbus(0x03),
    addrModbus(0),
    numCoils(6),
    verbose(false)

{

    QStringList cmdline_args =  *cmdline;

    int _verbose = cmdline_args.indexOf("-verbose");
    if (_verbose > 0)
    {
        verbose = true;
    }

    // UPS init
    m_ups_ip = cmdline_args.value(cmdline_args.indexOf("-upsip") +1);
    if (m_ups_ip == "")
    {
        qDebug ( "IP address of UPS is not set.");
    }
    else
    {

        m_ups_port = cmdline_args.value(cmdline_args.indexOf("-upsport") +1).toUShort();
        if (m_ups_port <= 0)
        {
            qDebug ("UPS port error:  expected parameter");
        }
        else
        {
            m_ups_username = cmdline_args.value(cmdline_args.indexOf("-upsuser") +1);

            m_ups = new ups_status(&m_ups_ip, &m_ups_port, &m_ups_username);
            //qDebug() << "UPS model: "<< QString::fromStdString(m_ups->m_model->getValue().data()[0]) <<"\n Voltage: "
            //<< QString::fromStdString(m_ups->m_voltage->getValue().data()[0]);
        }
    }

    QString port = cmdline_args.value(cmdline_args.indexOf("-port") +1);
    if (port == "")
    {
        qDebug ("Fatal error:  wrong data of the port parameter");
        exit(-1);
    }

    QString baud = cmdline_args.value(cmdline_args.indexOf("-baud") +1);
    if (baud == "")
    {         qDebug ( "Fatal error:  wrong data of the baud parameter");

        exit(-1);

    }

    QString parity = cmdline_args.value(cmdline_args.indexOf("-parity") +1).toUpper();
    if (parity == "" || parity == 'N' || parity == 'E' || parity == 'O' || parity == 'n' || parity == 'e' || parity == 'o')
    {  qDebug ( "Fatal error:  wrong data of theparity parameter - note empty and N [n] or E [e] or O [0] - non case sensitive value");
        exit(-1);

    }

    QString dataBits = cmdline_args.value(cmdline_args.indexOf("-data") +1);
    if (dataBits == "")
    {
        qDebug ( "Fatal error:  wrong data of the data bits parameter");
        exit(-1);

    }

    QString stopBits = cmdline_args.value(cmdline_args.indexOf("-stop") +1);
    if (stopBits == "")
    {
        qDebug ("Fatal error: wrong data of the stop bits parameter");
        exit(-1);

    }


    /* connect( ui->rtuSettingsWidget,   SIGNAL(serialPortActive(bool)), this, SLOT(onRtuPortActive(bool)));*/
    m_serialModbus = modbus_new_ascii
            ( port.toLatin1().constData(),
              baud.toInt(),
              *parity.toLatin1().constData(),
              dataBits.toInt(),
              stopBits.toInt());

    if( modbus_connect( m_serialModbus ) == -1 )
    {
        releaseModbus();

        qDebug ( "Fatal error: could not connect serial port");
        exit(-1);


    }

    QString db = cmdline_args.value(cmdline_args.indexOf("-db") +1);
    if (db == "")
    {
        releaseModbus();

        qDebug ( "Fatal error: wrong data of the database parameter");
        exit(-1);

    }

    QString user = cmdline_args.value(cmdline_args.indexOf("-user") +1);
    if (user == "")
    {
        releaseModbus();

        qDebug ( "Fatal error: wrong data of the user parameter");
        exit(-1);

    }

    QString pw = cmdline_args.value(cmdline_args.indexOf("-pwd") +1);
    if (pw == "")
    {
        releaseModbus();

        qDebug ( "Fatal error: wrong data of the password parameter");
        exit(-1);

    }

    m_conn = new QSqlDatabase();
    *m_conn = QSqlDatabase::addDatabase("QPSQL");
    m_conn->setHostName("localhost");
    m_conn->setDatabaseName(db);
    m_conn->setUserName(user);
    m_conn->setPassword(pw);


    bool status = m_conn->open();
    if (!status)
    {
        releaseModbus();

        qDebug ( QString("Connection error: " + m_conn->lastError().text()).toLatin1().constData());
        exit(-1);

    }



    connect( this, SIGNAL(AsciiPortActive(bool)), this, SLOT(onAsciiPortActive(bool)));

    QTimer * t = new QTimer( this );
    connect( t, SIGNAL(timeout()), this, SLOT(pollForDataOnBus()));
    t->start( 500 );

    m_pollTimer = new QTimer( this );
    connect( m_pollTimer, SIGNAL(timeout()), this, SLOT(sendModbusRequest()));
    connect( m_pollTimer, SIGNAL(timeout()), this, SLOT(readSocketStatus()));

    //  m_statusTimer = new QTimer( this );
    //connect( m_statusTimer, SIGNAL(timeout()), this, SLOT(resetStatus()));
    //m_statusTimer->setSingleShot(true);

    m_renovateTimer = new QTimer(this);
    connect( m_renovateTimer, SIGNAL(timeout()), this, SLOT(renovateSlaveID()));

    m_transactTimer = new QTimer(this);
    connect( m_transactTimer, SIGNAL(timeout()), this, SLOT(transactionDB()));
    // m_transactTimer->start(600000);

    //m_mutex = new QMutex();
    //int _pos = cmdline_args.indexOf("-slaveid");
    QStringListIterator iterator(cmdline_args);
    //mm_pool = new QList<int>;
    m_pool = new QMap<int, int>;

    while (iterator.hasNext())
    {
        if (iterator.next() == "-slaveid")
        {
            QString tmp = iterator.next().toLocal8Bit().constData();
            while (tmp.indexOf("-") == -1) {

                //m_pool->push_back(tmp.toInt());
                m_pool->insert(tmp.toInt(), numCoils);

                tmp = iterator.next().toLocal8Bit().constData();
            }
            // return;
        }
    }

    if (m_pool->size() == 0)
    {
        for ( int i = 0; i < 16; i++)
        {
            //m_pool->push_back(i);
            m_pool->insert(i, numCoils);

        }
    }



    slaveID = new QVector<bool>(16, true); //max slaveID number is hardcoded to 16 devices
    q_poll = new uint8_t[15];
    memset(q_poll, 24, 16);
    m_uuid = new  QMap<QString, QUuid>;
    m_data = new  QMap<QString, int>;
    m_measure =  new QMap<QString, int>;

    emit(AsciiPortActive(true));

    //alarm init
    m_alarmip = cmdline_args.value(cmdline_args.indexOf("-alarmip") +1);
    if (m_alarmip == "")
    {
        qDebug ( "IP address of fire alarm is not set.");
    }
    else
    {
        m_alarmport = cmdline_args.value(cmdline_args.indexOf("-alarmport") +1).toUShort();
        if (m_alarmport <= 0)
        {
            qDebug ( "Port of fire alarm is not set.");
        }
        else
        {
            m_fire = new TcpSock(this, &m_alarmip, &m_alarmport);
        }

    }

    // Dust equipment init

    m_dust_ip = cmdline_args.value(cmdline_args.indexOf("-dustip") +1);
    if (m_dust_ip == "")
    {
        qDebug ( "IP address of dust measure equipment is not set.");
    }
    else
    {
        m_dust_port = cmdline_args.value(cmdline_args.indexOf("-dustport") +1).toUShort();
        if (m_dust_port <= 0)
        {
            qDebug ( "Port of dust measure equipment is not set.");
        }
        else
        {
            m_dust = new DustTcpSock(this, &m_dust_ip, &m_dust_port);
            m_dust->sendData( "RDMN");
            // while (!m_dust->is_read);
            m_dust->is_read = false;
            m_dust->sendData( "MSTART");
            //while (!m_dust->is_read);
            m_dust->is_read = false;
        }

    }

    // Meteostation init
    //  -meteoip 192.168.1.200 -meteoport 22222

    m_meteo_ip = cmdline_args.value(cmdline_args.indexOf("-meteoip") +1);
    if (m_meteo_ip == "")
    {
        qDebug ( "IP address of meteostation is not set.");
    }
    else
    {
        m_meteo_port = cmdline_args.value(cmdline_args.indexOf("-meteoport") +1).toUShort();
        if (m_meteo_port <= 0)
        {
            qDebug ( "Port of meteostation is not set.");
        }
        else
        {
            m_meteo = new MeteoTcpSock(this, &m_meteo_ip, &m_meteo_port);

        }

    }



    m_renovateTimer->start(420000); // every 7 minutes we set all slave ID to active mode for polling despite of really state

    QString polltime = cmdline_args.value(cmdline_args.indexOf("-polltime") +1);
    if (polltime == "")
    {
        m_pollTimer->start(5000); //start polling timer with hardcoded period 5 sec.

        qDebug ( "Polling time is set up to 5 sec.");

    }
    else
    {
        m_pollTimer->start(polltime.toInt() * 1000);

        qDebug () << "Polling time is set up to " << polltime <<" sec.";
    }


    startTransactTimer(m_conn);    //start transaction timer - must be after polling timer!!!

    resetStatus();

    //range coefficients init
    m_range = new QMap<QString, int>;
    m_range->insert("CO", 10);
    m_range->insert("NO2", 1000);
    m_range->insert("NO", 1000);
    m_range->insert("SO2", 1000);
    m_range->insert("H2S", 10000);
    m_range->insert("Пыль общая", 1000);
    m_range->insert("PM", 1000);
    m_range->insert("PM1", 1000);
    m_range->insert("PM2.5", 1000);
    m_range->insert("PM4", 1000);
    m_range->insert("PM10", 1000);
    m_range->insert("Ресурс сенс. NO", 1);
    m_range->insert("Ресурс сенс. H2S", 1);
    m_range->insert("Напряжение мин.", 1);
    m_range->insert("Напряжение макс.", 1);


    //UPS data init
    m_ups->read_voltage();
    m_data->insert("Напряжение мин.", m_ups->voltage);
    //m_measure->insert("Напряжение мин.", 1);
    m_data->insert("Напряжение макс.", m_ups->voltage);
    //m_measure->insert("Напряжение макс.", 1);


}


processor::~processor()
{
    m_dust->sendData( "MSTOP");

    modbus_close( m_serialModbus );
    modbus_free( m_serialModbus );
    m_serialModbus = NULL;

}

void processor::releaseModbus()
{
    modbus_close( m_serialModbus );
    modbus_free( m_serialModbus );
    m_serialModbus = NULL;
    emit(AsciiPortActive(false));

}

void processor::onSendButtonPress( void )
{
    //if already polling then stop
    if( m_pollTimer->isActive() )
    {
        m_pollTimer->stop();
        m_transactTimer->stop();
        //ui->sendBtn->setText( tr("Send") );

    }
    else
    {
        //if polling requested then enable timer
        if( m_poll )
        {
            //m_pollTimer->start( 1000 * ui->pollingInterval->value() );
            m_transactTimer->start();
            //  qCDebug(QT_QM_ASCII_OPTEC_MAIN) <<   ui->pollingInterval->value() << " interval";

            // ui->sendBtn->setText( tr("Stop") );
        }

        sendModbusRequest();
    }
}







static QString descriptiveDataTypeName( int funcCode )
{
    switch( funcCode )
    {
    case MODBUS_FC_READ_COILS:
    case MODBUS_FC_WRITE_SINGLE_COIL:
    case MODBUS_FC_WRITE_MULTIPLE_COILS:
        return "Coil (binary)";
    case MODBUS_FC_READ_DISCRETE_INPUTS:
        return "Discrete Input (binary)";
    case MODBUS_FC_READ_HOLDING_REGISTERS:
    case MODBUS_FC_WRITE_SINGLE_REGISTER:
    case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
        return "Holding Register (16 bit)";
    case MODBUS_FC_READ_INPUT_REGISTERS:
        return "Input Register (16 bit)";
    default:
        break;
    }
    return "Unknown";
}


static inline QString embracedString( const QString & s )
{
    return s.section( '(', 1 ).section( ')', 0, 0 );
}


static inline int stringToHex( QString s )
{
    return s.replace( "0x", "" ).toInt( NULL, 16 );
}



void processor::cover_modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest, int *result)
{
    *result = modbus_read_registers(ctx,  addr, nb, dest);
}

void processor::sendModbusRequest( void )
{

    QString tmp_type_measure;
    int j = 0;
    QMap<int, int>::iterator slave;
    //if( m_tcpActive )
    //  ui->tcpSettingsWidget->tcpConnect();

    if( m_modbus == NULL )
    {
        qDebug() << "Modbus is not configured!\n" ;
        return;
    }
    //for( int j = 0; j < m_pool->size() ; j++ )
    for (slave = m_pool->begin(); slave != m_pool->end(); ++slave)
    {
        tmp_type_measure.clear(); //It's reset when new Slave ID is viewed

        if (slaveID->value(slave.key() - 1))
        {
            //QString slave = QString(j);
            const int func = funcModbus;
            const int addr = addrModbus;
            int num = numCoils;
            uint8_t dest[1024];
            uint16_t * dest16 = (uint16_t *) dest;

            memset( dest, 0, 1024 );

            int ret = -1;
            bool is16Bit = false;
            bool writeAccess = false;
            const QString dataType = descriptiveDataTypeName( func );

            //modbus_set_slave( m_modbus, m_pool->value(j) );
            modbus_set_slave( m_modbus, slave.key() );

            switch( func )
            {
            case MODBUS_FC_READ_COILS:
                ret = modbus_read_bits( m_modbus, addr, slave.value(), dest );
                break;
            case MODBUS_FC_READ_DISCRETE_INPUTS:
                ret = modbus_read_input_bits( m_modbus, addr, slave.value(), dest );
                break;
            case MODBUS_FC_READ_HOLDING_REGISTERS:
                //while(!m_mutex->tryLock());{

                // qDebug() << "ret... " << ret << " \n";
                //ret = modbus_read_registers( m_modbus, addr, num, dest16 );
                ret = modbus_read_registers( m_modbus, addr, slave.value(), dest16 );
                is16Bit = true;
                // m_mutex->unlock();
                break;
            case MODBUS_FC_READ_INPUT_REGISTERS:
                ret = modbus_read_input_registers( m_modbus, addr, slave.value(), dest16 );
                is16Bit = true;
                break;
            case MODBUS_FC_WRITE_SINGLE_COIL:
                /*  ret = modbus_write_bit( m_modbus, addr,
                                        ui->regTable->item( 0, DataColumn )->
                                        text().toInt(0, 0) ? 1 : 0 );
                writeAccess = true;
                num = 1;*/
                break;
            case MODBUS_FC_WRITE_SINGLE_REGISTER:
                /*ret = modbus_write_register( m_modbus, addr,
                                             ui->regTable->item( 0, DataColumn )->
                                             text().toInt(0, 0) );
                writeAccess = true;
                num = 1;*/
                break;

            case MODBUS_FC_WRITE_MULTIPLE_COILS:
            {
                /* uint8_t * data = new uint8_t[num];
                for( int i = 0; i < num; ++i )
                {
                    data[i] = ui->regTable->item( i, DataColumn )->
                            text().toInt(0, 0);
                }
                ret = modbus_write_bits( m_modbus, addr, num, data );
                delete[] data;
                writeAccess = true;*/
                break;
            }
            case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
            {
                /*uint16_t * data = new uint16_t[num];
                for( int i = 0; i < num; ++i )
                {
                    data[i] = ui->regTable->item( i, DataColumn )->
                            text().toInt(0, 0);
                }
                ret = modbus_write_registers( m_modbus, addr, num, data );
                delete[] data;
                writeAccess = true;*/
                break;
            }

            default:
                break;
            }

            if( ret == slave.value()  )
            {
                if( writeAccess )
                {
                    qDebug() <<  tr( "Values successfully sent" ) ;

                }
                else
                {
                    // RESULT PARSING

                    bool b_hex = is16Bit && true;
                    QString qs_num;

                    for( int i = 0; i < slave.value(); ++i )
                    {
                        int data = is16Bit ? dest16[i] : dest[i];


                        q_poll[slave.key() - 1] = 24; //reset of fault polling <i> counter

                        switch( i ) //register value explanation
                        {
                        case 0:{
                            QString result;
                            QTextStream(&result) << data;
                            qDebug() << QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss ")  << "\nSlave address = " << result;

                        }
                            break;
                        case 1:
                        {
                            uint8_t _mode = data & 0xFF;
                            QString md, name, result;



                            uint8_t _type = (data >> 8) & 0xF;

                            if (_type == 2){ name = "CO"; //detect type of sensor HARDCODED
                                if (_mode == 2) md = "fault";
                                else
                                {md = (_mode ?  "off" :  "measuring");};
                            }
                            if (_type == 4){ name = "NO2"; //detect type of sensor HARDCODED
                                if (_mode == 7) md = "change sensor";
                                else
                                {md = (_mode ?  "off" :  "measuring");};
                            }

                            if (_type == 6){ name = "SO2"; //detect type of sensor HARDCODED
                                if (_mode == 7) md = "change sensor";
                                else
                                {md = (_mode ?  "off" :  "measuring");};
                            }


                            tmp_type_measure = name;

                            uint8_t _number = (data >> 12) & 0xF;
                            QTextStream(&result) << name << " : " << md << " : " << _number;
                            qDebug() << result;

                            break;
                        }
                        case 2:{
                            QString result;
                            int tmp, cnt;

                            QTextStream(&result) << float (data)/m_range->value(tmp_type_measure) << " mg/m3";


                            tmp = m_data->value(tmp_type_measure, -1); //detect first measure
                            if ( tmp == -1){
                                m_data->insert(tmp_type_measure, QString::number(data, 16).toInt()); // insert into QMap ordering pair of measure first time
                                m_measure->insert(tmp_type_measure, 1);
                                qDebug() << "measure... \n type= " << tmp_type_measure << " value = " << (float)QString::number(data, 16).toInt()/m_range->value(tmp_type_measure);

                            } else {
                                m_data->insert(tmp_type_measure, tmp + QString::number(data, 16).toInt() );
                                cnt = m_measure->value(tmp_type_measure, 0);
                                m_measure->insert(tmp_type_measure, cnt+1);
                                qDebug()<<  "measure... \n type= " << tmp_type_measure << " value = " << (float)QString::number(data, 16).toInt()/m_range->value(tmp_type_measure);

                            }
                            break;
                        }
                        case 3:
                        {
                            uint8_t _mode = data & 0xFF;
                            QString md, name, result;

                            if (_mode == 2) md = "fault";
                            else
                            {md = (_mode ?  "off" :  "measuring");};

                            uint8_t _type = (data >> 8) & 0xF;

                            if (_type == 7){ name = "NO"; //detect type of sensor HARDCODED
                                if (_mode == 7) md = "change sensor";
                                else
                                {md = (_mode ?  "off" :  "measuring");};
                            }

                            if (_type == 3){ name = "H2S"; //detect type of sensor HARDCODED
                                if (_mode == 7) md = "change sensor";
                                else
                                {md = (_mode ?  "off" :  "measuring");};
                            }


                            tmp_type_measure = name;

                            uint8_t _number = (data >> 12) & 0xF;
                            QTextStream(&result) << name << " : " << md << " : " << _number;
                            qDebug() << result;


                            break;
                        }
                        case 4:{
                            QString result;
                            int tmp, cnt;

                            QTextStream(&result) << float (data)/m_range->value(tmp_type_measure) << " mg/m3";


                            tmp = m_data->value(tmp_type_measure, -1); //detect first measure
                            if ( tmp == -1){
                                m_data->insert(tmp_type_measure, QString::number(data, 16).toInt()); // insert into QMap ordering pair of measure first time
                                m_measure->insert(tmp_type_measure, 1);
                                qDebug() << "measure... \n type= " << tmp_type_measure << " value = " << (float)QString::number(data, 16).toInt()/m_range->value(tmp_type_measure);

                            } else {
                                m_data->insert(tmp_type_measure, tmp + QString::number(data, 16).toInt() );
                                cnt = m_measure->value(tmp_type_measure, 0);
                                m_measure->insert(tmp_type_measure, cnt+1);
                                qDebug()<<  "measure... \n type= " << tmp_type_measure << " value = " << (float)QString::number(data, 16).toInt()/m_range->value(tmp_type_measure);

                            }
                            break;
                        }

                        case 5:{
                            QString result, str;
                            int tmp, cnt;

                            QTextStream(&result) << float (data) << " %";



                            str = "Ресурс сенс. " % tmp_type_measure;
                            tmp = m_data->value(str, -1); //detect first measure

                            if ( tmp == -1){

                                m_data->insert(str, QString::number(data, 16).toInt()); // insert into QMap ordering pair of measure first time
                                m_measure->insert(str, 1);
                                qDebug() << "measure... \n type= " << str << " value = " << (float)QString::number(data, 16).toInt() << " %";

                            } else {
                                m_data->insert(str, tmp + QString::number(data, 16).toInt());
                                cnt = m_measure->value(str, 0);
                                m_measure->insert(str, cnt+1);
                                qDebug()<<  "measure... \n type= " << tmp_type_measure << " value = " << (float)QString::number(data, 16).toInt() << " %";

                            }
                            break;
                        }
                        default:
                            break;
                        }
                    }
                }
            }
            else
            {
                QString err;

                if( ret < 0 )
                {
                    if(
        #ifdef WIN32
                            errno == WSAETIMEDOUT ||
        #endif
                            errno == EIO
                            )
                    {
                        err += tr( "I/O error" );
                        err += ": ";
                        err += tr( "did not receive any data from slave." );
                        q_poll[slave.key() - 1] --;
                        if (q_poll[slave.key() - 1]==0)
                        {

                            slaveID->replace(slave.key() - 1, false);
                        }
                    }
                    else
                    {
                        err += tr( "Protocol error" );
                        err += ": ";
                        err += tr( "Slave threw exception '" );
                        err += modbus_strerror( errno );
                        err += tr( "' or function not implemented." );

                        if (errno == EMBXILADD)  slave.value()--; //in case the number registers is too much
                        q_poll[slave.key() - 1] --;
                        if (q_poll[slave.key() - 1]==0)
                        {

                            slaveID->replace(slave.key() - 1, false);
                        }
                    }
                }
                else
                {
                    err += tr( "Protocol error" );
                    err += ": ";
                    err += tr( "Number of registers returned does not "
                               "match number of registers requested!" );
                }

                if(( err.size() > 0 ) )
                    if (verbose){
                        qDebug()<< QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss ") <<"Slave ID = " << slave.key() << "\n"<< err;
                    }
                    else
                    {
                        qDebug() << QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss ") <<"Slave ID = " << slave.key() <<" is not detected.";
                    }
            }
        }
        j++;
    }


    //m_mutex->unlock();
}



void processor::resetStatus( void )
{
    qDebug() << tr( "Ready" ) ;

}

void processor::pollForDataOnBus( void )
{
    if( m_modbus )
    {
        modbus_poll( m_modbus );
    }
}




void processor::onRtuPortActive(bool active)
{
    if (active) {
        //m_modbus = ui->rtuSettingsWidget->modbus();
        if (m_modbus) {
            //modbus_register_monitor_add_item_fnc(m_modbus, processor::stBusMonitorAddItem);
            // modbus_register_monitor_raw_data_fnc(m_modbus, processor::stBusMonitorRawData);
        }
        m_tcpActive = false;
    }
    else {
        m_modbus = NULL;
    }
}

void processor::busMonitorRawData( uint8_t * data, uint8_t dataLen, bool addNewline )
{
    if( dataLen > 0 )
    {
        QString dump;
        for( int i = 0; i < dataLen; ++i )
        {
            dump += QString().sprintf( "%.2x ", data[i] );
        }
        if( addNewline )
        {
            qDebug().noquote() << "Raw data:  " << dump << "\r\n";

            //dump += "\n";
        }
        else {
            qDebug() << "Raw data:  " << dump;
        }
    }
}


// static
void processor::stBusMonitorRawData( modbus_t * modbus, uint8_t * data, uint8_t dataLen, uint8_t addNewline )
{
    Q_UNUSED(modbus);
    globalApp->_obj->busMonitorRawData( data, dataLen, addNewline != 0 );
}

void processor::onAsciiPortActive(bool active)
{
    if (active) {
        m_modbus = modbus();

        if (m_modbus) {
            if(verbose)
            {
                //modbus_register_monitor_add_item_fnc(m_modbus, processor::stBusMonitorAddItem);
                modbus_register_monitor_raw_data_fnc(m_modbus, processor::stBusMonitorRawData);
            }
        }
        m_tcpActive = false;
    }
    else {
        m_modbus = NULL;
    }
}

void processor::onTcpPortActive(bool active)
{
    m_tcpActive = active;

    if (active) {
        //m_modbus = ui->tcpSettingsWidget->modbus();
        if (m_modbus) {
            //modbus_register_monitor_add_item_fnc(m_modbus, processor::stBusMonitorAddItem);
            // modbus_register_monitor_raw_data_fnc(m_modbus, processor::stBusMonitorRawData);
        }
    }
    else {
        m_modbus = NULL;
    }
}

void processor::setStatusError(const QString &msg)
{
    qDebug() << msg ;


}

void processor::renovateSlaveID( void )
{
    memset(q_poll, 24, 16);
    for( int j = 0; j < slaveID->size(); ++j )
    {
        if (!slaveID->value(j))
        {
            slaveID->replace(j, true);
            m_pool->insert (j+1, numCoils); //renovate of registers quantity
        }
    }
    if ( (m_dust_ip != "") && (m_dust_port >0)){
        m_dust->~DustTcpSock();
        m_dust = new DustTcpSock(this, &m_dust_ip, &m_dust_port);
        m_dust->sendData( "MSTART"); //restart Dust measure equipment
    }

    if ( (m_meteo_ip != "") && (m_meteo_port >0)){

        m_meteo->~MeteoTcpSock();
        m_meteo = new MeteoTcpSock(this, &m_meteo_ip, &m_meteo_port);
    }

    if ( (m_ups_ip != "") && (m_ups_port >0)){

        m_ups->err_count = 0;
    }


}

void processor::transactionDB(void)
{
    QMap<QString, QUuid>::iterator sensor;
    int val;
    float average;
    QString tmp_time = QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss"); //all SQL INSERT should be in same time

    for (sensor = m_uuid->begin(); sensor != m_uuid->end(); ++sensor)
    {
        if (sensor.key() == "Пыль общая")
        {
            val = m_data->value("PM", -1); //Hardcoded for Cyrillic name of Dust total

        } else {

            val = m_data->value(sensor.key(), -1);
        }


        if (val != -1){
            QSqlQuery query = QSqlQuery(*m_conn);
            query.prepare("INSERT INTO sensors_data (idd, serialnum, date_time, typemeasure, measure, is_alert) "
                          "VALUES (:idd, :serialnum, :date_time, :typemeasure, :measure, false)");

            if ((sensor.key().indexOf("PM")!= -1) || (sensor.key().indexOf("Пыль общая")!= -1))
            {
                average = (float (val)) / m_measure->value(sensor.key(), 1) / m_range->value(sensor.key()); //for dust measure range is less
                if (sensor.key() == "Пыль общая")
                {
                    average = (float (val)) / m_measure->value("PM", 1) /m_range->value(sensor.key()); //Hardcoded for Cyrillic name of Dust total

                } else {

                    average = (float (val)) / m_measure->value(sensor.key(), 1) / 1000; //for dust measure range is less
                }

            }
            else
            {
                average = (float (val)) / m_measure->value(sensor.key(), 1) /m_range->value(sensor.key());
            }

            query.bindValue(":idd", QString(m_uuidStation->toString()).remove(QRegExp("[\\{\\}]")));
            query.bindValue(":serialnum",  QString(m_uuid->value(sensor.key()).toString()).remove(QRegExp("[\\{\\}]")));
            query.bindValue(":date_time", tmp_time);
            query.bindValue(":typemeasure",sensor.key());
            query.bindValue(":measure", average );
            qDebug() << "Transaction prepare: \n idd === "<< QString(m_uuidStation->toString()).remove(QRegExp("[\\{\\}]")) << "\n serial === " <<  QString(m_uuid->value(sensor.key()).toString()).remove(QRegExp("[\\{\\}]")) <<
                        "\n date_time ===" << QDateTime::currentDateTime().toString( "yyyy-MM-ddThh:mm:ss") << "\n typemeasure " <<  sensor.key() <<
                        "\n measure ===" <<average;
            if (!m_conn->isOpen())
                m_conn->open();

            if(!m_conn->isOpen())
            {
                qDebug() << "Unable to reopen database connection!";
            }
            else
            {
                if (verbose)
                {
                    qDebug() << "Transaction status is " << ((query.exec() == true) ? "successful!" :  "not complete!");
                    qDebug() << "The last error is " << (( query.lastError().text().trimmed() == "") ? "absent" : query.lastError().text());
                }
                else
                {
                    if (query.exec())
                    {
                        qDebug() << "Insertion is successful!";
                    }
                    else
                    {
                        qDebug() << "Insertion is not successful!";

                    }
                }
                query.finish();
                //  query.~QSqlQuery();

                m_data->remove(sensor.key());
                m_measure->remove(sensor.key());


            }
        }


    }

    m_data->clear();
    m_measure->clear();

}
void processor::startTransactTimer( QSqlDatabase *conn) //start by signal dbForm
{


    //m_conn = conn;
    QSqlQuery *query= new QSqlQuery ("select * from equipments where is_present = 'true' and measure_class = 'data' order by date_time_in", *conn);
    qDebug() << "Devices have been added, status is " <<   query->isActive()<< " and err " << query->lastError().text();
    query->first();
    QSqlRecord rec = query->record();

    m_transactTimer->start(rec.field("average_period").value().toInt() *1000);
    if( !m_pollTimer->isActive() ) m_transactTimer->stop();

    m_uuidStation  = new QUuid(rec.field("idd").value().toUuid());


    for (int i = 0; i < query->size(); i++ )
    {
        qDebug() << query->value("typemeasure").toString() << "  -----  "<< query->value("serialnum").toUuid();

        m_uuid->insert( query->value("typemeasure").toString(), query->value("serialnum").toUuid());
        query->next();
    }
    query->finish();
    //    query->~QSqlQuery();

}


//Read the status of devices that are connected via TCP
void processor::readSocketStatus()
{
    QString tmp_type_measure;
    QStringList dust = {"PM1", "PM2.5", "PM4", "PM10", "PM"  };
    int tmp;
    //m_dust->sendData( "MSTOP\r", 6);

    //m_dust->sendData( "MSTATUS");
    //while (!m_dust->is_read);
    //m_dust->is_read = false;

    //if (m_dust->status == "Running"){

    //Dust data reading
    if (m_dust->connected){
        m_dust->sendData( "RMMEAS");
        //while (!m_dust->is_read);
        m_dust->is_read = false;
        qDebug() << "count " << dust.length();

        for (int i = 0; i < dust.length(); i++)
        {
            tmp_type_measure = dust[i];
            tmp = m_data->value(tmp_type_measure, -1); //detect first measure

            if ( tmp == -1)
            {
                if (m_dust->measure->value(tmp_type_measure) >0)
                {
                    m_data->insert(tmp_type_measure, m_dust->measure->value(tmp_type_measure)); // insert into QMap ordering pair of measure first time
                    m_measure->insert(tmp_type_measure, 1);
                    qDebug() << "measure... " << tmp_type_measure << " value = " << (float)m_dust->measure->value(tmp_type_measure)/m_range->value(tmp_type_measure);
                }
            } else {
                if (m_dust->measure->value(tmp_type_measure) >0)
                {
                    m_data->insert(tmp_type_measure, tmp + m_dust->measure->value(tmp_type_measure));
                    m_measure->insert(tmp_type_measure, m_measure->value(tmp_type_measure, 0) +1); //increment counter
                    qDebug() << "measure... " << tmp_type_measure << " value = " << (float)m_dust->measure->value(tmp_type_measure)/m_range->value(tmp_type_measure);
                }
            }
        }
        // }

        m_dust->measure->clear();
    }
    //Meteostation data reading
    if (m_meteo->connected)
        m_meteo->sendData("LPS 2 1");//sendData("LOOP 1");

    //UPS handling
    if (m_ups->err_count <10){ //minimum error threshold
        m_ups->read_voltage();
        if (!m_measure->value("Напряжение мин."))
        {   m_data->insert("Напряжение мин.", m_ups->voltage);
            m_measure->insert("Напряжение мин.", 1); //we don't interested in average voltage - we need lowest or highest values

        }

        if (!m_measure->value("Напряжение макс."))
        {   m_data->insert("Напряжение макс.", m_ups->voltage);
            m_measure->insert("Напряжение макс.", 1); //we don't interested in average voltage - we need lowest or highest values

        }

        if (m_ups->voltage < m_data->value("Напряжение мин.")){
            m_data->insert("Напряжение мин.", m_ups->voltage);
        }

        if (m_ups->voltage > m_data->value("Напряжение макс.")){
            m_data->insert("Напряжение макс.", m_ups->voltage);
        }
    }


}
