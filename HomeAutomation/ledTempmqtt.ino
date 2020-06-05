#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>

#define LED D0
#define DHTPIN D2
#define DHTTYPE DHT11
#define TOPIC "relay" 
DHT dht(DHTPIN, DHTTYPE);

Servo myservo ;

const char* ssid = "wifi";
const char* psw = "psw";

const char* mqttServer = "host";
const int mqttPort = port;
const char* mqttUser = "userName";
const char* mqttPsw = "password" ;

WiFiClient espClient;
PubSubClient client(espClient);

void MQTTcallback(char *topic,byte *payload,unsigned int length){
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
 
  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];  //Conver *byte to String
  }
   Serial.print(message);
  if(message == "d0_on") {digitalWrite(LED,HIGH);}   //LED on  
  if(message == "d0_off") {digitalWrite(LED,LOW);} //LED off
  if(message == "perde_ac") { myservo.write(0);}
  if(message == "perde_kapat"){myservo.write(130);}
  Serial.println();
  Serial.println("-----------------------"); 
}

void wifisetup(){
  WiFi.begin(ssid, psw);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());
  }

void reconnect(){
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266", mqttUser, mqttPsw )) {
 
      Serial.println("connected");
      client.subscribe("relay");  
 
    } else {
  
      Serial.print("failed with state ");
      Serial.println(client.state());  //If you get state 5: mismatch in configuration
      delay(2000);
 
    }
  }
}
void setup() {
  delay(500);
  pinMode(LED,OUTPUT);
  myservo.attach(5);
  Serial.begin(9600);
  dht.begin();
  wifisetup();
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(MQTTcallback);
  
  client.publish("ESP8266","Hello from ESP");
  client.subscribe("relay");
}


void ServoBaslat(){;
   myservo.write(0);
   delay(100);
  }
void ServoKapat(){
   myservo.write(120);
   delay(100);
}




bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}


long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
   long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float newTemp = dht.readTemperature();
    float newHum = dht.readHumidity();

    if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      client.publish(TOPIC, String(temp).c_str());
    }

    if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      Serial.print("New humidity:");
      Serial.println(String(hum).c_str());
      client.publish(TOPIC, String(hum).c_str());
    }
  }
}
