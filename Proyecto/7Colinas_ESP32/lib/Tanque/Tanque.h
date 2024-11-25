#ifndef _TANQUE_H
#define _TANQUE_H

#include <Arduino.h>

class Tanque
{
public:
    Tanque(); // Constructor de la clase.
    float leerTemperatura(int numPinADC); // Para leer la temperatura solo hay que pasarle el numero del pin ADC.

private:
    float convertirRango(int valor); // Metodo interno de la clase, que es usado para pasar el valor del ADC a temperatura en °C.
    int valorADC;
};

#endif
