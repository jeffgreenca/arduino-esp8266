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
const int PWM_PIN_B = 4;   // D2
const int PIN_STATIC = 13; // D7, not pwm I think?

void setup()
{
    pinMode(PIN_STATIC, OUTPUT);
    analogWrite(PIN_STATIC, 0);

    pinMode(PWM_PIN_A, OUTPUT);
    pinMode(PWM_PIN_B, OUTPUT);

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

void setPWM(int pin, int inputValue)
{
    int val = (inputValue - 48) * 113;

    if (val < 0 || val > 1023)
    {
        Serial.println("value out of range for pwm");
        return;
    }
    Serial.print("pwm pin ");
    Serial.print(pin);
    Serial.print(" to: ");
    Serial.println(val);
    analogWrite(pin, val);
}

void echo()
{
    int recv = Udp.parsePacket();
    if (recv)
    {
        // read up to bufSize bytes into buffer and return length read
        int size = Udp.read(buf, bufSize);
        Serial.print("got ");
        Serial.print(size);
        Serial.println(" bytes");

        if (size == 3)
        {
            setPWM(PWM_PIN_A, (int)buf[0]);
            setPWM(PWM_PIN_B, (int)buf[1]);
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