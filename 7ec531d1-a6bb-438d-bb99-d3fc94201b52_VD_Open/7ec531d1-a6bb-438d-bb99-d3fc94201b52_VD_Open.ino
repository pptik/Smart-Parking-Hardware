
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// Update these with values suitable for your network.

const char* ssid = "SmartParking"; //ssid/nama wifi  
const char* password = "1234567890"; //password wifi
const char* mqtt_server = "192.168.0.2"; //server mqtt
const char* mqtt_user = "/parkir:parkir"; //user mqtt
const char* mqtt_pass = "parkir123"; //password mqtt
const char* mqttQueuePublish = "VD-Open";
const char* CL = "VD-Open"; //nama alat
const char* device_guid = "848e7a76-a6d5-42ef-810c-a16a8b6791d1";

int sensor = D1;
int statussensor = 0;

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

  Serial.println(message);
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
//      client.subscribe("Parkir"); //mengambil data dari antrian received
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
  pinMode(sensor, INPUT_PULLUP);
  Serial.begin(115200);
  setup_wifi();
  printMACAddress();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

    if (digitalRead(sensor) == LOW){
      statussensor = 1;
      Serial.println("Mobil Terdeteksi");
      }
     else if (digitalRead(sensor)==HIGH){
      statussensor = 0;
      Serial.println("Mobil Tidak Terdeteksi");
      }
    Serial.println(statussensor);
    String input_guid = String(device_guid);
    String dataSendtoMqtt = String(input_guid + "#" + String(statussensor));
    client.publish(mqttQueuePublish, dataSendtoMqtt.c_str());
    Serial.println(dataSendtoMqtt);
    delay(1000);
}
