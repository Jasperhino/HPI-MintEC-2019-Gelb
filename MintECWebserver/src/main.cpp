#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "website.hpp"

std::vector<int> data1 = {0, 0, 0, 0, 0, 0, 0};
std::vector<int> data2 = {0, 0, 0, 0, 0, 0, 0};
const char* ssid = "esp-test";
const char* password = "12345678";

WebServer server(80);

void connect(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected with IP: ");
  Serial.println(WiFi.localIP());
}

void updateData1(int newData) {
  // TO-DO: Receive data
  for (int i=0; i<6; i++) {
    data1[i] = data1[i+1];
  }
  data1[6] = newData;
}

void updateData2(int newData) {
  // TO-DO: Receive data
  for (int i=0; i<6; i++) {
    data2[i] = data2[i+1];
  }
  data2[6] = newData;
}

void showWebsite(){
  std::string content = renderWebsite(data1, data2);
  server.send(200, "text/html", content.c_str());
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);
  connect();
  server.on("/", showWebsite);
  server.on("/update/a1/{}", []() {
    String receivedData = server.pathArg(0);
    int convertedData = atoi(receivedData.c_str());
    Serial.print("A1: ");
    Serial.println(convertedData);
    updateData1(convertedData);
    server.send(200, "text/plain", "Ok.");
  });
  server.on("/update/a2/{}", []() {
    String receivedData = server.pathArg(0);
    int convertedData = atoi(receivedData.c_str());
    Serial.print("A2: ");
    Serial.println(convertedData);
    updateData2(convertedData);
    server.send(200, "text/plain", "Ok.");
  });
  server.begin();
}

void loop() {
  server.handleClient();

}