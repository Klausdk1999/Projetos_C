//Código Ganza Baby last ver 30/11

#include "SSD1306.h" // ou #include "SSD1306Wire.h"
#include "RTClib.h" //  Lady Ada
#include "DHT.h"
#include "font_roboto.h"
#include <WiFiManager.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h" 

//mqtt e thinkspeak
unsigned long myChannelNumber = 1376761;   //numero do canal
const char * myWriteAPIKey = "JZZIM6KCY21NIQCO"; //writeAPIkey, acesse o canal pelo navegador para encontrar
//status thingspeak
String myStatus = "";
WiFiClient client;


#define DHTPIN 3 //DHT rx
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//RTC, display, rele
RTC_DS1307 rtc;            // RTC
SSD1306  display(0x3c, 0, 2);//0x3C being the usual address of the OLED

// Variaveis do rtc
byte m = 0;       // contains the minutes, refreshed each loop
byte h = 0;       // contains the hours, refreshed each loop
byte s = 0;       // contains the seconds, refreshed each loop
byte mo = 0;      // contains the month, refreshes each loop
int j = 0;        // contains the year, refreshed each loop
byte d = 0;       // contains the day (1-31)
byte dia = 0;     // contains day of week (0-6)

float hic;
int horaatual;
int rega = 500;
int humidity;
int temperature;
int tiporega = 1;
bool tf = LOW;
int diasdecultivo = 2;
String mod;
int buzzer = 14; //d5
int botao1 = 12, botao2 = 13; //d6 d7
int modo = 0; //0 flora - 1 vega
int luz = 16, exaust = 05, sono = 04; //d0-rele luz  d1-rele exaust  d2-rele sono 
int agua=14; //d8 rele agua
int nivel=10;
int delayrega=0;
bool rede = LOW;
String id;

void setup()
{

  pinMode(luz,OUTPUT);//relé
  pinMode(exaust,OUTPUT);//relé
  pinMode(agua,OUTPUT);
  pinMode(buzzer,OUTPUT);//buzzer
  pinMode(nivel,INPUT);//nivel
  pinMode(13, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP); // define o pino do botao como entrada "INPUT"
  Serial.begin(9600);
  pinMode(sono, OUTPUT);

  dht.begin();
  
  //Serial - RTC, oled e o que mais for usar saidas tipo sda scl
  Wire.pins(0, 2);// 
  Wire.begin(0,2);// 0=sda D3, 2=scl D4
  rtc.begin(); 

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  wm.setClass("invert");
  wm.setConfigPortalTimeout(3);
  if(!wm.autoConnect("Ganza")){
    rede==LOW;
  }else{
    rede==HIGH;
    id=wm.getWiFiSSID();
  }
  
  ThingSpeak.begin(client);
  
  Serial.println("status:");
  Serial.println(rede);
  
  // Inicia o display.
  display.init();
  display.flipScreenVertically();// flipping came in handy for me with regard 
                                                              
  display.clear();
  drawProgressBar();
  display.clear(); 
}

