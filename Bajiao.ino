#include "SoftwareSerial.h"
#include <MsTimer2.h>
#define d_a 13
#define d_b 3
#define d_c 4
#define d_d 5
#define d_e 6
#define d_f 7
#define d_g 8
#define d_h 9

//==========================
String user = "bajiao";

String ssid="hahahahaha";
String pwd="1qaz2wsx";

//==========================

#define COM1 15
#define COM2 17
#define COM3 16
#define COM4 14


int Shock = 19; //定义震动传感器接口
int ShockTime = 20;
bool isShock = 0;

unsigned timeNum = 0;
bool timedot = 0;
bool isShow = 0;

SoftwareSerial ESP8266(10, 11);// RX, TX

unsigned char num[21][8] = {
  //a  b  c  d  e  f  g  h
  {1, 1, 1, 1, 1, 1, 0, 0},     //0
  {0, 1, 1, 0, 0, 0, 0, 0},     //1
  {1, 1, 0, 1, 1, 0, 1, 0},     //2
  {1, 1, 1, 1, 0, 0, 1, 0},     //3
  {0, 1, 1, 0, 0, 1, 1, 0},     //4
  {1, 0, 1, 1, 0, 1, 1, 0},     //5
  {1, 0, 1, 1, 1, 1, 1, 0},     //6
  {1, 1, 1, 0, 0, 0, 0, 0},     //7
  {1, 1, 1, 1, 1, 1, 1, 0},     //8
  {1, 1, 1, 1, 0, 1, 1, 0},     //9
  {1, 1, 1, 0, 1, 1, 1, 1},     //A
  {1, 1, 1, 1, 1, 1, 1, 1},     //B
  {1, 0, 0, 1, 1, 1, 0, 1},     //C
  {1, 1, 1, 1, 1, 1, 0, 1},     //D
  {1, 0, 0, 1, 1, 1, 1, 1},     //E
  {1, 0, 0, 0, 1, 1, 1, 1},     //F
  {0, 0, 0, 0, 0, 0, 0, 1},     //.          16
  {0, 0, 0, 0, 0, 0, 1, 0},     //-          17
  {1, 0, 0, 1, 0, 0, 0, 0},     //上下两道杠   18
  {1, 0, 0, 1, 0, 0, 1, 0},     //三道杠      19
  {0, 0, 0, 0, 0, 0, 0, 0},     //null       20
};

String server = "www.laozhihualu.com"; // np. www.examble.pl albo adres ip 129.75.1.32

void setup()
{
  ESP8266.begin(9600); //odpowiednia prędkość dla twojego modułu
  Serial.begin(9600);
  //Serial.print("Start");
  pinMode(d_a, OUTPUT);
  pinMode(d_b, OUTPUT);
  pinMode(d_c, OUTPUT);
  pinMode(d_d, OUTPUT);
  pinMode(d_e, OUTPUT);
  pinMode(d_f, OUTPUT);
  pinMode(d_g, OUTPUT);
  pinMode(d_h, OUTPUT);

  pinMode(COM1, OUTPUT);
  pinMode(COM2, OUTPUT);
  pinMode(COM3, OUTPUT);
  pinMode(COM4, OUTPUT);

  pinMode(Shock, INPUT);

  //timeNum = 32000; 测试用，时间初始值
  MsTimer2::set(1000, TimeGo);
  MsTimer2::start();

 

  reset();
  ESP8266.println("AT+CWMODE=1");
  delay("1000");
  ESP8266.println("AT+CIPMUX=1");
  delay("1000");
  //ESP8266.println("AT+CIOBAUD=9600");

  connectWifi();


}


void TimeGo() {

  if (timedot == 1) {
    timedot = 0;
  } else {
    timedot = 1;
  }

  if (isShock == 1) {
    ShockTime = ShockTime - 1;
    isShow = 1;

    if (ShockTime > 0) {
      ShockTime = ShockTime - 1;
    } else {
      isShow = 0;
      ShockTime = 20;
      isShock = 0;
    }


  }

  timeNum = timeNum + 1;
}

void loop()
{ 
  if (analogRead(Shock) < 800) {
    isShock = 1;
  }

  ClockShow(isShow);


  if (digitalRead(12) == HIGH) {
    LedLoading();
    Serial.println("Sending...");
    String cmd;
    cmd = "AT+CIPSTART=1,\"TCP\",\"";
    delay(500);
    cmd += server;
    delay(500);
    cmd += "\",80";
    delay(500);
    ESP8266.println(cmd);
    delay(500);

    if (ESP8266.find("OK"))
    {
      Serial.println("Connected to the web!");
    }

    cmd =  "GET /milkclock.asp?milk=" + (String)analogRead(A4) + "&time=" + (String)timeNum + "&user=" + user + " HTTP/1.1\r\n";

    Serial.println(cmd);
    delay(500);
    cmd += "Host: www.laozhihualu.com\r\n\r\n";
    delay(500);
    ESP8266.print("AT+CIPSEND=1,");
    delay(500);
    ESP8266.println(cmd.length());
    delay(500);

    if (ESP8266.find(">"))
    {
      Serial.println(">");
    }

    ESP8266.println(cmd);
    delay(500);

    if (ESP8266.find("OK"))
    {
      Serial.println("send!");
      printmssage();
    }

    Led3Loading();
    isShock = 1;
    timeNum=0;

 
  }
}

