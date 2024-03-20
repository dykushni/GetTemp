#include <DHT.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h>

#define ONE_WIRE_BUS D6
#define DH11_BUS D5
#define DHTTYPE DHT11
#define DSINC D4

#define TASK1 D1
#define TASK2 D2
#define TASK3 D3


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DH11_BUS, DHTTYPE);

float temps[4];
int chartupd = 25;
int task1 =9;
int task2 =9;
int task3 =9;


void setup() {
   Serial.begin(9600);
     Serial.println("start");
  
  
      pinMode(TASK1, OUTPUT);
      pinMode(TASK2, OUTPUT);
      pinMode(TASK3, OUTPUT);
      pinMode(DSINC, OUTPUT);

      digitalWrite(TASK1,1);
      digitalWrite(TASK2,1);
      digitalWrite(TASK3,1);
      digitalWrite(DSINC,LOW);
      
         
          sensors.begin();
          dht.begin();
          
          WiFi.mode(WIFI_STA);
          WiFi.begin("D_117", "tomas_2310");
  
    }
  
void getsensors () {
    
           char url[256]="";         
           float temp[4]; 
           int err =0;
           sensors.requestTemperatures(); 
           delay(750);
                 
           temp[0] = sensors.getTempCByIndex(0); 
           temp[1] = sensors.getTempCByIndex(1);
           
           temp[2] =dht.readTemperature();
           temp[3] =dht.readHumidity();  
             

               
               sprintf (url, "http://tmp1.h1n.ru/setData.php?login=tomas&t1=%.2f&t2=%.2f&t3=%.2f&t4=%.2f&hw=ESP8266&er=%d",temp[0],temp[1],temp[2],temp[3],err);
              
               Serial.println(url);

                          WiFiClient client;
                          HTTPClient http; 
                          http.begin(client, url);
                          
                          int httpCode = http.GET();
                          Serial.printf("[HTTP] GET... code: %d\n", httpCode);  
            
                          if(httpCode == 200)  {  
                            Serial.println("Send OK"); 

                            String payload = http.getString();

                            int chartupd_0 = pars(payload,"upd=");
                            int task1_0 = pars(payload,"task1=");
                            int task2_0 = pars(payload,"task2=");
                            int task3_0 = pars(payload,"task3=");

                            if(chartupd_0 != NULL )chartupd=chartupd_0;
                            if(task1_0 != NULL )task1=task1_0;
                            if(task2_0 != NULL )task2=task2_0;
                            if(task3_0 != NULL )task3=task3_0;
                            
                            int sinc = pars(payload,"sin=");
                            if(sinc == 1) {digitalWrite(DSINC,1);}
    
                   
               
                          }
                                         
                          http.end(); 
                        
                         
                   }


  void loop() 
  {
     
  if(WiFi.status() == WL_CONNECTED) 
          {            
            getsensors();
            
            }

  switch (task1) {
    case 1:   
    digitalWrite(TASK1, HIGH); 
    break;
    
    case 2:    
    digitalWrite(TASK1, HIGH); 
    delay(500);
    digitalWrite(TASK1, LOW); 
    task1 =0;
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
    task2 =0;
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
    task3 =0;
    break;
    case 0:    
    digitalWrite(TASK3, LOW); 
    break;
 
  }
  
  delay(chartupd*1000); 

  }

/*
  int pars_array(String buffer,const char* id,int result[7])  
    {
       int i=0;
    
  
       int st=strlen(id);
       int firstsimbol;
        
     int nacalo_stroki = buffer.indexOf(id);
     firstsimbol = nacalo_stroki + st;
     
     for (i=0;i<7;i++) 
     {
     
     int rasdelitel = buffer.indexOf(",",firstsimbol+1);
     String value = buffer.substring(firstsimbol, rasdelitel);
     
     if( value.toInt() )
     { 
        result[i] = value.toInt(); 
        firstsimbol = rasdelitel +1 ;
        result[i] = value.toInt();
   
     }}}
*/


      int pars(String buffer,const char* id) 
    {
     
      int st=strlen(id);
      
      int nacalo_stroki = buffer.indexOf(id);
      int rasdelitel = buffer.indexOf(";",nacalo_stroki+1);
      String value = buffer.substring(nacalo_stroki+st, rasdelitel);
      
    
       if( value.toInt() ) return value.toInt();
       else return NULL;
        
      }