int progress;
int counter=1;
void drawProgressBar() {
  
  while(progress<99){
  progress = (counter / 5) % 100;
  display.clear();
  display.setFont(Roboto_Black_Italic_16);
  String t = "Ganza";
  display.drawString(63, 0, t);
  // draw the progress bar
  display.drawProgressBar(0, 35, 120, 10, progress);
  
  display.display();
  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 18, String(progress) + "%");
  counter=counter+7;
  display.display();
  delay(15);
  }
  display.clear();

  for (int16_t i = 0; i < display.getWidth(); i += 4) {
    display.drawLine(0, 0, i, display.getHeight() - 1);
    display.display();
    delay(10);
  }
  for (int16_t i = 0; i < display.getHeight(); i += 4) {
    display.drawLine(0, 0, display.getWidth() - 1, i);
    display.display();
    delay(10);
  }
  delay(250);

  display.clear();

  for (int16_t i = 0; i < display.getWidth(); i += 4) {
    display.drawLine(0, display.getHeight() - 1, i, 0);
    display.display();
    delay(10);
  }
  for (int16_t i = display.getHeight() - 1; i >= 0; i -= 4) {
    display.drawLine(0, display.getHeight() - 1, display.getWidth() - 1, i);
    display.display();
    delay(10);
  }
  delay(250);

  display.clear();

  display.setFont(Roboto_Black_Italic_24);
  String t = "Ganza";
  display.drawString(63, 5, t);
  display.setFont(ArialMT_Plain_10);
  String z="Levante Lab ";
  display.drawString(63, 36, z);
  String y= "© 2021 | BRASIL";
  display.drawString(63, 48, y);
  display.display();
  //musiquinha do incio
  tone(buzzer, 523) ; //DO note 523 Hz
  delay (100); 
  tone(buzzer, 587) ; //RE note ...
  delay (150); 
  tone(buzzer, 659) ; //MI note ...
  delay (300); 
  tone(buzzer, 783) ; //FA note ...
  delay (400); 
  tone(buzzer, 880) ; //SOL note ...
  delay (250); 
  tone(buzzer, 587) ; //RE note ...
  delay (200); 
  tone(buzzer, 659) ; //MI note ...
  delay (500); 
  tone(buzzer, 523) ; //DO note 523 Hz
  delay (400); 
  
  noTone(buzzer) ; //Turn off the pin attached to the tone()
  delay(2000);
  display.clear();
}

void envia_valores(){
  ThingSpeak.setField(1, humidity);//umidade
  ThingSpeak.setField(2, temperature);//temperatura
  ThingSpeak.setField(3, hic);//sensação ter.
  ThingSpeak.setField(4, modo);//modo
  ThingSpeak.setField(5, digitalRead(luz));//estado da luz
  ThingSpeak.setField(6, digitalRead(sono));//luz sono
  ThingSpeak.setField(7, digitalRead(exaust));//exaustor
  ThingSpeak.setField(8, diasdecultivo);//dias d cult.
  
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

void wifim(){
  tela_ap();
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  wm.setClass("invert");
  wm.setAPClientCheck(true); // avoid timeout if client connected to softap
  wm.setConfigPortalTimeout(20000);//tempo para conectar ao ap antes de continuar o código
  wm.startConfigPortal("Ganza");
  
   
  id=wm.getWiFiSSID();
  if(id!=NULL){
    rede==HIGH;
  }
}

//Atualiza a tela com variaveis de modo, hora atual e leituras dos sensores
void tela(){
  temperature =dht.readTemperature();
  humidity =dht.readHumidity();
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(temperature, humidity, false);

  display.clear();
  display.fillRect(0, 0, 128, 13);
  display.setColor(BLACK);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Arimo_Bold_10);
  String t = String(h) + ":" + String(m) +"              "+ mod;
  if(m<10){
    t = String(h) + ":0" + String(m) +"              "+ mod;
   }
  display.drawString(63, 0, t);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setColor(WHITE);
  display.setFont(Roboto_Black_16);
  String t2 = String(temperature) + " °C | " + String(hic) + " °C" ;
  String t3 = "U:" + String(humidity) + "% | "  + String(rega)+" mls";
  display.drawString(63, 16, t2);
  display.drawString(63, 38, t3);
  
    display.display();

}

