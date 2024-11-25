#include <WiFiManager.h>
#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DNSServer.h>
#include <ESP32Time.h>
#include <secrets.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Tanque.h>
#include <BD.h>
#include <BOT.h>
#include <CTBot.h>

////// LO QUE TIENE QUE VER CON TELEGRAM //////
CTBot miBot;
CTBotInlineKeyboard botonesTelegram;
TBMessage msg;

////// LO QUE TIENE QUE VER CON SERVER NTP (FECHA/HORA) //////
ESP32Time rtc;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", -10800, 60000); // Configuración del cliente NTP (congifuro zona horaria UTC-3 y sincronizo cada 1 min el horario)

////// LO QUE TIENE QUE VER CON FIREBASE //////
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

////// OBJETOS DE NUESTRAS CLASES //////
//Con esta clase (Tanque), hacemos la lectura de temperaturas de cada uno de los tanques.
Tanque tanques; 

//Con esta clase (BOT), hacemos todo lo que tiene que ver con las notificaciones a Telegram.
BOT mensajeTelegram(miBot, botonesTelegram, msg); 

//Con esta clase (BD), hacemos el manejo de la base de datos (lecturas y escrituras).
BD publicacionT1(1, "Tanque1", timeClient, fbdo);
BD publicacionT2(2, "Tanque2", timeClient, fbdo);
BD publicacionT3(3, "Tanque3", timeClient, fbdo);
BD publicacionT4(4, "Tanque4", timeClient, fbdo);
BD publicacionT5(5, "Tanque5", timeClient, fbdo);
BD publicacionT6(6, "Tanque6", timeClient, fbdo);
BD *publicaciones[] = {&publicacionT1, &publicacionT2, &publicacionT3, &publicacionT4, &publicacionT5, &publicacionT6}; 

////// DEFINICION DE VARIABLES Y CONSTANTES //////
// Generales:
unsigned long previousMillisRetardo = 0;
const unsigned int intervalRetardo = 1000;

// Base de datos:
int publicacionActual = 0;
unsigned long previousMillisPublicar = 0;
const unsigned int intervalPublicar = 180000; // Intervalo para publicar (30min) / (AHORA ESTA PUBLICANDO CADA 2 MIN)
int tempSetMin[6];
int tempSetMax[6];

// Telegram:
bool administrarNotificaciones = false;
char mensaje[80];
char nombreTanque[10];

// Lectura de temperatura:
unsigned long previousMillisLeer = 0;
const unsigned int intervalLeer = 120000; // Intervalo para leer (10min) / (AHORA ESTA LEYENDO CADA 3 SEG)
float tempTanque[6];
uint8_t adcPins[] = {34, 35, 32, 33, 39, 36}; // Pines GPIO asociados al ADC1

////// FUNCIONES //////
void controlTemperatura(float tempTanque, float tempSetMin, float temp, int o);


void setup()
{
    Serial.begin(115200);

    ////// CONFIGURACION DEL WIFI //////
    WiFiManager wm;
    bool res = wm.autoConnect("Microcontrolador");
    if (!res)
    {
        Serial.println("Falló la conexión a WiFi");
        ESP.restart();
    }
    else
    {
        Serial.println("Conectado a WiFi :)");
    }

    ////// CONFIGURACION PARA EL HORARIO //////
    timeClient.begin();
    configTime(-10800, 0, "europe.pool.ntp.org");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        rtc.setTimeStruct(timeinfo);
    }

    ////// CONFIGURACION DE FIRABASE //////
    config.api_key = API_KEY;
    auth.user.email = USUARIO_EMAIL;
    auth.user.password = USUARIO_CONTRA;
    config.database_url = DATABASE_URL;
    Firebase.reconnectWiFi(true);
    Firebase.begin(&config, &auth);

    ////// CONFIGURACION DEL ADC //////
    analogSetAttenuation(ADC_11db);
    analogReadResolution(10);

    ////// CONFIGURACION DEL BOT DE TELEGRAM //////
    mensajeTelegram.configurarTelegram("Estoy en conocimiento");
}

void loop()
{
    // Actualizar el cliente NTP
    timeClient.update();

    /////// ALGORITMO PARA ADMINISTRAR NOTIFICACIONES DE TELEGRAM ///////
    if (mensajeTelegram.manejoBoton() == true)
    {
        administrarNotificaciones = true;
    }

    ///////  ALGORITMO PARA EL MANEJO DE LECTURAS, PUBLICACIONES Y NOTIFICACIONES.  ///////

    if (millis() - previousMillisRetardo >= intervalRetardo)
    {

        if (millis() - previousMillisLeer >= intervalLeer)
        {
            Serial.println("Estoy leyendo");
            for (int i = 0; i < 6; i++)
            {
                tempTanque[i] = tanques.leerTemperatura(adcPins[i]);

                tempSetMin[i] = publicaciones[i]->leerDocMin(rtc.getDate(), timeClient.getFormattedTime());
                tempSetMax[i] = publicaciones[i]->leerDocMax(rtc.getDate(), timeClient.getFormattedTime());

                if (administrarNotificaciones != true)
                {
                    controlTemperatura(tempTanque[i], tempSetMin[i], tempSetMax[i], i);
                }
            }
            previousMillisLeer = millis();
        }

        if (millis() - previousMillisPublicar >= intervalPublicar)
        {
            publicaciones[publicacionActual]->publicar(tempTanque[publicacionActual], rtc.getDate(), timeClient.getFormattedTime());
            if (administrarNotificaciones == true)
            {
                controlTemperatura(tempTanque[publicacionActual], tempSetMin[publicacionActual], tempSetMax[publicacionActual], publicacionActual);
            }
            publicacionActual++;

            if (publicacionActual >= (sizeof(publicaciones) / sizeof(*publicaciones)))
            {
                administrarNotificaciones = false;
                publicacionActual = 0;
                previousMillisPublicar = millis();
                previousMillisLeer = millis();
            }
        }

        previousMillisRetardo = millis();
    }

}

///// FUNCION PARA EL MANEJO DE NOTIFICACIONES POR TEMPERATURAS FUERA DE RANGO Y PUBLICACIONES CRITICAS /////
void controlTemperatura(float temp, float tempMin, float tempMax, int o)
{
    if (temp < tempMin)
    {
        sprintf(nombreTanque, "Tanque %d", o + 1);
        sprintf(mensaje, "La temperatura del %s se encuentra por debajo del rango deseado, revisar cuanto antes.", nombreTanque);
        mensajeTelegram.enviarTelegram(CHAT_ID, mensaje);
        publicaciones[o]->publicar(tempTanque[o], rtc.getDate(), timeClient.getFormattedTime());
    }
    if (temp > tempMax)
    {
        sprintf(nombreTanque, "Tanque %d", o + 1);
        sprintf(mensaje, "La temperatura del %s se encuentra por encima del rango deseado, revisar cuanto antes.", nombreTanque);
        mensajeTelegram.enviarTelegram(CHAT_ID, mensaje);
        publicaciones[o]->publicar(tempTanque[o], rtc.getDate(), timeClient.getFormattedTime());
    }
}