#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFi.h>
#include <constants/constants.h>

class WiFiHandler
{
private:
    const char *ssid;
    const char *password;

public:
    WiFiHandler(const char *wifiSSID, const char *wifiPassword)
        : ssid(wifiSSID), password(wifiPassword) {}

    void connectWiFi()
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            WiFi.begin(ssid, password);
            Serial.println("\nConectando a WiFi...");

            unsigned long startAttemptTime = millis();

            // Intentar conectar por 5 segundos
            int counter = 0;
            while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 5000)
            {
                Serial.print(".");
                delay(500);
                counter++;
            }
            IPAddress ip = WiFi.localIP();
            if (ip.toString() != "0.0.0.0")
            {
                Serial.print("\nWiFi connected. IP address: ");
                Serial.println(ip);
            }

            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.println("\nConexión exitosa!");
            }
            else
            {
                Serial.println("\nNo se pudo conectar a WiFi.");
            }
        }
    }

    bool isWiFiConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }
};

#endif