void tela_rega(){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Roboto_Black_16);
  String t1 = "Regando "+String(rega)+" mls" ;
  display.drawString(63, 6, t1);
  display.setFont(DSEG_Weather_30);
  String t2 = "3";
  display.drawString(63, 20, t2);
  display.display();
    tone(buzzer, 880) ; //SOL note ...
    delay (250);
    tone(buzzer, 523) ; //DO note 523 Hz
    delay (100); 
    tone(buzzer, 880) ; //SOL note ...
    delay (250);
    tone(buzzer, 587) ; //RE note ...
    delay (150); 
    tone(buzzer, 880) ; //SOL note ...
    delay (250); 
    noTone(buzzer) ;
}
void tela_wifi(){
  display.clear();
  display.fillRect(0, 0, 128, 13);
  display.setColor(BLACK);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Arimo_Bold_10);
  String t = "Configurar Wi-Fi (1/2)";
  display.drawString(63, 0, t);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setColor(WHITE);
  display.setFont(Roboto_Black_16);
  String t2 = "Pressione os" ;
  String t3 = "dois botões";
  if(rede==HIGH){
    t2 = "Conectado à:";
    t3 = id;
  }
  display.drawString(63, 16, t2);
  display.drawString(63, 38, t3);
  
  display.display();
  if(rede==HIGH){
    delay(5000);
  }
}

void tela_update(){
  display.clear();
  display.clear();
  display.fillRect(0, 0, 128, 13);
  display.setColor(BLACK);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Arimo_Bold_10);
  String t = "...Atualizar...";
  display.drawString(63, 0, t);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setColor(WHITE);
  display.setFont(Roboto_Black_16);
  String t2 = "Pressione os" ;
  String t3 = "dois botões";
  if(rede==HIGH){
    t2 = "Conectado à:";
    t3 = id;
  }
  display.drawString(63, 16, t2);
  display.drawString(63, 38, t3);
  
  display.display();
  
}

void tela_ap(){
  display.clear();
  display.fillRect(0, 0, 128, 13);
  display.setColor(BLACK);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Arimo_Bold_10);
  String t = "Configurar Wi-Fi (2/2)";
  display.drawString(63, 0, t);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setColor(WHITE);
  display.setFont(Roboto_Black_16);
  String t2 = "Conecte-se ao" ;
  String t3 = "Wi-Fi Ganza";
  display.drawString(63, 16, t2);
  display.drawString(63, 38, t3);
  
  display.display();
  delay (1500);
}

void regar(){
    tela_rega();
    delayrega=((rega/100)*5200);
    digitalWrite(agua,LOW);
    delay(delayrega);
    digitalWrite(agua,HIGH); 
    
}

void sensor_nivel(){
  if(digitalRead(nivel)== LOW){
     tone(buzzer, 523) ; //DO note 523 Hz
     delay (70);
     noTone(buzzer) ;
     delay (70);
     tone(buzzer, 659) ; //DO note 523 Hz
     delay (70);
     noTone(buzzer) ;
     delay (70);
     tone(buzzer, 523) ; //DO note 523 Hz
     delay (70);
     noTone(buzzer) ;
     delay (70);
     tone(buzzer, 587) ; //DO note 523 Hz
     delay (200);
     noTone(buzzer) ; 
     delay(100);
  }
  
}

