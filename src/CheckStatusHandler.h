#ifndef CHECK_STATUS_HANDLER_H
#define CHECK_STATUS_HANDLER_H

#include <constants/constants.h>
#include <TelegramMsgHandler.h>

class CheckStatusHandler
{

private:
    TelegramMsgHandler &telegramMsgHandler;
    float temperatura = 0.0;
    float ph = 0.0;
    bool flagTempErrorTelegram = false;
    bool flagPhErrorTelegram = false;

public:
    CheckStatusHandler(TelegramMsgHandler &telegramHandler)
        : telegramMsgHandler(telegramHandler) {}

    void checkSensorStatus(int sensorPin, int motorPin)
    {
        int sensorValue = digitalRead(sensorPin);
        Serial.println("Estado Sensor: " + String(sensorValue));
        while (sensorValue == LOW && !SYSTEM_RELEASED)
        {
            delay(1000);
            sensorValue = digitalRead(sensorPin);
            if (sensorValue == LOW)
            {
                Serial.println("Sensor Activado!");
                telegramMsgHandler.sendTelegramMessage("Sensor Activado!\nCerrar Trampa?\n/si");
                SENSOR_VALUE = true;
                telegramMsgHandler.processNewMessages();
                if (RELEASE)
                {
                    digitalWrite(motorPin, HIGH);
                    delay(1500);
                    digitalWrite(motorPin, LOW);
                    SYSTEM_RELEASED = true;
                    SENSOR_VALUE = false;
                    telegramMsgHandler.sendTelegramMessage("Trampa Liberada!");
                }
            }
            else
            {
                SENSOR_VALUE = false;
                Serial.println("Sensor Normal\nFalsa alarma");
                telegramMsgHandler.sendTelegramMessage("Sensor Normal\nFalsa alarma");
            }
        }
    }

    void calibrateMotor(int motorPin, int times)
    {
        if (times != 0)
        {
            for (int i = 0; i < times; i++)
            {
                digitalWrite(motorPin, HIGH);
                delay(25);
                digitalWrite(motorPin, LOW);
                delay(500);
            }
            TIMES = 0;
        }
    }
};
#endif