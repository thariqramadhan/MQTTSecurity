/*
  I include an example of an implementation of AES cryptography using the library
  https://github.com/rweather/arduinolibs because it is hard to find examples of
  implementations from the library especially for ESP8266 . Therefore I will give an example of its implementation here.
  The flow of this program is:
  String -> Bytes -> Encryption -> Strings -> Bytes -> Decryption -> Strings
  I change the encryption and then become a string again because in sending data through MQTT,
  it is necessary to send a string. Later there will be 2 types of strings sent by Base64 and HEX
  in the form of strings.
*/

// Library for Cryptography
#include <Crypto.h>
#include <AES.h>
// Variable fo Cryptography
byte key[32]         = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
}; // Key is make in string
const int N = 16; // Length of message max 16
AES128 cipher;
byte Encrypt[N];
byte Decrypt[N];

byte messageSendByte[N];
byte buff[N];

byte tempByte;
char tempC;
char messageDeliverSH[2 * N];

byte tempB;
byte messageDeliverByte[N];

String messageReceive = "";

// Message will be delivered
String messageSend = "This is message ";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  
  // Setting Key for Cryptography
  crypto_feed_watchdog();
  cipher.setKey(key, cipher.keySize());

  // Print message send in string
  Serial.print("Message Send in String : ");
  Serial.println(messageSend);

  // String to Byte
  messageSend.getBytes(buff, N + 1);
  for (int i = 0; i < N; i++) {
    messageSendByte[i] = buff[i];
  }

  // print message send in byte
  Serial.print("Message Send in Byte : ");
  for (int i = 0; i < N; i++) {
    Serial.print(int(messageSendByte[i]));
    Serial.print(" ");
  }
  Serial.println();

  // Byte to Encryption
  cipher.encryptBlock(Encrypt, messageSendByte);

  // print message encryption in byte
  Serial.print("Encryption in Byte : ");
  for (int i = 0; i < N; i++) {
    Serial.print(int(Encrypt[i]));
    Serial.print(" ");
  }
  Serial.println();

  // Encryption to String

  //    HEX in String
  for (int i = 0; i < N; i++) {
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
      messageDeliverSH[2 * i + j] = tempC;
    }
  }
 memset(messageDeliverSH + 2*N, NULL, 2);
   
  // Print message deliver in string HEX
  Serial.print("Message deliver in String HEX : ");
  Serial.println(messageDeliverSH);

  // String HEX to Byte
  for (int i = 0; i < 2 * N; i++) {
    switch (messageDeliverSH[i]) {
      case '0' :
        tempB = 0x00;
        break;
      case '1' :
        tempB = 0x01;
        break;
      case '2' :
        tempB = 0x02;
        break;
      case '3' :
        tempB = 0x03;
        break;
      case '4' :
        tempB = 0x04;
        break;
      case '5' :
        tempB = 0x05;
        break;
      case '6' :
        tempB = 0x06;
        break;
      case '7' :
        tempB = 0x07;
        break;
      case '8' :
        tempB = 0x08;
        break;
      case '9' :
        tempB = 0x09;
        break;
      case 'A' :
        tempB = 0x0A;
        break;
      case 'B' :
        tempB = 0x0B;
        break;
      case 'C' :
        tempB = 0x0C;
        break;
      case 'D' :
        tempB = 0x0D;
        break;
      case 'E' :
        tempB = 0x0E;
        break;
      case 'F' :
        tempB = 0x0F;
        break;
    }
    if ((i % 2) == 0) {
      messageDeliverByte[i / 2] = tempB;
    } else {
      messageDeliverByte[(i - 1) / 2] = ( messageDeliverByte[(i - 1) / 2] << 4) ^ tempB;
    }
  }

  // print message encryption back to byte
  Serial.print("Message Deliver Byte : ");
  for (int i = 0; i < N; i++) {
    Serial.print(int(messageDeliverByte[i]));
    Serial.print(" ");
  }
  Serial.println();

  // Byte to Decryption
  cipher.decryptBlock(Decrypt, messageDeliverByte);

  // print message decryption in byte
  Serial.print("Decryption in Byte : ");
  for (int i = 0; i < N; i++) {
    Serial.print(int(Decrypt[i]));
    Serial.print(" ");
  }
  Serial.println();

  // Decryption to String
  for (int i = 0; i < N; i++) {
    messageReceive = String(messageReceive+ char(Decrypt[i]));
  }

  // print message receive
  Serial.print("message receive : ");
  Serial.println(messageReceive);

}

void loop() {
  // put your main code here, to run repeatedly:

}
