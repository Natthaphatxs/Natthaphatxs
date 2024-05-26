//นำเข้า library
#include <ESP8266WiFi.h>
#include "DHT.h"

//กำหนดขา pin ของ sensor และ led
#define Smoke_PIN 2
#define Smoke_ANG A0
#define Red_LED 5
#define Green_LED 4
#define Buzzer_PIN 0

//var
int mode;
int sm_state = 0;
int sm_value = 0;
int state = 0; //0 default 1 normal 2 warning 3 alarm
bool buzzer_sent;
const String status[2] = {"ปกติ","ผิดปกติ!"};
String S_SM;

//line notify
#include <TridentTD_LineNotify.h>
#define LINE_TOKEN "jpgCH5fPlDlHjCeirAachzPdLaWKWzDa9xZXgCjArf4" //Token ใหม่ line นักเรียน
String line_msg ;

//wifi config
char ssid[] = "IoTV2";
char pass[] = "06070809";

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid,pass);
  LINE.setToken(LINE_TOKEN);
  pinMode(Smoke_PIN,INPUT);
  pinMode(Smoke_ANG,INPUT);
  pinMode(Red_LED,OUTPUT);
  pinMode(Green_LED,OUTPUT);
  pinMode(Buzzer_PIN,OUTPUT);
 
  digitalWrite(Red_LED,HIGH);
  digitalWrite(Green_LED,LOW);
  digitalWrite(Buzzer_PIN,HIGH);
  int w =1;
  Serial.print("Connecting wifi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    w++;
    if(WiFi.status() == WL_CONNECTED){
      mode=1;
      Serial.println("[ Online mode ]");
    }
    if(w==40){
      mode=2;
      WiFi.disconnect();
      Serial.println("[ Offline mode ]");
      break;
    }
  }
}

void loop() {
  SM_sensor();
  Logic();
  buzzer();
  delay(400);
}

void SM_sensor(){
  sm_state = digitalRead(Smoke_PIN);
  sm_value = analogRead(Smoke_ANG);
  Serial.println(sm_state);
  Serial.println(sm_value);
}

void Logic(){
  if(sm_state==HIGH and state!=1){
    digitalWrite(Red_LED,HIGH);
    digitalWrite(Green_LED,LOW);
    buzzer_sent = false;
    state =1;
    if(mode==1){
      Line_Notify(0);
    }
    else{}
    Serial.println("normal");
  }
  if(sm_state==LOW and state!=2){
    digitalWrite(Red_LED,LOW);
    digitalWrite(Green_LED,HIGH);
    buzzer_sent = true;
    state =2;
    if(mode==1){
      Line_Notify(1);
    }
    else{}
    Serial.println("alarm");
  }
}

void Line_Notify(int level){
  
  if(sm_state==HIGH){
    S_SM = status[0];
  }
  else{
    S_SM = status[1];
  }

  if(level==0){
    line_msg="\nสถานะปกติ\nสถานะจากเซ็นเซอร์ : "+S_SM+"\nค่าจากเซ็นเซอร์ : "+String(sm_value);
  }
  if(level==1){
    line_msg="\nสถานะผิดปกติ!! ตรวจพบแก๊สรั่ว\nสถานะจากเซ็นเซอร์ : "+S_SM+"\nค่าจากเซ็นเซอร์ : "+String(sm_value);
  }
  LINE.setToken(LINE_TOKEN);
  LINE.notify(String(line_msg));
  line_msg = "";
}

void buzzer(){
  if(buzzer_sent==true){
    digitalWrite(Buzzer_PIN,LOW);
  }
  if(buzzer_sent==false){
    digitalWrite(Buzzer_PIN,HIGH);
  }
}
