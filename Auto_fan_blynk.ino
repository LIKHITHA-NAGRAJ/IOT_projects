#define BLYNK_TEMPLATE_ID   "TMPL3qOCKlhr6"
#define BLYNK_TEMPLATE_NAME "Auto Fan"
#define BLYNK_AUTH_TOKEN    "acmOe-6ObyJ5ANekr02FbxvySyfCYwg8"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

#define DHTPIN    26
#define DHTTYPE   DHT11
#define RELAY_PIN 27

char ssid[] = "Redmi Note 13 5G";
char pass[] = "12345678";

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

float threshold = 33.0;             // ✅ raised to 33°C
bool manualOverride = false;
bool manualFanON = false;

BLYNK_WRITE(V4) {
  int val = param.asInt();
  if (val == 1) {
    manualOverride = true;
    manualFanON = true;
    digitalWrite(RELAY_PIN, HIGH);   // ✅ swapped — fan ON
    Blynk.virtualWrite(V3, 1);
    Serial.println("Manual ON!");
  } else {
    manualOverride = true;
    manualFanON = false;
    digitalWrite(RELAY_PIN, LOW);    // ✅ swapped — fan OFF
    Blynk.virtualWrite(V3, 0);
    Serial.println("Manual OFF!");
  }
}

BLYNK_WRITE(V5) {
  manualOverride = false;
  Serial.println("Auto mode resumed!");
}

void readAndControl() {
  delay(100);
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT11 error!");
    return;
  }

  Serial.print("Temp: "); Serial.print(temp);
  Serial.print("°C  Hum: "); Serial.print(hum);
  Serial.println("%");

  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, hum);

  if (!manualOverride) {
    if (temp > threshold) {
      digitalWrite(RELAY_PIN, HIGH); // ✅ swapped — fan ON
      Blynk.virtualWrite(V3, 1);
      Serial.println("Fan AUTO ON!");
    } else {
      digitalWrite(RELAY_PIN, LOW);  // ✅ swapped — fan OFF
      Blynk.virtualWrite(V3, 0);
      Serial.println("Fan AUTO OFF");
    }
  } else {
    if (manualFanON) {
      Serial.println("Fan MANUAL ON!");
    } else {
      Serial.println("Fan MANUAL OFF!");
    }
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);     // ✅ swapped — start OFF

  dht.begin();
  delay(2000);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(3000L, readAndControl);

  Serial.println("Auto Fan System Ready!");
}

void loop() {
  Blynk.run();
  timer.run();
}