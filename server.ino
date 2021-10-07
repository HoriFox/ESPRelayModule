#include "server.h"

void handleButtonPress() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && isYetChangeStage) {
    relayAction(!currentStage, true);
    isYetChangeStage = false;
  }
  if (buttonState == HIGH) {
    isYetChangeStage = true;
  }
}

void serverRoot() {
  server.send(200, "text/html", MAIN_page);
}

// isLocalChange - изменения реле локальны (с кнопки) или удалённы (из админ-панели)
// нет смысла отправлять ответы, если локальны, тем более это приведёт к подвисанию сервера
void relayAction(bool switchModule, bool isLocalChange) {
  if (switchModule == 0) {
    currentStage = false;
    digitalWrite(CONTROL_PIN, LOW);
    if (!isLocalChange) {
      server.send(200, "text/plain", "switch off");
    }
  } else {
    currentStage = true;
    digitalWrite(CONTROL_PIN, HIGH);
    if (!isLocalChange) {
      server.send(200, "text/plain", "switch on");
    }
  }
  delay(DELAY_SWITCH);
}

void relayPage() {
  String value = server.arg("value");
  int activeValue = value.toInt();
  relayAction(activeValue, false);
}

void metricsPage() {
  static const size_t BUFSIZE = 1024;
  static const char compile_date[] = __DATE__ " " __TIME__;
  static const char *response_template =
        "# HELP esp_module_reboot Device reboot count.\n"
        "# TYPE esp_module_reboot gauge\n"
        "# UNIT esp_module_reboot \n"
        "esp_module_reboot %d\n"
        "# HELP esp_module_uptime Device uptime in ms.\n"
        "# TYPE esp_module_uptime gauge\n"
        "# UNIT esp_module_uptime \n"
        "esp_module_uptime %d\n"
        "# HELP esp_module_info Device info.\n"
        "# TYPE esp_module_info gauge\n"
        "# UNIT esp_module_info \n"
        "esp_module_info{build=\"%s\",date=\"%s\"} 1\n";
  uint32_t count_reboot = getROMData(0);
  uint32_t ms = millis();
  char response[BUFSIZE];
  snprintf(response, BUFSIZE, response_template, count_reboot, ms, BUILD, compile_date);
  server.send(200, "text/plain; charset=utf-8", response);
}

void restartPage() {
  server.send(200, "text/plain", "restart esp");
  ESP.restart();
}

void fileNotFound() {
  server.send(404, "text/plain", "File Not Found");
}

void setupServer() {
  server.on("/", serverRoot);
  server.on("/relay", relayPage);
  server.on("/restart", restartPage);
  server.on("/metrics", metricsPage);
  server.onNotFound(fileNotFound);
  server.begin();
}

void handleRequests() {
  server.handleClient();
}
