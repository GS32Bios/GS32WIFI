#ifndef GS32WIFI_H
#define GS32WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

// Перечисление режимов работы WiFi
enum GS32WifiMode {
    GS32_WIFI_OFF,       // Wi-Fi выключен
    GS32_WIFI_STA,       // Подключение к существующей сети (Station)
    GS32_WIFI_AP,        // Собственная точка доступа (Access Point)
    GS32_WIFI_AP_STA     // Совмещенный режим (AP + STA)
};

class GS32WIFI {
private:
    GS32WifiMode _mode;
    String _staSsid;
    String _staPass;
    String _apSsid;
    String _apPass;
    bool _autoReconnect;
    unsigned long _lastReconnectAttempt;
    const unsigned long _reconnectInterval = 10000; // Интервал переподключения (10 сек)

    void connectSTA();
    bool startAP();

public:
    GS32WIFI();
    
    // Инициализация библиотеки (вызывается в setup)
    void begin(GS32WifiMode mode, 
               const char* staSsid = "", const char* staPass = "", 
               const char* apSsid = "", const char* apPass = "");
    
    // Обновление состояния. Должно вызываться внутри loop() для автоподключения
    void handle();

    // Проверка состояния
    bool isConnected() const;
    GS32WifiMode getMode() const;
    String getModeString() const;
    
    // Получение IP адресов
    String getSTAIP() const;
    String getAPIP() const;
    
    // Сила сигнала (RSSI) текущего подключения
    int getRSSI() const;
    
    // Количество клиентов, подключенных к нашей AP
    int getConnectedClientsCount() const;
    
    // Сканирование сетей вокруг
    std::vector<String> scanAvailableNetworks();

    // Настройки поведения
    void setAutoReconnect(bool enable);
    void disconnect();
};

#endif // GS32WIFI_H