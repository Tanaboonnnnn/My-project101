#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
// #define DHTTYPE DHT11
#define Buzer 4
#define MQ_4 35
#define fai 27
int gren,rod;
int R = 25;
int G = 32;
int B = 33;

//=================================================================================================
WiFiClient espClient;
PubSubClient client(espClient);
//=================================================================================================
const char* ssid = "CENextGen-IoT-2p4G";  //wifi name
const char* password = "123456789";
//=================================================================================================
const char* mqtt_broker = "broker.hivemq.com";
const int mqtt_port = 1883;
//=================================================================================================
// const int sensor_rain = 34;
// // const int sensor_dht11 = 15;
// const int relay_fan = 33;
// const int relay_ligth = 32;
long long time_start;

int curState = 0;
int lastState = 1;

// DHT dht(sensor_dht11, DHTTYPE);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  // if(strcmp(topic,"smartHom/actuator/light")==0)
  // {
  //   Serial.println((char*)payload);
  //   if(strcmp((char*)payload,"on")==0)
  //   {
  //     digitalWrite(relay_ligth,HIGH);
  //     Serial.println("high");
  //   }
  //   else
  //   {
  //     digitalWrite(relay_ligth,LOW);
  //     Serial.println("low");
  //   }
  // }else if (strcmp(topic, "smartHom/actuator/fan") == 0) {
  //   // Code to control the fan

  //   if (strcmp((char*)payload, "on") == 0) {
  //     digitalWrite(relay_fan,HIGH);
  //     Serial.println("Fan turned on");
  //   } else {
  //     digitalWrite(relay_fan,LOW);
  //     Serial.println("Fan turned off");
  //   }}
  // your code to control fan
}

void reconnect() {
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str()))
      Serial.println("Public emqx mqtt broker connected");
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  //   pinMode(relay_ligth,OUTPUT);
  //   digitalWrite(relay_ligth,LOW);
  //   pinMode(relay_fan,OUTPUT);
  //   digitalWrite(relay_fan,LOW);

  Serial.begin(9600);
  // dht.begin();
  setup_wifi();
  reconnect();
  //  // client.publish("smartHom/sensor/rain", "false");
  //   client.subscribe("smartHom/actuator/#");
  pinMode(Buzer, OUTPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(fai, OUTPUT);
}
int sensorValue;

void red() {
  rod += 50;
  if (rod >= 254) {
    rod = 0;
  }
  analogWrite(R, rod);
  analogWrite(G, 0);
  analogWrite(B, 0);
}
void white() {
   
  analogWrite(R, 255);
  analogWrite(G, 255);
  analogWrite(B, 255);
}
void pidfai(){analogWrite(R, 0);
  analogWrite(G, 0);
  analogWrite(B, 0);}

void loop() {
  client.loop();

  if (millis() - time_start >= 1500) {

    sensorValue = analogRead(35);
    char buffersmoke[40];
    sprintf(buffersmoke, "%d", sensorValue);
    client.publish("toilet/1/number", buffersmoke);
    // Serial.println(analogRead(35));
    time_start = millis();
    if (sensorValue >= 1500) {
      Serial.println("true");
      digitalWrite(Buzer,LOW);
      digitalWrite(fai, HIGH);
      curState = 1;
      red();
    } else if (sensorValue < 1500) {
      Serial.println("false");
      digitalWrite(Buzer, HIGH);
      digitalWrite(fai, LOW);
      curState = 0;
      white();
    }

    if (lastState != curState) {
      if (curState == 1) {

        client.publish("toilet/1/text", "มีคนสูบบุหรี่");

      } else {

        client.publish("toilet/1/text", "ไม่มีคนสูบบุหรี่");
      }
    }

    lastState = curState;

    // char buffersmoke[40];
    // sprintf(buffersmoke, "%d", sensorValue);

    //  client.publish("toilet/1", buffersmoke);
    // client.publish("smartHom/sensor/humid", "b");
  }
}