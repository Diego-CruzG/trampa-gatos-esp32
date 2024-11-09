#include <Arduino.h>
#include <TelegramMsgHandler.h>
#include <WifiHandler.h>
#include <TelegramMsgHandler.h>
#include <CheckStatusHandler.h>
#include <constants/constants.h>

const int SENSOR_PIN = 15;
const int MOTOR_PIN = 13; // LED 2
// Definir tus credenciales Wi-Fi
const char *ssid = SSID;
const char *password = PASSWORD;
unsigned long previousMillis = 0;
const long interval = 10000;

const int LED_PIN1 = 2; // LED1

bool flagCalibration = false;

WiFiHandler wifiHandler(ssid, password);
TelegramMsgHandler telegramMsgHandler;
CheckStatusHandler statusHandler(telegramMsgHandler);

void setup()
{
  SENSOR_PIN_COPY = SENSOR_PIN;
  pinMode(LED_PIN1, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  // Inicia el pin del sensor como entrada.
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(115200);
  wifiHandler.connectWiFi();
}

void loop()
{
  unsigned long currentMillis = millis();

  if (!wifiHandler.isWiFiConnected())
  {
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      Serial.println("Intentando reconectar a WiFi...");
      wifiHandler.connectWiFi();
    }
  }

  while (wifiHandler.isWiFiConnected())
  {
    // activa la trampa
    while (!TRAMPA_ACTIVADA)
    {
      telegramMsgHandler.sendTelegramMessage("Sistema en linea.\nPor favor ingrese el numero de de veces que el motor girara para calibrar su posicion.");
      telegramMsgHandler.sendTelegramMessage("Cuando este calibrado envie:\n/calibrado");

      // calibra la trampa
      do
      {
        Serial.println("Calibrando trampa");
        telegramMsgHandler.processNewMessages();
        statusHandler.calibrateMotor(MOTOR_PIN, TIMES);
      } while (!SYSTEM_READY);
      TRAMPA_ACTIVADA = true;
    }

    statusHandler.checkSensorStatus(SENSOR_PIN, MOTOR_PIN);
    telegramMsgHandler.processNewMessages();
  }

  digitalWrite(LED_PIN1, HIGH);
  delay(250);
  digitalWrite(LED_PIN1, LOW);
  delay(250);
}