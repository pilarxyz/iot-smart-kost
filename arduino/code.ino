#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <TimeLib.h> 
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi credentials
const char* ssid = "xx";
const char* password = "xx";

// Telegram Bot Token
const char* botToken = "xx";

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600);

#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int pirPin = D8;
int ldrPin = A0;
int lampuLedPin = D3;
int kipasLedPin = D4;

const int SUHU_NORMAL = 25;
const int KELEMBAPAN_NORMAL = 50;
const int BATAS_INTENSITAS_CAHAYA = 800;

bool lampuTerakhirMenyala = false;
bool kipasTerakhirMenyala = false;
bool lastPirState = false;

// Menambahkan variabel untuk status switch dari server
bool switchLampu = false;
bool switchKipas = false;
bool sistemOtomatis = true;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(pirPin, INPUT);
  pinMode(ldrPin, INPUT);
  pinMode(lampuLedPin, OUTPUT);
  pinMode(kipasLedPin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");

  client.setInsecure(); // Only necessary for ESP8266 without a valid certificate
}

void sendTelegramMessage(String message) {
  Serial.print("Sending Telegram Message: ");
  Serial.println(message);
  bot.sendMessage("xx", message, "");
}

void updateSwitchStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient wifiClient;
    HTTPClient http;
    http.begin(wifiClient, "http://xx/iot/status.json");

    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      switchLampu = doc["switchLampu"];
      switchKipas = doc["switchKipas"];
      sistemOtomatis = doc["sistemOtomatis"];
    } else {
      Serial.println("Error in HTTP request");
    }

    http.end();
  }
}

void sendDataToServer(float temperature, float humidity, int ldrValue, bool lampState, bool fanState) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient wifiClient;
        HTTPClient http;

        String serverPath = "http://xx/iot/input.php";

        DynamicJsonDocument doc(1024);
        doc["temperature"] = temperature;
        doc["humidity"] = humidity;
        doc["ldrValue"] = ldrValue;
        doc["lampState"] = lampState;
        doc["fanState"] = fanState;
        String jsonData;
        serializeJson(doc, jsonData);

        http.begin(wifiClient, serverPath);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(jsonData);

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String response = http.getString();
            Serial.println(response);
        }
        else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } 
    else {
        Serial.println("WiFi not connected");
    }
}

void loop() {
  timeClient.update();

  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int currentHour = ptm->tm_hour;
  int currentMinute = ptm->tm_min;
  int currentWeekday = ptm->tm_wday + 1;

  updateSwitchStatus();

  int pirValue = digitalRead(pirPin);
  int ldrValue = analogRead(ldrPin);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();


  bool lampuSekarangMenyala = (ldrValue > BATAS_INTENSITAS_CAHAYA) && sistemOtomatis;
  bool kipasSekarangMenyala = (temperature > SUHU_NORMAL || humidity > KELEMBAPAN_NORMAL) && sistemOtomatis;
  bool currentPirState = digitalRead(pirPin) == HIGH;

  // Kontrol berdasarkan switch dari server jika sistem otomatis dimatikan
  if (!sistemOtomatis) {
    lampuSekarangMenyala = switchLampu;
    kipasSekarangMenyala = switchKipas;
  }

  sendDataToServer(temperature, humidity, ldrValue, lampuSekarangMenyala, kipasSekarangMenyala);

  // Logika untuk lampu
  if (lampuSekarangMenyala != lampuTerakhirMenyala) {
    digitalWrite(lampuLedPin, lampuSekarangMenyala ? HIGH : LOW); // Kontrol LED lampu
    sendTelegramMessage("Lampu " + String(lampuSekarangMenyala ? "menyala" : "mati"));
    lampuTerakhirMenyala = lampuSekarangMenyala;
  }

  // Logika untuk kipas
  if (kipasSekarangMenyala != kipasTerakhirMenyala) {
    digitalWrite(kipasLedPin, kipasSekarangMenyala ? HIGH : LOW); // Kontrol LED kipas
    sendTelegramMessage("Kipas " + String(kipasSekarangMenyala ? "menyala" : "mati"));
    kipasTerakhirMenyala = kipasSekarangMenyala;
  }

  // Print states
  Serial.print("Lamp State: ");
  Serial.println(lampuSekarangMenyala ? "On" : "Off");
  Serial.print("Fan State: ");
  Serial.println(kipasSekarangMenyala ? "On" : "Off");

 if (currentPirState && !lastPirState) {
    // Menampilkan waktu dan hari
    Serial.print("Waktu saat ini: ");
    Serial.print(currentHour);
    Serial.print(":");
    Serial.println(currentMinute);
    Serial.print("Hari dalam seminggu: ");
    Serial.println(currentWeekday);

    // Mengirim pesan jika terdeteksi gerakan pada waktu yang ditentukan
    if (currentHour >= 7 && currentHour < 23 && currentWeekday >= 2 && currentWeekday <= 6) {
        Serial.println("Ada gerakan pada waktu yang ditentukan");
        sendTelegramMessage("Ada orang pada jam " + String(currentHour) + ":" + String(currentMinute));
    } else {
        Serial.println("Gerakan terdeteksi, tapi tidak pada waktu yang ditentukan");
    }
  } else if (!currentPirState) {
    Serial.println("Tidak ada gerakan terdeteksi");
  }

  lastPirState = currentPirState;
  Serial.println("================");
  delay(1000);
}