void connectWifi()
{

  LedLoading();
  String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + pwd + "\"";

  ESP8266.println(cmd);
  delay(4000);

  if (ESP8266.find("OK"))
  {
    
    Serial.println("Connected!");
    Led3Loading();
    //get mac
    //Serial.println("Get mac...");
    //cmd = "AT+CIPSTAMAC?";
    //ESP8266.println(cmd);
    //delay(500);

    //mac=getMessage().substring(28);
    //Serial.println("mac:"+mac);

    //printmssage();


    //end get mac
  } else {
    LedError();
    Serial.println("Cannot connect to wifi,wait for 5s...");
    delay(5000);
    connectWifi();
  }
}

void reset()
{
  ESP8266.println("AT+RST");
  delay(1000);

  //if (ESP8266.find("OK")) Serial.println("Module Reset Ready" + (String)millis());
}


String getMessage() {
  String info = "";

  while (ESP8266.available() > 0)
  {
    info += char(ESP8266.read());
    delay(2);
  }
  return info;
}


void printmssage() {
  String info = "";

  while (ESP8266.available() > 0)
  {
    info += char(ESP8266.read());
    delay(2);
  }

  Serial.println(info);

}

void ClockShow(bool f_isShow) {
  if (f_isShow == 0) {
    LedClear();
    Display(2, 20);
    Display(2, 20);
    Display(2, 20);
    Display(2, 20);

  } else {
    //------------------------
    //小时
    int TG_hour, TG_minte;
    TG_hour = (int)(timeNum / 60 / 60);
    TG_minte = (int)((timeNum / 60) - TG_hour * 60);
    //Serial.println(String(timeNum)+"  "+String(TG_hour)+"  "+String(TG_minte));



    if (timedot == 1) {
      Display(2, 16);
    } else {
      Display(2, 20);
    }

    if (timeNum < 35940) {//35940 最大显示值 9小时59分
      Display(1, TG_hour);
      //Display(2, TG_hour % 10);



      if (TG_minte < 10) {
        Display(3, 0);
        Display(4, TG_minte);
      } else {
        Display(3, (int)((TG_minte) / 10));
        Display(4, TG_minte % 10);
      }
    }
    else {
      //Serial.println(String(timeNum)+"  "+String(TG_hour)+"  "+String(TG_minte));
      Led3Loading();
    }


    //------------------------
  }

}

void LedLoading() {
  for (int i = 1; i < 5; i++) {
    Display(i, 17);
    delay(50);
    Display(i, 17);
    delay(50);
    Display(i, 17);
    delay(50);
    Display(i, 17);
    delay(50);
  }
  LedClear();
}

void LedError() {
  
    Display(1, 14);
    delay(1000);
    Display(2, 10);
    delay(1000);
    Display(3, 10);
    delay(1000);
    Display(4, 16);
    delay(1000);

}

void Led3Loading(){
  for (int i = 1; i < 5; i++) {
        Display(i, 19);
        delay(50);
        Display(i, 19);
        delay(50);
        Display(i, 19);
        delay(50);
        Display(i, 19);
        delay(50);
      }
  }

void LedClear() {
  digitalWrite(d_a, LOW);
  digitalWrite(d_b, LOW);
  digitalWrite(d_c, LOW);
  digitalWrite(d_d, LOW);
  digitalWrite(d_e, LOW);
  digitalWrite(d_f, LOW);
  digitalWrite(d_g, LOW);
  digitalWrite(d_h, LOW);
}

void Display(unsigned char com, unsigned char n) {

  LedClear();
  switch (com) {
    case 1:
      digitalWrite(COM1, LOW);
      digitalWrite(COM2, HIGH);
      digitalWrite(COM3, HIGH);
      digitalWrite(COM4, HIGH);
      break;
    case 2:
      digitalWrite(COM1, HIGH);
      digitalWrite(COM2, LOW);
      digitalWrite(COM3, HIGH);
      digitalWrite(COM4, HIGH);
      break;
    case 3:
      digitalWrite(COM1, HIGH);
      digitalWrite(COM2, HIGH);
      digitalWrite(COM3, LOW);
      digitalWrite(COM4, HIGH);
      break;
    case 4:
      digitalWrite(COM1, HIGH);
      digitalWrite(COM2, HIGH);
      digitalWrite(COM3, HIGH);
      digitalWrite(COM4, LOW);
      break;
    default: break;
  }
  digitalWrite(d_a, num[n][0]);
  digitalWrite(d_b, num[n][1]);
  digitalWrite(d_c, num[n][2]);
  digitalWrite(d_d, num[n][3]);
  digitalWrite(d_e, num[n][4]);
  digitalWrite(d_f, num[n][5]);
  digitalWrite(d_g, num[n][6]);
  digitalWrite(d_h, num[n][7]);
}

