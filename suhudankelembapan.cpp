//========================================
//Include Libraries
#include <DHTesp.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif


//========================================
//Define FirebaseESP8266 data object

// firebase admin
#define FIREBASE_HOST "https://dht11-with-esp8266-fb132-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "T61QqE8aJh6wdJDcxUzlDIe5RbJNpTCZLDQGfOtW"

FirebaseData firebaseData;
FirebaseData ledData;
FirebaseData ambilSuhu;
FirebaseData ambilKelembapan;

FirebaseJson json;

DHTesp dhtSensor;
const int DHT_PIN = D4;

int LED = D5;               

// Wifi Details
const char* ssid = "Kopi kapal api ABC susu";
const char* password = "luwak white kawfee";

void setup() {
  configTime(0, 0, "pool.ntp.org");  // get UTC time via NTPorg

  //set Role
  pinMode(LED, OUTPUT);

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  Serial.begin(115200); //delay
  // Menghubungkan ke Wifi:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
    //Mengambil Data nilai 0/1
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  float sensorTemp = data.temperature;
  float sensorHumidity = data.humidity;


  int LEDValue = digitalRead(LED);              // mengambil nilai LED 0/1
  Serial.print(sensorTemp);
  Serial.println("derajat");
  Serial.print(sensorHumidity);
  Serial.println("persen");

  String sendsuhu = String(sensorTemp) + String("C");
  String sendkelembapan = String(sensorHumidity) + String("%");

  // Mengirim data suhu dan kelembapan ke firebase
  if (Firebase.setFloat(firebaseData, "/suhu", sensorTemp)) {
    Serial.println("TEMP PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  } else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  if (Firebase.setFloat(firebaseData, "/kelembapan", sensorHumidity)) {
    Serial.println("HUMIDITY PASSED");
    Serial.println("------------------------------------");
    Serial.println();
  } else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  // mengambil data 0/1 dari firebase
  if (Firebase.getInt(ledData, "/perangkat1")) {

    if (ledData.stringData() == "1") {
      Serial.println("Nilai Lampu 1 sama dengan 1");
      digitalWrite(LED, HIGH);
    } else if (ledData.stringData() == "0") {
      Serial.println("Nilai Lampu 0 sama dengan 0");
      digitalWrite(LED, LOW);
    }
  } else {
    Serial.println(ledData.errorReason());
  }

// mengambil suhu dan menyalakan atau mematikan perangkat
  if (Firebase.getInt(ambilSuhu, "/suhu") && Firebase.getInt(ambilKelembapan, "/kelembapan")) {

    if ((ambilSuhu.intData() >= -4 && ambilSuhu.intData() >= 33) && (ambilKelembapan.intData() >= 50)) {
      Serial.println("Perangkat Akan Dinyalakan");
      digitalWrite(LED, HIGH);
      if (Firebase.setInt(ledData, "/perangkat1", 1)) {
        Serial.println("perangkat 1 UPDATED");
        Serial.println("------------------------------------");
        Serial.println();
      }
    }
    else if (ambilSuhu.intData() <= -5) {
      Serial.println("Perangkat Akan dimatikan");
      digitalWrite(LED, LOW);
      if (Firebase.setInt(ledData, "/perangkat1", 0)) {
        Serial.println("perangkat 1 UPDATED");
        Serial.println("------------------------------------");
        Serial.println();
      }
      delay(100000);
      digitalWrite(LED, HIGH);
      if (Firebase.setInt(ledData, "/perangkat1", 1)) {
        Serial.println("perangkat 1 UPDATED");
        Serial.println("------------------------------------");
        Serial.println();
      }
    }
  }
  else{
    Serial.print(firebaseData.errorReason());
  }

delay(2000);

}