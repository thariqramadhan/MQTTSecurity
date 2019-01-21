// Declaration Cryptography AES Library
#include <Crypto.h>
#include <AES.h>
#include <string.h>

// Declaration MQTT Library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Declaration SRAM Library
#include <SRAM.h>

const int N = 16; // 16 max length

// Declaration Variabel for Cryptography AES
AES128 cipher;
int nData = 0;
byte key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}; // Key
byte text[N], Decrypt[16];
char message[2 * N];

// Declaration MQTT
WiFiClient espClient;
PubSubClient client(espClient);
const char* ssid = "Virus"; // Your WiFi name
const char* password =  "hahapanggakjelashah"; // Your WiFi password
const char* mqttServer = "192.168.43.4"; // Your MQTT server
const int mqttPort = 1883; // Your MQTT port

// Variable to calculate error
int nDataR = 0;
int totError = 0;

// Allocate RAM
SRAM sram(4, SRAM_1024);

// Declaration time
long int timeSend = 0;
long int timeReceive;
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000); // Set waktu time out
  pinMode(LED_BUILTIN, OUTPUT); // Memanggil LED
  ConnectWiFi(); // I unite connect to WiFi and MQTT Server

  sram.begin();
  sram.seek(1);

  Serial.println("No Data\tNo Data Receive\tHumidty(%)\tTemperature(*C)\tTotal Error\tDelay(milis)");
}

void loop() {
  client.loop();
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
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client2" )) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.subscribe("ESP/text");
}


void callback(char* topic, byte* payload, unsigned int length) {
  sram.seek(1);
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  timeReceive = millis();
  // Calculate delay
  int j = 2 * N + 1;
  int i = 0;
  int power;
  char buf[8];
  while (message[j] != '-') {
    buf[i] = message[j];
    i++;
    j++;
  }
  timeSend =atoi(buf);
  int dela = timeReceive - timeSend;
  
  // Setting Key
  crypto_feed_watchdog();
  cipher.setKey(key, cipher.keySize());

  // Mengubah string dalam HEX format ke byte char[16]

  byte dataRec[N];
  byte temp;
  for (int i = 0; i < 2 * N; i++) {
    switch (message[i]) {
      case '0' :
        temp = 0x00;
        break;
      case '1' :
        temp = 0x01;
        break;
      case '2' :
        temp = 0x02;
        break;
      case '3' :
        temp = 0x03;
        break;
      case '4' :
        temp = 0x04;
        break;
      case '5' :
        temp = 0x05;
        break;
      case '6' :
        temp = 0x06;
        break;
      case '7' :
        temp = 0x07;
        break;
      case '8' :
        temp = 0x08;
        break;
      case '9' :
        temp = 0x09;
        break;
      case 'A' :
        temp = 0x0A;
        break;
      case 'B' :
        temp = 0x0B;
        break;
      case 'C' :
        temp = 0x0C;
        break;
      case 'D' :
        temp = 0x0D;
        break;
      case 'E' :
        temp = 0x0E;
        break;
      case 'F' :
        temp = 0x0F;
        break;
    }
    if ((i % 2) == 0) {
      dataRec[i / 2] = temp;
    } else {
      dataRec[(i - 1) / 2] = ( dataRec[(i - 1) / 2] << 4) ^ temp;
    }
  }

  // Dekripsi
  cipher.decryptBlock(Decrypt, dataRec);

  // Menampilkan data yang di dekripsi
  char messageR[N];
  Serial.print(nDataR);
  Serial.print("\t");
  nDataR++;
  bool checkError = false;
  for (int i = 0; i < N; i++) {
    messageR[i] = char(Decrypt[i]);
    if (int(messageR[i]) == 0) {
    } else if ((int(messageR[i]) < 45) || (int (messageR[i] > 59))) {
      checkError = true;
    }
  }

  if (checkError) {
    Serial.print("Message Error");
    Serial.print("\t\t\t");
    totError++;
  } else {
    int i = 0;
    while (messageR[i] != ';') {
      if (messageR[i] == ':') {
        Serial.print("\t\t");
      } else {
        Serial.print(char(messageR[i]));
      }
      i++;
    }
  }
  Serial.print("\t\t");
  Serial.print(totError);
  Serial.print("\t\t");
  Serial.println(dela);

  Serial.println();
  memset(payload, NULL, 2 * N);

  sram.seek(1);
}
