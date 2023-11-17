Como breve introducción, al comienzo de cada actividad del trabajo, colocaremos las condiciones y los métodos necesarios que nos indican los respectivos enunciados.
Parte I: Comunicación serie
Desarrolle una aplicación gráfica de escritorio que permita controlar, a través de una
comunicación serie sobre USB, el encendido y apagado del led presente en la placa de desarrollo
NodeMCU (ESP32). Para esto considere que:
- El tiempo en el que el led está encendido y el tiempo que está apagado se deben poder
configurar de modo independiente, por lo que el led puede estar totalmente encendido
(tiempo encendido diferente de 0 y tiempo apagado igual a 0), totalmente apagado
(tiempo encendido igual a 0 y tiempo apagado diferente de cero), o cualquier combinación
(por ejemplo 100 ms apagado y 400 ms encendido).
- Una vez aplicada la configuración de los tiempos de encendido de encendido y apagado, el
led debe repetir el proceso de actualización indefinidamente.
- El máximo tiempo al que deberán responder es de 2047 [ms].
- Los valores de configuración deben ser proporcionados desde una aplicación desarrollada
en Qt y que mantenga comunicación a través de un objeto de la clase QSerialPort (o
QExtSerialPort) y la UART0 disponible en la placa de desarrollo.
- Se propone utilizar widgets QSlider a fin de establecer los valores de configuración,
utilizando el slot ValueChanged. Cada vez que uno de los dos widgets cambie su valor
deberá enviarse la información correspondiente a la placa de desarrollo a fin de reflejar en
el led la configuración establecida utilizando un protocolo que incluya, como mínimo, un
encabezado (o header = 0x0B), dos bytes con la configuración de tiempo de apagado, dos
bytes con la configuración de tiempo de encendido, un byte con la suma de comprobación
y una marca de finalización (footer 0xB0).
- Una vez aplicada la configuración, la placa de desarrollo deberá contestar con un paquete
que conste, como mínimo, de un encabezado, código indicando el éxito de la operación
(0xCC por ejemplo), suma de comprobación y marca de finalización. Al recibirlo y verificar
su correcta estructura, la aplicación deberá presentar un mensaje en una widget QList
conteniendo tanto el mensaje original como su interpretación (éxito o fracaso de la
operación).
- La aplicación desarrollada para el sistema embebido deberá contar, como mínimo, con dos
tareas. Una dedicada al procesamiento de los datos provenientes de la UART y otra
dedicada al control del led (encendido y apagado durante el tiempo correspondiente). La
comunicación entre ambas deberá realizarse utilizando una estructura de datos de tipo
cola (xQueue).
- Se recomienda, en todos los casos, procesar los datos provenientes de la comunicación
serie utilizando una máquina de estados finita implementada a través de un vector de
punteros a funciones.

Codigo:

Clase portcfg:

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

Incluimos la definición de la clase QDialog. Declaramos un “namespace” llamado Ui y una clase portCfg dentro de ese espacio; portCfg es una clase que hereda de QDialog. Luego Definimos un constructor (explicit portCfg(QWidget *parent = 0);) y un destructor (~portCfg();).


Clase VentanaPrincipal:
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

#define header_datos 0x0B
#define header_resp 0x0C

QT_BEGIN_NAMESPACE
namespace Ui { class ventanaPrincipal; }
QT_END_NAMESPACE


typedef enum { stHeader = 0, ST_NivelAlto, ST_NivelBajo, stSuma } EstadoValues;

typedef union {
    unsigned short val;
    uint8_t p[2];
} shortVal;

typedef struct {
    shortVal value;
    uint8_t inx;
} shortParts;

typedef struct
{
    EstadoValues ActividadEstado;
    shortParts ValorAlto;
    shortParts ValorBajo;
    unsigned int sum;
} Estado;

typedef bool (*fcnState)(Estado *, unsigned char);

bool EstadoHeader(Estado *s, unsigned char newVal);
bool EstadoAlto(Estado *s, unsigned char newVal);
bool EstadoBajo(Estado *s, unsigned char newVal);
bool EstadoSumaComprobacion(Estado *s, unsigned char newVal);


class ventanaPrincipal : public QDialog
{
    Q_OBJECT

public:
    ventanaPrincipal(QWidget *parent = nullptr);
    ~ventanaPrincipal();

