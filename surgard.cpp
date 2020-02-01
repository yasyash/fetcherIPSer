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

#include "surgard.h"

#include <QDebug>

surgard::surgard(QObject *parent) : QObject (parent)
{

    m_table =  new QMap<QString, QString>;

    m_table->insert("E110", "Пожар");
    m_table->insert("E111", "Дым");
    m_table->insert("E112", "Пламя");
    m_table->insert("E120", "Нападение");
    m_table->insert("E121", "Тревога: принуждение");
    m_table->insert("E122", "Тихая тревога");
    m_table->insert("E123", "Тревога ШС");
    m_table->insert("E137", "Взлом");
    m_table->insert("E139", "Тревога: проникновение");
    m_table->insert("E140", "Тревога: перемещение");
    m_table->insert("E141", "Обрыв ШС");
    m_table->insert("E142", "Короткое замыкание ШС");
    m_table->insert("E144", "Вскрытие корпуса");
    m_table->insert("E151", "Тревога: газ");
    m_table->insert("E154", "Тревога: затопление");
    m_table->insert("E158", "Тревога: высокая температура");
    m_table->insert("E159", "Тревога: низкая температура");
    m_table->insert("E230", "Вскрытие КТС");
    m_table->insert("E301", "Сбой сетевого питания");
    m_table->insert("E302", "Разряд батареи");
    m_table->insert("E307", "Неизвестное сообщение");
    m_table->insert("E308", "Ошибка программного кода");
    m_table->insert("E310", "Саботаж");
    m_table->insert("E324", "Неисправность выхода реле");
    m_table->insert("E330", "Неисправность периферийных устройств");
    m_table->insert("E333", "Неисправность расширителя");
    m_table->insert("E344", "Возникла помеха в канале");
    m_table->insert("E345", "Радиопомеха");
    m_table->insert("E350", "Недоступен канал связи");
    m_table->insert("E351", "Неисправность линии связи");
    m_table->insert("E353", "Неисправность радиоканала");
    m_table->insert("E355", "Ошибка опроса баланса");
    m_table->insert("E378", "Неисправность канала");
    m_table->insert("E380", "Неисправность ШС");
    m_table->insert("E383", "Неисправность (взлом)");
    m_table->insert("E393", "Техническое обслуживание устройства");
    m_table->insert("E401", "Снят с охраны пользователем");
    m_table->insert("E402", "Снят с охраны пользователем");
    m_table->insert("E403", "Снятие с охраны");
    m_table->insert("E407", "Снят с охраны оператором");
    m_table->insert("E408", "Снятие СМС");
    m_table->insert("E412", "Удалённое управление устройством");
    m_table->insert("E422", "Ошибка изменения ключа шифрации");
    m_table->insert("E428", "Регистрация нового пользователя");
    m_table->insert("E429", "Авторизация пользователя");
    m_table->insert("E431", "Смена IP адреса");
    m_table->insert("E451", "Взятие раньше времени начала графика охраны");
    m_table->insert("E452", "Взятие позже времени окончания графика охраны");
    m_table->insert("E456", "Обход (исключение) раздела/зоны при взятии");
    m_table->insert("E457", "Не взят оператором");
    m_table->insert("E458", "Невзятие раздела/зоны");
    m_table->insert("E459", "Невзятие СМС");
    m_table->insert("E461", "Подбор кода");
    m_table->insert("E601", "Ручной тест устройства");
    m_table->insert("E602", "Периодический тест устройства");
    m_table->insert("E608", "Баланс ниже критического");
    m_table->insert("E624", "Переполнение архива событий");
    m_table->insert("E627", "Начало программирования");
    m_table->insert("E630", "Активная СИМ-карта");
    m_table->insert("E631", "Замена СИМ-карты");
    m_table->insert("E654", "Выключение устройства");
    m_table->insert("E655", "Нет ключей GSM");
    m_table->insert("E658", "Авария основного питания");
    m_table->insert("E661", "Невзятие вне графика охраны");
    m_table->insert("E662", "Перенастройка устройства");
    m_table->insert("E663", "Сбой основного питания");
    m_table->insert("E710", "Включение О.К.");
    m_table->insert("E724", "Код опроса баланса не задан");
    m_table->insert("E726", "Состояние баланса");
    m_table->insert("E728", "Контроль наряда");
    m_table->insert("E801", "Выход за пределы контролируемого параметра");
    m_table->insert("E802", "Превышение контролируемого параметра");
    m_table->insert("E803", "Снижение контролируемого параметра");
    m_table->insert("E804", "Тревога ручная");
    m_table->insert("E805", "Внешняя тревога");
    m_table->insert("E806", "Внутренняя тревога");
    m_table->insert("E808", "Маскирование");
    m_table->insert("E809", "Перенастройка");
    m_table->insert("E810", "Перемещение");
    m_table->insert("E811", "Отрыв от поверхности установки");
    m_table->insert("E812", "Программное вмешательство");
    m_table->insert("E814", "Потеря управления устройством");
    m_table->insert("E815", "Неисправность схемотехники");
    m_table->insert("E819", "Снятие с задержкой");
    m_table->insert("E820", "Взятие вне графика охраны");
    m_table->insert("E821", "Включение устройства");
    m_table->insert("E822", "Сброс на заводские настройки");
    m_table->insert("E823", "Вход в геозону");
    m_table->insert("E824", "Пониженное сетевое питание");
    m_table->insert("E825", "Повышенное сетевое питание");
    m_table->insert("E828", "Восстановление основного питания");
    m_table->insert("E829", "Переход на резервное питание");
    m_table->insert("E831", "Отсутствует несущая сигнала");
    m_table->insert("E832", "Канал занят");
    m_table->insert("E833", "Нет ответа от устройства");
    m_table->insert("E834", "Тест устройства");
    m_table->insert("E835", "Не взят пользователем");
    m_table->insert("E839", "Авария резервного питания");
    m_table->insert("E841", "Восстановление резервного питания");
    m_table->insert("E847", "Удалена SIM-карта 1");
    m_table->insert("E848", "Удалена SIM-карта 2");
    m_table->insert("E850", "Потеря связи с устройством");
    m_table->insert("E851", "Помехи в канале связи с ПЦО");
    m_table->insert("E852", "Отключение канала связи с ПЦО");
    m_table->insert("E854", "Слабый уровень сигнала");
    m_table->insert("E866", "Потеря связи с расширителем объектовой сети");
    m_table->insert("E871", "Уровень сигнала ");
    m_table->insert("E873", "Уровень принимаемого сигнала ");
    m_table->insert("E874", "Качество принимаемого сигнала ");
    m_table->insert("E875", "Емкость аккумулятора ");
    m_table->insert("E876", "Емкость аккумулятора ниже критической");
    m_table->insert("E877", "Уровень шума в канале ");
    m_table->insert("E878", "Ошибка передатчика");
    m_table->insert("E879", "Ошибка передатчика");
    m_table->insert("E880", "Баланс в норме");
    m_table->insert("E881", "Качество связи ");
    m_table->insert("E998", "Начало проверки тревожной кнопки");
    m_table->insert("R0", "Неопределено");
    m_table->insert("R110", "Восстановление (пожар)");
    m_table->insert("R111", "Восстановление (дым)");
    m_table->insert("R118", "Сработка пожарного датчика (переопрос)");
    m_table->insert("R120", "Восстановление (нападение)");
    m_table->insert("R130", "Восстановление ШС");
    m_table->insert("R131", "Восстановление");
    m_table->insert("R134", "Вход");
    m_table->insert("R137", "Блокировка корпуса");
    m_table->insert("R139", "Восстановление (проникновение)");
    m_table->insert("R141", "В норме (Обрыв ШС)");
    m_table->insert("R142", "В норме (КЗ ШС)");
    m_table->insert("R150", "Температура в норме");
    m_table->insert("R151", "Восстановление (газ)");
    m_table->insert("R154", "Восстановление (затопление)");
    m_table->insert("R158", "Восстановление (высокая температура)");
    m_table->insert("R159", "Восстановление (низкая температура)");
    m_table->insert("R200", "Сброс пожарных датчиков пользователем");
    m_table->insert("R201", "Сброс пожарных датчиков");
    m_table->insert("R202", "Сброс пожарных датчиков СМС");
    m_table->insert("R220", "Разблокирование КТС");
    m_table->insert("R230", "Закрытие КТС");
    m_table->insert("R301", "Восстановление сетевого питания");
    m_table->insert("R305", "Сброс/перезагрузка устройства");
    m_table->insert("R306", "Изменение настроек (USB)");
    m_table->insert("R311", "Батарея в норме");
    m_table->insert("R333", "Расширитель в норме");
    m_table->insert("R345", "Отсутствие помехи в радиоканале");
    m_table->insert("R350", "Восстановление связи с устройством");
    m_table->insert("R355", "Восстановление канала");
    m_table->insert("R380", "ШС в норме");
    m_table->insert("R393", "Завершение технического обслуживания устройства");
    m_table->insert("R401", "Взят под охрану пользователем");
    m_table->insert("R402", "Взят под охрану пользователем");
    m_table->insert("R403", "Взятие под охрану");
    m_table->insert("R407", "Взят под охрану оператором");
    m_table->insert("R408", "Взятие СМС");
    m_table->insert("R421", "Команда не выполнена");
    m_table->insert("R422", "Выполнена удалённая команда");
    m_table->insert("R423", "Ключ шифрации успешно изменен");
    m_table->insert("R450", "Отказ от взятия раздела");
    m_table->insert("R462", "Отказ в авторизации пользователя");
    m_table->insert("R463", "Перевзятие");
    m_table->insert("R464", "Сброс тревоги");
    m_table->insert("R465", "Сброс тревоги с пульта");
    m_table->insert("R466", "Сброс тревоги СМС");
    m_table->insert("R467", "Сброс тревоги");
    m_table->insert("R520", "Сброс сирены пользователь");
    m_table->insert("R521", "Сброс сирены с пульта");
    m_table->insert("R522", "Сброс сирены СМС");
    m_table->insert("R602", "Дежурный режим");
    m_table->insert("R627", "Окончание программирования");
    m_table->insert("R628", "Добавлено ключей");
    m_table->insert("R631", "Канал Ethernet");
    m_table->insert("R632", "Канал GPRS");
    m_table->insert("R633", "Канал CSD");
    m_table->insert("R634", "Канал SMS");
    m_table->insert("R635", "Радиоканал");
    m_table->insert("R655", "Ключей");
    m_table->insert("R664", "Частичное взятие");
    m_table->insert("R710", "Выключение О.К.");
    m_table->insert("R818", "Взятие с задержкой");
    m_table->insert("R820", "Снятие вне графика охраны");
    m_table->insert("R821", "Начало снятия (Вероятная тревога)");
    m_table->insert("R823", "Выход из геозоны");
    m_table->insert("R866", "Восстановление связи с расширителем объектовой сети");
    m_table->insert("R998", "Конец проверки тревожной кнопки");
    m_table->insert("R999", "Ошибка проверки тревожной кнопки");


    m_event = new QMap<QDateTime, QString>;
    m_event_code = new QMap<QDateTime, QString>;

}

