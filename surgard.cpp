#include "surgard.h"

#include <QDebug>

surgard::surgard(QObject *parent) : QObject (parent)
{

}

surgard::~surgard()
{
}

void surgard::setData(QByteArray &data)
{

    QByteArray chnl_in = data.mid(1, 2); //surgard message format decoding
    QByteArray line = data.mid(3, 1);
    QByteArray equip_id = data.mid(7, 4);
    QByteArray  flag = data.mid(11, 1);
    QByteArray event = data.mid(12, 3);
    QByteArray group = data.mid(15, 2);
    QByteArray chain = data.mid(17, 3);

    qDebug() << "Fire alarm channel: " << (chnl_in);
    qDebug() << "Fire alarm line: " << (line);
    qDebug() << "Fire alarm equipment code: " << (equip_id);
    qDebug() << "Fire alarm flag: " << (flag);
    qDebug() << "Fire alarm event: " << (event);
    qDebug() << "Fire alarm group: " << (group);
    qDebug() << "Fire alarm chain: " << (chain);


}