    QSerialPort *puertoSerie;
    void sendData(uint16_t t_on, uint16_t t_off);

private slots:
    void on_btnConectar_clicked();
    void on_btnDesconectar_clicked();
    void on_btnLimpiar_clicked();
    void on_tiempoEncendidoSlider_valueChanged(int value);
    void on_tiempoApagadoSlider_valueChanged(int value);
    void on_tiempoEncendidoSlider_sliderReleased();
    void on_tiempoApagadoSlider_sliderReleased();
    void on_lineEditTiempoEncendido_editingFinished();
    void on_lineEditTiempoApagado_editingFinished();
    void datos_recibidos();
    void sendData();

private:
    Ui::ventanaPrincipal *ui;
};

#endif // VENTANAPRINCIPAL_H


 
La clase anterior es la encargada de controlar la comunicación serie. Incluimos las clases, bibliotecas necesarias y definimos las constantes de los headers. Declaramos un tipo enum llamado EstadoValues, que puede tomar lo distintos valores que indicamos. Definimos una unión para permitir el acceso al mismo bloque de memoria, esta puede ser interpretada como un unsigned short o como un array de dos uint8_t. Luego declaramos la estructura que contiene a shortVal y un uint8_t. Definimos la estructura Estado cuyos campos son las representaciones de los distintos cambios. Estos son prototipos de funciones y un tipo de puntero a función (fcnState) que apunta a funciones con un formato específico. Desarrollamos los prototipos de funciones de cambio de estado. Indicamos cual va a ser el constructor y el destructor.

Ahora pasaremos a las implementaciones de las clases y por último el “main”.
Implementación “portcfg”:
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

Incluimos los encabezados y implementamos del constructor de la clase portCfg. Este toma un puntero a un objeto QWidget, inicializa la clase base QDialog y luego crea un objeto Ui::portCfg. Luego llamamos al método setupUi(this) para configurar la interfaz de usuario.        Por ultimo liberamos la memoria asociada con el objeto Ui::portCfg.

Implementación “VentanaPrincipal”:
#include "ventanaprincipal.h"
#include "ui_ventanaprincipal.h"
#include "portcfg.h"
#include <QSerialPortInfo>

// Constructor
ventanaPrincipal::ventanaPrincipal(QWidget *parent) : QDialog(parent), ui(new Ui::ventanaPrincipal) {
    ui->setupUi(this);

    // Configuración del puerto serie
    puertoSerie = new QSerialPort();
    puertoSerie->setBaudRate(QSerialPort::Baud115200);
    puertoSerie->setDataBits(QSerialPort::Data8);
    puertoSerie->setFlowControl(QSerialPort::NoFlowControl);
    puertoSerie->setParity(QSerialPort::NoParity);
    puertoSerie->setStopBits(QSerialPort::TwoStop);

    // Configuración de la ventana y conexiones de señales
    this->setWindowTitle("Control (No Conectado)");
    connect(puertoSerie, SIGNAL(readyRead()), this, SLOT(datos_recibidos()));
    connect(ui->btnDeconectar, SIGNAL(clicked()), this, SLOT(on_btnDesconectar_clicked()));

    // Historial de comunicación serie
    ui->listWidget->addItem(new QListWidgetItem("HISTORIAL COMUNICACION SERIE:"));
}

// Destructor
ventanaPrincipal::~ventanaPrincipal() {
    // Desconectar antes de cerrar y eliminar el puerto serie
    disconnect(puertoSerie, SIGNAL(readyRead()), this, SLOT(datos_recibidos()));
    if (puertoSerie->isOpen())
        puertoSerie->close();
    delete puertoSerie;
    ui->listWidget->clear();
    delete ui;
}

Primero tenemos el constructor y destructor; en el constructor inicializamos la interfaz de usuario (UI), configuramos el puerto serie, establecemos las conexiones de señales y slots. Por otro lado, en el destructor desconectamos las señales, cerramos el puerto serie en el caso de estar abierto y liberamos la memoria asociada.

