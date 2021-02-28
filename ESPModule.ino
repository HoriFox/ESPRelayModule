#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "index.h"
#include "data.h"

//Для прошивки по WiFi
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h> // Библиотека для OTA-прошивки

const char* nameDevice = nameDeviceValue;
const char* passDevice = passDeviceValue;
const char* ssid = ssidValue;
const char* password = passwordValue;

//IPAddress ip(192,168,1,20);  // Статический IP
//IPAddress gateway(192,168,1,1);
//IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

void serverRoot() {
  server.send(200, "text/html", MAIN_page);
}

void relayAction(bool switchModule) {
  if (switchModule == 0) {
    digitalWrite(0, LOW);
    digitalWrite(2, LOW);
    server.send(200, "text/plain", "switch off");
  } else {
    digitalWrite(0, HIGH);
    digitalWrite(2, HIGH);
    server.send(200, "text/plain", "switch on");
  }
}

void relayPage() {
  String value = server.arg("value");
  int activeValue = value.toInt();
  relayAction(activeValue);
}

void metricsPage() {
  int countReboot = EEPROM.read(0);
  String message = "{\n";
  message += "  \"countReboot\": "; message += countReboot; message += "\n";
  message += "}";
  server.send(200, "application/json", message);
}

void restartPage() {
  server.send(200, "text/plain", "restart esp");
  ESP.restart();
}

void fileNotFound() {
  server.send(404, "text/plain", "File Not Found");
}

void setup(void) {
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA); //Режим клиента (WIFI_AP - точка доступа, WIFI_AP_STA - оба режима)
  WiFi.begin(ssid, password);
  //WiFi.config(ip, gateway, subnet); //Ставим статический адрес
  Serial.println("");

  // Ждём соединения
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Для прошивки по WiFi
  ArduinoOTA.setHostname(nameDevice);     // Задаем имя сетевого порта
  //ArduinoOTA.setPassword(passDevice);   // Задаем пароль доступа для удаленной прошивки
  ArduinoOTA.begin();

  server.on("/", serverRoot);
  server.on("/relay", relayPage);
  server.on("/restart", restartPage);
  server.on("/metrics", metricsPage);
  server.onNotFound(fileNotFound);
  server.begin();
  Serial.println("HTTP server started");

  // Инициализируем EEPROM
  EEPROM.begin(512);
  int countReboot = EEPROM.read(0); // Получаем количество перезапусков
  EEPROM.write(0, countReboot + 1); // Записываем в память количество перезапусков модуля
  EEPROM.commit(); // Сохраняем в памяти
}

void loop(void) {
  ArduinoOTA.handle();  // Всегда готовы к прошивке
  server.handleClient();
}
