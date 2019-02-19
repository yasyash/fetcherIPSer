#ifndef SURGARDWIDGET_H
#define SURGARDWIDGET_H

#include <QByteArray>
#include <QObject>

class surgard : public QObject
{
    Q_OBJECT

public:
     surgard(QObject *parent);
    ~surgard();

private slots:
    void setData(QByteArray &data);



};

#endif // SURGARDWIDGET_H
