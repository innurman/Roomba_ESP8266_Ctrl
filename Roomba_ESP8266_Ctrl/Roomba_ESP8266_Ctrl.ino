#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "Gandalf";
const char* password = "simonecleomartina";

ESP8266WebServer server(80);

#include "roombaDefines.h"
#include <SoftwareSerial.h>

// Roomba Create2 connection
int rxPin = 12;
int txPin = 14;
SoftwareSerial Roomba(rxPin, txPin);


//---------------------------------------------
void setup()
{
  Roomba.begin(19200);
  Serial.begin(19200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  pinMode(ddPin, OUTPUT);
  pinMode(ledPin, OUTPUT); //spare if needed for test purpose

  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  Serial.print("Roomba Remote Control");

  server.on("/", handleRoot);

  server.on("/wakeup", []() {
    wakeUp();
    server.send(200, "text/plain", "WakeUp Send!");
  });

  server.on("/start", []() {
    startSafe();
    server.send(200, "text/plain", "StartSafe Send!");
  });

  server.on("/stop", []() {
    Roomba.write(137);
    server.send(200, "text/plain", "Stop Send!");
  });

  server.on("/seekdock", []() {
    seekDock();
    server.send(200, "text/plain", "seekDock Send!");
  });

  server.on("/clean", []() {
    Clean();
    server.send(200, "text/plain", "Clean Send!");
  });

  server.on("/off", []() {
    powerOff();
    server.send(200, "text/plain", "Off Send!");
  });

  server.on("/cleanDisplay", []() {
    cleanDigitLED();
    server.send(200, "text/plain", "Display Clear Send!");
  });

  server.begin();
}

void loop()
{
  server.handleClient();
  manualCmd();
}

void handleRoot() {

  if (server.args() > 0)
    for (uint8_t i = 0; i < server.args(); i++) {
      if (server.argName(i) == "display")
      {
        String d = server.arg(i);
        writeLEDs(d[0], d[1], d[2], d[3]);
        server.send(200, "text/plain", "Text Sended!\nMessage: " + server.arg(i));
      } else if (server.argName(i) == "send")
      {
        int c = server.arg(i).toInt();
        Roomba.write(c);
        server.send(200, "text/plain", "Code sended!\nMessage: " + server.arg(i));
      }
      else if (server.argName(i) == "cmd")
      {
        command = server.arg(i)[0];
        server.send(200, "text/plain", "Command sended!\nMessage: " + server.arg(i));
      }
    }
  else
    server.send(200, "text/plain", "Hello from Roomba!");
}

