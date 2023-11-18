Control de LED a través de Comunicación Serie
En este proyecto, se desarrolla una aplicación gráfica de escritorio para controlar el encendido y apagado de un LED en una placa de desarrollo NodeMCU (ESP32) mediante comunicación serie sobre USB. La aplicación se implementa en C++ utilizando el framework Qt. A continuación, se presentan las clases y su funcionamiento:

claseportCfg
cpp

Copiar código
#ifndef PORTCFG_H
#define PORTCFG_H

#include <QDialog>

namespace Ui {
class portCfg;
}

class portCfg : public QDialog
{
    Q_OBJECT

public:
    explicit portCfg(QWidget *parent = 0);
    ~portCfg();
    Ui::portCfg *ui;

private:
};

#endif // PORTCFG_H
Esta clase define la ventana de configuración del puerto serie. Contiene un constructor que inicializa la interfaz de usuario y un destructor para liberar la memoria asociada.

claseventanaPrincipal
cpp

Copiar código
#ifndef VENTANAPRINCIPAL_H
#define VENTANAPRINCIPAL_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "ui_portcfg.h"
#include <QListWidgetItem>
#include <QString>
#include <QDebug>
#include <stdbool.h>
#include <cstdint>

// ... (código omitido para brevedad)

class ventanaPrincipal : public QDialog
{
    Q_OBJECT

public:
    ventanaPrincipal(QWidget *parent = nullptr);
    ~ventanaPrincipal();

    QSerialPort *puertoSerie;
    void sendData(uint16_t t_on, uint16_t t_off);

private slots:
    // ... (métodos y slots omitidos para brevedad)

private:
    Ui::ventanaPrincipal *ui;
};

#endif // VENTANAPRINCIPAL_H
Esta clase representa la ventana principal de la aplicación. Contiene la configuración del puerto serie, funciones para enviar datos y diversas ranuras para manejar eventos de la interfaz de usuario.

Implementación deportCfg
cpp

Copiar código
#include "portcfg.h"
#include "ui_portcfg.h"

portCfg::portCfg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::portCfg)
{
    ui->setupUi(this);
}

portCfg::~portCfg()
{
    delete ui;
}
En la implementación de la clase portCfg, se realiza la inicialización de la interfaz de usuario en el constructor y se libera la memoria en el destructor.

Implementación deventanaPrincipal
cpp

Copiar código
#include "ventanaprincipal.h"
#include "ui_ventanaprincipal.h"
#include "portcfg.h"
#include <QSerialPortInfo>

// ... (código omitido para brevedad)

ventanaPrincipal::ventanaPrincipal(QWidget *parent) : QDialog(parent), ui(new Ui::ventanaPrincipal) {
    // ... (código omitido para brevedad)
}

ventanaPrincipal::~ventanaPrincipal() {
    // ... (código omitido para brevedad)
}

// ... (implementación de otros métodos y slots)

void ventanaPrincipal::sendData() {
    // ... (código omitido para brevedad)
}

// ... (implementación de otros métodos y slots)
La implementación de la clase ventanaPrincipalincluye la configuración del puerto serie en el constructor, la función sendDatapara enviar datos al ESP32 y la gestión de eventos de la interfaz de usuario.

Máquina de Estados para Recepción de Datos
cpp

Copiar código
bool EstadoHeader(Estado *s, unsigned char newVal);
bool EstadoAlto(Estado *s, unsigned char newVal);
bool EstadoBajo(Estado *s, unsigned char newVal);
bool EstadoSumaComprobacion(Estado *s, unsigned char newVal);

// ... (código omitido para brevedad)

bool EstadoHeader(Estado *s, unsigned char newVal) {
    // ... (código omitido para brevedad)
}

bool EstadoAlto(Estado *s, unsigned char newVal) {
    // ... (código omitido para brevedad)
}

bool EstadoBajo(Estado *s, unsigned char newVal) {
    // ... (código omitido para brevedad)
}

bool EstadoSumaComprobacion(Estado *s, unsigned char newVal) {
    // ... (código omitido para brevedad)
}
Se implementa una máquina de estados finita para interpretar los datos recibidos a través del puerto serie. Se definen funciones para cada estado de la máquina ( EstadoHeader, EstadoAlto, EstadoBajo, EstadoSumaComprobacion).

Funciónmain
cpp

Copiar código
#include <QApplication>
#include "ventanaprincipal.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ventanaPrincipal w;
    qDebug() << "TODO OK!!!";
    w.show();

    return a.exec();
}
En la función main, se crea una instancia de la aplicación Qt ( QApplication) y de la ventana principal ( ventanaPrincipal). Se muestra la ventana principal en la interfaz gráfica.

Este es un resumen del código proporcionado para el proyecto. La aplicación permite configurar y controlar un LED en una placa ESP32 mediante una interfaz gráfica de escritorio utilizando Qt y comunicación serie.
