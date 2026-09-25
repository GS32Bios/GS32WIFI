#include "GS32WIFI.h"

GS32WIFI::GS32WIFI() : 
    _mode(GS32_WIFI_OFF), 
    _autoReconnect(true), 
    _lastReconnectAttempt(0) {}

void GS32WIFI::begin(GS32WifiMode mode, const char* staSsid, const char* staPass, const char* apSsid, const char* apPass) {
    _mode = mode;
    _staSsid = String(staSsid);
    _staPass = String(staPass);
    _apSsid = String(apSsid);
    _apPass = String(apPass);

    // Сброс настроек перед инициализацией
    WiFi.disconnect(true, true);
    delay(100);

    switch (_mode) {
        case GS32_WIFI_OFF:
            WiFi.mode(WIFI_OFF);
            break;

        case GS32_WIFI_STA:
            WiFi.mode(WIFI_STA);
            connectSTA();
            break;

        case GS32_WIFI_AP:
            WiFi.mode(WIFI_AP);
            startAP();
            break;

        case GS32_WIFI_AP_STA:
            WiFi.mode(WIFI_AP_STA);
            startAP();
            connectSTA();
            break;
    }
}

void GS32WIFI::connectSTA() {
    if (_staSsid.length() == 0) {
        return;
    }
    WiFi.begin(_staSsid.c_str(), _staPass.length() > 0 ? _staPass.c_str() : NULL);
}

bool GS32WIFI::startAP() {
    if (_apSsid.length() == 0) {
        _apSsid = "GS32_AP";
    }
    
    // WPA2 требует пароль длиной от 8 символов. Если меньше - делаем сеть открытой.
    const char* passParam = NULL;
    if (_apPass.length() >= 8) {
        passParam = _apPass.c_str();
    } 

    bool success = WiFi.softAP(_apSsid.c_str(), passParam);
    
    return success;
}

void GS32WIFI::handle() {
    // Автореконнект работает только для режимов, где активен клиент (STA / AP_STA)
    if (_mode == GS32_WIFI_STA || _mode == GS32_WIFI_AP_STA) {
        if (_autoReconnect && WiFi.status() != WL_CONNECTED) {
            unsigned long currentMillis = millis();
            if (currentMillis - _lastReconnectAttempt >= _reconnectInterval) {
                _lastReconnectAttempt = currentMillis;
                WiFi.begin(_staSsid.c_str(), _staPass.length() > 0 ? _staPass.c_str() : NULL);
            }
        }
    }
}

bool GS32WIFI::isConnected() const {
    if (_mode == GS32_WIFI_STA || _mode == GS32_WIFI_AP_STA) {
        return (WiFi.status() == WL_CONNECTED);
    }
    return false;
}

GS32WifiMode GS32WIFI::getMode() const {
    return _mode;
}

String GS32WIFI::getModeString() const {
    switch (_mode) {
        case GS32_WIFI_OFF:    return "Disabled";
        case GS32_WIFI_STA:    return "Connection Mode (STA)";
        case GS32_WIFI_AP:     return "Access Point (AP)";
        case GS32_WIFI_AP_STA: return "Combined mode (AP+STA)";
        default:               return "Unknown";
    }
}

String GS32WIFI::getSTAIP() const {
    if ((_mode == GS32_WIFI_STA || _mode == GS32_WIFI_AP_STA) && WiFi.status() == WL_CONNECTED) {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

String GS32WIFI::getAPIP() const {
    if (_mode == GS32_WIFI_AP || _mode == GS32_WIFI_AP_STA) {
        return WiFi.softAPIP().toString();
    }
    return "0.0.0.0";
}

int GS32WIFI::getRSSI() const {
    if (isConnected()) {
        return WiFi.RSSI();
    }
    return -100; // Минимальный уровень сигнала, если нет подключения
}

int GS32WIFI::getConnectedClientsCount() const {
    if (_mode == GS32_WIFI_AP || _mode == GS32_WIFI_AP_STA) {
        return WiFi.softAPgetStationNum();
    }
    return 0;
}

std::vector<String> GS32WIFI::scanAvailableNetworks() {
    std::vector<String> networks;
    
    // Для надежного сканирования временно переводим в STA, если Wi-Fi был выключен
    uint8_t currentWiFiMode = WiFi.getMode();
    if (currentWiFiMode == WIFI_OFF) {
        WiFi.mode(WIFI_STA);
        delay(100);
    }

    int16_t n = WiFi.scanNetworks(false, false, false, 150); // Быстрое сканирование
    
    if (n > 0) {
        for (int i = 0; i < n; ++i) {
            //String networkEntry = WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + "dBm)";
            networks.push_back(WiFi.SSID(i));
        }
    }
    
    WiFi.scanDelete(); // Обязательная очистка динамической памяти результатов сканирования

    // Возвращаем режим обратно, если меняли его ради сканирования
    if (currentWiFiMode == WIFI_OFF) {
        WiFi.mode(WIFI_OFF);
    }
    
    return networks;
}

void GS32WIFI::setAutoReconnect(bool enable) {
    _autoReconnect = enable;
}

void GS32WIFI::disconnect() {
    WiFi.disconnect();
}