// Función para enviar datos
void ventanaPrincipal::sendData() {
    if (puertoSerie->isOpen()) {
        // Obtener valores de sliders
        uint16_t t_on = ui->tiempoEncendidoSlider->value();
        uint16_t t_off = ui->tiempoApagadoSlider->value();

        // Construir arreglo de bytes a enviar
        uint8_t toSend[6];
        toSend[5] = header_datos +
                     (t_on % 256) +
                     (t_on >> 8) +
                     (t_off % 256) +
                     (t_off >> 8);

        toSend[0] = header_datos;
        toSend[1] = t_on % 256;
        toSend[2] = t_on >> 8;
        toSend[3] = t_off % 256;
        toSend[4] = t_off >> 8;

        // Mostrar valores enviados en hexadecimal
        QString v = " ";
        for (unsigned int k = 0; k < sizeof(toSend); k++) {
            v = v + QString::number((unsigned)toSend[k], 16) + "  ";
            puertoSerie->write((const char*)&toSend[k], 1);
        }
        qDebug() << v;

        // Mostrar mensaje en la interfaz
        ui->listWidget->addItem("ENVIO : : Configuracion a aplicar: T.E= " + QString::number(t_on) +
                                  " [ms] ; T.A= " + QString::number(t_off) + " [ms]");
    } else {
        // Mostrar error si no está conectado
        ui->listWidget->addItem("ERROR : : No conectado");
    }
}
La función senData primero se encarga de verificar si el puerto serie está abierto antes de intentar enviar datos. Obtiene los valores actuales de “t_on” y  “t_off”. Creamos el arreglo toSend de longitud 6. Los últimos 5 bytes contienen información sobre el tiempo de encendido y apagado. Recorremos el arreglo, mostramos cada byte en formato hexadecimal en la consola y por medio de puertoSerie->write enviamos los bytes al puerto serie. Mostramos un mensaje que indique que la configuración se está enviando. En caso que el puerto serie este cerrado, se muestra un mensaje de error.
// Funciones llamadas al editar las etiquetas
void ventanaPrincipal::on_lineEditTiempoEncendido_editingFinished() {
    // Validar y actualizar el valor del slider.
    int tiempoEncendido = ui->lineEditTiempoEncendido->text().toInt();

     if (tiempoEncendido > 2047) {
         ui->lineEditTiempoEncendido->setText(QString::number(2047));
         ui->listWidget->addItem(new QListWidgetItem("ERROR!!! : : Rango de 0 a 2047 [ms]"));
     } else if (tiempoEncendido < 0) {
         ui->lineEditTiempoEncendido->setText(QString::number(0));
         ui->listWidget->addItem(new QListWidgetItem("ERROR!!! : : Rango de 0 a 2047 [ms]"));
     }
     ui->tiempoEncendidoSlider->setValue(tiempoEncendido);
     // Enviar datos al editar la etiqueta
     sendData();
}

Obtenemos el valor actual de la etiqueta lineEditTiempoEncendido y lo convertimos a un entero. Verificamos si el valor del tiempo de encendido está dentro del rango permitido (0 a 2047 ms). En el caso de estar excedido, se ajusta el valor y se muestra un mensaje de error. Si es menor que cero, se realiza el mismo procedimiento. Establecemos el valor del slider con el valor valido. Se llama a la función sendData para enviar los datos actualizados.

void ventanaPrincipal::on_lineEditTiempoApagado_editingFinished(){
    // Validar y actualizar el valor del slider.
    int tiempoApagado = ui->lineEditTiempoEncendido->text().toInt();

     if (tiempoApagado > 2047) {
         ui->lineEditTiempoApagado->setText(QString::number(2047));
         ui->listWidget->addItem(new QListWidgetItem("ERROR!!! : : Rango de 0 a 2047 [ms]"));
     } else if (tiempoApagado < 0) {
         ui->labelTiempoApagado->setText(QString::number(0));
         ui->listWidget->addItem(new QListWidgetItem("ERROR!!! : : Rango de 0 a 2047 [ms]"));
     }
     ui->tiempoEncendidoSlider->setValue(tiempoApagado);
     // Enviar datos al editar la etiqueta
     sendData();
}

Acá obtenemos el valor actual de la etiqueta lineEditTiempoEncendido y lo convierte a un entero. Verificamos si el valor del tiempo de apagado está dentro del rango permitido (0 a 2047 ms). En el caso de estar excedido, se ajusta el valor y se muestra un mensaje de error. Si es menor que cero, se realiza el mismo procedimiento.                                                                 Establecemos el valor del slider (tiempoEncendidoSlider) con el valor valido. Se llama a la función sendData para enviar los datos actualizados. 
// Función llamada al hacer clic en el botón de conectar
void ventanaPrincipal::on_btnConectar_clicked() {
    // Crear ventana de configuración de puerto
    portCfg cfg(this);

    // Cerrar el puerto si ya está abierto
    if (puertoSerie->isOpen()) {
        puertoSerie->close();
    }

    // Obtener lista de puertos disponibles y agregar al ComboBox
    QList<QSerialPortInfo> puertos = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo unPuerto, puertos ) {
        if (unPuerto.portName() != "")
            cfg.ui->cmbPuertos->addItem(unPuerto.portName());
    }

    // Abrir la ventana de configuración y conectar el puerto si se selecciona uno
    if(cfg.exec()) {
        puertoSerie->setPortName("\\\\.\\\\" + cfg.ui->cmbPuertos->currentText());
        puertoSerie->open(QIODevice::ReadWrite);
    }

    // Actualizar la interfaz según si se conecta o no
    if (puertoSerie->isOpen()) {
        this->setWindowTitle("Comunicacion Serie: (Conectado a: " + cfg.ui->cmbPuertos->currentText() + ")");
        ui->labEstado->setText("Conectado");
        ui->listWidget->addItem(new QListWidgetItem("CONN :: Conectado puerto --> " + cfg.ui->cmbPuertos->currentText()));
        ui->btnDeconectar->setEnabled(true);
    } else {
        ui->labEstado->setText("Desconectado");
        ui->listWidget->addItem(new QListWidgetItem("CONN :: No conectado"));
        this->setWindowTitle("Comserie (No Conectado)");
    }
}

