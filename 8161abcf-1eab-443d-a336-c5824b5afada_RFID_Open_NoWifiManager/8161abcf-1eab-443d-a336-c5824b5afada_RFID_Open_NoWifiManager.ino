#include <ESP8266WiFi.h>
#include "MFRC522.h"
#include <PubSubClient.h>
#include "config.h"
#include <SPI.h>

void setup() {
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED1, OUTPUT);
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED2, HIGH);
  Serial.begin(115200);
  Serial.println(F("Booting...."));
  setup_wifi();
  printMACAddress();
  client.setServer(mqtt_server, mqtt_port);
//  client.setCallback(callback);
  delay(100);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println(F("Ready!"));
  Serial.println(F("======================================================"));
  Serial.println(F("Scan for Card and print UID:"));
  watchdogSetup();
}

String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

void printMACAddress() {
  WiFi.macAddress(mac);
  MACAddress = mac2String(mac);
  Serial.println(MACAddress);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String sTopic = topic;
  
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    digitalWrite(LED1, LOW);
    digitalWrite(BUZZER, HIGH);
    delay(500);
    digitalWrite(BUZZER, LOW);
    delay(1500);
    Serial.println(mqtt_server);
    if (client.connect(device_guid, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      digitalWrite(LED1, HIGH);
//      client.subscribe(mqtt_keywords2);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      ESP.restart();
      delay(5000);
    }
  }
}

void watchdogSetup(void) {
  ESP.wdtDisable();
}

unsigned long previousMillis = 0;
const long interval = 1000;

void loop() {
unsigned long currentMillis = millis();
  if (!client.connected()) {
      reconnect();
    }
    client.loop();
  if ((currentMillis - previousMillis) >= interval)
    {
      previousMillis = currentMillis;
      loop_count++;
      ESP.wdtFeed();
      Serial.print(loop_count);
      Serial.print(". Watchdog fed in approx. ");
      Serial.print(loop_count * 1000);
      Serial.println(" milliseconds.");
  }
  else
  {
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      digitalWrite(BUZZER, LOW);
      delay(550);
      return;
    }

    if ( ! mfrc522.PICC_ReadCardSerial()) {
      digitalWrite(BUZZER, LOW);
      delay(900);
      return;
    }
    char msg[150];
    char msg1[150];
    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    msg[0] = char(0);
    msg1[0] = char(0);
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      sprintf(msg, "%s0x%02x ", msg, mfrc522.uid.uidByte[i]);
    }
    String rfid=String(msg);
    rfid.replace(" ","");
    String kirim = String(device_guid)+"#"+rfid;
    Serial.println();
    Serial.print("Publish message: ");
    Serial.println(kirim);
    if((client.publish(mqtt_keywords1, kirim.c_str()) == true) ){
      tone(BUZZER,3000,500);
      delay(1000);
    }
    msg[0] = char(0);
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED1, HIGH);
    delay(220);
    digitalWrite(LED1, LOW);
    delay(100);
    digitalWrite(LED1, HIGH);
    delay(200);
    digitalWrite(LED1, LOW);
    delay(100);
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
    digitalWrite(LED1, HIGH);
  }
}
