#ifndef DIALOG_H
#define DIALOG_H


#include <QDialog>
#include <QList>
#include "qmqtt.h"
#include <QPixmap>
#include <QPainter>
#include <QVector>
#include <QTimer>
#include <QVBoxLayout>
#include <QBrush>
#include <QColor>


QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

using namespace QMQTT;


class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    Client cliente2;

    ~Dialog();

private slots:
    void connected();
    void disconnected();
    void error(const QMQTT ::  ClientError);
    void subscribed(const QString& topic, const quint8 qos);
    void unsubscribed(const QString& topic);
    void published(const QMQTT::Message& message, quint16 msgid=0);
    void pingresp();
    void received(const QMQTT::Message&);

    void on_btn_connect_clicked();

    void on_btn_desconectar_clicked();

    void on_btn_led_on_clicked();

    void on_btn_GETID_clicked();

    void on_btn_led_Off_clicked();

    void on_pushButton_clicked();

    void actualizarGrafica(double val);

     void on_boxdispositivos_currentIndexChanged(const QString &arg1);

private:
    Ui::Dialog *ui;
    double tiempo;


};
#endif // DIALOG_H
