#include <ESP8266WiFi.h>
#include <SD.h>

// Súbor na SD karte pre ukladanie údajov o skenovaní
const char* scanFileName = "/Scan.txt";

void setup() {
  Serial.begin(115200);

  // Inicializácia SD karty
  if (SD.begin(D1)) {
    Serial.println("SD karta inicializovaná.");
  } else {
    Serial.println("Chyba inicializácie SD karty.");
    return;
  }

  // Pripojenie k Wi-Fi
  connectToWiFi();

  // Spustenie cyklu skenovania a ukladania dát
  while (true) {
    scanAndSave();
    delay(6000);  // Čakanie 6 sekúnd pred ďalším skenovaním
  }
}

void loop() {
  // Loop nie je používaný, všetko sa deje v setup().
}

void connectToWiFi() {
  Serial.print("Pripájam sa k Wi-Fi sieti");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nPripojené k Wi-Fi sieti");
}

void scanAndSave() {
  Serial.println("Spúšťam skenovanie...");

  // Skenovanie dostupných sietí Wi-Fi
  int networkCount = WiFi.scanNetworks();
  if (networkCount == 0) {
    Serial.println("Nenašli sa žiadne dostupné siete.");
    return;
  }

  // Otvorenie alebo vytvorenie súboru pre zápis
  File file = SD.open(scanFileName, FILE_WRITE);
  if (!file) {
    Serial.println("Chyba otvorenia súboru.");
    return;
  }

  // Ukladanie údajov o sietiach do súboru
  for (int i = 0; i < networkCount; ++i) {
    file.println("-----------------------------------------------------");
    file.println("SSID : " + WiFi.SSID(i));
    file.println("MAC : " + WiFi.BSSIDstr(i));
    file.println("Channel : " + String(WiFi.channel(i)));
    file.println("Security Type : " + getEncryptionType(WiFi.encryptionType(i)));
    file.println("Signal Strenght (dB) : " + String(WiFi.RSSI(i)));
    file.println("-----------------------------------------------------");
    file.println();
  }

  // Zatvorenie súboru
  file.close();

  Serial.println("Dáta boli uložené do súboru " + String(scanFileName));
}

String getEncryptionType(int authType) {
  switch (authType) {
    case ENC_TYPE_NONE:
      return "None";
    case ENC_TYPE_WEP:
      return "WEP";
    case ENC_TYPE_TKIP:
      return "TKIP";
    case ENC_TYPE_CCMP:
      return "CCMP";
    case ENC_TYPE_AUTO:
      return "Auto";
    default:
      return "Unknown";
  }
}
