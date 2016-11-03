#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#define trackLeftOne 5
#define trackLeftTwo 4
#define trackRightOne 0
#define trackRightTwo 2

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch (type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      //USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
      USE_SERIAL.println((const char *) &payload[0]);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject((const char *) &payload[0]);
      
      if (!root.success()) {
        Serial.println("parseObject() failed");
        return;
      }
      int leftTrack = root["leftTrack"];
      int rightTrack = root["rightTrack"];
      tracks(leftTrack, rightTrack);
      break;
  }

}

void tracks(int leftTrackSpeed, int rightTrackSpeed) {
  // neg number is backwards and pos forewards
  if(leftTrackSpeed >= 0) {
    analogWrite(trackLeftOne, leftTrackSpeed);
    analogWrite(trackLeftTwo, 0);
  } else if(leftTrackSpeed <= 0) {
    analogWrite(trackLeftOne, 0);
    analogWrite(trackLeftTwo, leftTrackSpeed * -1);
  }
  if(rightTrackSpeed >= 0) {
    analogWrite(trackRightOne, rightTrackSpeed);
    analogWrite(trackRightTwo, 0);
  } else if(rightTrackSpeed <= 0) {
    analogWrite(trackRightOne, 0);
    analogWrite(trackRightTwo, rightTrackSpeed * -1);
  }
}

void setup() {
  USE_SERIAL.begin(115200);
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP("Empire", "5038038883");

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  if (MDNS.begin("esp8266")) {
    USE_SERIAL.println("MDNS responder started");
  }

  // handle index
  server.on("/", []() {
    String currentMillis = String(millis());
    // send index.html
    server.send(200, "text/html", "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Tank</title><meta name='viewport' content='user-scalable=no, initial-scale=1, maximum-scale=1, minimum-scale=1, width=device-width'><link href='https://fonts.googleapis.com/css?family=Roboto' rel='stylesheet'><link rel='stylesheet' href='https://drifterz28.github.io/ESP-tank/assets/main.css?" + currentMillis + "'/><script src='https://unpkg.com/react@15.3.1/dist/react.js'></script><script src='https://unpkg.com/react-dom@15.3.1/dist/react-dom.js'></script><script src='https://unpkg.com/babel-core@5.8.38/browser.min.js'></script></head><body><div class='container'></div><script type='text/babel' src='https://drifterz28.github.io/ESP-tank/assets/main.js?" + currentMillis + "'></script></body></html>");
  });

  server.begin();

  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
  analogWrite(trackLeftOne, 0);
  analogWrite(trackLeftTwo, 0);
  analogWrite(trackRightOne, 0);
  analogWrite(trackRightTwo, 0);
}

void loop() {
  webSocket.loop();
  server.handleClient();
}

