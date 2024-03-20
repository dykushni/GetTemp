#include <DHT.h>
#include <DHT_U.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include <TM1637Display.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>
#include <math.h> 
#define SERIAL_BUFFER_SIZE 64
#define DLS1820   11     //dls1820
#define DLS1820DOP   9  //dls1820dop

#define DH11_BUS  12     //temp + h
#define DHTTYPE DHT11



#define DH10_BUS  10     //temp + h
//#define DHTTYPE DHT10

                  
#define RESET_KEY A4
#define RESET_SIM 8
#define TASK1 10
#define TASK2 2
#define TASK3 3
#define LED_PIN 13
#define SPEED_TERM 9600

#define RX 5
#define TX 4

#define CLK 6
#define DIO 7


OneWire oneWire(DLS1820);
DallasTemperature sensors(&oneWire);

OneWire oneWireDop(DLS1820DOP);
DallasTemperature sensorsDop(&oneWireDop);

DeviceAddress *sensorsUnique;
DeviceAddress *sensorsUniqueDop;

DHT dht(DH11_BUS, DHTTYPE);
DHT dht10(DH10_BUS, DHTTYPE);

SoftwareSerial GSMport(RX, TX); 
TM1637Display display(CLK, DIO);

int temp_MAX[7] ={95,95,95,95,95,95,95};
int temp_MIN[7]= {-35,-35,-35,-35,-35,-35,-35};

unsigned long son = 130000;
unsigned long timing;

int task1 =9;
int task2 =9;
int task3 =9;
int show_temp =4;
int sendme =0;
int shuher_count =4;
//int sendsms=1;
int sms=0;
int count =1;
int firstrun=0;
int testinet =0;
void(* resetFunc) (void) = 0; //declare reset function @ address 0

void setup() {
  
  Serial.begin(SPEED_TERM); 
  GSMport.begin(SPEED_TERM);
  
  
  pinMode(TASK1, OUTPUT);
  pinMode(TASK2, OUTPUT);
  pinMode(TASK3, OUTPUT);
  pinMode(KEY, INPUT);
  pinMode(LED_PIN, OUTPUT);

  pinMode(RESET_SIM, OUTPUT);
  digitalWrite(RESET_SIM, LOW); 
  delay(200);
  digitalWrite(RESET_SIM, HIGH);
   
   display.clear();
   display.setBrightness(4);

   carusel ();
 
 
  sensors.begin();
  sensorsDop.begin();
  dht.begin();
          
           
           
           //dht10.begin();
           //dht10.readTemperature();
           //dht10.readHumidity();
           //Serial.print(dht10.readTemperature());
          // Serial.print(dht10.readHumidity());
  
  
  Serial.println(float(Thermister(analogRead(A7))), 1); // выводим показание 1 датчика 
 
  
  int countSensorsDop = sensorsDop.getDeviceCount();
  Serial.print(F("Found sensors Dop: "));
  Serial.println(countSensorsDop);
  sensorsUniqueDop = new DeviceAddress[countSensorsDop];


  int countSensors = sensors.getDeviceCount();
  Serial.print(F("Found sensors: "));
  Serial.println(countSensors);
  sensorsUnique = new DeviceAddress[countSensors];

   
// digitalWrite(DLS1820, HIGH); 
// digitalWrite(DLS1820DOP, HIGH); 
  
 // delay(5000);
  Gprs_init();
 // delay(1500);
 // Get_clock ();
 // delay(1000);
 // Sendsensors( false );
   
}



void Gprs_init() {  //Процедура начальной инициализации GSM модуля
uint8_t data[] = { 0x6,0x54,0x79,0x78};

display.setSegments(data);

  byte reply = 0;
  int i = 0;
  int sabr =0;

 sendATcommand("AT","OK", 1000);
  delay(1000);
  
  while (i < 10 && reply == 0)
  { 
    reply = sendATcommand("AT+CREG?","+CREG: 0,1", 1000);
    i++;
    delay(1000);
  }
  
    sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK", 1000);
      delay(1000);
    sendATcommand("AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\"", "OK", 1000);
      delay(1000);
    sendATcommand("AT+SAPBR=3,1,\"USER\",\"mts\"", "OK", 1000);
      delay(1000);
  sendATcommand("AT+SAPBR=3,1,\"PWD\",\"mts\"", "OK", 1000);
      delay(3000);
  
  
  i=0;
  while( i<2 && sabr !=1 ) 
  {      
      sabr = sendATcommand("AT+SAPBR=1,1", "OK", 20000);
      delay(2000);
        if(sabr!=1)Serial.println(sendATcommand("AT+SAPBR=0,1", "OK", 1000));
     
      Serial.println(i++);
    }
  
  sendATcommand("AT+SAPBR=2,1", "OK",1000);
  sendATcommand("AT+SAPBR=4,1", "OK",1000);

 testinet++;
 
 }

 
 