void modos(){
  if(modo==0){ //FLORA
    mod = "     Flora";
    //sono
    if( h>=7 && h<19 ){
      digitalWrite(sono,LOW);
    }
    else{
      if( h==19 && m<=26){
        digitalWrite(sono,LOW);
      }
      else{
        digitalWrite(sono, HIGH);
      }
      
    }
    //luz
    if( h>=7 && h<=19 ){
      digitalWrite(luz,LOW);
    }
    else{
      if((h<7)|| ( h>19)){
        digitalWrite(luz,HIGH);
      }
      
    }
    //exaustor
    if( temperature>=29 || humidity >=80 ){
      digitalWrite(exaust,LOW);
    }
    else{
      digitalWrite(exaust,HIGH);
    }
  }
  if(modo==1){ //VEGA
    mod = "    Vega";
    if( h>=5 && h<23 ){
      digitalWrite(sono,LOW);
    }
    else{
      if( h==23 && m<=26){
        digitalWrite(sono,LOW);
      }
      else{
        digitalWrite(sono, HIGH);
      }
      
    }
    
    if(h>=5 && h<=23){
      digitalWrite(luz,LOW);
    }
    else{
      if((h<5)||(h>23)){
        digitalWrite(luz,HIGH);
      }
    }
    if( temperature>=29 || humidity >=80 ){
      digitalWrite(exaust,LOW);
    }
    else{
        digitalWrite(exaust,HIGH);
    }
  }
  if(modo==2){ //OFF
      mod = "     OFF";
      digitalWrite(luz,HIGH);
      digitalWrite(exaust,HIGH);
      digitalWrite(sono,HIGH);
    }
   if(modo == 3){ //Flora Inverter
      mod = "Flora Invert";
      if(h>=19 || h<7){
        digitalWrite(luz, LOW);
        digitalWrite(sono, LOW);
      }
      else{
        digitalWrite(luz, HIGH);
      }
      if(h==7 && m<=25){
        digitalWrite(sono, LOW);
      }
      if(h==7 && m>25){
        digitalWrite(sono, HIGH);
      }
   }
   if(modo == 4){
    mod = "Vega Invert";
    if(h>=18 || h<10){
      digitalWrite(luz, LOW);
      digitalWrite(sono, LOW);
    }
    else{
      digitalWrite(luz, HIGH);
    }
    if(h==10 && m<=25){
      digitalWrite(sono, LOW);
    }
    if(h==7 && m>25){
        digitalWrite(sono, HIGH);
      }
   }
    if(modo == 5){
      mod = "Wi-Fi";
      display.clear();
      tela_wifi();
      digitalWrite(luz,HIGH);
      digitalWrite(exaust,HIGH);
      digitalWrite(sono,HIGH);
      
    }
    if(modo == 7 || modo==8){
      mod = "update";
      display.clear();
      tela_update();
      digitalWrite(luz,HIGH);
      digitalWrite(exaust,HIGH);
      digitalWrite(sono,HIGH);
    }
}
bool deixa;
byte ajusteh;
void hora(){
  delay(500);
  DateTime now = rtc.now();
  h = now.hour();
  m = now.minute();
  s = now.second();
  ajusteh=now.hour();
  do{
    
   // digitalRead(04);
   // digitalRead(12);
   display.clear();
   rtc.adjust(DateTime(j, mo, d, ajusteh, m, s));
    //Pega a hora do rtc e salva em variaveis

  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  String t = "Ajuste da hora";
  String t1 = String(ajusteh) + ":" + String(m);
  if(m<10){
    t1 = String(ajusteh) + ":0" + String(m);
   }
  display.drawString(63, 0, t);
  display.drawString(63, 30, t1);

  display.display();// write the buffer to the display

  if(digitalRead(botao2)==LOW){
       ajusteh++;
       if(ajusteh==24){
           ajusteh=0;
       }
        tone(buzzer, 659) ; //MI note ...
        delay (50);
        noTone(buzzer);
        delay(250);
  }
  if(digitalRead(botao1)==LOW){
       ajusteh--;
       if(ajusteh==-1){
           ajusteh=23;
       }
       tone(buzzer, 523) ; //DO note 523 Hz
       delay (50); 
       noTone(buzzer); 
       delay (250);  
  }
  if(digitalRead(botao1)==LOW && digitalRead(botao2)==LOW){
    deixa = true;
  }
  
  }while(deixa==false);
  deixa=false;
  display.clear();
  display.setFont(Roboto_Black_16);
  display.drawString(63, 20, "Hora Salva!");
  display.display();
  delay(1500);
  minuto();
}

void update_system(){
  String sa;
//  WiFiClient client;
  Serial.println("............................ENTREI NA FUNCAO DE ATUALIZACAO..............");
  const char* URL = "https://app-update.ganza.app/ftp_update/Boot";
  WiFiClientSecure client1;
  //WiFiClient client;
  client1.setInsecure();
      
    //HTTPClient http;
  ESPhttpUpdate.rebootOnUpdate(false);
  //Callback - Progresso
  ESPhttpUpdate.onProgress([](size_t progresso, size_t total) {
    
  });
  //Atualiza Software
  t_httpUpdate_return r = ESPhttpUpdate.update(client1, URL);

  switch(r){
    case HTTP_UPDATE_FAILED: {
       sa = ESPhttpUpdate.getLastErrorString();
    } break;
    case HTTP_UPDATE_NO_UPDATES:
    //não ha atualizações
    break;
    case HTTP_UPDATE_OK: {
      // atualizando, reiniciando...
      ESP.restart();
    } break;
  }
  Serial.println("Erro de atualização:");
  Serial.println(sa);  
} //end update

