#ifndef TELEGRAM_MSG_HANDLER_H
#define TELEGRAM_MSG_HANDLER_H

#include <Arduino.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <constants/constants.h>
#include <iostream>
#include <string>

class TelegramMsgHandler
{
private:
    WiFiClientSecure client;
    UniversalTelegramBot *bot;                                               // Usa puntero para inicializar más tarde
    const char *botToken = "7717112465:AAFcvwfl4SpyarNjD2anlm3xv5i8HTTl08U"; // Token del bot de Telegram
    const char *chat_id = "5360688446";                                      // Chat ID de Telegram

    void sincronizeTime()
    {
        if (!bot)
        {
            // Sincroniza tiempo y configura cliente antes de inicializar el bot
            client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
            Serial.print("Retrieving time: ");
            configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
            time_t now = time(nullptr);
            while (now < 24 * 3600)
            {
                Serial.print(".");
                delay(100);
                now = time(nullptr);
            }
            Serial.println(now);
            bot = new UniversalTelegramBot(botToken, client);
        }
    }

    bool stringInArray(String valor, String arreglo[], int tamano)
    {
        for (int i = 0; i < tamano; i++)
        {
            if (arreglo[i] == valor)
            {
                return true; // El valor fue encontrado
            }
        }
        return false; // El valor no fue encontrado
    }

public:
    TelegramMsgHandler()
        : bot(nullptr) // Inicializa puntero a bot
    {
    }

    void sendTelegramMessage(const char *message)
    {
        Serial.println("Enviando Mensaje a Telegram.");
        sincronizeTime();
        if (bot->sendMessage(chat_id, message, ""))
        {
            Serial.println("Mensaje Enviado");
        }
        else
        {
            Serial.println("Error Enviando Mensaje!");
        }
    }

    void processNewMessages()
    {
        sincronizeTime();

        // Verifica si hay nuevos mensajes desde la última actualización
        int numNewMessages = bot->getUpdates(bot->last_message_received + 1);

        while (numNewMessages)
        {
            Serial.println("Mensaje nuevo recibido!");
            for (int i = 0; i < numNewMessages; i++)
            {
                String chatId = bot->messages[i].chat_id;
                String text = bot->messages[i].text;
                String fromName = bot->messages[i].from_name;

                if (fromName == "")
                {
                    fromName = "Usuario desconocido";
                }

                // Mostrar el mensaje
                String de = "De: " + fromName;
                String msj = "Mensaje: " + text;
                Serial.println(de + "\n" + msj);

                // Puedes responder automáticamente si lo deseas
                bot->sendMessage(chatId, "Procesando mensaje: " + text, "");

                std::string s = text.c_str();

                if (esEntero(s))
                {
                    if (!TRAMPA_ACTIVADA)
                    {
                        int times = text.toInt();
                        TIMES = times;
                    }
                    else
                    {
                        bot->sendMessage(chatId, "La trampa ya esta armada\nPara reiniciar envie\n/reiniciar", "");
                    }
                }
                else
                {

                    if (stringInArray(text, COMMNADS, 4))
                    {
                        if (text.equals("/status"))
                        {
                            int sensorValue = digitalRead(SENSOR_PIN_COPY);
                            if (sensorValue == LOW) {
                                bot->sendMessage(chatId, "Sensor activado", "");
                            }
                            else{
                                bot->sendMessage(chatId, "Sensor NO activado", "");
                            }
                        }
                        else if (text.equals("/calibrado"))
                        {
                            if (!TRAMPA_ACTIVADA)
                            {
                                SYSTEM_READY = true;
                                bot->sendMessage(chatId, "Sistema Armado.", "");
                            }
                            else
                            {
                                bot->sendMessage(chatId, "La trampa ya esta armada\nPara reiniciar envie\n/reiniciar", "");
                            }
                        }
                        else if (text.equals("/si"))
                        {
                            if (!SYSTEM_RELEASED && SENSOR_VALUE)
                            {
                                RELEASE = true;
                            }
                            else if (SYSTEM_RELEASED)
                            {
                                bot->sendMessage(chatId, "La trampa ya se disparo!", "");
                            }
                            else if (!SYSTEM_RELEASED && !SENSOR_VALUE) 
                            {
                                bot->sendMessage(chatId, "No se puede liberar la trampa\nEl sensor no ha detectado movimiento", "");
                            }
                        }
                        else if (text.equals("/reiniciar"))
                        {
                            bot->sendMessage(chatId, "Reiniciando Trampa...", "");
                            numNewMessages = bot->getUpdates(bot->last_message_received + 1);
                            delay(2000);
                            esp_restart();
                        }
                    }
                    else
                    {
                        bot->sendMessage(chatId, "Error!\nComando: " + text + " no existe.", "");
                    }
                }
            }

            // Actualiza los mensajes entrantes
            numNewMessages = bot->getUpdates(bot->last_message_received + 1);
        }
    }

    bool checkForNewMessages()
    {
        sincronizeTime();

        // Verifica si hay nuevos mensajes desde la última actualización
        int numNewMessages = bot->getUpdates(bot->last_message_received + 1);

        return numNewMessages != 0;
    }

    bool esEntero(const std::string &str)
    {
        try
        {
            std::size_t pos;
            std::stoi(str, &pos);
            // Verificamos que toda la cadena fue procesada
            return pos == str.size();
        }
        catch (std::invalid_argument &e)
        {
            return false;
        }
        catch (std::out_of_range &e)
        {
            return false;
        }
    }

    ~TelegramMsgHandler()
    {
        if (bot)
        {
            delete bot; // Libera memoria del bot al destruir el objeto
        }
    }
};
#endif