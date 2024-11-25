#ifndef _BOT_H
#define _BOT_H

#include <secrets.h>
#include <CTBot.h>

class BOT
{
public:
    BOT(CTBot &miBot, CTBotInlineKeyboard &botonesTelegram, TBMessage &msg); // Constructor de la clase, crea referencias a objetos de otras clases necesarias.
    void enviarTelegram(uint64_t id, char *mensaje); // Para enviar mensaje solo hay que pasarle el id del usuario y el mensaje.
    void configurarTelegram(char *nombreBoton); // Para configurar solo hay que pasarle el nombre del boton que queremos agregar, si no necesitamos boton, le mandamos un "0"
    IRAM_ATTR bool manejoBoton(); // La funcion devuelve un "verdadero", si pulsamos el boton.

private:
    bool adminNotif = false;
    char *_mensajeEnviar;
    CTBot &_miBot; // Referencia a la instancia de CTBot
    CTBotInlineKeyboard &_botonesTelegram; // Referencia a la instancia de CTBotInlineKeyboard
    TBMessage &_msg; // Referencia a la instancia de TBMessage
};

#endif