void minuto(){
  delay(300);
   DateTime now = rtc.now();
  byte ajustm=now.minute();
  deixa=false;
  do{
    display.clear();
   rtc.adjust(DateTime(j, mo, d, ajusteh, ajustm, s));
    //Pega a hora do rtc e salva em variaveis
  DateTime now = rtc.now();
  h = now.hour();
  m = now.minute();
  s = now.second();

  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  String t = "Ajuste minutos";
  String t1 = String(h) + ":" + String(m);
  if(m<10){
    t1 = String(h) + ":0" + String(m);
   }
  display.drawString(63, 0, t);
  display.drawString(63, 30, t1);

  display.display();// write the buffer to the display

  if(digitalRead(botao2)==LOW){
       ajustm++;
       if(ajustm==60){
           ajustm=0;
       }
        tone(buzzer, 659) ; //MI note ...
        delay (50);
        noTone(buzzer);
        delay(250);
  }
  if(digitalRead(botao1)==LOW){
       ajustm--;
       if(ajustm==-1){
           ajustm=0;
       }
       tone(buzzer, 523) ; //DO note 523 Hz
       delay (50); 
       noTone(buzzer); 
       delay (250);
  }
  if(digitalRead(botao1)==LOW && digitalRead(botao2)==LOW){
    deixa = true;
  }
  
  }while(deixa==false);
  deixa=false;
  display.clear();
  display.setFont(Roboto_Black_16);
  display.drawString(63, 20, "Minutos Salvos!");
  display.display();// write the buffer to the display
  delay(1500);
}

void loop(){
  //id=wm.getWiFiSSID();
  //if(id!=NULL){
   // rede==HIGH;
 // }
  //digitalWrite(14,HIGH);
  if(digitalRead(botao1)==LOW && digitalRead(botao2)==LOW){
    if(modo==5 || modo==6){
      wifim();
      delay (250);
    }
    if(modo==7 || modo==8){
      update_system();
    }
    else{
      hora();
      delay (250);
    }
    
  }
  else{if(digitalRead(botao1)==LOW){
          modo++;
          if (modo==9){
            modo=0;
          }
          tone(buzzer, 523) ; //DO note 523 Hz
          delay (50); 
          noTone(buzzer); 
          delay (200);          
        }
        if(digitalRead(botao2)==LOW){
            rega=rega+100;
            if (rega==5000){
              rega=0;
            }
          tone(buzzer, 659) ; //MI note ...
          delay (50);
          noTone(buzzer);
          delay(200);
        }
      }
  modos();
  
  //Pega a hora do rtc e salva em variaveis
  DateTime now = rtc.now();
  h = now.hour();
  m = now.minute();
  s = now.second();
  
  display.clear();
  if(modo<=4){
    tela();
  }else if(modo==5 || modo==6){
    tela_wifi();
  }else{
    tela_update();
  }
  
  if((h%4==0) && m==0 && s==1){
    sensor_nivel();
    delay(200);
    sensor_nivel();
    delay(200);
    sensor_nivel();
    delay(200);
    sensor_nivel();
    delay(2000);
  }
 
   
  if(diasdecultivo%2==0){
    if(h>=20 && rega!=0 && modo!=3 && tf==LOW){
      //regar();        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      tf = HIGH;
    }
  }
  
  if(h==0 && m==0 && s==1){
    diasdecultivo++;
    //update_system();
    tf = LOW;
    delay(2000);
  }
  envia_valores();
  
  
}
