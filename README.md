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
