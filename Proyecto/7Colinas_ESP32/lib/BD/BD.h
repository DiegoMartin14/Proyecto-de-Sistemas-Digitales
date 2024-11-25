#ifndef _BD_H
#define _BD_H

#include <NTPClient.h>
#include <Firebase_ESP_Client.h>
#include <Arduino.h>
#include <secrets.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <CTBot.h>
#include <BOT.h>

class BD
{
public:
    BD(int numTanque, char* nombreTanque, NTPClient& timeClient, FirebaseData& fbdo); // Constructor de la clase, setea variables y crea referencias a objetos de otras clases necesarias.
    void publicar(float temperatura, String fecha, String hora); // Para publicar solo necesitamos pasarle la temperatura, fecha y hora.
    int leerDocMin(String fecha, String hora); // Para leer los documentos solo necesitamos pasarle la fecha y hora.
    int leerDocMax(String fecha, String hora);
private:
    int _numTanque;
    const char* _nombreTanque;
    NTPClient& _timeClient;   // Referencia a la instancia de NTPClient
    FirebaseData& _fbdo;   // Referencia a la instancia de FirebaseData
    void sendTelegramNotification(String message, int tanque); // Metodo interno encargado de enviar los mensajes.
};

#endif