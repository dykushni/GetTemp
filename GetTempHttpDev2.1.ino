  #include <DHT.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define ONE_WIRE_BUS D6
#define DH11_BUS D4
#define DHTTYPE DHT11
#define DSINC D3
  
#define TASK1 D6
#define TASK2 D7
#define TASK3 D8


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DH11_BUS, DHTTYPE);

float temps[4];
int chartupd = 25;
int task1 = 9;
int task2 = 9;
int task3 = 9;
unsigned long son = 40000;
unsigned long timing;

void setup() {
  Serial.begin(9600);
  Serial.println("start");


  pinMode(TASK1, OUTPUT);
  pinMode(TASK2, OUTPUT);
  pinMode(TASK3, OUTPUT);
  pinMode(DSINC, OUTPUT);

  digitalWrite(TASK1, 1);
  digitalWrite(TASK2, 1);
  digitalWrite(TASK3, 1);
  digitalWrite(DSINC, LOW);


  sensors.begin();
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin("D_117", "tomas_2310");


ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  

}

void getsensors () {

  char url[256] = "";
  float temp[4];
  float at5[3];
  float at6[3];
  float at7[3];
  int err = 0;
  //   sensors.requestTemperatures();
  delay(750);

  

  for(int m=0;m<3;m++) 
  {
      at5[m] = dht.readTemperature();
      at6[m] = dht.readHumidity();
      at7[m] = sensors.getTempCByIndex(0);
      
      delay(20);
  }

  float t5 = middle(at5[0],at5[1],at5[2]);
  float t6 = middle(at6[0],at6[1],at6[2]);
  float t7 = middle(at7[0],at7[1],at7[2]);
  
  sprintf (url, "http://tmp1.h1n.ru/setData.php?login=tomas&t5=%.2f&t6=%.2f&t7=%.2f&hw=ESP8266v2&er=%d", t5,t6,t7,err);

  Serial.println(url);

  WiFiClient client;
  HTTPClient http;
  http.begin(client, url);

  int httpCode = http.GET();
  Serial.printf("[HTTP] GET... code: %d\n", httpCode);

  if (httpCode == 200)  {
    Serial.println("Send OK");

    String payload = http.getString();

    int chartupd_0 = pars(payload, "upd=");
    int task1_0 = pars(payload, "task1=");
    int task2_0 = pars(payload, "task2=");
    int task3_0 = pars(payload, "task3=");

    if (chartupd_0 != NULL )son = chartupd_0 *1000;
    if (task1_0 != NULL )task1 = task1_0;
    if (task2_0 != NULL )task2 = task2_0;
    if (task3_0 != NULL )task3 = task3_0;

    int sinc = pars(payload, "sin=");
    if (sinc == 1) { digitalWrite(DSINC, 1);}
    
    }

  http.end();


}


void loop()
{


    if (millis()-timing>son && WiFi.status() == WL_CONNECTED ) 
    {
       digitalWrite(DSINC, LOW);
       getsensors();
       timing = millis(); 
       }
 
  

  switch (task1) {
    case 1:
      digitalWrite(TASK1, HIGH);
      break;

    case 2:
      digitalWrite(TASK1, HIGH);
      delay(500);
      digitalWrite(TASK1, LOW);
      task1 = 0;
      break;

    case 0:
      digitalWrite(TASK1, LOW);
      break;
  }


  switch (task2) {
    case 1:
      digitalWrite(TASK2, HIGH);
      break;
    case 2:
      digitalWrite(TASK2, HIGH);
      delay(500);
      digitalWrite(TASK2, LOW);
      task2 = 0;
      break;
    case 0:
      digitalWrite(TASK2, LOW);
      break;

  }

  switch (task3) {
    case 1:
      digitalWrite(TASK3, HIGH);
      break;
    case 2:
      digitalWrite(TASK3, HIGH);
      delay(500);
      digitalWrite(TASK3, LOW);
      task3 = 0;
      break;
    case 0:
      digitalWrite(TASK3, LOW);
      break;

  }
  ArduinoOTA.handle();
  
  delay(5);

}

float  middle(float a, float b, float c) 
{
    float middle;
    if ((a <= b) && (a <= c)) {
      middle = (b <= c) ? b : c;
    }
    else {
   
    if ((b <= a) && (b <= c)) {
        middle = (a <= c) ? a : c;
    }
    else  middle = (a <= b) ? a : b;
      
    }
  return middle;
}


int pars(String buffer, const char* id)
{

  int st = strlen(id);

  int nacalo_stroki = buffer.indexOf(id);
  int rasdelitel = buffer.indexOf(";", nacalo_stroki + 1);
  String value = buffer.substring(nacalo_stroki + st, rasdelitel);


  if ( value.toInt() ) return value.toInt();
  else return NULL;

}
