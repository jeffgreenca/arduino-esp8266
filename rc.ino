#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "secrets.h"

WiFiUDP Udp;
unsigned int localUdpPort = 1011;
const int bufSize = 255;
char buf[bufSize];
char out[] = "";

const char* ssid = MY_WIFI_NETWORK_NAME;
const char* password = MY_WIFI_NETWORK_PASS;

void setup()
{
    Serial.begin(115200);
    Serial.println();

    WiFi.begin(ssid, password);

    ensureConnected();

    Udp.begin(localUdpPort);
}

void loop()
{
    ensureConnected();

    echo();
}

void echo() {
    int recv = Udp.parsePacket();
    if (recv) {
        // read up to bufSize bytes into buffer and return length read
        int size = Udp.read(buf, bufSize);
        // echo back to client
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(buf, size);
        Udp.endPacket();
    }
}

void ensureConnected()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }
    Serial.print("reconnecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("OK");
    Serial.print("Host ");
    Serial.println(WiFi.hostname());
    Serial.print("IP ");
    Serial.println(WiFi.localIP());
}