// code ensures this :
// - when esp8266 find some WiFi Network, it automatically tell GPS neo 6m to start getting GPS Data (latitude, longitude, altitude and TIME)
// - and when no WiFi is near you (randw is approximatelly 20 - 60 meters) then it STOPs getting GPS data (to ensure to GET ONLY WiFi Network GPS LOCATION)
// - and every time esp8266 find WiFi Network that is near, it will get GPS latitude, longitude, altitude and TIME and save this to SD card

// -------------- CONNECTION --------------
// neo 6m GPS -> esp8266
// VCC -> 3.3V
// GND -> GND
// GPS_RX -> D1
// -> GPS_TX -> D2

// SD Card -> esp8266
// VCC -> 3.3V
// GND -> GND
// CS -> D8
// MOSI -> D7
// MISO -> D6
// SCK -> D5
// -------------- CONNECTION END --------------

//  -------------------- CODE DOWN -------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SD.h>

const int CHIP_SELECT = D8;  // Chip select pin for SD card
const int GPS_RX = D1;       // GPS module RX pin
const int GPS_TX = D2;       // GPS module TX pin
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600);

  // Initialize SD card
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  int networkCount = WiFi.scanNetworks();  // Scan for available networks

  if (networkCount > 0) {
    Serial.println("WiFi network detected. Starting GPS acquisition...");

    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());

      // Check if GPS data is valid
      if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        float altitude = gps.altitude.meters();
        unsigned long timestamp = gps.time.value();  // Get timestamp from GPS data

        // Write GPS and WiFi data to SD card
        File dataFile = SD.open("/gps_wifi_data.txt", FILE_APPEND);
        if (dataFile) {
          // Write GPS data
          dataFile.print("Latitude: ");
          dataFile.print(latitude, 6);
          dataFile.print(", Longitude: ");
          dataFile.print(longitude, 6);
          dataFile.print(", Altitude: ");
          dataFile.print(altitude);
          dataFile.print(" m, Time: ");
          dataFile.println(timestamp);

          // Write WiFi data for each network
          for (int i = 0; i < networkCount; i++) {
            dataFile.print("SSID: ");
            dataFile.print(WiFi.SSID(i));
            dataFile.print(", MAC: ");
            dataFile.print(WiFi.BSSIDstr(i));
            dataFile.print(", Channel: ");
            dataFile.print(WiFi.channel(i));
            dataFile.print(", Security: ");
            
            // Get security type
            switch (WiFi.encryptionType(i)) {
              case ENC_TYPE_WEP: dataFile.print("WEP"); break;
              case ENC_TYPE_TKIP: dataFile.print("WPA/PSK"); break;
              case ENC_TYPE_CCMP: dataFile.print("WPA2/PSK"); break;
              case ENC_TYPE_NONE: dataFile.print("Open"); break;
              case ENC_TYPE_AUTO: dataFile.print("Auto"); break;
              default: dataFile.print("Unknown"); break;
            }

            dataFile.print(", Signal Strength: ");
            dataFile.print(WiFi.RSSI(i));
            dataFile.println(" dBm");
          }

          dataFile.println("--------------------------------------------");  // Separate entries
          dataFile.close();
          Serial.println("Data saved to SD card.");
        } else {
          Serial.println("Failed to open SD card file.");
        }

        // Stop GPS data acquisition to save power
        delay(5000);  // Optional delay for reducing GPS polling rate
        break;        // Exit loop once data is written to SD card
      }
    }
  } else {
    Serial.println("No WiFi network found. Stopping GPS acquisition...");
    // Power down or stop GPS if supported to save power.
    delay(10000); // Wait for a while before next WiFi scan
  }

  // Clear WiFi scan results to save memory
  WiFi.scanDelete();
  delay(5000);  // Wait before rescanning WiFi networks
}
