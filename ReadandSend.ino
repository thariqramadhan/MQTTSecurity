// Declaration DHT Library
#include "DHT.h"

// Declaration Cryptography AES Library
#include <Crypto.h>
#include <AES.h>
#include <string.h>

// Declaration MQTT Library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTPIN 4 // Define pin for DHT use pin D2
#define DHTTYPE DHT22 // Define type for DHT use 22

DHT dht(DHTPIN, DHTTYPE); // Declaration DHT

// Declaration Variabel for Cryptography AES
AES128 cipher;
int nData = 0;
byte key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}; // Key
byte text[20], Encrypt[16], Decrypt[16];

// Declaration MQTT
WiFiClient espClient;
PubSubClient client(espClient);
const char* ssid = "Virus"; // Your WiFi name
const char* password =  "hahapanggakjelashah"; // Your WiFi password
const char* mqttServer = "192.168.43.4"; // Your MQTT server
const int mqttPort = 1883; // Your MQTT port

void setup() {
  Serial.begin(115200); // For using serial monitor
  Serial.setTimeout(2000); // Setting time out
  pinMode(LED_BUILTIN, OUTPUT); // Calling LED
  ConnectWiFi(); // I unite connect to WiFi and MQTT Server
}

void loop() {
  String dataDHT = "";

  dataDHT += ReadDHT();

  // Cryptography AES

  // Change String to byte array
  byte buff[20];
  dataDHT.getBytes(buff, 21);
  for (int i = 0; i < 20; i++) {
    text[i] = buff [i];
  }

  // Setting Key
  crypto_feed_watchdog();
  cipher.setKey(key, cipher.keySize());

  // Encription
  cipher.encryptBlock(Encrypt, text);

  Serial.print("Text Encyrypt(int) : ");
  for (int i = 0; i < 16; i++) {
    Serial.print(Encrypt[i]);
    Serial.print(" ");
  }
  Serial.println();

  // Change byte array[20] to String in HEX format
  byte tempByte;
  char tempC;
  char message[32];
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 2; j++) {
      if ((j % 2) == 0) {
        tempByte = Encrypt[i] >> 4;
      } else {
        tempByte = Encrypt[i] - ((Encrypt[i] >> 4) << 4);
      }
      switch (tempByte) {
        case 0x00 :
          tempC = '0';
          break;
        case 0x01 :
          tempC = '1';
          break;
        case 0x02 :
          tempC = '2';
          break;
        case 0x03 :
          tempC = '3';
          break;
        case 0x04 :
          tempC = '4';
          break;
        case 0x05 :
          tempC = '5';
          break;
        case 0x06 :
          tempC = '6';
          break;
        case 0x07 :
          tempC = '7';
          break;
        case 0x08 :
          tempC = '8';
          break;
        case 0x09 :
          tempC = '9';
          break;
        case 0x0A :
          tempC = 'A';
          break;
        case 0x0B :
          tempC = 'B';
          break;
        case 0x0C :
          tempC = 'C';
          break;
        case 0x0D :
          tempC = 'D';
          break;
        case 0x0E :
          tempC = 'E';
          break;
        case 0x0F :
          tempC = 'F';
          break;
      }
      message[2 * i + j] = tempC;
    }
  }

  memset(message + 32, NULL, 16); // Clear array
  Serial.print("String message : ");
  Serial.println(message);


  // Send data
  memset(message + 32, NULL, 16); // Clear array
  client.publish("ESP/text", message);
  client.loop();

  Serial.println();

  // Read data every 5 second
  delay(1000);

  // Give delay after 3 hour
  if (nData > 3 * 3600) {
    delay(10 * 3600 * 1000);
  }
}

void ConnectWiFi() {
  // Connect ke Wifi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // Connect ke MQTT
  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client1")) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.subscribe("ESP/led");
}

String ReadDHT() {
  float h = dht.readHumidity(); // Read humidty
  float t = dht.readTemperature(); // Read temperature
  String dataSend = "";
  dataSend += nData;
  dataSend += ":";
  // Mengecheck bila gagal terbaca
  if (isnan(h) || isnan(t)) {
    dataSend += "--:--;;";
  } else {
    // Memprint data ke serial monitor
    dataSend += String(h, 2);
    dataSend += ":";
    dataSend += String(t, 2);
    dataSend += ";";
  }
  Serial.print("Data : ");
  Serial.println(dataSend);
  nData++;

  return dataSend;
}


