#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "secrets.h"

WiFiUDP Udp;
unsigned int localUdpPort = 1011;
const int bufSize = 255;
char buf[bufSize];
char out[] = "";

// secrets.h contains #define statements for these values
const char *ssid = MY_WIFI_NETWORK_NAME;
const char *password = MY_WIFI_NETWORK_PASS;

const int PWM_PIN_A = 12;  // D6

void setup()
{
    pinMode(PWM_PIN_A, OUTPUT);

    Serial.begin(115200);
    Serial.println();

    WiFi.begin(ssid, password);

    ensureConnected();

    Udp.begin(localUdpPort);
}

void loop()
{
    ensureConnected();
    checkInput();
    // digitalWrite(PWM_PIN_A, HIGH);
    // delay(80);
    // digitalWrite(PWM_PIN_A, LOW);
    // delay(10000);
}

void triggerRelay(int d) {
    digitalWrite(PWM_PIN_A, HIGH);
    delay(d);
    digitalWrite(PWM_PIN_A, LOW);
}

void checkInput()
{
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        // read up to bufSize bytes into buffer and return length read
        int size = Udp.read(buf, bufSize);
        Serial.print("got ");
        Serial.print(size);
        Serial.println(" bytes");

        if (size == 2)
        {
            switch (buf[0]) {
                case '0':
                triggerRelay(20);
                break;
                case '1':
                triggerRelay(40);
                break;
                case '2':
                triggerRelay(80);
                break;
                case '3':
                triggerRelay(160);
                break;
                case '4':
                triggerRelay(320);
                break;
            }
            buf[0] = 'O';
            buf[1] = 'K';
            buf[2] = '\n';
            size = 3;
        }
        // echo back to client, or something
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