void loop() {
       
          time_t t = now();
          String clockf="" ;
          clockf = hour(t) ;
          
          if(minute(t) < 10) clockf = clockf + "0";
          clockf = clockf + minute(t);
   
         display.showNumberDecEx(clockf.toInt(), 0x40, true, 4, 0); 
        
        
        //Serial.println(float(Thermister(analogRead(A7))), 1);
            
        if (millis()-timing>son){

       
      
            if(sendATcommand("AT+SAPBR=2,1","+SAPBR: 1,3,\"0.0.0.0\"",2000)==1) { RestartSim800(0) ;}    
             
            timing = millis(); 
      
             if(count%5==0) Sendsensors( false );
             else  Sendsensors( true );
             
             if(count==4) {  display.showNumberDec(7777, false);  Get_clock (); }
			       if(count==20) {count=0;}
          
           Serial.print("count ="); Serial.println(count++);  
           Serial.print("testinet ="); Serial.println(testinet);  
          } 
    
    if(testinet == 13) { resetFunc(); }
    
    if (analogRead(KEY) == 1023 || firstrun == 0 )
       {

        
       // Serial.println(float(Thermister(analogRead(A7))), 1);
        
        display.showNumberDec(5555, false);
            if(sendATcommand("AT+SAPBR=2,1","+SAPBR: 1,3,\"0.0.0.0\"",2000)==1) { RestartSim800(0) ;}  
        Sendsensors( false );
        delay(3000);
        Get_clock (); 
        
      }

if (analogRead(RESET_KEY) == 0) 
{
   delay(100);
   if (analogRead(RESET_KEY) == 0) {resetFunc();}
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
      

 delay(10);
}


int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100); 
    delay(10); 
    
    while( GSMport.available() > 0) GSMport.read();  
    
    GSMport.println(ATcommand);    
    previous = millis();

  
    do{
      delay(1);
      if(GSMport.available() > 0){    
            response[x] = GSMport.read();
            x++;
       delay(1);
          if (strstr(response, expected_answer) != NULL){  answer = 1;}
          if (strstr(response, "ERROR") != NULL){ answer = -1;}
        }} while((answer == 0) && ((millis() - previous) < timeout)); 

Serial.println(response);
           
return answer;
}



