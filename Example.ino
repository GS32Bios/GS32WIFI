#include <Arduino.h>
#include "GS32WIFI.h"

GS32WIFI myWifi;

// Конфигурация для подключения к роутеру (STA)
const char* target_ssid = "Avangart";
const char* target_pass = "12345678";

// Конфигурация для нашей точки доступа (AP)
const char* ap_ssid = "GS32WiFiAP";
const char* ap_pass = "ConfigPassword123"; // Не менее 8 символов!

unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- Старт программы ---");

  // Сначала сделаем сканирование доступных сетей вокруг
  std::vector<String> foundNetworks = myWifi.scanAvailableNetworks();
  Serial.println("\n--- Результаты сканирования сетей вокруг ---");
  for (const String& net : foundNetworks) {
    Serial.println(" -> " + net);
  }
  Serial.println("-------------------------------------------\n");

  // Инициализируем Wi-Fi. Выберите нужный режим ниже:
  // 1. GS32_WIFI_OFF    - Выключен
  // 2. GS32_WIFI_STA    - Только клиентское подключение
  // 3. GS32_WIFI_AP     - Только своя точка доступа
  // 4. GS32_WIFI_AP_STA - Совмещенный режим (AP + подключение к роутеру)
  
  GS32WifiMode selectedMode = GS32_WIFI_AP_STA; 
  
  myWifi.begin(selectedMode, target_ssid, target_pass, ap_ssid, ap_pass);
}

void loop() {
  // Вызов handle() обязателен для работы фонового автоподключения
  myWifi.handle();

  // Каждые 7 секунд выводим информацию о текущем состоянии сети в Serial
  if (millis() - lastPrintTime > 7000) {
    lastPrintTime = millis();

    // Простой пример проверки: подключены ли мы к внешней сети?
    if (myWifi.isConnected()) {
      Serial.printf("[INFO] Соединение с интернетом/роутером активно. IP: %s\n", myWifi.getSTAIP().c_str());
    } else {
      if (myWifi.getMode() == GS32_WIFI_STA || myWifi.getMode() == GS32_WIFI_AP_STA) {
         Serial.println("[INFO] Ожидание подключения к роутеру...");
      }
    }

  }
}