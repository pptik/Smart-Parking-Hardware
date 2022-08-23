
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// Update these with values suitable for your network.
const char* device_guid = "432ec8ac-4ffa-4905-9a73-665c953db6ea";
const char* ssid = "SmartParking"; //ssid/nama wifi  
const char* password = "1234567890"; //password wifi
const char* mqtt_server = "192.168.0.2"; //server mqtt
const char* mqtt_user = "/parkir:parkir"; //user mqtt
const char* mqtt_pass = "parkir123"; //password mqtt
const char* mqttQueuePublish = "Gate-Open";
const char* CL = device_guid; //nama alat

int relay1 = D1;
int relay2 = D2;
int relay3 = D3;
int relay4 = D4;

String kondisi_gate = "Tertutup";
int loop_count  = 0 ;

char msg[100];
WiFiClient espClient;
PubSubClient client(espClient);

byte mac[6];
String MACAddress;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
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

char sPayload[100];
char message [40] ;
char address[40];
void callback(char* topic, byte* payload, unsigned int length) {
  memcpy(sPayload,payload,length);
  memcpy(address,payload,36);
  memcpy(message,&payload[37],length-37);
  Serial.print("Message arrived [");
  Serial.print(sPayload);
  Serial.println("] ");

  Serial.println(device_guid);
  Serial.println(address);
  if(String((char *)address) == String((char *)device_guid)) 
  {
    Serial.println("tersambung");
  }
  else
  { 
    Serial.println("tidak tersambung");
    return;    
  }

  if (message [0] == '1') {
    digitalWrite(relay1,HIGH);
    digitalWrite(relay2,LOW);
    digitalWrite(relay3,LOW);
    digitalWrite(relay2,LOW);
    Serial.println("Gate Terbuka");
    kondisi_gate = "Terbuka";
  }
  if (message [0] == '0' ) {
    digitalWrite(relay1,LOW);
    digitalWrite(relay2,HIGH);
    digitalWrite(relay3,LOW);
    digitalWrite(relay4,LOW);
    Serial.println("Gate Tertutup");
    kondisi_gate = "Tertutup";
  }
}

void reconnect() {
  // Loop until we're reconnected
  printMACAddress();
  const char* CL;
  CL = MACAddress.c_str();
  Serial.println(CL);
  while (!client.connected()) {     
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CL, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(mqttQueuePublish); //mengambil data dari antrian received
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      ESP.restart();
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);

  Serial.begin(115200);
  setup_wifi();
  printMACAddress();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  watchdogSetup();
}

String previous_kondisi;
unsigned long previousMillis = 0;
const long interval = 1000;

void loop() {
  unsigned long currentMillis = millis();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if (previous_kondisi != kondisi_gate)
  {
    previous_kondisi = kondisi_gate;
    Serial.println(kondisi_gate);
    String input_guid = String(device_guid);
    String dataSendtoMqtt = String(input_guid + "#" + String(kondisi_gate));
    client.publish(mqttQueuePublish, dataSendtoMqtt.c_str());
    Serial.println(dataSendtoMqtt);
  }
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
}

void watchdogSetup(void) {
  ESP.wdtDisable();
//  ESP.wdtEnable(WDTO_8S);
}
