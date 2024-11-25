#include <Tanque.h>

Tanque::Tanque()
{
}

////// METODO ENCARGADO DE LEER EL VALOR DEL ADC CORESPONDIENTE AL PIN RECIBIDO //////
float Tanque::leerTemperatura(int numPinADC)
{
    valorADC = random(876, 935); // IMPORTANTE, cambiar "random(876, 935);" por "analogRead(_numPinADC);" para leer los distintos canales del ADC
    return convertirRango(valorADC);
}

float Tanque::convertirRango(int valor_ADC)
{
    float m = (25.0 - (-10.0)) / 1023.0;
    float b = 25.0 - m * 1023.0;
    return m * valor_ADC + b;
}