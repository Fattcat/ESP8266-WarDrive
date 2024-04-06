#include <ESP8266WiFi.h>
#include <SD.h>

#define LED_PIN 2  // Pin pre LED diódu
#define SCAN_INTERVAL 2000  // Interval skenovania v milisekundách
#define BLINK_DURATION 300  // Trvanie blikania v milisekundách

File dataFile;



// ------------ CONNECTION ------------
// NodeMCU ESP8266   ->    SD Card modul
//
// D5 (GPIO14)       ->    CLK
// D6 (GPIO12)       ->    MISO
// D7 (GPIO13)       ->    MOSI
// D8 (GPIO15)       ->    CS
// 3V3               ->    VCC
// GND               ->    GND
// PIN CS JE NA "D1 Mini ESP8266 3.3V" SD Card module na pine D8 a teda to je pin na NodeMCU D4
// ------------ CONNECTION ------------




void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  // Pripojenie k WiFi sieti
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Inicializácia SD karty...");
  if (!SD.begin(4)) {
    Serial.println("Chyba inicializácie SD karty!");
    return;
  }

  Serial.println("SD karta je inicializovaná.");
}

void loop() {
  // Skenovanie Wi-Fi sietí
  int networksFound = WiFi.scanNetworks();
  if (networksFound == 0) {
    Serial.println("Žiadne Wi-Fi siete neboli nájdené.");
  } else {
    Serial.print(networksFound);
    Serial.println(" Wi-Fi sietí bolo nájdených.");

    // Zapnutie zelenej LED diódy
    digitalWrite(LED_PIN, HIGH);
    delay(BLINK_DURATION);
    digitalWrite(LED_PIN, LOW);

    // Otvorenie alebo vytvorenie súboru ScanData.txt
    dataFile = SD.open("ScanData.txt", FILE_WRITE);
    if (dataFile) {
      for (int i = 0; i < networksFound; ++i) {
        // Zapisovanie údajov do súboru
        dataFile.println("----------------------------------------------");
        dataFile.print("SSID : ");
        dataFile.println(WiFi.SSID(i));
        dataFile.print("MAC : ");
        dataFile.println(WiFi.BSSIDstr(i));
        dataFile.print("CH : ");
        dataFile.println(WiFi.channel(i));
        dataFile.print("Sec Type : ");
        dataFile.println(getEncryptionType(WiFi.encryptionType(i)));
        dataFile.print("Signal Strenght : ");
        dataFile.println(WiFi.RSSI(i));
        dataFile.println("----------------------------------------------\n");
      }
      dataFile.close();
    } else {
      Serial.println("Chyba pri otváraní súboru.");
    }
  }

  // Čakanie pred ďalším skenovaním
  delay(SCAN_INTERVAL);
}

String getEncryptionType(int encryptionType) {
  switch(encryptionType) {
    case ENC_TYPE_WEP:
      return "WEP";
    case ENC_TYPE_TKIP:
      return "WPA/TKIP";
    case ENC_TYPE_CCMP:
      return "WPA2/AES";
    case ENC_TYPE_NONE:
      return "None";
    case ENC_TYPE_AUTO:
      return "Auto";
    default:
      return "Unknown";
  }
}