Creamos la instancia de la clase portCfg, que es la ventana de configuración del puerto serie. Cerramos el puerto si está abierto. Se obtiene una lista de los puertos serie disponibles utilizando QSerialPortInfo. Cada puerto disponible se agrega al ComboBox en la ventana de configuración. Abrimos la ventana de configuración (cfg.exec()). Luego el usuario selecciona un puerto, acepta la configuración y se ejecuta el siguiente código. Se configura el puerto serie con el nombre del puerto seleccionado y se abre para lectura/escritura.
Actualizamos la interfaz: si el puerto serie está abierto, se actualiza la interfaz con información sobre la conexión exitosa. Se muestra el nombre del puerto al que se ha conectado en el título de la ventana, se actualiza el estado a "Conectado" en la interfaz y se habilita el botón de desconectar. 
En el caso de no poder abrir el puerto o si el usuario canceló la configuración, se actualiza la interfaz indicando que no está conectado.

// Función llamada al hacer clic en el botón de desconectar
void ventanaPrincipal::on_btnDesconectar_clicked() {
    // Desconectar el puerto y actualizar la interfaz


    if (puertoSerie->isOpen()) {
        puertoSerie->close();
        ui->labEstado->setText("Desconectado");
        this->setWindowTitle("Comserie (No Conectado)");
        ui->listWidget->addItem(new QListWidgetItem("CONN :: No conectado"));
    }
}

// Función llamada al hacer clic en el botón de limpiar historial
void ventanaPrincipal::on_btnLimpiar_clicked() {
    // Limpiar la lista de historial
    ui->listWidget->clear();
    ui->listWidget->addItem(new QListWidgetItem("HISTORIAL COMUNICACION SERIE:"));
}

Verificamos si el puerto serie está abierto. De ser el caso, se cierra el puerto y actualizamos el estado en la interfaz indicando "Desconectado". Se restaura el título de la ventana principal. Agrega un mensaje al historial indicando que no está conectado.
Luego borramos todos los elementos de la lista del historial de comunicación.

// Función llamada al recibir datos en el puerto serie
void ventanaPrincipal::datos_recibidos() {
    // Máquina de estados para interpretar los datos recibidos
    static fcnState listEstado[4] = {
       EstadoHeader,
       EstadoAlto,
       EstadoBajo,
       EstadoSumaComprobacion
    };
    static Estado vEstado = {stHeader, {{0},0}, {{0},0}, 0};
    char newData;

    // Leer datos mientras estén disponibles
    while (puertoSerie->bytesAvailable() > 0) {
        puertoSerie->read(static_cast<char *>(&newData), 1);
        // Procesar datos según la máquina de estados
        if (listEstado[vEstado.ActividadEstado](&vEstado, newData)) {
            // Mostrar respuesta en la interfaz
            ui->listWidget->addItem("RESPUESTA : : Configuracion Aplicada: T.E= " + QString::number(vEstado.ValorAlto.value.val) +
                                      " [ms] ; T.A= " + QString::number(vEstado.ValorBajo.value.val) + " [ms]");
        }
    }
}

Definimos un array de funciones de estado “listEstado” que representan los posibles estados de la máquina. Cada uno de estos estados es una función que toma un puntero a una estructura Estado y un nuevo dato como parámetros.
La estructura Estado contiene información sobre el estado actual de la máquina de estados.
Recorremos los datos disponibles en el puerto serie. Se lee un byte (newData) del puerto serie. El byte leído se procesa usando la función de estado correspondiente al estado actual de la máquina. Si la función devuelve true, se muestra una respuesta en la interfaz gráfica utilizando la función addItem de un listWidget.
// Funciones para manejar cambios en sliders y etiquetas
void ventanaPrincipal::on_tiempoEncendidoSlider_valueChanged(int value) {
    Q_UNUSED(value);
    ui->lineEditTiempoEncendido->setText(QString::number(ui->tiempoEncendidoSlider->value()));
}

