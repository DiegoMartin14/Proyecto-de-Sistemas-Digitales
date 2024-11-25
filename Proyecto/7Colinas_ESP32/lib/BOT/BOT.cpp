#include <BOT.h>

///// METODO CONSTRUCTOR DE LA CLASE //////
BOT::BOT(CTBot &miBot, CTBotInlineKeyboard &botonesTelegram, TBMessage &msg)
    : _miBot(miBot), _botonesTelegram(botonesTelegram), _msg(msg) // Inicialización de referencias
{
}

///// METODO ENCARGADO DE CONFIGURAR EL BOT //////
void BOT::configurarTelegram(char *nombreBoton)
{
    _miBot.setTelegramToken(TOKEN_BOT);
    if (_miBot.testConnection())
    {
        Serial.println("Bot conectado");
    }
    else
    {
        Serial.println("Bot no conectado");
    }
    if (nombreBoton != 0)
    {
        _botonesTelegram.addButton(nombreBoton, "1", CTBotKeyboardButtonQuery);
    }
}

///// METODO ENCARGADO DE ENVIAR MENSAJES //////
void BOT::enviarTelegram(uint64_t id, char *mensaje)
{
    _miBot.sendMessage(id, mensaje, _botonesTelegram);
}

///// METODO ENCARGADO DE CONTROLAR EL BOTON //////
IRAM_ATTR bool BOT::manejoBoton()
{
    adminNotif = false;
    if (_miBot.getNewMessage(_msg))
    {
        if (_msg.messageType == CTBotMessageQuery)
        {
            if (_msg.callbackQueryData.equals("1"))
            {
                Serial.println("Se administran notificaciones.");
                adminNotif = true;
                _miBot.endQuery(_msg.callbackQueryID, "Se administraran las notificaciones.");
            }
        }
    }
    return adminNotif;
}
