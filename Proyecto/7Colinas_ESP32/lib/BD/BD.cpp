#include <BD.h>

///// METODO CONSTRUCTOR DE LA CLASE //////
BD::BD(int numTanque, char *nombreTanque, NTPClient &timeClient, FirebaseData &fbdo)
    : _timeClient(timeClient), _fbdo(fbdo) // Inicialización de referencias
{
    _numTanque = numTanque;
    _nombreTanque = nombreTanque;
}

////// FUNCION ENCARGADA DE PUBLICAR EN LA BASE DE DATOS //////
void BD::publicar(float temperatura, String fecha, String hora)
{
    if (Firebase.ready())
    {
        _timeClient.update();
        String documentId = String(_timeClient.getEpochTime());
        String documentPath = "Produccion/" + documentId;
        float temperaturaRedondeada = roundf(temperatura * 100) / 100.0;

        FirebaseJson json;
        json.set("fields/temperatura/stringValue", String(temperaturaRedondeada));
        json.set("fields/Tanque/integerValue", _numTanque);
        json.set("fields/Fecha/stringValue", fecha);
        json.set("fields/Hora/stringValue", hora);

        if (Firebase.Firestore.createDocument(&_fbdo, FIREBASE_PROYECTO_ID, "", documentPath.c_str(), json.raw()))
        {
            Serial.printf("Documento creado: %s\n", documentPath.c_str());
        }
        else
        {
            Serial.println("Error al crear documento: " + String(_fbdo.errorReason()));
        }
    }
}

////// FUNCION ENCARGADA DE OBTENER LA TEMPERATURA MINIMA SETEADA DESDE LA PAGINA WEB ///////
int BD::leerDocMin(String fecha, String hora)
{   
    String nombreTanque = _nombreTanque;

    String docId = "Configuracion/"; // Iniciar con "Configuracion/"
    docId += nombreTanque;           // Concatenar el nombre del tanque

    // Convertir el String a const char* antes de pasar a la función getDocument
    const char *docIdCStr = docId.c_str(); // Convertir a const char*

    if (Firebase.Firestore.getDocument(&_fbdo, FIREBASE_PROYECTO_ID, "", docIdCStr, "", "", ""))
    {
        FirebaseJson &json = _fbdo.jsonObject();
        FirebaseJsonData jsonData;

        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, _fbdo.payload());

        if (!error)
        {
            // Leer TempMin
            if (doc["fields"]["TempMin"]["stringValue"])
            {
                int tempMix = doc["fields"]["TempMin"]["stringValue"].as<int>();
                return tempMix;
            }
            else
            {
                Serial.println("Error al obtener TempMin.stringValue");
            }
        }
        else
        {
            Serial.print("Error al deserializar JSON: ");
            Serial.println(error.c_str());
        }
    }
    else
    {
        Serial.println("Error al leer el documento: " + String(_fbdo.errorReason()));
    }
}

////// FUNCION ENCARGADA DE OBTENER LA TEMPERATURA MINIMA SETEADA DESDE LA PAGINA WEB ///////
int BD::leerDocMax(String fecha, String hora)
{
    String nombreTanque = _nombreTanque;

    String docId = "Configuracion/"; // Iniciar con "Configuracion/"
    docId += nombreTanque;           // Concatenar el nombre del tanque

    // Convertir el String a const char* antes de pasar a la función getDocument
    const char *docIdCStr = docId.c_str(); // Convertir a const char*

    if (Firebase.Firestore.getDocument(&_fbdo, FIREBASE_PROYECTO_ID, "", docIdCStr, "", "", ""))
    {
        FirebaseJson &json = _fbdo.jsonObject();
        FirebaseJsonData jsonData;

        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, _fbdo.payload());

        if (!error)
        {
            // Leer TempMax
            if (doc["fields"]["TempMax"]["stringValue"])
            {
                int tempMax = doc["fields"]["TempMax"]["stringValue"].as<int>();
                return tempMax;
            }
            else
            {
                Serial.println("Error al obtener TempMax.stringValue");
            }
        }
        else
        {
            Serial.print("Error al deserializar JSON: ");
            Serial.println(error.c_str());
        }
    }
    else
    {
        Serial.println("Error al leer el documento: " + String(_fbdo.errorReason()));
    }
}