bool Sendsensors ( bool test ) {
  
         
          bool alert = 1;
          float temp0[7];
          int alertid=0;


         /* digitalWrite(DLS1820, LOW); 
          digitalWrite(DLS1820DOP, LOW); 
           delay(200);
          digitalWrite(DLS1820, HIGH); 
          digitalWrite(DLS1820DOP, HIGH);   
          */
          oneWire.reset(); 
          oneWireDop.reset(); 

        //  sensors.begin();
        //  sensorsDop.begin();
      
          delay(750);
          sensors.requestTemperatures(); 
          sensorsDop.requestTemperatures(); 
          delay(2000);
         
                 
          temp0[0] = sensors.getTempCByIndex(0); 
          temp0[1] = sensors.getTempCByIndex(1); 
        
            
           temp0[2] =dht.readTemperature();
           temp0[3] =dht.readHumidity();  
           
           temp0[4] = sensorsDop.getTempCByIndex(0); 
           temp0[5] = sensorsDop.getTempCByIndex(1); 
          
           
           temp0[6] = float(Thermister(analogRead(A7)));
 //          temp0[7] = dht10.readHumidity();
         
  // Serial.print(dht10.readTemperature());
  //  Serial.print(dht10.readHumidity());
/*
  for (int i = 0; i < 7; i++) 
  {
    Serial.println(temp0[i]);
    Serial.print(" MIN="); Serial.print(temp_MIN[i]);
    Serial.print(" MAX="); Serial.println(temp_MAX[i]);
   
    }*/
     
              if ( test  ) 
              {     
               
                  for(int u=0;u<7;u++)
                      { 

                      
                                           if( (temp0[u]  > temp_MAX[u] ||  temp0[u]  < temp_MIN[u] ) && (temp0[u]>(-45) )) 
                                                      { 
                                                        // проверка на выход за пределы температуры алерт =0  значит тревога 
                                                        alert = 0;
                                                        alertid = u;
                                            //            Serial.print("Alert id="); Serial.println(u);
                                           //             Serial.print("temp id="); Serial.println(temp0[u]);
                                                       } 
                      
                       }
                         
                
                  if(alert) {
                      sendme  =0;
                      Serial.println(F("test ok")); 
                    //display.showNumberDec(temp0[show_temp]); 
                      return 0;

                  }
                  else { 
                
                      display.showNumberDec(temp0[alertid]); 
                      
                      if(sendme > shuher_count)  
                     {
                      Serial.print(F("Send Alert OFF"));  
                      Serial.println( sendme++); 
                      return 0;  
                      }
       
                     if(sendme == 1 && sms == 1) 
                     {
                      Serial.println(F("Send SMS"));   
            
                      sendATcommand("AT+CMGF=1", "OK",  500);
                     
                      delay(100);
                      GSMport.println(F("AT+CMGS=\"+79176500205\""));
                      delay(100);
                      GSMport.print(F("Shuher!! datchik-"));
                      GSMport.print(alertid+1);
                      GSMport.print(F("= "));
                      delay(100);
                      GSMport.print(temp0[alertid]);
                      GSMport.print(F(" trevoga-"));GSMport.print(sendme);
                      GSMport.print((char)26);
                      delay(3000);
                      }

                     Serial.print(F("Send Alert ")); 
                     Serial.println( sendme++); ; 
                     } 
                
                }
                else 
                {
                 Serial.println(F("Send standart")); 
               }
              

              display.showNumberDec(temp0[show_temp]); 
              digitalWrite(LED_PIN, LOW);               

              sendATcommand("AT+HTTPINIT", "OK",  3500);
              sendATcommand("AT+HTTPPARA=\"CID\",1", "OK",  3500);
              testinet++;
              delay(2000);
              
              GSMport.print(F("AT+HTTPPARA=\"URL\",\"http://tmp1.h1n.ru/setData.php?login=dnantosh&t1="));GSMport.print(temp0[0]); // 29.01.2020
              GSMport.print(F("&t2="));GSMport.print(temp0[1]);
              GSMport.print(F("&t3="));GSMport.print(temp0[2]);              
              GSMport.print(F("&t4="));GSMport.print(temp0[3]);
              GSMport.print(F("&t5="));GSMport.print(temp0[4]);
              GSMport.print(F("&t6="));GSMport.print(temp0[5]);
              GSMport.print(F("&t7="));GSMport.print(temp0[6]);
            //  GSMport.print(F("&t8="));GSMport.print(temp0[7]);
              GSMport.print(F("&err="));GSMport.print(sendme);
              GSMport.println(F("&hw=SIM800v14\""));

              delay(2000);
              
              sendATcommand("AT+HTTPACTION=0","+HTTPACTION:0,200",10000);
			
			  
              if(sendATcommand("AT+HTTPREAD","+HTTPREAD:",30000)==1) 
                {

			        	  Serial.println(F("Send read")); 
				  
                  String v0= ReadGSM();
                  const char *str2 = v0.c_str();
       
                  pars_array(str2,"rt=",temp_MAX);
                  pars_array(str2,"rf=",temp_MIN);
         
                 int task1_0 = pars(str2,"task1=");
                 int task2_0 = pars(str2,"task2=");
                 int task3_0 = pars(str2,"task3=");
                 int shuher_count_0 = pars(str2,"shuher_count=");
                 int sms_0 = pars(str2,"sms=");
                 int show_temp_0 = pars(str2,"show_temp=");         
                 int sinc=pars(str2,"sin=");
                 int chartupd = pars(str2,"upd=");
                  
                  str2=NULL;
                  v0="";
                  
                  
                 if(chartupd > 20) { son = (chartupd/5);son=son*1000;}
                 if(task1_0 != NULL && task1_0 < 10)task1=task1_0;
                 if(task2_0 != NULL && task2_0 <10 )task2=task2_0; 
                 if(task3_0 != NULL && task3_0 <10)task3=task3_0;
                 if(shuher_count_0 != NULL && shuher_count_0 <10)shuher_count=shuher_count_0;
                 
                 if(sms_0 != NULL && sms_0 <10) sms=sms_0;
                 if(show_temp_0 != NULL && show_temp_0<10) show_temp=show_temp_0;
                 

                   switch (sinc) {
                         case 1:   
                          digitalWrite(LED_PIN, HIGH); 
                          firstrun = 1;
                          testinet=0;
                         break;
                         case 9999:
                              resetFunc(); 
                         break;
                           }
                    
                  Serial.println(chartupd); Serial.println(sms); Serial.println(task1); Serial.println(task2); Serial.println(task3); Serial.println(show_temp ); Serial.println(sinc); Serial.println(shuher_count);
                 }
                     
              delay(10);
             
              sendATcommand("AT+HTTPTERM", "OK", 3500);
              return 1;
  }

  

