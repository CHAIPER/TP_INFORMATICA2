#include "dialog.h"
#include "ui_dialog.h"
#include "QPixmap"
#include <QTimer>

void Dialog::connected(){
  ui->list->addItem("Conectado");
   cliente.subscribe("/ej02/cmd");
   cliente.subscribe("/ej02/"+ui->le_ID->text()+"/cmd");
     ui->label_6->setText("Apagado");
     ui->LedImageLabel->setPixmap(QPixmap(":/led_apagado.jpg"));
}
void Dialog::disconnected(){
  ui->list->addItem("Desconectado");
}
void Dialog::error(const QMQTT::ClientError error){
  ui->list->addItem("Error: " + QString::number(error));
}
void Dialog::subscribed(const QString& topic, const quint8 qos){
  ui->list->addItem("Subscripto: " + topic + "( Qos: " + QString::number(qos) + ")");
}
void Dialog::unsubscribed(const QString& topic){
 ui->list->addItem("Unsubscripto: " + topic);
}
void Dialog::published(const QMQTT::Message& message, quint16 msgid){
 ui->list->addItem("Publicado:" + message.topic() + ":" + message.payload());
}
void Dialog::pingresp(){

}
void Dialog::received(const Message &message){
  Message msg;
  connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));

  if(message.topic() == "/ej02/cmd"){
      QMQTT :: Message mensID;
      mensID.setTopic( "/ej02/id");
      mensID.setPayload(ui->le_ID->text().toLatin1());
      cliente.publish(mensID);
   }
  if(message.topic()=="/ej02/"+ui->le_ID->text()+"/cmd"){
      if(QString(message.payload()) == "ledon" ){
        ui->LedImageLabel->setPixmap(QPixmap(":/images.png"));
        ui->label_6->setText("Prendido");
      }else{
         ui->LedImageLabel->setPixmap(QPixmap(":/led_apagado.jpg"));
         ui->label_6->setText("Apagado");
  }}



  else
    ui->list->addItem("Recibido - Tópico: " + message.topic() + " Message:" + message.payload());
}


Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->dial->setRange(0,100);
    connect(&cliente, SIGNAL(connected()), this, SLOT(connected()));
    connect(&cliente, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(&cliente, SIGNAL(error(const QMQTT::ClientError)), this, SLOT(error(const QMQTT::ClientError)));
    connect(&cliente, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(subscribed(const QString&, const quint8)));
    connect(&cliente, SIGNAL(unsubscribed(const QString&)), this, SLOT(unsubscribed(const QString&)));

    connect(&cliente, SIGNAL(published(const QMQTT::Message&, quint16 )), this, SLOT(published(const QMQTT::Message&, quint16 )));
    connect(&cliente, SIGNAL(pingresp()), this, SLOT(pingresp()));
    connect(&cliente, SIGNAL(received(const QMQTT::Message&)), this, SLOT(received(const QMQTT::Message&)));
    timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
        timer->start(1000); // El timer emitirá la señal timeout cada 5 segundos



}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_dial_valueChanged(int value)
{
    lastDialValue = value; // Guardamos el último valor del dial
       publishDialValue(value); // Publicamos el valor del dial
}
void Dialog::on_timer_timeout()
{
    publishDialValue(lastDialValue); // Publicamos el último valor del dial
}
void Dialog::publishDialValue(int value)
{
    QMQTT::Message msg;
    msg.setTopic("/ej02/sensor");
    msg.setPayload(QString :: number(value).toLatin1()+"/"+ui->le_ID->text().toLatin1());
         cliente.publish(msg);
}
void Dialog::on_btn_Desconectar_clicked()
{
    if(cliente.isConnectedToHost()){
       cliente.disconnectFromHost();
      }
    ui->list->clear();
}

void Dialog::on_btnConnect_clicked()
{
        ui->list->clear();
    cliente.setHostName(ui->le_serv->text());
    if((ui->le_serv->text().length() > 0) && (ui->le_clave->text().length() > 0)){
        cliente.setUsername(ui->le_serv->text());
        cliente.setPassword(ui->le_clave->text().toLatin1());
      }
    cliente.setPort(ui->le_puerto->text().toInt());
    cliente.connectToHost();
}



void Dialog::on_LedImageLabel_linkActivated(const QString &link)
{

}

void Dialog::on_dial_actionTriggered(int action)
{

}
