#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include<stdlib.h>
#include<stdio.h>
const char *ssid = "Esp8266";
const char *password = "12345678";
unsigned int localPort = 4210; 
WiFiUDP Udp;
void setup() 
{
    delay(1000);
    Serial.begin(9600);
    WiFi.softAP(ssid, password);
    Udp.begin(localPort);
}
void Send(char ch[],unsigned long previousTime)
{
  unsigned long ttt = millis();
Udp.beginPacket("192.168.4.2", localPort);
   Udp.write(ch);
Udp.endPacket();
Udp.beginPacket("192.168.4.3", localPort);
   Udp.write(ch);
Udp.endPacket();
Udp.beginPacket("192.168.4.4", localPort);
   Udp.write(ch);
Udp.endPacket();
Udp.beginPacket("192.168.4.5", localPort);
   Udp.write(ch);
Udp.endPacket();
   Serial.println(millis() - ttt);
   Serial.println(millis() - previousTime);
}
unsigned long previousTime = millis();
int rssi = 123456789;
char chh[13];
void loop() 
{

  while(millis() - previousTime >= 950)
  {
    rssi = rssi - 1;
    itoa(rssi,chh,10);
    Send(chh,previousTime);
    previousTime = millis();
  }
}