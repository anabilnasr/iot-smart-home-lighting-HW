#include <WiFi.h>
#include <PubSubClient.h>
#include <String>
// Update these with values suitable for your network.
const char* ssid = "Mohamed ElKhouly";
const char* password = "Mm159357";
const char* mqtt_server = "46.101.2.154";
#define mqtt_port 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_SERIAL_PUBLISH_Living "home/Living-Room/movement"
#define MQTT_SERIAL_PUBLISH_Kitchen "home/Kitchen/movement"
#define MQTT_SERIAL_PUBLISH_Bedroom "home/Bedroom/movement"
#define MQTT_SERIAL_PUBLISH_Garage "home/Garage/movement"

#define MQTT_SERIAL_RECEIVER_Living "home/Living-Room/lighting"
#define MQTT_SERIAL_RECEIVER_Kitchen "home/Kitchen/lighting"
#define MQTT_SERIAL_RECEIVER_Bedroom "home/Bedroom/lighting"
#define MQTT_SERIAL_RECEIVER_Garage "home/Garage/lighting"

int LivingBulb = 26;                
int LivingPIR = 27;
int LivingStatus=LOW; 
int KitchenBulb = 33;                
int KitchenPIR = 32; 
int KitchenStatus=LOW;
int BedroomBulb = 35;                
int BedroomPIR = 34; 
int BedroomStatus=LOW;
int GarageBulb = 18;                
int GaragePIR = 19;
int GarageStatus =LOW;
int relay = 32;
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;
WiFiClient wifiClient;

PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish("/enter topic name/", "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_Living);
      client.subscribe(MQTT_SERIAL_RECEIVER_Kitchen);
      client.subscribe(MQTT_SERIAL_RECEIVER_Bedroom);
      client.subscribe(MQTT_SERIAL_RECEIVER_Garage);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void callback(char* topic, byte *payload, unsigned int length) {
    String rtopic(topic);
    String rdata="";
    for(int i = 0 ; i < length ; i++){
      rdata+=(char)payload[i];
    } 
    if(rtopic =="home/Living-Room/lighting"){
      if(rdata=="on"){
        LivingStatus=HIGH;
      }
      else if(rdata == "off"){
        LivingStatus=LOW;
      }
    }
    else if(rtopic == "home/Kitchen/lighting"){
      if(rdata=="on"){
        KitchenStatus=HIGH;
      }
      else if(rdata == "off"){
        KitchenStatus=LOW;
      }
    }
    else if(rtopic == "home/Bedroom/lighting"){
      if(rdata=="on"){
        BedroomStatus=HIGH;
      }
      else if(rdata == "off"){
        BedroomStatus=LOW;
      }
    }
    else if(rtopic == "home/Garage/lighting"){
      if(rdata=="on"){
        GarageStatus=HIGH;
      }
      else if(rdata == "off"){
        GarageStatus=LOW;
      }
    }
    
}
bool PIR(){
  val = digitalRead(LivingPIR);  // read input value
  
  if (val == HIGH)  // check if the input is HIGH
  {            
    LivingStatus=HIGH;  // turn LED ON
  
    if (pirState == LOW && GarageStatus==LOW) 
  {
      client.publish(MQTT_SERIAL_PUBLISH_Garage,"on"); // print on output change
      Serial.println("7aramy");
      pirState = HIGH;
    }
  } 
  else 
  { 
    if (pirState == HIGH)
  {
      pirState = LOW;
    }
  }
}
void setup() {
  pinMode(LivingBulb, OUTPUT);      // declare LED as output
  pinMode(relay, OUTPUT);
  pinMode(LivingPIR, INPUT);     // declare sensor as input
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_Living, serialData);
}
void loop() {
   client.loop();
   PIR();
   digitalWrite(relay,HIGH);
   delay(2000);
   digitalWrite(relay,LOW);
   delay(2000);
   
 }