void ventanaPrincipal::on_tiempoApagadoSlider_valueChanged(int value) {
    Q_UNUSED(value);
    ui->lineEditTiempoApagado->setText(QString::number(ui->tiempoApagadoSlider->value()));
}

void ventanaPrincipal::on_tiempoEncendidoSlider_sliderReleased() {
    // Enviar datos al soltar el slider
    sendData();
}

void ventanaPrincipal::on_tiempoApagadoSlider_sliderReleased() {
    // Enviar datos al soltar el slider
    sendData();
}

Llamamos a las funciones cuando cambia el valor de los sliders de tiempo de encendido y apagado, respectivamente. Estas funciones actualizan el texto en etiquetas de línea con el valor actual del slider correspondiente.
Llamamos a las funciones cuando se libera el slider de tiempo de encendido y apagado, respectivamente. Ambas funciones llaman a la función sendData para enviar datos, indicando que se ha soltado el slider.




La máquina de estados tiene cuatro estados representados por funciones diferentes: EstadoHeader, EstadoAlto, EstadoBajo, y EstadoSumaComprobacion.
La función “EstadoHeader” se encarga de identificar el inicio de un nuevo conjunto de datos cuando se recibe el valor 0x0C. Si se recibe otro valor, se vuelve al estado stHeader. Las funciones “EstadoAlto” y “EstadoBajo” se encargan de recibir y almacenar los bytes de datos en las partes alta y baja de un valor tipo short, respectivamente. La función “EstadoSumaComprobacion” verifica la suma de comprobación y devuelve true si es correcta. 
La estructura Estado contiene información sobre el estado actual de la máquina de estados. ActividadEstado indica el estado actual (ST_NivelAlto, ST_NivelBajo, stHeader, y stSuma son posibles valores).
Las estructuras ValorAlto y ValorBajo contienen un puntero a un arreglo de bytes (value.p) y un índice (inx) para almacenar los datos recibidos. Por ultimo “sum” se utiliza para calcular la suma de comprobación.

// Máquina de estados
bool EstadoHeader(Estado *s, unsigned char newVal) {
    if(newVal == 0x0C) {
        s->ActividadEstado = ST_NivelAlto;
        s->ValorAlto.inx = 0;
        s->ValorBajo.inx = 0;
        s->sum = 0x0C;
    } else {
        s->ActividadEstado = stHeader;
    }
    return false;
}

bool EstadoAlto(Estado *s, unsigned char newVal) {
    s->ValorAlto.value.p[s->ValorAlto.inx] = newVal;
    s->sum += newVal;
    s->ValorAlto.inx++;
    if(s->ValorAlto.inx == sizeof(short))
        s->ActividadEstado = ST_NivelBajo;
    return false;
}

bool EstadoBajo(Estado *s, unsigned char newVal) {
    s->ValorBajo.value.p[s->ValorBajo.inx] = newVal;
    s->sum += newVal;
    s->ValorBajo.inx++;
    if(s->ValorBajo.inx == sizeof(short))
        s->ActividadEstado = stSuma;
    return false;
}

bool EstadoSumaComprobacion(Estado *s, unsigned char newVal) {
    bool ret = false;
    if(newVal == (s->sum % 256))
        ret = true;
    s->ActividadEstado = stHeader;
    return ret;
}

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


Para finalizar en el “main” creamos un objeto de la clase QApplication pasándole como argumento de línea de comandos argc y argv. Creamos también un objeto de la clase ventanaPrincipal. Se imprime un mensaje de depuración en la consola indicando "TODO OK!!!". Por último, se muestra la ventana principal en la interfaz gráfica.
Esta es la Interfaz Grafica ni bien iniciamos, como podemos apreciar, cuenta con 6 puntos 
 
Cuando apretamos el botón Conectar, se nos abre un cuadro de dialogo, en donde podremos seleccionar cual es el Puerto que deseamos utilizar:
 
Si el puerto se conecto correctamente, se nos dará aviso de 3 formas diferentes:
Se nos avisara en el titulo de la ventana, en la descripción del estado, y también aparecerá una referencia en el Historial de la Comunicación serie.
 
Los valores del PWM, tano en Alto ( Tiempo encendido), como en Bajo (Tiempo Apagado), serán ingresado desplazando los Sliders 
El historial, nos marcara no solo el dato recibido, sino que mostrara la Respuesta del ESP32, de esta forma podemos comprobar la correcta recepción del dato.
 
Para desconectarnos, es tan sencillo como apretar el botón correspondiente, y se nos marcara el nuevo estado de la misma forma que cuando esta conectado.
 
Por ultimo, podremos borrar el historial desde el botón con el nombre indicado.
 