void Get_clock ()
{
  
  String clock1; String clock2; String clock3; String clock4; String clockf;
        
          sendATcommand("AT+CNTPCID=1","OK",2000);
          
          sendATcommand("AT+CNTP=\"ntp2.stratum2.ru\",12","OK",2000);
          sendATcommand("AT+CNTP","OK",4500);
          sendATcommand("AT+CCLK?","+CCLK:",6500);

  while (GSMport.available()) {     clock1=ReadGSM();   }
  
          Serial.println(clock1);
 
          clock1.replace('\n',' ');
          clock1.trim();
          int f = clock1.indexOf('"'); 
          clock2 =clock1.substring(f+10,f+12);
          clock3 =clock1.substring(f+13,f+15);
          clock4 =clock1.substring(f+16,f+18);
             
          setTime(clock2.toInt(),clock3.toInt(),clock4.toInt(),1,1,2022);
   //       sendATcommand("AT+CLTS=1","OK",1000);
   //       sendATcommand("AT&W","OK",1000);


       
          
         
 }
 

int pars(const char* buffer,const char* id) 
 {
  int st=strlen(id);
  char* ptr = strstr(buffer,(id));
  String result="";
  if(ptr)
    {
         ptr += st;
         while(*ptr && *ptr != ';')
         result += *ptr++;
    }

   return result.toInt();
}

  int pars_array(const char* buffer,const char* id,int result_array[7])  
    {
   
  int st=strlen(id);
  char* ptr = strstr(buffer,(id));
  ptr += st;
  
  for (int i=0;i<7;i++)
   { 
    String result="";
    if(ptr)
     {    while(*ptr && *ptr != ',')
         result += *ptr++;   }
    ptr++;
    if(result.toInt()!= 0) result_array[i] = result.toInt();
    result="";
   }
   
   } 


String ReadGSM() {  
  int c;
  String v;
  while (GSMport.available()) { 
    c = GSMport.read();
    v += char(c);
    delay(1);
  }
  return v;
}


bool RestartSim800 ( bool jmp ) 
{
 
  display.showNumberDec(8888, false);
 
  digitalWrite(RESET_SIM, LOW); 
  delay(200);
  digitalWrite(RESET_SIM, HIGH); 
  delay(3000);

  Gprs_init();
  
  }

double Thermister(int RawADC) {
  double Temp;
  Temp = log(((9840000/RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 229;
  
  if(Temp < (-50)  && Temp > 120) Temp = -127;
  
  return Temp;
}

void carusel ( )
{
 
  uint8_t data[] = { 0x6E, 0x73, 0x77, 0x0, 0x6E, 0x73, 0x77, 0x0, 0x6E, 0x73, 0x77, 0x0 ,0x0,0x0 ,0x0 };
  uint8_t data1[] = { };
 
   for (int i=0; i <=11; i++){
                data1[0] = data[i]; data1[1] = data[i+1]; data1[2] = data[i+2];  data1[3] = data[i+3];
                display.setSegments(data1);
                delay(700);
    }}