surgard::~surgard()
{
}

void surgard::setData(QByteArray &data)
{
    QString contact_id = QString(data.mid(5, 2));
    if ( !contact_id.compare("18")) //detect Contact ID flag
    {
        QByteArray chnl_in = data.mid(1, 2); //surgard message format decoding
        QByteArray line = data.mid(3, 1);
        QByteArray equip_id = data.mid(7, 4);
        QByteArray  flag = data.mid(11, 1);
        QByteArray event = data.mid(12, 3);
        QByteArray group = data.mid(15, 2);
        QByteArray chain = data.mid(17, 3);

        qDebug() << "Fire alarm channel: " << (chnl_in) << "\n\r";
        qDebug() << "Fire alarm line: " << (line)<< "\n\r";
        qDebug() << "Fire alarm equipment code: " << (equip_id)<< "\n\r";
        qDebug() << "Fire alarm flag: " << (flag)<< "\n\r";
        qDebug() << "Fire alarm event: " << (event)<< "\n\r";
        qDebug() << "Fire alarm group: " << (group)<< "\n\r";
        qDebug() << "Fire alarm chain: " << (chain)<< "\n\r";

        QDateTime now =  QDateTime::currentDateTime();

        if ( QString(flag).append(event).compare("R602")) //if not duty mode wich receive each xx seconds
        {
        m_event ->insert( now , m_table->value(QString(flag).append(QString(event))));
        m_event_code ->insert( now , QString(flag).append(QString(event)));

        }

    }


}
