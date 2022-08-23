#ifndef __CONFIGFILE__
#define __CONFIGFILE__

// PPTIK Setting
char wifi_ssid[34] = "LSKKWork";
char wifi_password[34] = "1234567890";
char mqtt_server[40] = "192.168.0.2";
int  mqtt_port = 1883;
char smqtt_port[5] = "1883";
char mqtt_user[40] = "/parkir:parkir";
char mqtt_password[40] = "parkir123";
char mqtt_keywords1[40] = "RFID-Open";
char device_guid[40] = "8161abcf-1eab-443d-a336-c5824b5afada";
int loop_count  = 0 ; //loop count loop

byte mac[6];
String MACAddress; 
WiFiClient espClient;
PubSubClient client(espClient);
                     
#define RST_PIN  5  // RST-PIN für RC522 - RFID - SPI - Modul GPIO5 
#define SS_PIN  4  // SDA-PIN für RC522 - RFID - SPI - Modul GPIO4 
#define BUZZER  D3 // pcb merah (D3) (kalau hijau, biru D8) 
#define LED1 D8// pcb merah (kalau hijau,biru d4)
#define LED2 D4 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
#endif
