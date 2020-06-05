#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <MFRC522.h>
#include <SPI.h>

#define RST_PIN D3 // RST-PIN for RC522 - RFID - SPI - Module GPIO15 
#define SS_PIN  D8  // SDA-PIN for RC522 - RFID - SPI - Module GPIO2
MFRC522 rfid(SS_PIN, RST_PIN);   

byte ID[4]= {41,100,219,42};
//byte ID1[4]= {69, 197, 233, 169};
#define LED D0
#define DHTPIN D2
#define DHTTYPE DHT11
#define TOPIC "relay" 
DHT dht(DHTPIN, DHTTYPE);

Servo myservo ;

const char* ssid = "wifi";  const char* psw = "wifiPsw";

const char* mqttServer = "host";
const int mqttPort = "portNumber";
const char* mqttUser = "user";
const char* mqttPsw = "psw" ;

int buzzer = 2;

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
  if(message == "perde_kapat"){myservo.write(340);}
  Serial.println();
  Serial.println("-----------------------"); 
}
//setting up wifi connection
void wifisetup(){
  WiFi.begin(ssid, psw);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());
  }
// connect as a client agan when disconnected
void reconnect(){
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266", mqttUser, mqttPsw )) {
 
      Serial.println("connected");
      client.subscribe("relay");

    } else {
  
      Serial.println("failed with state ");
      Serial.println(client.state());  //If you get state 5: mismatch in configuration
      delay(2000);
 
    }
  }
}

void setup() {
  delay(500);
  pinMode(LED,OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(A0,INPUT);
  myservo.attach(5);
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  dht.begin();
  wifisetup();
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(MQTTcallback);
  client.subscribe("relay");
  client.publish("relay","Hello from ESP");
  
}

//Pir sensor

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

 // mq2 okuma
  int value = analogRead(A0);
  Serial.println(value);
  if ( value > 1000){
    digitalWrite(buzzer,HIGH);
    Serial.println("Gaz kacagı var");
    client.publish("ALERT","GAZ KACAGII!!");
    delay(10000);
  }
  else {
    digitalWrite(buzzer,LOW);
    delay(500);
  }

  
   
//sıcaklık okuma ve guncelleme
   long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float newTemp = dht.readTemperature();
    client.publish("sicaklik", String(newTemp).c_str());
    float newHum = dht.readHumidity();
    client.publish("nem", String(newHum).c_str());

    if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;
      Serial.println("New temperature:");
      Serial.println(String(temp).c_str());
      client.publish("sicaklik", String(temp).c_str());
    }

    if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      Serial.println("New humidity:");
      Serial.println(String(hum).c_str());
      client.publish("nem", String(hum).c_str());
    }

    //rfid kart okuma
  if(! rfid.PICC_IsNewCardPresent())
    return;
   if(! rfid.PICC_ReadCardSerial())
    return;
   //Dogru kartın gelip gelmediğini kontrol et
   if(rfid.uid.uidByte[0] == ID[0] && 
   rfid.uid.uidByte[1] == ID[1] && 
   rfid.uid.uidByte[2] == ID[2] && 
   rfid.uid.uidByte[3] == ID[3]){ 
    Serial.print("Kapi Acildi");
    client.publish("relay","Kapi açıldı");
    digitalWrite(LED , HIGH);
    delay(3000);
   }
   else{
    Serial.print("Yetkisiz Giris");
   client.publish("ALERT","YETKISIZ GIRIS");
   }
   rfid.PICC_HaltA();
  
  }

 
}
