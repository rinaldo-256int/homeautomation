
#include <SoftwareSerial.h>
// IMPORT ALL REQUIRED LIBRARIES

#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef ARDUINO_H
#include <Arduino.h>
#endif

#include <math.h>
   
//**********ENTER IP ADDRESS OF SERVER******************//

#define HOST_IP     "192.168.0.2"       // REPLACE WITH IP ADDRESS OF SERVER ( IP ADDRESS OF COMPUTER THE BACKEND IS RUNNING ON) 
#define HOST_PORT   "8080"            // REPLACE WITH SERVER PORT (BACKEND FLASK API PORT)
#define route       "api/update"      // LEAVE UNCHANGED 
#define idNumber    "620153775"       // REPLACE WITH YOUR ID NUMBER 


// WIFI CREDENTIALS
#define SSID        "CWC-9128576 2.4"      // "REPLACE WITH YOUR WIFI's SSID"   
#define password    "sb3Rkgqzxtcy"  // "REPLACE WITH YOUR WiFi's PASSWORD" 

//#define SSID        "MonaConnect"      // "REPLACE WITH YOUR WIFI's SSID"   
//#define password    ""  // "RE

#define stay        100
#define ARDUINOJSON_USE_DOUBLE 1


 
//**********PIN DEFINITIONS******************//

 
#define espRX         10
#define espTX         11
#define espTimeout_ms 300

int trigPin = 2;    // Trigger
int echoPin = 3;    // Echo
long duration, radar_reading;
int radar_reading_inches = 0;
int waterHeight = 0;
int reserveGallon = 0;
int percentage = 0;
int reserve_inches = 0;
int gal = 0;
int percent = 0;
 
 
/* Declare your functions below */
double calcWaterHeight(double radar_reading);
double calcWaterReserve(double water_height);
double calcPercentage(double water_reserve);

 

SoftwareSerial esp(espRX, espTX); 
 

void setup(){

  Serial.begin(115200); 
  // Configure GPIO pins here
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  espInit();  
 
}

void loop(){ 

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

   pinMode(echoPin, INPUT);
   duration = pulseIn(echoPin, HIGH);

   radar_reading_inches = (duration/2) / 74;
   waterHeight = calcWaterHeight(radar_reading_inches);
   reserveGallon = calcWaterReserve(waterHeight);
   percentage = calcPercentage(waterHeight);

   //Serial.println(radar_reading_inches);
   //Serial.println(waterHeight);
   //Serial.println(reserveGallon);
  // Serial.println(percentage); StaticJsonDocument<768> doc;  // Create JSon object
      StaticJsonDocument<768> doc;  // Create JSon object
      char message[260] = { 0 };

      // Add key:value pairs to Json object according to below schema
      doc["id"] = "620153775";
      doc["type"] = "ultasonic";
      doc["radar"] = radar_reading_inches;
      doc["waterheight"] = waterHeight;
        doc["reserve"] = reserveGallon;
         doc["percentage"] = percentage;

         serializeJson(doc ,message);
  // send updates with schema ‘{"id": "student_id", "type": "ultrasonic", "radar": 0, "waterheight": 0, "reserve": 0, "percentage": 0}’
  espUpdate(message);
  delay(1000);  
}

 
void espSend(char command[] ){   
    esp.print(command); // send the read character to the esp    
    while(esp.available()){ Serial.println(esp.readString());}    
}


void espUpdate(char mssg[]){ 
    char espCommandString[50] = {0};
    char post[290]            = {0};

    snprintf(espCommandString, sizeof(espCommandString),"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",HOST_IP,HOST_PORT); 
    espSend(espCommandString);    //starts the connection to the server
    delay(stay);

    // GET REQUEST 
    //snprintf(post,sizeof(post),"GET /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",route,HOST_IP,strlen(mssg),mssg);

    // POST REQUEST
    snprintf(post,sizeof(post),"POST /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",route,HOST_IP,strlen(mssg),mssg);
  
    snprintf(espCommandString, sizeof(espCommandString),"AT+CIPSEND=%d\r\n", strlen(post));
    espSend(espCommandString);    //sends post length
    delay(stay);
    Serial.println(post);
    espSend(post);                //sends POST request with the parameters 
    delay(stay);
    espSend("AT+CIPCLOSE\r\n");   //closes server connection
   }

void espInit(){
    char connection[100] = {0};
    esp.begin(115200); 
    Serial.println("Initiallizing");
    esp.println("AT"); 
    delay(1000);
    esp.println("AT+CWMODE=1");
    delay(1000);
    while(esp.available()){ Serial.println(esp.readString());} 

    snprintf(connection, sizeof(connection),"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,password);
    esp.print(connection);

    delay(3000);  //gives ESP some time to get IP

    if(esp.available()){   Serial.print(esp.readString());}
    
    Serial.println("\nFinish Initializing");    
   
}

//***** Design and implement all util functions below ******
 
 double calcWaterHeight(double radar_reading)
 {
    return 94.5 - radar_reading;
 }

 double calcWaterReserve(double water_height)
 {
    //3.14 * ()
    //reserve_inches = 77.763 - water_height;
    //percent = calcPercentage(water_height);
    //gal = calcGal(percent);
    double volume = (3.1415 * 30.75 * 30.75  * water_height) /231.0;
    return volume;
 }

double calcPercentage(double water_height)
{
  return (water_height/77.763) * 100;
}






