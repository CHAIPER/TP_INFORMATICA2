#include "dialog.h"
#include "ui_dialog.h"
#include <ctime>
#include <QStringList>
#include <QBrush>
#include <QColor>

void Dialog::connected(){
  ui->list->addItem("Conectado");
  cliente2.subscribe("/ej02/id");
  cliente2.subscribe("/ej02/sensor");

}

void Dialog::disconnected(){
  ui->list->addItem("Desconectado");
}
void Dialog::error(const QMQTT::ClientError error){
  ui->list->addItem("Error: " + QString::number(error));
}
void Dialog::subscribed(const QString& topic, const quint8 qos){
  ui->list->addItem("Subscripto:  "  + topic  + "( Qos: " +  QString::number(qos) + ")");
}
void Dialog::unsubscribed(const QString& topic){
 ui->list->addItem("Unsubscripto: " + topic);
}
void Dialog::published(const QMQTT::Message& message, quint16 msgid){
 ui->list->addItem("Publicado:" + message.topic() + ":" + message.payload()+"MsgId: " +QString::number(msgid));
}
void Dialog::pingresp(){

}
void Dialog::received(const Message &message){
  Message msg;
  QStringList datos;

  if(message.topic() == "/ej02/sensor"){
      QString dato = QString(message.payload());
      datos=dato.split("/",QString::SkipEmptyParts);
      if(datos.count()==2){
          if(ui->boxdispositivos->currentText()==datos[1]){
          actualizarGrafica(datos[0].toDouble());
      }
         }
            }
  if(message.topic()=="/ej02/id"){
      ui->ListDispo->addItem( QString(message.payload()));
      ui->boxdispositivos->addItem(QString(message.payload()));
    }
        }

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);


    connect(&cliente2, SIGNAL(connected()), this, SLOT(connected()));
    connect(&cliente2, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(&cliente2, SIGNAL(error(const QMQTT::ClientError)), this, SLOT(error(const QMQTT::ClientError)));
    connect(&cliente2, SIGNAL(subscribed(const QString&, const quint8)), this, SLOT(subscribed(const QString&, const quint8)));
    connect(&cliente2, SIGNAL(unsubscribed(const QString&)), this, SLOT(unsubscribed(const QString&)));

    connect(&cliente2, SIGNAL(published(const QMQTT::Message&, quint16 )), this, SLOT(published(const QMQTT::Message&, quint16 )));
    connect(&cliente2, SIGNAL(pingresp()), this, SLOT(pingresp()));
    connect(&cliente2, SIGNAL(received(const QMQTT::Message&)), this, SLOT(received(const QMQTT::Message&)));

    ui->grafico->addGraph();
    ui->grafico->xAxis->setLabel("Tiempo");
    ui->grafico->yAxis->setLabel("Valores del sensor");
    tiempo = 0;
   ui->grafico->xAxis->setRange(0,1000);
   ui->grafico->yAxis->setRange(0,100);
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_btn_connect_clicked()
{
    ui->list->clear();
    cliente2.setHostName(ui->le_servidor->text());
    if((ui->le_servidor->text().length() > 0) && (ui->le_clave->text().length() > 0)){
        cliente2.setUsername(ui->le_servidor->text());
        cliente2.setPassword(ui->le_clave->text().toLatin1());
      }
    cliente2.setPort(ui->le_puerto->text().toInt());
    cliente2.connectToHost();


}

void Dialog::on_btn_desconectar_clicked()
{
    if(cliente2.isConnectedToHost()){
       cliente2.disconnectFromHost();
      }
    ui->list->clear();
    ui->ListDispo->clear();
    ui->grafico->removeGraph(0);
    ui->grafico->addGraph();
    ui->grafico->replot();
}


void Dialog::on_btn_led_on_clicked()
{

QMQTT ::Message ledon;
QList<QListWidgetItem*> selected;
QList<QListWidgetItem*> :: iterator item;
selected = ui->ListDispo->selectedItems();
for(item = selected.begin(); item != selected.end(); item++){
    ledon.setTopic("/ej02/"+(*item)->text()+"/cmd");
    ledon.setPayload("ledon");
    cliente2.publish(ledon);
}

}

void Dialog::on_btn_GETID_clicked()
{
    ui->list->clear();

    QMQTT ::Message mensaje;
    mensaje.setTopic("/ej02/cmd");
    mensaje.setPayload("getid");
    cliente2.publish(mensaje);
    ui->boxdispositivos->clear();
    ui->list->clear();
    ui->ListDispo->clear();
    ui->grafico->removeGraph(0);
    ui->grafico->addGraph();
    ui->grafico->replot();
    ui->grafico->clearGraphs();

}

void Dialog::on_btn_led_Off_clicked()
{
    QMQTT ::Message ledoff;
    QList<QListWidgetItem*> selected;
    QList<QListWidgetItem*> :: iterator item;
    selected = ui->ListDispo->selectedItems();
    for(item = selected.begin(); item != selected.end(); item++){
        ledoff.setTopic("/ej02/"+(*item)->text()+"/cmd");
        ledoff.setPayload("ledoff");
        cliente2.publish(ledoff);
}
}


void Dialog::on_pushButton_clicked()
{
    QApplication ::exit();
}

void Dialog :: actualizarGrafica(double val) {

    QCPGraph *graph = ui->grafico->graph(0);


    graph->addData(tiempo,val); // Añadir punto al gráfico
    tiempo++;

    // Ajustar el rango del eje X para mostrar solo un intervalo de tiempo
    ui->grafico->xAxis->setRange(tiempo - 1, tiempo);  // Mostrar el último segundo

    // acomodar el eje Y para adaptarse a los nuevos datos
    ui->grafico->rescaleAxes();

     double Rangoinferior = tiempo - 1;
     int eliminarcont = 0;
     while (graph->data()->at(eliminarcont)->key < Rangoinferior) {
         eliminarcont++;
     }
     graph->data()->removeBefore(eliminarcont);
     // Crear un nuevo gráfico para el área debajo de la curva
       QCPGraph *fillGraph = ui->grafico->addGraph();
       fillGraph->setPen(Qt::NoPen); // Sin línea de borde
       fillGraph->setBrush(QBrush(QColor(81, 209, 246, 10))); // Color Celeste con opacidad 90

       // Añadir datos al gráfico de relleno para crear el área sombreada
       for (int i = 0; i < graph->data()->size(); ++i) {
           fillGraph->addData(graph->data()->at(i)->key, graph->data()->at(i)->value);
       }



    ui->grafico->replot(); // Redibujar el gráfico
}



void Dialog::on_boxdispositivos_currentIndexChanged(const QString &arg1)
{
    ui->grafico->removeGraph(0);
    ui->grafico->addGraph();
    ui->grafico->replot();
    ui->list->clear();
}
