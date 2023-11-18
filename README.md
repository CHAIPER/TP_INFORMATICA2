# Control de LED a través de Comunicación Serie

En este proyecto, se desarrolla una aplicación gráfica de escritorio para controlar el encendido y apagado de un LED en una placa de desarrollo NodeMCU (ESP32) mediante comunicación serie sobre USB. La aplicación se implementa en C++ utilizando el framework Qt. A continuación, se presentan las clases y su funcionamiento:

## Clase `portCfg`

```cpp
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
```

Esta clase define la ventana de configuración del puerto serie. Contiene un constructor que inicializa la interfaz de usuario y un destructor para liberar la memoria asociada.

## Clase `ventanaPrincipal`

```cpp
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
```

Esta clase representa la ventana principal de la aplicación. Contiene la configuración del puerto serie, funciones para enviar datos y diversos slots para manejar eventos de la interfaz de usuario.

## Implementación de `portCfg`

```cpp
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
```

En la implementación de la clase `portCfg`, se realiza la inicialización de la interfaz de usuario en el constructor y se libera la memoria en el destructor.

## Implementación de `ventanaPrincipal`

```cpp
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
```

La implementación de la clase `ventanaPrincipal` incluye la configuración del puerto serie en el constructor, la función `sendData` para enviar datos al ESP32 y la gestión de eventos de la interfaz de usuario.

## Máquina de Estados para Recepción de Datos

```cpp
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
```

Se implementa una máquina de estados finita para interpretar los datos recibidos a través del puerto serie. Se definen funciones para cada estado de la máquina (`EstadoHeader`, `EstadoAlto`, `EstadoBajo`, `EstadoSumaComprobacion`).

## Función `main`

```cpp
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
```

En la función `main`, se crea una instancia de la aplicación Qt (`QApplication`) y de la ventana principal (`ventanaPrincipal`). Se muestra la ventana principal en la interfaz gráfica.

# Implementacion ESP32
¡Por supuesto! Vamos a desglosar el código parte por parte:

### 1. Inclusión de Librerías y Definiciones:
```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include <freertos/queue.h>
#include "serial.h"
#include <driver/gpio.h>
```
Esto incluye las bibliotecas necesarias para el desarrollo en el entorno ESP32, como FreeRTOS para el manejo de tareas y colas, funciones de UART, GPIO y una biblioteca personalizada llamada "serial.h".

### 2. Definiciones de Constantes:
```c
#define pkg_h 0x0C
#define ledPin GPIO_NUM_2
```
Aquí se definen constantes para el encabezado del paquete (`pkg_h`) y el pin del LED (`ledPin`).

### 3. Inicialización de Hardware:
```c
void hardware_init(void)
```
Esta función configura el pin del LED como salida.

### 4. Estructuras y Enumeraciones:
Se definen estructuras y enumeraciones que representan el estado y las partes de datos del paquete.

### 5. Funciones de Estado:
```c
bool EstadoHeader(Estado *s, unsigned char ValorNuevo);
bool EstadoAlto(Estado *s, unsigned char ValorNuevo);
bool EstadoBajo(Estado *s, unsigned char ValorNuevo);
bool EstadoSumaComprobacion(Estado *s, unsigned char ValorNuevo);
```
Estas funciones implementan las acciones de cada estado de la máquina de estados finitos (FSM) utilizada para procesar los datos recibidos por UART.

### 6. Función de Inicialización de PWM:
```c
void ProgramaPwm(double high, double low);
```
Esta función controla el LED mediante PWM.

### 7. Tarea de Manejo del LED (`led_management_task`):
```c
void led_management_task(void *param)
```
Esta tarea maneja el LED basándose en los parámetros recibidos desde la cola.

### 8. Tarea de Comunicación Serie (`serial_comm_task`):
```c
void serial_comm_task(void *param)
```
Esta tarea implementa una máquina de estados para procesar datos provenientes de UART. Coloca el estado en una cola cuando se completa un paquete válido.

### 9. Función Principal (`app_main`):
```c
void app_main()
```
En la función principal, se inicializa la comunicación serie, el hardware, y se crean las tareas `led_management_task` y `serial_comm_task`.

### 10. Máquina de Estados (`EstadoHeader`, `EstadoAlto`, `EstadoBajo`, `EstadoSumaComprobacion`):
Estas funciones implementan la lógica de la máquina de estados finitos (FSM) utilizada para procesar los datos recibidos por UART. Determinan el estado actual y realizan transiciones basadas en los datos recibidos.

En resumen, el código controla un LED mediante PWM y procesa datos de UART utilizando una máquina de estados finitos, enviando la información procesada a través de una cola para que una tarea la utilice. La implementación está estructurada utilizando el paradigma de programación por tareas y la lógica de máquinas de estados.
