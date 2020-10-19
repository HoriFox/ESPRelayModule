#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "WIFIModuleSSID";
const char* password = "password";

ESP8266WebServer server(80);

void server_root() {
  server.send(200, "text/plain", "Hello from esp8266 server module!");
}

void light_action() {
  //http://x.x.x.x/light?value=1
  String value = server.arg("value");
  int number = value.toInt();
  if(number == 0) {
    digitalWrite(LED_BUILTIN, LOW);
    server.send(200, "text/plain", "switch OFF light");
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    server.send(200, "text/plain", "switch ON light");
  }
}

void fileNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: " + server.uri();
  message += "\nMethod: " + (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: " + server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
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

  server.on("/", server_root);
  server.on("/light", light_action);
  server.onNotFound(fileNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
