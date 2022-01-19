// Nextion Ganzá 6/11 - Klaus e Fabricio
// Libraries
#include "Nextion.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "DHT.h" // sensor dht 11 biblioteca de teste`
#include "RTClib.h" //  Lady Ada
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <FS.h> // SPIFFS memoria flash
//bibliotecas de rede
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <WiFiManager.h>
#include <ESP8266httpUpdate.h>

#include "ThingSpeak.h" // Deixar como ultima biblioteca - recomendação no site oficial


int codev = 3;

// ----------------------------------------------------------------------------------- Variáveis globais ----------------------------------------------------------------
float TempLamp, HumLamp;
int luz;
//int horaoff, minon, minoff;
int rele = 5;//PINO RELE
int led_vermelho1 = 14;
int led_vermelho2 = 12;
int led_verde1 = 13;
int led_verde2 = 15;
int led_amarelo = 10;
  
const int buzzer = 04; //PINO BUZZER
int modo; //= 1;
int diascultivo; // dias de cultivo
char cultivo[5]; //vetor para pegar do elemento de texto e transformar em int
char diascultivo2[5];

char bufferhora[10];

byte m_descanso;
DHT dht(D0, DHT11); // definições do dht11

String status_wpp;
Adafruit_BME280 bmp; //OBJETO DO TIPO Adafruit_BMP280 (I2C)

//mqtt e thinkspeak
unsigned long myChannelNumber = 1376761;   //numero do canal
const char * myWriteAPIKey = "JZZIM6KCY21NIQCO"; //writeAPIkey, acesse o canal pelo navegador para encontrar
//status thingspeak
String myStatus = "";
WiFiClient client;

// NTP variaveis
const long utcOffsetInSeconds= -10800;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // dias da semana para captar do ntp
// Define NTP Client to get time

int wifi_on;
int pic_wifi;

//variaveis nome do usuario, telefone e canal do thinkspeak
char nome_usuario[20];
char char_nome_usuario[20];
char telefone[15];
char char_telefone[15];
char canal[20];
char char_canal[20];

String string_telefone;
String string_nome_usuario;
String string_canal;

// Variavel de modo para envio das mensagens

int vegad, florad, colheitad, germinacaod;

//---------------------- pagina iniciando ( page0 ) --------------------------
NexProgressBar progress_inicial = NexProgressBar(0, 7, "j0");
NexPicture conect = NexPicture(0, 5, "p0");
NexPicture buscando = NexPicture(0, 6, "p1");
NexPicture iniciando = NexPicture(0, 7, "p2");
bool page0b;

// ------------------ pagina modos (page3)------------------------------------
NexButton home3 = NexButton(3, 1, "b0");
NexButton vega = NexButton(3, 2, "b1");
NexButton flora = NexButton(3, 3, "b2");
NexButton germ = NexButton(3, 4, "b3");
NexButton trabalho = NexButton(3, 5, "b4");
NexButton meumodo = NexButton(3, 6, "b5");
NexButton colheita = NexButton(3, 7, "b6");
NexButton btn_on = NexButton(3, 8, "b7");
NexButton btn_off = NexButton(3, 9, "b8");
NexButton conf = NexButton(3, 10, "b9");

bool pagemodosb;
// -------------------pagina inicial (page 1) --------------------------------
NexButton modos = NexButton(1, 1, "bmodos");
NexText temps = NexText(1,2,"t0");
NexText hums = NexText(1,3,"t1");
NexText horartc = NexText(1,4,"t2");
NexButton configurara1b = NexButton(1, 5, "b3");

//openweathermap infos
NexText cidade = NexText(1,6,"t3");
NexText temp_min = NexText(1,8,"t4");
NexText temp_max = NexText(1,9,"t5");
NexText weathermap_h = NexText(1,10,"t6");
NexButton clima = NexButton(1, 7, "b4");

NexButton wifi_mod = NexButton(1, 11, "b8");
NexText daycultivo = NexText(1,12,"t7");

NexButton envia_zap = NexButton(1, 13, "b9");


bool page1b;


//------------------new page meu modo page7------------------------
NexButton homemeumodo = NexButton(7, 1, "b0");
NexButton confmeumodo = NexButton(7, 2, "b1");

NexText show_ligar = NexText(7,3,"t0");
NexText show_desligar = NexText(7,4,"t1");

NexButton edita_ligar = NexButton(7, 5, "b2");
NexButton edita_desligar = NexButton(7, 6, "b3");
bool pagemeumodob;

//--------------------------- page editar hora ligar page 20 -----------------
NexText horaligar = NexText(20,2,"t1");
NexButton salva_horaon = NexButton(20, 3, "b0"); // botao de avançar

//----------------------------page editar minuto de ligar page 21--------------
NexText min_ligar = NexText(21,2,"t1");
NexButton salva_minon = NexButton(21, 3, "b0"); //botao de avançar

//---------------------------page editar hora de ligar page 22 ----------------
NexText hora_desligar = NexText(22,2,"t1");
NexButton salva_horaoff = NexButton(22, 3, "b0");

//---------------------------page editar minuto de desligar page 23------------
NexText min_desligar = NexText(23,2,"t1");
NexButton salva_minoff = NexButton(23, 3, "b0");

//-------------------pagina configurações (1°)  -------------------------


//new elements pagina configurações cultivo (page 8)
NexText dias_cultivo = NexText(8,2,"t2");
NexButton salvacultivo = NexButton(8, 3, "b1"); // fazer alterar a pagina na sua chamada
NexButton back_cultivo = NexButton(8,4, "b0");
// end cultivo

//new element pagina configurando cidade (page9)
NexText cidademap = NexText(9,2,"t1"); // cidade para openweathermap
NexButton salvacidade = NexButton(9, 3, "b0"); // fazer alterar a pagina na sua chamada
NexButton back_cidade = NexButton(9, 4, "b4");
// end cidade

//new element pagina configurando pais (page16)
NexText paismap = NexText(16,2,"t1"); //pais para openweathermap
NexButton salvapais = NexButton(16, 3, "b0"); // salvar e enviar para outra pagina
// end pais



bool pageconfig1b;

//-------------------pagina configurações (2°)  -------------------------

//new element pagina configurando ssid (page 17)
NexText conf_ssid = NexText(17,2,"t1");
NexButton salvar_ssid = NexButton(17, 3, "b0");
NexButton back_ssid = NexButton(17, 4, "b1");
bool navegacao_conf_1_bool;
// end ssid

//new element pagina configurando senha (page18)
NexText conf_senha = NexText(18,2,"t1");
NexButton salvar_senha = NexButton(18, 3, "b0");
//end senha


bool pageconfig2b;

//-------------------pagina configurações (3°) page 11 -------------------------

// new element pagina configurando MEU NOME (page 11)
NexText get_nome = NexText(11,2,"t1");
NexButton conf_nome = NexButton(11,3,"b0");
NexButton back_apelido = NexButton(11,4,"b1");
//end meu apelido

// new element pagina configurando telefone (page 19)
NexText get_numero = NexText(19,2,"t1");
NexButton conf_numero = NexButton(19,3,"b0");
NexButton back_telefone = NexButton(19,4,"b1");
//end telefone

bool pageconfig3b;

//-------------------pagina descanso page 2 -------------------------
//NexButton volta_conf2 = NexButton(2, 0, "b0");
NexButton modo_descanso = NexButton(2, 1, "b0");

NexText horartc_descanso = NexText(2, 2, "t0");
NexText temp_descanso = NexText(2, 3, "t1");
NexText humidade_descanso = NexText(2, 4, "t2");
bool page2b;

// novas paginas de navegação das configurações

//----------------------- pagina configurações 1 (page13)
NexText show_redewifi = NexText(13, 6, "t2");
NexText show_cidade = NexText(13, 9, "t3");

NexButton editar_wifi = NexButton(13, 4, "b1");
NexButton editar_local = NexButton(13, 5, "b2");
NexButton avanca_navegacao_conf_1 = NexButton(13, 7, "b3");
NexButton volta_navegacao_conf_1 = NexButton(13, 8, "b4");
NexButton home_navegacao_conf_1 = NexButton(13, 1, "b0");
//bool navegacao_conf_1_bool;
//end page13

//------------------------- pagina configurações 2 (page14)
NexText show_nome = NexText(14, 6, "t2");
NexText show_telefone = NexText(14, 9, "t3");

NexButton editar_nome = NexButton(14, 4, "b1");
NexButton editar_telefone = NexButton(14, 5, "b2");
NexButton avanca_navegacao_conf_2 = NexButton(14, 7, "b3");
NexButton volta_navegacao_conf_2 = NexButton(14, 8, "b4");
NexButton home_navegacao_conf_2 = NexButton(14, 1, "b0");
bool navegacao_conf_2_bool;
//end page14

// ------------------------ pagina configurações page15
NexText show_cultivo = NexText(15, 6, "t2");
NexText show_hora = NexText(15, 9, "t3");

NexButton editar_cultivo = NexButton(15, 4, "b1");
NexButton atualiza_hora = NexButton(15, 5, "b2");
NexButton avanca_navegacao_conf_3 = NexButton(15, 7, "b3");
NexButton volta_navegacao_conf_3 = NexButton(15, 8, "b4");
NexButton home_navegacao_conf_3 = NexButton(15, 1, "b0");
bool navegacao_conf_3_bool;
//end page15

//-------------------------- paginas nextion ----------------------------------
NexPage page0 = NexPage(0, 0, "page0");
NexPage page1 = NexPage(1, 0, "page1");
NexPage page2 = NexPage(2, 0, "page2");
NexPage pagemodos = NexPage(3, 0, "page3");
NexPage pagemeumodo = NexPage(7, 0, "page7");
NexPage page_cultivo = NexPage(8,0, "page8");
NexPage pagina_cidade = NexPage(9,0, "page9");
NexPage page_salvo = NexPage(10,0, "page10");
NexPage page_apelido = NexPage(11,0, "page11");
NexPage zaperro = NexPage(12, 0, "page12");
NexPage navegacao_conf_1 = NexPage(13, 0, "page13");
NexPage navegacao_conf_2 = NexPage(14, 0, "page14");
NexPage navegacao_conf_3 = NexPage(15, 0, "page15");
NexPage pagina_pais = NexPage(16, 0, "page16");
NexPage pagina_wifi = NexPage(17, 0, "page17");
NexPage pagina_senha_wifi = NexPage(18, 0, "page18");
NexPage pagina_telefone = NexPage(19, 0, "page19");
NexPage pagina_hon = NexPage(20, 0, "page20");
NexPage pagina_mon = NexPage(21, 0, "page21");
NexPage pagina_hoff = NexPage(22, 0, "page22");
NexPage pagina_moff = NexPage(23, 0, "page23");



NexTouch *nex_listen_list[] = 
{
      &home3,
      &modos,
      &vega,
      &flora,
      &germ,
      &trabalho,
      &meumodo,
      &colheita,
      &btn_on,
      &btn_off,
      &conf,
      &homemeumodo,
      &confmeumodo,
      &salva_horaon,
      &salva_minon,
      &salva_horaoff,
      &salva_minoff,
//      &homeconf1,
//      &home_conf2,
//      &salvar_conf1,
      &salvacultivo,
      &salvacidade,
      &salvapais,
      &salvar_ssid,
      &salvar_senha,
      &editar_wifi,
      &configurara1b,
//      &salvar_memoria_conf2,
    //  &proxima_conf,
    //  &avanca_conf3,
    //  &volta_conf1,
    //  &volta_conf2,
      &page2,
      &conf_nome,
      &conf_numero,
      &envia_zap,
      &editar_local,
      &avanca_navegacao_conf_1,
      &volta_navegacao_conf_1,
      &home_navegacao_conf_1, 
      &editar_nome,
      &editar_telefone,
      &avanca_navegacao_conf_2,
      &volta_navegacao_conf_2,
      &home_navegacao_conf_2,
      &editar_cultivo,
      &atualiza_hora,
      &avanca_navegacao_conf_3,
      &volta_navegacao_conf_3,
      &home_navegacao_conf_3,
      &back_ssid,
      &back_cidade,
      &back_telefone,
      &back_cultivo,
      &wifi_mod,
      &edita_ligar,
      &edita_desligar,
      NULL
};


// ------------------------------------------------RTC VARIAVEIS E DECLACACAO -----------------------------------------------------
RTC_DS1307 rtc;            // RTC

// Variaveis do rtc
byte m = 0;          // contains the minutes, refreshed each loop
byte h = 0;          // contains the hours, refreshed each loop
byte s = 0;         // contains the seconds, refreshed each loop
byte mo = 0;       // contains the month, refreshes each loop
int j = 0;           // contains the year, refreshed each loop
byte d = 0;         // contains the day (1-31)
byte dia = 0;     // contains day of week (0-6)
//----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- Variaveis para envio de textos pagina 1 ---------------------------------------------
String horanex;
char hora[50];
int pic = 0; //variavel de controle para alterar imagem de incones

//----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------variáveis para controle do meu modo----------------------------------------------
char horaon[2];
char min_on[2];
char horaoff[2];
char min_off[2];

char char_horaon[3];
char char_minon[3];
char char_horaoff[3];
char char_minoff[3];

int h1_on, h1_off, m1_on, m1_off;
String h1_on1, h1_off1, m1_on1, m1_off1;
//----------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------ variáveis para controle configuracoes tela -------------------------------------------------
int ctrl_dias;
// *--------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------- Variáveis OpenWeatherMap ---------------------------------------------------
char char_cidade[20];
char char_pais[5];

char char_cidade2[20];
char char_pais2[5];

String L1 = "http://api.openweathermap.org/data/2.5/weather?q="; // primeira parte compor o link
String cidade1; // = "blumenau";
String L2 = ",";
String pais1; // = "br";
String L3 = "&APPID=e36be7b2ff2803b55607b040624fcc79"; // apos o igual é a chave do open, API KEY
String link_open;

StaticJsonDocument<1024> doc;


char weathermap_humidade[10];
char weathermap_mintemp[10];
char weathermap_maxtemp[10];

float main_temp_min;
float main_temp_max;
float main_humidity;


//---------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------- VARIAVEIS PARA OBTER DADOS SSID PASS E TIMEZONE ----------------------------------
char char_ssid[15]; // da nextion
char ssid[15];

char char_pass[15];
char pass[15];

char char_timezone[7];
char salvo_timezone[7];
// --------------------------------------------------------------------------------------------------------------------------------

//Bme 280
float sensor_bmeT, sensor_bmeP, sensor_bmeA, sensor_bmeH;
String message;
void zap_link(){
  verifica_conexao();
  if(wifi_on==1){
    String link1 = " https://thingspeak.com/channels/";
    String link_zap = link1 + string_canal;
    message = "Para verificar seus ultimos dados acesse: " + link_zap;
    WiFiClient client;
    HTTPClient http;
    http.begin(client,"http://v4.chatpro.com.br/chatpro-oxj6lhn8ub/api/v1/send_message");
    //http.begin("http://v4.chatpro.com.br/chatpro-hbjlut6yxw/api/v1/send_message"); //https://v4.chatpro.com.br/chatpro-eadpc477y7/v1/send_message
    DynamicJsonDocument zap(2048);
    zap["message"]=message;
    zap["number"]=string_telefone;
    String envio;
    serializeJson(zap,envio);
    http.addHeader("accept", "application/json"); // aceitar arquivos json
    http.addHeader("Authorization", "ju7yh2i83w64ugsmihnynggno421d7"); //token de autorização
    http.addHeader("Content-Type", "application/json"); // envio json
    int httpResponseCode = http.POST(envio);
    http.end();
    if(httpResponseCode==400)
    {
      zaperro.show();
      delay(3500);
      page1.show();
      pagina_home();
    }
  }
}

void clima_weathermap(){                    // função para obter dados do openweathermap
  cidade1 =String(char_cidade2);
  pais1 = String(char_pais2);
  link_open = L1 + cidade1 + L2 + pais1 + L3;
  HTTPClient http;
  http.begin(client,link_open);

  int httpCode = http.GET();
  if(httpCode >0){
    String input = http.getString();
    DeserializationError error = deserializeJson(doc, input);

    float coord_lon = doc["coord"]["lon"]; // -48.5012
    float coord_lat = doc["coord"]["lat"]; // -27.6146

    JsonObject weather_0 = doc["weather"][0];
    int weather_0_id = weather_0["id"]; // 804
    const char* weather_0_main = weather_0["main"]; // "Clouds"
    const char* weather_0_description = weather_0["description"]; // "overcast clouds"
    String weather_0_icon = weather_0["icon"]; // "04d"

    const char* base = doc["base"]; // "stations"

    JsonObject main = doc["main"];
    float main_temp = main["temp"]; // 285.79
    float main_feels_like = main["feels_like"]; // 285.12
    main_temp_min = main["temp_min"]; // 283.85
    main_temp_max = main["temp_max"]; // 287.39
    int main_pressure = main["pressure"]; // 1024
    main_humidity = main["humidity"]; // 77

    int visibility = doc["visibility"]; // 10000

    float wind_speed = doc["wind"]["speed"]; // 1.54
    int wind_deg = doc["wind"]["deg"]; // 130

    int clouds_all = doc["clouds"]["all"]; // 90

    long dt = doc["dt"]; // 1623932300

    JsonObject sys = doc["sys"];
    int sys_type = sys["type"]; // 1
    int sys_id = sys["id"]; // 8360
    const char* sys_country = sys["country"]; // "BR"
    long sys_sunrise = sys["sunrise"]; // 1623924187
    long sys_sunset = sys["sunset"]; // 1623961612
    int timezone = doc["timezone"]; // -10800

    //utcOffsetInSeconds = long(timezone);
    long id = doc["id"]; // 6323121
    const char* name = doc["name"]; // "Florianópolis"
    int cod = doc["cod"]; // 200
 

      if(weather_0_icon=="04d"){
        clima.Set_background_image_pic(25);
      }
      if(weather_0_icon=="01d"){
        clima.Set_background_image_pic(20);
      }
        

     if(weather_0_icon=="01n"){
       clima.Set_background_image_pic(21);
     }

     if(weather_0_icon=="02d"){
        clima.Set_background_image_pic(22);
     }

     if(weather_0_icon=="02n"){
        clima.Set_background_image_pic(23);
     }

     if(weather_0_icon=="03d"){
       clima.Set_background_image_pic(24);
     }

     if(weather_0_icon=="03n"){
       clima.Set_background_image_pic(24);
     }

     if(weather_0_icon=="04n"){
      clima.Set_background_image_pic(25);
     }
        
     if(weather_0_icon=="09d"){
        clima.Set_background_image_pic(26);
     }
   
 

     if(weather_0_icon=="09n"){
      clima.Set_background_image_pic(26);
     }

     if(weather_0_icon=="10d"){
      clima.Set_background_image_pic(27);
     }

     if(weather_0_icon=="10n"){
      clima.Set_background_image_pic(28);
     }
      
     if(weather_0_icon=="11d"){
       clima.Set_background_image_pic(29);
     }

     if(weather_0_icon=="11n"){
      clima.Set_background_image_pic(29);
     }
        
 

     if(weather_0_icon=="13d"){
        clima.Set_background_image_pic(30);
     }
        
     

     if(weather_0_icon=="13n"){
        clima.Set_background_image_pic(30);
     }
    
  }
  else{
    temp_min.setText("00");
    temp_max.setText("00");
    weathermap_h.setText("00");
  }
  main_temp_min = main_temp_min - 273;
  main_temp_max = main_temp_max - 273;
  
  snprintf(weathermap_mintemp, 8, "%.0f", main_temp_min);
  temp_min.setText(weathermap_mintemp);
  snprintf(weathermap_maxtemp, 8, "%.0f", main_temp_max);
  temp_max.setText(weathermap_maxtemp);
  snprintf(weathermap_humidade, 8, "%.0f", main_humidity);
  weathermap_h.setText(weathermap_humidade);


  http.end();
  
}
void NTP(){
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

  timeClient.update();
  byte horantp = timeClient.getHours(); // pega hora, minuto e segundos do ntp
  byte minntp = timeClient.getMinutes();
  byte segundosntp = timeClient.getSeconds();
  byte diantp = timeClient.getDay();
  Serial.println(horantp);
  Serial.println("Hora do rtc:");
  Serial.println(h);
  rtc.adjust(DateTime(j, mo, diantp, horantp, minntp, segundosntp)); // ano, mes, dia, hora, minuto, segundo
} // end NTP


void mudamodo(){
  String h10, m10;
  h10 = String(h);
  m10 = String(m);
  if(modo==0){ //custom
    if(h10>h1_on1 && h10<h1_off1  || (h10==h1_on1 && m10>=m1_on1  && h10==h1_off1 && m10<m1_off1) || (h10==h1_on1 && m10>=m1_on1 && h10<h1_off1) || (h10==h1_off1 && m10<=m1_off1)){
      digitalWrite(rele,LOW);
      luz=1;
    }
    else{
      if(luz==1)
      {
        digitalWrite(rele,HIGH);
        luz=0;
      }
      
    }
//leds
    if(pic==0){
       modos.Set_background_image_pic(10);
       pic=1;
       
    }
  }
  
  if(modo==1 && page1b==true && pic==0){ //modo desligado
    modos.Set_background_image_pic(11);
    pic = 1;
  }
  if(modo==1){//modo desligado
    digitalWrite(rele,HIGH);
    luz = 0;
    }
  
  if(modo==2){ //modo flora
    if( h>=6 && h<=18 && luz==0){
      digitalWrite(rele,LOW);
      luz = 1;
    }
    else{
      if((luz==1 && h<6)|| (luz==1 && h>18)){
        digitalWrite(rele,HIGH);
        luz = 0;
      }
      
    }
    if((h>=5 && h<=6)||(h>=18 && h<=19)){
      digitalWrite(led_vermelho1, HIGH);
      digitalWrite(led_vermelho2, HIGH);
    }
    else{
      digitalWrite(led_vermelho1, LOW);
      digitalWrite(led_vermelho2, LOW);
    }
    if((h==5 && m>=30)||(h==18 && m<=30)){
      digitalWrite(led_amarelo, HIGH);
    }
    else{
      digitalWrite(led_amarelo, LOW);
    }
    if(page1b==true && pic==0){
      modos.Set_background_image_pic(8);
    }
    
  }
  if(modo==3){ //modo vega
    if(h>=5 && h<=23 && luz==0){
      digitalWrite(rele,LOW);
      luz=1;
    }
    else{
      if((luz==1 && h<5)||(luz==1 && h>23)){
        digitalWrite(rele,HIGH);
        luz=0;
      }
    }
     if((h>=4 && h<=5)||(h==23)){
      digitalWrite(led_vermelho1, HIGH);
      digitalWrite(led_vermelho2, HIGH);
    }
    else{
      digitalWrite(led_vermelho1, LOW);
      digitalWrite(led_vermelho2, LOW);
    }
    if((h==4 && m>=30)||(h==23 && m<=30)){
      digitalWrite(led_amarelo, HIGH);
    }
    else{
      digitalWrite(led_amarelo, LOW);
    }
    if(page1b==true && pic==0){
      modos.Set_background_image_pic(14);
      pic=1;
    }
  }

  if(modo==4 && luz==0){ //modo ligado
    digitalWrite(rele,LOW);
    luz = 1;
  }
  if(modo==4 && page1b==true && pic==0){
    modos.Set_background_image_pic(12);
    pic=1;
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);
  }

  if(modo==5){
    digitalWrite(rele,HIGH);
    analogWrite(05,250);
    if(page1b==true && pic==0){
      modos.Set_background_image_pic(13);
      pic=1;
    }
  }

  if(modo==7){
    digitalWrite(rele,HIGH );
    analogWrite(05,180);
    luz = 0;
    if(page1b==true && pic==0){
      modos.Set_background_image_pic(9);
      pic=1;
    }
  }

  if(modo==8){
    digitalWrite(rele,HIGH);
    luz=0;
    if(page1b==true && pic==0){
      modos.Set_background_image_pic(7);
      pic=1;
    }
  }
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
} // end muda modo


//função para printar valores tela conf3
void tela3(){
  DateTime now = rtc.now();
  h = now.hour();
  m = now.minute();
  s = now.second();

  horanex.toCharArray(hora, 50);
  show_hora.setText(hora);

  String c_txt = String(diascultivo);
  char char_diacultivo[5];
  c_txt.toCharArray(char_diacultivo, 5);
  show_cultivo.setText(char_diacultivo);
}

void tela1(){
  char ssid_tela [50];
  char cidade2_tela [80];
//  ssid.toCharArray(ssid_tela, 50);
  //char_cidade2.toCharArray(cidade2_tela, 80);
  show_redewifi.setText(ssid);
  show_cidade.setText(char_cidade2);
}

void tela2(){
/*  
  char telefone_tela [50];
  char nome_tela [50];
  char_nome_usuario.toCharArray(nome_tela, 50);
  char_telefone.toCharArray(telefone_tela, 50);
*/
  show_nome.setText(char_nome_usuario);
  show_telefone.setText(char_telefone);
}


void enviaValores() {
  ThingSpeak.setField(1, sensor_bmeH);
  ThingSpeak.setField(2, sensor_bmeT);
  ThingSpeak.setField(3, luz);
  ThingSpeak.setField(4, modo);
  ThingSpeak.setField(5, bmp.readPressure());
  ThingSpeak.setField(6, diascultivo);
  ThingSpeak.setField(8, TempLamp);
  
   int compara = ThingSpeak.readIntField(myChannelNumber, 7);
  if(compara==420){
      update_system();
     }
//  else{
  //  ThingSpeak.setField(7, codev);
  //}
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
}

// sensor que esta dentro da lampada
void sensordht(){
  TempLamp = dht.readTemperature();
  HumLamp = dht.readHumidity();

}

void rtcnextion(){
  if(m<10){
    horanex = String(h) + ":0" + String(m);
  }
  else{
   horanex = String(h) + ":" + String(m);
  }
  if(s==0){
    horanex.toCharArray(hora, 50);
    horartc.setText(hora);
  }
  
}
void ler_memoria2()
{
  StaticJsonDocument<768> infos3;
  File file2 = SPIFFS.open("/dadosusuario.json", "r");
  if(deserializeJson(infos3, file2)){
    
  }
  else{
    strlcpy(char_nome_usuario, infos3["nome"]      | "", sizeof(char_nome_usuario));
    strlcpy(char_telefone, infos3["telefone"]      | "", sizeof(char_telefone));
    strlcpy(char_canal, infos3["canal"]      | "", sizeof(char_canal));
    file2.close();
    converte_memoria2();
  }
}
void ler_memoria_modo(){
  StaticJsonDocument<768> infos4;
  File file = SPIFFS.open("/infos4.json", "r");
  if (deserializeJson(infos4, file)) {

  }
  else{
    modo = infos4["modo"];
    file.close();
  }
}
void ler_memoria(){
 // h1_on, h1_off, m1_on, m1_off;
  StaticJsonDocument<768> infos2;
  File file = SPIFFS.open("/infos2.json", "r");
  if (deserializeJson(infos2, file)) {

  }
  else{

  strlcpy(ssid, infos2["ssid"]      | "", sizeof(ssid));
  strlcpy(pass, infos2["password"]      | "", sizeof(pass));
  strlcpy(char_horaon, infos2["hora_on"]      | "", sizeof(char_horaon));
  strlcpy(char_horaoff, infos2["hora_off"]      | "", sizeof(char_horaoff));
  strlcpy(char_minon, infos2["min_on"]      | "", sizeof(char_minon));
  strlcpy(char_minoff, infos2["min_off"]      | "", sizeof(char_minoff));
  strlcpy(char_cidade2, infos2["cidade"]      | "", sizeof(char_cidade2));
  strlcpy(char_pais2, infos2["pais"]      | "", sizeof(char_pais2));
  strlcpy(salvo_timezone, infos2["timezone"]      | "", sizeof(salvo_timezone));
  diascultivo = infos2["dias_cultivo"];
 // strlcpy(diascultivo2, infos2["dias_cultivo"]      | "", sizeof(diascultivo2));
  file.close();  
  converte_memoria();
  }
}
void converte_memoria2(){
  string_telefone = String(char_telefone);
  string_nome_usuario = String(char_nome_usuario);
  string_canal = String(char_canal);
}
void converte_memoria(){
  
  h1_on = atoi(&char_horaon[0]);
  h1_off = atoi(&char_horaoff[0]);
  m1_on = atoi(&char_minon[0]);
  m1_off = atoi(&char_minoff[0]);

  h1_on1 = String(h1_on);
  h1_off1 = String(h1_off);
  m1_on1 = String(m1_on);
  m1_off1 = String(m1_off);
  
}

String link;
String message_v;
String q;
void zap(){
  verifica_conexao();
   if(wifi_on==1){
    /*if(modo==0){ //meu modo
      //String message = "Meumodo ";
    }*/

    //Tematica DIAS
    if(modo==3)
    {
    if(diascultivo == 0){
         link = "https://www.youtube.com/watch?v=Sp3zj0E17pc&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo?click=robot-lamp";
         message_v = "Animado pra colocar o Ganza pra funcionar? Assista esse vídeo pra aprender como instalar ele no seu grow! " + link; 
         zap_envio_video();
      }
    if(diascultivo == (vegad + 23)){ // 23 dias apos iniciar o modo vega
         link = "https://www.youtube.com/watch?v=vJYMu-uxvxg&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=2?click=robot-lamp";
         message_v = "Dia 23 no modo vega!! Está chegando a hora de fazer as primeiras amarras! Olha que vídeo interessante encontrei pra você:  " + link; 
         zap_envio_video();
      }
    if(diascultivo == (vegad + 27)){ //27 dias no modo vega
         link = "https://www.youtube.com/watch?v=vJYMu-uxvxg&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=3?click=robot-lamp";
         message_v = "27 dias no modo vega! Ja fez suas primeiras amarras grower? Olha aqui um vídeo pra você ter uma ideia de como é o dia 27 do cultivo:  " + link; 
         zap_envio_video();
      }
    if(diascultivo == 26){
         link = "https://www.youtube.com/watch?v=Tr_eJbMrkMg&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=4?click=robot-lamp";
         message_v = "Terminamos o primeiro mês de cultivo!! Se liga nessas dicas: " + link; 
         zap_envio_video();
      }
    if(diascultivo == 31){
         link = "https://www.youtube.com/watch?v=UsWuQse2gFg&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=5?click=robot-lamp";
         message_v = "Dia 31!! Olha essas sugestões do Nartural pra você deixar seu cultivo muito show: " + link; 
         zap_envio_video();
      }
    if(diascultivo == 36){
         link = "https://www.youtube.com/watch?v=2R6jl-OAV6g&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=6?click=robot-lamp";
         message_v = "Dia 36 do cultivo!! Nosso desenvolvimento ta top em! Olha ai mais algumas dicas pra você: " + link; 
         zap_envio_video();
      }
    if(diascultivo == 41){
         link = "https://www.youtube.com/watch?v=Jd005p43GI4&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=7?click=robot-lamp";
         message_v = "Estamos chegando na floração! 41 dias de cultivo! Sua planta ta top! " + link; 
         zap_envio_video();
      }
    }
    if(modo==2){
    if(diascultivo == florad){
         link = "https://www.youtube.com/watch?v=Tr_eJbMrkMg&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=8?click=robot-lamp";
         message_v = "Entramos no modo flora!! Começou a flora! Só pra não perder o costume, olha umas dicas do natural ai pra você: " + link; 
         zap_envio_video();
      }
    if(diascultivo == (florad + 5)){
         link = "https://www.youtube.com/watch?v=Tr_eJbMrkMg&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=9?click=robot-lamp";
         message_v = "Dia 5 do modo flora!! Pega essas dicas: " + link; 
         zap_envio_video();
      }
    if(diascultivo == (florad + 10)){
         link = "https://www.youtube.com/watch?v=bqOQYfwLo6o&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=10?click=robot-lamp";
         message_v = "9 dia de cultivo no modo flora!! Videozinho pra você ficar ligado: " + link; 
         zap_envio_video();
      }
    if(diascultivo == (florad + 15)){
         q = String(diascultivo);
         link = "https://www.youtube.com/watch?v=Y4_rpJ4zJtQ&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=11?click=robot-lamp";
         message_v = "Dia " + q + " do cultivo! Não esquece do seu cultivo grower! Mais um vídeo pra você deixar tudo no desempenho máximo: " + link; 
         zap_envio_video();
      }
    if(diascultivo == (florad + 20)){
         q = String(diascultivo);
         link = "https://www.youtube.com/watch?v=WJeWvHmNkXs&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=12?click=robot-lamp";
         message_v = "Dia " + q + " do cultivo! Como ta a sua planta grower? Olha um vídeo pra você: " + link; 
         zap_envio_video();
      }
    if(diascultivo == (florad + 30)){
         q = String(diascultivo);
         link = "https://www.youtube.com/watch?v=ajYFiAb4jsc&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=13?click=robot-lamp";
         message_v = "Dia "+ q +"! Olha essas sugestões do Nartural pra você deixar seu cultivo muito show: " + link;
         zap_envio_video(); 
      }
    if(diascultivo == (florad + 35)){
         q = String(diascultivo);
         link = "https://www.youtube.com/watch?v=_FJHw7BHjbY&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=14?click=robot-lamp";
         message_v = "Vamos turbinar essas plantas: dia 76 do cultivo!! Video do natural com umas ideias pra você: " + link; 
         zap_envio_video();
      }
    if(diascultivo ==(florad + 35)){
         q = String(diascultivo);
         link = "https://www.youtube.com/watch?v=qT3zteJnjxs&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=15?click=robot-lamp";
         message_v = q + " dias! Ta gostando do seu cultivo? Acompanha o vídeo do Natural pra você entender melhor esse processo: " + link; 
         zap_envio_video();
      }
    if(diascultivo == (florad + 45)){
         q = String(diascultivo);
         link= "https://www.youtube.com/watch?v=RZVpGov2d9E&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=16?click=robot-lamp";
         message_v = "Dia 45 no modo flora!! Eee pra não perder o costume, olha umas dicas do natural ai pra você: " + link; 
         zap_envio_video();
      }
    if(diascultivo == (florad + 50)){
         q = String(diascultivo);
         link = "https://www.youtube.com/watch?v=kIDy0OP3Qpk&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=17?click=robot-lamp";
         message_v = "Dia "+ q + " do cultivo!! Olha aqui um vídeo pra você ter uma ideia de como é essa semana no cultivo: " + link; 
         zap_envio_video();
      }
    if(diascultivo == (florad + 55)){
         link = "https://www.youtube.com/watch?v=yPUNrkeqzIc&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=18?click=robot-lamp";
         message_v = "Chegando o dia 55 do modo flora e você ja sabe né, tem video te explicando tudo: " + link; 
         zap_envio_video();
      }
    if(diascultivo == (florad + 65)){
         q = String(diascultivo);
         link = "https://www.youtube.com/watch?v=OqUtFdenYsg&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=19?click=robot-lamp";
         message_v = "Dia " + q + " do cultivo e estamos na reta finaal, segue no vídeo pra você fechar do melhor jeito: " + link; 
         zap_envio_video();
      }
    if(diascultivo == (florad + 70)){
         q = String(diascultivo);
         link= "https://www.youtube.com/watch?v=mbwGhGSNCdI&list=PL1fin4OYe-XA3_-FKBmShxwjKVOP9N1Mo&index=20?click=robot-lamp";
         message_v = "Finalinho do nosso cultivo! Dia " + q + "! Vamos nos preparar para a colheita com as dicas do Natural: " + link; 
         zap_envio_video();
      }
    }

    //Tematica basica +modos
    if(diascultivo == 0){
         link = "https://www.youtube.com/watch?v=Cu2YLBqo2N4&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=21?click=robot-lamp";
         message_v = "Caramba!! Eu to muito ansioso e curioso pra saber do que vamos cuidar juntos! Aqui tem um videozinho sobre sementes, pra você separar e escolher a nossa! Não vejo a hora de começarmos 🌱 " + link; 
         zap_envio_video();
      }
    if(diascultivo == 2){
         link = "https://www.youtube.com/watch?v=jEZv_3TYbOw&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=16?click=robot-lamp";
         message_v = "Salve Grower! Deu uma olhada nas sementes hoje? Enquanto isso, vamos escolher os vasos? É pra lá que nossa plantinha vai! Se precisar de ajuda com isso, vou deixar um link aqui pra você ficar fera no assunto! " + link; 
         zap_envio_video();
      }
    if(diascultivo == 3){
        link = "https://www.youtube.com/watch?v=3o4IX_iKBXk&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=15?click=robot-lamp";
        message_v = "Então grower, se liga porque as nossas lâmpadas são as mais tops pra sua planta! " + link; 
        zap_envio_video();
      }
    if(diascultivo == 4){
         link = "https://www.youtube.com/watch?v=4T2ZKlgn5V0&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=11?click=robot-lamp";
         message_v = "E aí grower? Já se preocupou com a circulação de ar dentro do seu Grow? " + link; 
         zap_envio_video();
      }
    if(diascultivo == 5){
         link = "https://www.youtube.com/watch?v=a3Kg--fdEf8&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=14?click=robot-lamp";
         message_v = "Então grower, quer ouvir um papo iluminado pra saber um pouco mais sobre iluminação? Olha o liiink: " + link; 
         zap_envio_video();
      }
    if(diascultivo == 6){
         link = "https://www.youtube.com/watch?v=1P6B1H6nnYQ&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=12?click=robot-lamp";
         message_v = " Salvee grower, você sabe que eu fico de olho na temperatura pra você né, mas aqui tem um vídeo se você quiser entender melhor isso tudo! " + link; 
         zap_envio_video();
      }
    if(diascultivo == 7){
         link = "https://www.youtube.com/watch?v=Aqac-9VhOiM&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=13?click=robot-lamp";
         message_v = " Fala grower, ja sacou que eu cuido da umidade do seu cultivo pra você, né!? Que tal aproveitar pra entender mais sobre a umidade relativa no seu cultivo: " + link;
         zap_envio_video(); 
      }
     //FLORA
    if(modo==2){ 
      if(diascultivo == florad){
         link = "https://www.youtube.com/watch?v=ikHGEkvjzJE&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=6?click=robot-lamp";
         message_v = "Você quer saber como funciona a floração da sua planta? Vem ver: " + link; 
         zap_envio_video();
      }
    }
    //vega
    if(modo==3){//VERIFICAR CONDIÇÂO
      if(diascultivo == vegad){
         link = "https://www.youtube.com/watch?v=B9YOJQuR05w&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=17?click=robot-lamp";
         message_v = "Como é o seu subtrato? Quer saber um pouco mais? Segue no link " + link; 
         zap_envio_video();
      }
      if(diascultivo ==vegad){
         link = "https://www.youtube.com/watch?v=7ua0k-GNohc&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=18?click=robot-lamp";
         message_v = "Hoje é dia de dar um turbo, vídeo sobre nutrientes e fertilizantes na produção: " + link; 
         zap_envio_video();
      }
      if(diascultivo ==vegad){
         link = "https://www.youtube.com/watch?v=iP3BVV_8wD0&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=5?click=robot-lamp";
         message_v = "Modo Vega, o que é isso? Vem assistir: " + link; 
         zap_envio_video();
      }
    }
    //germinação
    if(modo==5){
      if(diascultivo ==germinacaod){
         link = "https://www.youtube.com/watch?v=ddaAQu1k7i8&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=4?click=robot-lamp";
         message_v = "Que legal!!! Vamos começar a germinar! Eu realmente estava esperando muito por isso! Já que entramos nesse modo, aqui tem um vídeo que fala um pouquinho sobre, pra não passar nada batido. Se liga aí grower: " + link; 
         zap_envio_video();
      }
      if(diascultivo ==germinacaod){
         link = "https://www.youtube.com/watch?v=O3o9sjKjP5E&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=19?click=robot-lamp";
         message_v = "Agora que estamos germinando, como fica água e rega? Se liga nesse vídeo " + link; 
         zap_envio_video();
      }
      if(diascultivo ==germinacaod){
         link = "https://www.youtube.com/watch?v=0Kq8d5IDGGU&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=20?click=robot-lamp";
         message_v = "Você sabe a importância do Ph do solo? O que é isso? Segue o vídeoo " + link; 
         zap_envio_video();
      }
      
    }
    //colheita
    if(modo==7){
      if(diascultivo ==colheitad){
         link = "https://www.youtube.com/watch?v=zdCQrpRNZeQ&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=7?click=robot-lamp";
         message_v = "Ta chegando a hora da colheita então? Como saber o momento certo, assiste o vídeo: " + link;
         zap_envio_video(); 
      }
      if(diascultivo ==colheitad){
         link = "https://www.youtube.com/watch?v=u8CWXEZFbXE&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=8?click=robot-lamp";
         message_v = "Segue esse link pra entender como fazer a secagem da sua planta da forma certa:  " + link;
         zap_envio_video(); 
      }
      if(diascultivo ==colheitad){
         link = "https://www.youtube.com/watch?v=4akO0ldGS5M&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=9?click=robot-lamp";
         message_v = "Ta ficando pronta a sua sativa! Olha como fazer a cura pra finalizar o processo de produção da sua planta!  " + link; 
         zap_envio_video();
      }
    }
    /*if(modo==8){//trabalho outros
      if(diascultivo ==1000){
        String link = "https://www.youtube.com/watch?v=mAdSOtFklLo&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=10";
        String message = "Salve salve humano!!! Já to chegando pra te ajudar a cuidar do seu Grow. Mas calma, você já sabe o que é um grow? Tá preparado pra me receber aí? Se não, nesse vídeo aqui tem algumas dicas sobre o que é e como montar o seu. Se liga aí 😁:" + link; 
      }
      if(diascultivo ==1000){
        String link = "https://www.youtube.com/watch?v=Cu2YLBqo2N4&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=21";
        String message = "Caramba!! Eu to muito ansioso e curioso pra saber do que vamos cuidar juntos! Aqui tem um videozinho sobre sementes, pra você separar e escolher a nossa! Não vejo a hora de começarmos 🌱 " + link; 
      }
      if(diascultivo ==1000){
        String link = "https://www.youtube.com/watch?v=jEZv_3TYbOw&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=16";
        String message = "Calma que eu tô chegando haha, falta pouco! Espero que você já tenha separado as sementes! Enquanto isso, vamos escolher os vasos? É pra lá que nossa plantinha vai! Se precisar de ajuda com isso, vou deixar um link aqui pra você ficar fera no assunto! " + link; 
      }
      if(diascultivo ==1000){
        String link = "https://www.youtube.com/watch?v=3o4IX_iKBXk&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=15";
        String message = "Então grower, se liga porque as nossas lâmpadas são as mais tops pra sua sativa! " + link; 
      }
      if(diascultivo ==1000){
        String link = "https://www.youtube.com/watch?v=4T2ZKlgn5V0&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=11";
        String message = "E aí grower? Vamos ver como fazer a melhor circulação pro seu grow! " + link; 
      }
      if(diascultivo ==1000){
        String link = "https://www.youtube.com/watch?v=a3Kg--fdEf8&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=14";
        String message = "Então grower, quer ouvir um papo iluminado pra saber um pouco mais sobre iluminação? Olha o liiink: " + link; 
      }
      if(diascultivo ==1000){
        String link = "https://www.youtube.com/watch?v=1P6B1H6nnYQ&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=12";
        String message = " Salvee grower, você sabe que eu fico de olho na temperatura pra você né, mas aqui tem um vídeo se você quiser entender melhor isso tudo! " + link; 
      }
      if(diascultivo ==1000){
        String link = "https://www.youtube.com/watch?v=Aqac-9VhOiM&list=PL1fin4OYe-XAu85YBcGhPu00IHXxzArGO&index=13";
        String message = " Fala grower, quando eu chegar ja vejo como ta a umidade pra você, enquanto isso que tal aproveitar pra entender mais sobre a umidade relativa: " + link; 
      }*/
      
}
   }

void zap_envio_video(){
    WiFiClient client;
    HTTPClient http;
    http.begin(client,"http://v4.chatpro.com.br/chatpro-oxj6lhn8ub/api/v1/send_message"); //https://v4.chatpro.com.br/chatpro-eadpc477y7/v1/send_message
    DynamicJsonDocument zap(2048);
    zap["message"]=message_v;
    zap["number"]=string_telefone;
    String envio;
    serializeJson(zap,envio);
    http.addHeader("accept", "application/json"); // aceitar arquivos json
    http.addHeader("Authorization", "ju7yh2i83w64ugsmihnynggno421d7"); //token de autorização
    http.addHeader("Content-Type", "application/json"); // envio json
    int httpResponseCode = http.POST(envio);
    http.end(); 
}
void zap_envio(){
    HTTPClient http;
    http.begin(client,"http://v4.chatpro.com.br/chatpro-oxj6lhn8ub/api/v1/send_message"); //https://v4.chatpro.com.br/chatpro-eadpc477y7/v1/send_message
    DynamicJsonDocument zap(2048);
    zap["message"]=message;
    zap["number"]=string_telefone;
    String envio;
    serializeJson(zap,envio);
    http.addHeader("accept", "application/json"); // aceitar arquivos json
    http.addHeader("Authorization", "ju7yh2i83w64ugsmihnynggno421d7"); //token de autorização
    http.addHeader("Content-Type", "application/json"); // envio json
    int httpResponseCode = http.POST(envio);
    http.end(); 
}


void wifi(){
  int w = 0;
  String ssidb = String(ssid);
  String passb = String(pass);
  WiFi.begin(ssidb,passb);
  WiFi.begin(ssidb,passb);
  WiFi.begin(ssidb,passb);
  WiFi.begin(ssidb,passb);

}

void verifica_conexao(){
  if(WiFi.status() != WL_CONNECTED){
    wifi_on = 0;
    wifi_mod.Set_background_image_pic(4);
  }
  if(WiFi.status() == WL_CONNECTED){
    wifi_on=1;
    wifi_mod.Set_background_image_pic(16);
  }
}
void pagina_home(){
  DateTime now = rtc.now();
  h = now.hour();
  m = now.minute();
  s = now.second();

  horanex.toCharArray(hora, 50);
  horartc.setText(hora);
  sensorbme();
  if(WiFi.status() == WL_CONNECTED){
    clima_weathermap();
    cidade.setText(char_cidade2);
  }
  else{
    cidade.setText("sem conexao :(");
  }
  verifica_conexao();
  String c_txt = String(diascultivo);
  char char_diacultivo[5];
  c_txt.toCharArray(char_diacultivo, 5);
  //itoa(diascultivo, char_diacultivo, 4);
  //snprintf(char_diacultivo, 8, "%.0f", diascultivo);
  daycultivo.setText(char_diacultivo);
  m_descanso = m + 2;

     if(modo==0){
     modos.Set_background_image_pic(10);
   }
   if(modo==1){
    modos.Set_background_image_pic(11);
   }
   if(modo==2){
    modos.Set_background_image_pic(8);
   }
   if(modo==3){
    modos.Set_background_image_pic(14);
   }
   if(modo==4){
    modos.Set_background_image_pic(12);
   }
   if(modo==5){
    modos.Set_background_image_pic(13);
   }
   if(modo==7){
    modos.Set_background_image_pic(9);
   }
   if(modo==8){
    modos.Set_background_image_pic(7);
   }
}

void pagina_descanso(){
  DateTime now = rtc.now();
  h = now.hour();
  m = now.minute();
  s = now.second();
  if(m<10){
    horanex = String(h) + ":0" + String(m);
  }
  else{
   horanex = String(h) + ":" + String(m);
  }
  
   horanex.toCharArray(hora, 50);
   horartc_descanso.setText(hora);
   sensorbme_descanso(); 
   if(modo==0){
     modo_descanso.Set_background_image_pic(10);
   }
   if(modo==1){
    modo_descanso.Set_background_image_pic(11);
   }
   if(modo==2){
    modo_descanso.Set_background_image_pic(8);
   }
   if(modo==3){
    modo_descanso.Set_background_image_pic(14);
   }
   if(modo==4){
    modo_descanso.Set_background_image_pic(12);
   }
   if(modo==5){
    modo_descanso.Set_background_image_pic(13);
   }
   if(modo==7){
    modo_descanso.Set_background_image_pic(9);
   }
   if(modo==8){
    modo_descanso.Set_background_image_pic(7);
   }
}

void update_system(){
  const char* URL = "https://app-update.ganza.app/ftp_update/Nextion";
  WiFiClientSecure client;
  client.setInsecure();
  ESPhttpUpdate.rebootOnUpdate(false);
  //Callback - Progresso
  ESPhttpUpdate.onProgress([](size_t progresso, size_t total) {
    
  });
  //Atualiza Software
  t_httpUpdate_return r = ESPhttpUpdate.update(client, URL);

  switch(r){
    case HTTP_UPDATE_FAILED: {
      String s = ESPhttpUpdate.getLastErrorString();
    } break;
    case HTTP_UPDATE_NO_UPDATES:
    //não ha atualizações
    break;
    case HTTP_UPDATE_OK: {
      // atualizando, reiniciando...
      ESP.restart();
    } break;
  }  
} //end update

// usar!=
byte hora_zap;
void zera_hora_zap(){
  hora_zap = 0;
}
void verifica_temperatura(){
  if(modo==3){
    if(sensor_bmeT>29 && sensor_bmeT<31 && h>hora_zap){
      message_v = "Salve Grower, tudo bem!? Verifiquei que a temperatura do seu grow está um pouco acima do ideal (22-28) para o estágio da sua planta(VEGA). Da uma olhada nos nossos vídeos em https://ganza.app/labs/ que temos dicas para você controlar melhor a temperatura e seguir tranquilo no seu cultivo!";
      zap_envio_video();
      hora_zap = h;
    }
    if(sensor_bmeT > 31 && h>hora_zap){
      message_v = "Salve Grower, tudo bem!? Verifiquei que a temperatura do seu grow está muito acima do ideal para o estágio da sua planta(VEGA). Da uma olhada nos nossos vídeos em https://ganza.app/labs/ que temos dicas para você controlar melhor a temperatura e seguir tranquilo no seu cultivo!";
      zap_envio_video();
      hora_zap=h;
    }
    if(sensor_bmeH > 75 && h>hora_zap){
      message_v = "Salve Grower, tudo bem!? Verifiquei que a umidade do seu grow está um pouco acima do ideal (70%) para o estágio do seu cultivo(VEGA). Da uma olhada nos nossos vídeos em https://ganza.app/labs/ que temos dicas para você controlar melhor a umidade do seu grow e seguir tranquilo no processo de crescimento das suas plantinhas!";
      zap_envio_video();
      hora_zap = h;
    }
    if(sensor_bmeH < 55 && h>hora_zap){
      message_v = "Salve Grower, tudo bem!? Verifiquei que a Umidade do seu grow está um pouco abaixo do ideal(60%) para o estágio do seu cultivo(VEGA). Da uma olhada nos nossos vídeos em https://ganza.app/labs/ que temos dicas para você controlar melhor a umidade do seu grow e seguir tranquilo no processo crescimento das suas plantinhas!";
      zap_envio_video();
      hora_zap = h;
    }
  }
  if(modo==2){
    if(sensor_bmeT > 27 && sensor_bmeT < 29 && h>hora_zap){
      message_v = "Salve Grower, tudo bem!? Verifiquei que a temperatura do seu grow está um pouco acima do ideal(20-26) para o estágio da sua planta(FLORA). Da uma olhada nos nossos vídeos em https://ganza.app/labs/ que temos dicas para você controlar melhor a temperatura e seguir tranquilo no processo floração das suas plantinhas!";
      zap_envio_video();
      hora_zap = h;
    }
    if(sensor_bmeT > 30 && h>hora_zap){
      message_v = "Salve Grower, tudo bem!? Verifiquei que a temperatura do seu grow está bem acima do ideal(20-26) para o estágio da sua planta(FLORA). Evite transtornos com pragas, fungos e com o desenvolvimento da sua planta! Sugiro que vc aumente a refrigeração e a exaustão do seu grow imediatamente. Da uma olhada nos nossos vídeos em https://ganza.app/labs/ que temos dicas para você controlar melhor a temperatura e seguir tranquilo no processo floração das suas plantinhas!";
      zap_envio_video();
      hora_zap = h;
    }
    if(sensor_bmeH > 65 && h>hora_zap){
      message_v = "Salve Grower, tudo bem!? Verifiquei que a umidade do seu grow está um pouco acima do ideal(60%) para o estágio do seu cultivo(FLORA). Da uma olhada nos nossos vídeos em https://ganza.app/labs/ que temos dicas para você controlar melhor a umidade do seu grow e seguir tranquilo no processo floração das suas plantinhas!";
      zap_envio_video();
      hora_zap = h;
    }
    if(sensor_bmeH<40 && h>hora_zap){
      message_v="Salve Grower, tudo bem!? Verifiquei que a Umidade do seu grow está um pouco abaixo do ideal(50%) para o estágio do seu cultivo(FLORA). Da uma olhada nos nossos vídeos em https://ganza.app/labs/ que temos dicas para você controlar melhor a umidade do seu grow e seguir tranquilo no processo de floração das suas plantinhas!";
      zap_envio_video();
      hora_zap = h;
    }
  }
}

void show_meumodo(){

//    char ht[25] = char_horaon + char_minon;
  //  show_ligar.setText(ht);
   char ht[20]; //hora ligar
   char hf[20]; // hora de desligar
   String u = String(char_horaon) + ":" + String(char_minon);
   String o = String(char_horaoff) + ":" + String(char_minoff);
   u.toCharArray(ht, 20);
   show_ligar.setText(ht);
   o.toCharArray(hf, 20);
   show_desligar.setText(hf);
}

void setup() {
  Serial.begin(9600);
  SPIFFS.begin();
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  dht.begin();
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  wm.setClass("invert");
  wm.autoConnect("GanzaGrow");
  wm.setConfigPortalTimeout(3);
  //inicia nextion 
  nexInit(); // inicializa display
  pinMode (buzzer,OUTPUT) ;//buzzer
  pinMode(rele,OUTPUT);//relé
  pinMode(led_vermelho1, OUTPUT);
  pinMode(led_vermelho2, OUTPUT);
  pinMode(led_verde1, OUTPUT);
  pinMode(led_verde2, OUTPUT);
  pinMode(led_amarelo, OUTPUT);
  progress_inicial.setValue(20);
  delay(1000);
  progress_inicial.setValue(30);
  delay(1000);
  progress_inicial.setValue(35);
  ler_memoria();
  ler_memoria2();
  ler_memoria_modo();
  wifi();
 // clima_weathermap();
  conect.setPic(5);
  delay(2000);
  Wire.pins(0,2);// 
  Wire.begin(0,2);// 0=sda, 2=scl
  rtc.begin();
  //bme
  bmp.begin(0x76);//remover se for usar loop de verificação
  /*if(!bmp.begin(0x76)){ //SE O SENSOR NÃO FOR INICIALIZADO NO ENDEREÇO I2C 0x76, FAZ
    Serial.println("Sensor BMP280 não foi identificado! Verifique as conexões."); //IMPRIME O TEXTO NO MONITOR SERIAL
    while(1); //SEMPRE ENTRE NO LOOP
 }*/

 
  
  progress_inicial.setValue(40);
  delay(1000);
  progress_inicial.setValue(45);
  delay(1000);
  progress_inicial.setValue(60);
  delay(1000);
  if(wifi_on==1){
      NTP();
      delay (50);
      NTP();
      NTP();
  }
  progress_inicial.setValue(65);
  delay(1000);
  buscando.setPic(5);
  delay(1000);
  progress_inicial.setValue(80);
  delay(500);
  progress_inicial.setValue(85);
  delay(500);
  progress_inicial.setValue(90);
  iniciando.setPic(5);
  delay(1000);
  progress_inicial.setValue(95);
  delay(1000);
  progress_inicial.setValue(100);

  
  //ajusta hora
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   
  //const size_t capacity = JSON_OBJECT_SIZE(1);
  //DynamicJsonDocument doc(capacity);
  // enviar msg para o zap
/*
   StaticJsonDocument<768> zap;
   {
   zap["phone"] = "5547997267419";
   zap["message"] = "Respeita a firma";
   }
  HTTPClient http;
  /*http.begin("http://104.236.113.58:3333/status?sessionName=zaplevante");
  //const byte teste = http.GET(); retornou 255
  String teste = http.getString();
  Serial.println(http.GET());
  http.end();
 */
/*
 // ENVIANDO MESNAGEM PARA O WHATSAPP---------------------------------------------------------------------------------------------------------------------------------
 // ------------------------------------------------------------------------------------------------------------------------------------------------------------------
 HTTPClient http;
  http.begin("https://api.z-api.io/instances/39C19B6A466F804CB15E52059E828D91/token/4F2A3B27E643E77EF30DAACD/send-text");
  
  http.addHeader("Content-Type", "application/json");
  delay(1000);
  int httpResponseCode = http.POST("{\"phone\" : \"5547997267419\",\"message\" : \"modelo_antigo\"}");
//  int httpResponseCode = http.POST(phone, mensagem);
  http.end();
   */
  

  home3.attachPop(home3Callback, &home3);
  modos.attachPop(modosCallback, &modos);
  vega.attachPop(vegaCallback, &vega);
  flora.attachPop(floraCallback, &flora);
  germ.attachPop(germCallback, &germ);
  trabalho.attachPop(trabalhoCallback, &trabalho);
  meumodo.attachPop(meumodoCallback, &meumodo);
  colheita.attachPop(colheitaCallback, &colheita);
  btn_on.attachPop(btn_onCallback, &btn_on);
  btn_off.attachPop(btn_offCallback, &btn_off);
  conf.attachPop(confCallback, &conf);
  homemeumodo.attachPop(homemeumodoCallback, &homemeumodo);
  confmeumodo.attachPop(confmeumodoCallback, &confmeumodo);
  salva_horaon.attachPop(salva_horaonCallback, &salva_horaon);
  salva_minon.attachPop(salva_minonCallback, &salva_minon);
  salva_horaoff.attachPop(salva_horaoffCallback, &salva_horaoff);
  salva_minoff.attachPop(salva_minoffCallback, &salva_minoff);
  
  //homeconf1.attachPop(homeconf1Callback, &homeconf1);
//  salvar_conf1.attachPop(salvar_conf1Callback, &salvar_conf1);
  //configurara1.attachPop(configurara1Callback, &configurara1);
  salvacultivo.attachPop(salvacultivoCallback, &salvacultivo);
  salvacidade.attachPop(salvacidadeCallback, &salvacidade);
  salvapais.attachPop(salvapaisCallback, &salvapais);
  configurara1b.attachPop(configurara1bCallback, &configurara1b);
  salvar_ssid.attachPop(salvar_ssidCallback, &salvar_ssid);
  salvar_senha.attachPop(salvar_senhaCallback, &salvar_senha);
//  salvar_memoria_conf2.attachPop(salvar_memoria_conf2Callback, &salvar_memoria_conf2);
//  proxima_conf.attachPop(proxima_confCallback, &proxima_conf);
//  home_conf2.attachPop(home_conf2Callback, &home_conf2);
 // volta_conf1.attachPop(volta_conf1Callback, &volta_conf1);
//  avanca_conf3.attachPop(avanca_conf3Callback, &avanca_conf3);
 // volta_conf2.attachPop(volta_conf2Callback, &volta_conf2);
  page2.attachPop(page2Callback, &page2);
  conf_nome.attachPop(conf_nomeCallback, &conf_nome);
  conf_numero.attachPop(conf_numeroCallback, &conf_numero);
  envia_zap.attachPop(envia_zapCallback, &envia_zap);

  editar_wifi.attachPop(editar_wifiCallback, &editar_wifi);
  editar_local.attachPop(editar_localCallback, &editar_local);
  avanca_navegacao_conf_1.attachPop(avanca_navegacao_conf_1Callback, &avanca_navegacao_conf_1);
  volta_navegacao_conf_1.attachPop(volta_navegacao_conf_1Callback, &volta_navegacao_conf_1);
  home_navegacao_conf_1.attachPop(home_navegacao_conf_1Callback, &home_navegacao_conf_1);
  editar_nome.attachPop(editar_nomeCallback, &editar_nome);
  editar_telefone.attachPop(editar_telefoneCallback, &editar_telefone);
  avanca_navegacao_conf_2.attachPop(avanca_navegacao_conf_2Callback, &avanca_navegacao_conf_2);
  volta_navegacao_conf_2.attachPop(volta_navegacao_conf_2Callback, &volta_navegacao_conf_2);
  home_navegacao_conf_2.attachPop(home_navegacao_conf_2Callback, &home_navegacao_conf_2);
  editar_cultivo.attachPop(editar_cultivoCallback, &editar_cultivo);
  atualiza_hora.attachPop(atualiza_horaCallback, &atualiza_hora);
  avanca_navegacao_conf_3.attachPop(avanca_navegacao_conf_3Callback, &avanca_navegacao_conf_3);
  volta_navegacao_conf_3.attachPop(volta_navegacao_conf_3Callback, &volta_navegacao_conf_3);
  home_navegacao_conf_3.attachPop(home_navegacao_conf_3Callback, &home_navegacao_conf_3);
  back_ssid.attachPop(back_ssidCallback, &back_ssid);
  back_cidade.attachPop(back_cidadeCallback, &back_cidade);
  back_apelido.attachPop(back_apelidoCallback, &back_apelido);
  back_telefone.attachPop(back_telefoneCallback, &back_telefone);
  back_cultivo.attachPop(back_cultivoCallback, &back_cultivo);
  wifi_mod.attachPop(wifi_modCallback, &wifi_mod);
  edita_ligar.attachPop(edita_ligarCallback, &edita_ligar);
  edita_desligar.attachPop(edita_desligarCallback, &edita_desligar);
  
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
  delay(600);
  pic_wifi=0;
  m_descanso = m;

  rtcnextion();
  sensorbme();

  page1.show();
  page1b = true;
  delay(1300);
  pagina_home();
}

void wifim(){
  // Wifimanager
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  wm.setClass("invert");
  wm.setAPClientCheck(true); // avoid timeout if client connected to softap
  wm.setConfigPortalTimeout(100000);//tempo para conectar ao ap antes de continuar o código
 
  wm.startConfigPortal("Ganzá-automated v.1.0");
}

void sensorbme_descanso(){
    sensor_bmeT = bmp.readTemperature();
    sensor_bmeH = bmp.readHumidity();
    sensor_bmeP = bmp.readPressure();
    sensor_bmeA = bmp.readAltitude(1013.25);
    char tempc1[7];
    char humc1[7];
    snprintf(tempc1, 8, "%.0f", sensor_bmeT);
    temp_descanso.setText(tempc1);
    snprintf(humc1, 8, "%.0f", sensor_bmeH);
    humidade_descanso.setText(humc1);
}

void sensorbme(){

    sensor_bmeT = bmp.readTemperature();
    sensor_bmeH = bmp.readHumidity();
    sensor_bmeP = bmp.readPressure();
    sensor_bmeA = bmp.readAltitude(1013.25);
    char tempc[7];
    char humc[7];
    snprintf(tempc, 8, "%.0f", sensor_bmeT);
    temps.setText(tempc);
    snprintf(humc, 8, "%.0f", sensor_bmeH);
    hums.setText(humc);
    
    
}


void modo_spiffs(){
    StaticJsonDocument<768> infos4;
    File file = SPIFFS.open("/infos4.json", "w+");
    if (file) {
    infos4["modo"] = modo;
    serializeJsonPretty(infos4, file);
    file.close();
    //ler_memoria();
    //pagina_home();
    delay(1000);
  }
}

void loop() {

  DateTime now = rtc.now();
  h = now.hour();
  m = now.minute();
  s = now.second();
  nexLoop(nex_listen_list); // verifica se algum botao da lista foi apertado e chama o callback
  mudamodo();
  if(h==4 && m<=2 && s<=10){
    if(wifi_on==1){
      NTP();
      delay (50);
      NTP();
      NTP();
  }
  }
  if(h==16 && m==20 && s==0){
    zap();
    if(wifi_on==1){
      NTP();
      delay (50);
      NTP();
      NTP();
  }
  }
  if(page1b==true){
    rtcnextion();
    if(s==11 || s==30 || s==40 || s==50){
      sensorbme();
    }
    if(s==15){
      verifica_conexao();
      if(wifi_on==1){
       
      }
    }
    if(s==48){
      clima_weathermap();
    }
  }
  if(m==4 || m==20 || m==40 || m==53){
    verifica_temperatura();
  }
  if(s==37){
    sensordht();
    enviaValores();
  }
  if(h==0 && m==1 && s==15){
    diascultivo=diascultivo + 1;
     StaticJsonDocument<768> infos2;
    File file = SPIFFS.open("/infos2.json", "w+");
    if (file) {
    infos2["dias_cultivo"] = diascultivo;
    infos2["hora_on"]  = char_horaon;  
    infos2["cidade"] = char_cidade2;  //pega pais do teclado
    infos2["pais"] = char_pais2;  
    infos2["ssid"] = ssid; 
    // Atribui valores ao JSON e grava
    infos2["hora_off"] = char_horaoff;
    infos2["min_on"] = char_minon;
    infos2["min_off"] = char_minoff;
    infos2["timezone"] = salvo_timezone;
    infos2["password"] =pass;
    
    serializeJsonPretty(infos2, file);
    file.close();
    ler_memoria();
    pagina_home();
    delay(1000);
  }
  }
  if(page2b==true){
    
    DateTime now = rtc.now();
    h = now.hour();
    m = now.minute();
    s = now.second();
  if(m<10){
    horanex = String(h) + ":0" + String(m);
  }
  else{
   horanex = String(h) + ":" + String(m);
  }
  if(s==0){
     horanex.toCharArray(hora, 50);
     horartc_descanso.setText(hora);
  }
  
  if(s==15 || s==31 || s==50){
    sensorbme_descanso();
  }
  }
  if(navegacao_conf_3_bool==true){
    if(m<10){
    horanex = String(h) + ":0" + String(m);
  }
  else{
   horanex = String(h) + ":" + String(m);
  }
    if(s==0){
      horanex.toCharArray(hora, 50);
      show_hora.setText(hora);
    }
    
  }
  if(m==59){
    m_descanso = 0;
  }
  if(m_descanso == m && s==7){
    page2.show();
    page2b = true;
    page1b=false;
    pagina_descanso();
  }
  
}

void home3Callback(void *ptr)
{
  page1.show();
  page1b = true;
  page2b=false;
  pagemodosb=false;
  if(m<10){
    horanex = String(h) + ":0" + String(m);
  }
  else{
   horanex = String(h) + ":" + String(m);
  }
   horanex.toCharArray(hora, 50);
   horartc.setText(hora);
   pagina_home();
   m_descanso = m + 2;
}
void modosCallback(void *ptr)
{
  page1b=false;
  pagemodos.show();
  pagemodosb = true;
  page2b=false;
  pagemeumodob=false;
  m_descanso = m + 2;
}

void vegaCallback(void *ptr)
{
  modo = 3;
  pic=0;
  pageconfig1b = false;
  page1b=true;
  page2b=false;
  pagemeumodob=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
  vegad=diascultivo;
  modo_spiffs();
}

void floraCallback(void *ptr)
{
  modo = 2;
  pic=0;
  pageconfig1b = false;
  page1b=true;
  page2b=false;
  pagemeumodob=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
  florad = diascultivo;
  modo_spiffs();
}

void germCallback(void *ptr)
{
  modo = 7; 
  pic=0;
  pageconfig1b = false;
  page1b=true;
  page2b=false;
  pagemeumodob=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
  modo_spiffs();
}

void trabalhoCallback(void *ptr)
{
  modo = 5;
  pic=0;
  pageconfig1b = false;
  page1b=true;
  page2b=false;
  pagemeumodob=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
  modo_spiffs();
}

void meumodoCallback(void *ptr)
{
  //modo = 0;
  pic=0;
  pagemeumodo.show();
  show_meumodo();
  
  page1b=false;
  pagemodosb = false;
  page2b=false;
  pagemeumodob=true;
  m_descanso = m + 2;
  modo_spiffs();
}

void colheitaCallback(void *ptr)
{
  modo = 8;
  pic=0;
  pageconfig1b = false;
  page1b=true;
  page2b=false;
  pagemeumodob=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
  colheitad = diascultivo;
  modo_spiffs();
 
}
void btn_onCallback(void *ptr)
{
  modo = 4;
  pic=0;
  pageconfig1b = false;
  page1b=true;
  page2b=false;
  pagemeumodob=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
  modo_spiffs();

}
void btn_offCallback(void *ptr)
{
  modo = 1;
  pic=0;
  pageconfig1b = false;
  page1b=true;
  page2b=false;
  pagemeumodob=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
  modo_spiffs();
}

void confCallback(void *ptr)
{
  //pageconfig1.show();
  m_descanso = m + 2;
}

void confmeumodoCallback(void *ptr)//botao para salvar spiffs
{
  navegacao_conf_1.show();
  m_descanso = m + 2;

}

void homemeumodoCallback(void *ptr)
{
  ler_memoria();
  converte_memoria();
  modo = 0;
  pic=0;
  page1b=true;
  pagemodosb = false;
  page2b=false;
  pagemeumodob=false;
  pageconfig1b=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
}

void salva_horaonCallback(void *ptr)
{
  horaligar.getText(horaon, 2);
  horaligar.getText(horaon, 2);

  
        StaticJsonDocument<768> infos2;
    File file = SPIFFS.open("/infos2.json", "w+");
    if (file) {
    infos2["hora_on"]  = horaon;  
    infos2["dias_cultivo"] = diascultivo;
    infos2["cidade"] = char_cidade2;  //pega pais do teclado
    infos2["pais"] = char_pais2;  
    infos2["ssid"] = ssid; 
    // Atribui valores ao JSON e grava
    infos2["hora_off"] = char_horaoff;
    infos2["min_on"] = char_minon;
    infos2["min_off"] = char_minoff;
     
    
    infos2["timezone"] = salvo_timezone;
    //infos2["dias_cultivo"] = diascultivo;
    infos2["password"] =pass;
    
    serializeJsonPretty(infos2, file);
    file.close();
    ler_memoria();
    
}
 m_descanso = m + 2; 
 pagina_mon.show();
}

void salva_minonCallback(void *ptr)
{
  min_ligar.getText(min_on, 2);
  min_ligar.getText(min_on, 2);

  
        StaticJsonDocument<768> infos2;
    File file = SPIFFS.open("/infos2.json", "w+");
    if (file) {
    infos2["min_on"] = min_on;
    infos2["dias_cultivo"] = diascultivo;
    infos2["cidade"] = char_cidade2;  //pega pais do teclado
    infos2["pais"] = char_pais2;  
    infos2["ssid"] = ssid; 
    // Atribui valores ao JSON e grava
    infos2["hora_on"]  = char_horaon;
    infos2["hora_off"] = char_horaoff;
    
    infos2["min_off"] = char_minoff;
     
    
    infos2["timezone"] = salvo_timezone;
    //infos2["dias_cultivo"] = diascultivo;
    infos2["password"] =pass;
    
    serializeJsonPretty(infos2, file);
    file.close();
    ler_memoria();
}
m_descanso = m + 2;
pagemeumodo.show();
show_meumodo();
pagemeumodob = true;
}

void salva_horaoffCallback(void *ptr)
{
  hora_desligar.getText(horaoff, 2);
  hora_desligar.getText(horaoff, 2);

  
        StaticJsonDocument<768> infos2;
    File file = SPIFFS.open("/infos2.json", "w+");
    if (file) {
    infos2["hora_off"] =horaoff; 
    infos2["min_off"] = char_minoff; 
    infos2["dias_cultivo"] = diascultivo;
    infos2["cidade"] = char_cidade2;  //pega pais do teclado
    infos2["pais"] = char_pais2;  
    infos2["ssid"] = ssid; 
    // Atribui valores ao JSON e grava
    infos2["hora_on"]  = char_horaon;
    infos2["min_on"] = char_minon;
    
     
    
    infos2["timezone"] = salvo_timezone;
    //infos2["dias_cultivo"] = diascultivo;
    infos2["password"] =pass;
    
    serializeJsonPretty(infos2, file);
    file.close();
    ler_memoria();
}
  m_descanso = m + 2;
  pagina_moff.show();
  
}

void salva_minoffCallback(void *ptr)
{
  min_desligar.getText(min_off, 2);
  min_desligar.getText(min_off, 2);

          StaticJsonDocument<768> infos2;
    File file = SPIFFS.open("/infos2.json", "w+");
    if (file) {
      
    infos2["min_off"] = min_off;
    infos2["dias_cultivo"] = diascultivo;
    infos2["cidade"] = char_cidade2;  
    infos2["pais"] = char_pais2;  
    infos2["ssid"] = ssid; 
    // Atribui valores ao JSON e grava
    infos2["hora_on"]  = char_horaon;
    infos2["hora_off"] = char_horaoff;
    infos2["min_on"] = char_minon;
   // infos2["min_off"] = min_off;
     
    
    infos2["timezone"] = salvo_timezone;
    //infos2["dias_cultivo"] = diascultivo;
    infos2["password"] =pass;
    
    serializeJsonPretty(infos2, file);
    file.close();
    ler_memoria();
}
m_descanso = m + 2;
pagemeumodo.show();
show_meumodo();
pagemeumodob = true;
}

void homeconf1Callback(void *ptr) // voltar pagina de conf1
{
  page1b=true;
  pagemodosb = false;
  page2b=false;
  pagemeumodob=false;
  pageconfig1b=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
}

void salvar_conf1Callback(void *ptr) // botao salvar infos da pagina 8
{
    ler_memoria();
    m_descanso = m + 2;
}

void salvacultivoCallback(void *ptr) // botao salvar infos cultivo
{
  dias_cultivo.getText(cultivo, 5);
  dias_cultivo.getText(cultivo, 5);
  int diascultivo10 = atoi(&cultivo[0]);

        StaticJsonDocument<768> infos2;
    File file = SPIFFS.open("/infos2.json", "w+");
    if (file) {
    infos2["dias_cultivo"] = diascultivo10;
    infos2["cidade"] = char_cidade2;  //pega pais do teclado
    infos2["pais"] = char_pais2;  
    infos2["ssid"] = ssid; 
    // Atribui valores ao JSON e grava
    infos2["hora_on"]  = char_horaon;
    infos2["hora_off"] = char_horaoff;
    infos2["min_on"] = char_minon;
    infos2["min_off"] = char_minoff;
     
    
    infos2["timezone"] = salvo_timezone;
    //infos2["dias_cultivo"] = diascultivo;
    infos2["password"] =pass;
    
    serializeJsonPretty(infos2, file);
    file.close();
    page_salvo.show();
    delay(1500);
   // pageconfig1.show();
    ler_memoria();
}
navegacao_conf_3.show();
navegacao_conf_3_bool = true;
navegacao_conf_1_bool = false;
navegacao_conf_2_bool = false;
m_descanso = m + 2;
}

void salvacidadeCallback(void *ptr) // botao salvar nome cidade
{
  cidademap.getText(char_cidade, 20);
  cidademap.getText(char_cidade, 20);

      StaticJsonDocument<768> infos2;
    File file = SPIFFS.open("/infos2.json", "w+");
    if (file) {
    infos2["cidade"] = char_cidade;  //pega pais do teclado
    infos2["pais"] = char_pais2;  
    infos2["ssid"] = ssid; 
    // Atribui valores ao JSON e grava
    infos2["hora_on"]  = char_horaon;
    infos2["hora_off"] = char_horaoff;
    infos2["min_on"] = char_minon;
    infos2["min_off"] = char_minoff;
     
    
    infos2["timezone"] = salvo_timezone;
    infos2["dias_cultivo"] = diascultivo;
    infos2["password"] =pass;
    
    serializeJsonPretty(infos2, file);
    file.close();
    //page_salvo.show();
    //delay(1500);
   // pageconfig1.show();
    ler_memoria();
}
pagina_pais.show();
m_descanso = m + 2;
}

void salvapaisCallback(void *ptr) // botao salvar pais
{
  paismap.getText(char_pais, 5);
  paismap.getText(char_pais, 5);

    StaticJsonDocument<768> infos2;
    File file = SPIFFS.open("/infos2.json", "w+");
    if (file) {
    infos2["pais"] = char_pais;  // pega pais do teclado
    infos2["ssid"] = ssid; 
    // Atribui valores ao JSON e grava
    infos2["hora_on"]  = char_horaon;
    infos2["hora_off"] = char_horaoff;
    infos2["min_on"] = char_minon;
    infos2["min_off"] = char_minoff;
     
    
    infos2["timezone"] = salvo_timezone;
    infos2["dias_cultivo"] = diascultivo;
    infos2["cidade"] = char_cidade2;
    infos2["password"] =pass;
    
    serializeJsonPretty(infos2, file);
    file.close();
    page_salvo.show();
    delay(1500);
  //  pageconfig1.show();
    ler_memoria();
}
navegacao_conf_1.show();
m_descanso = m + 2;
navegacao_conf_1_bool = true;
}
void configurara1bCallback(void *ptr) // configurações da pagina inicial
{
  navegacao_conf_1.show();
  //pageconfig1.show();
  page1b=false;
  navegacao_conf_1_bool = true;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = false;
  m_descanso = m + 2;
  tela1();
}

void salvar_ssidCallback(void *ptr) // page 9 ssid
{
  conf_ssid.getText(char_ssid, 15);
  conf_ssid.getText(char_ssid, 15);
  StaticJsonDocument<768> infos2;
    File file = SPIFFS.open("/infos2.json", "w+");
    if (file) {
    infos2["ssid"] = char_ssid; // pega ssid teclado
    // Atribui valores ao JSON e grava
    infos2["hora_on"]  = char_horaon;
    infos2["hora_off"] = char_horaoff;
    infos2["min_on"] = char_minon;
    infos2["min_off"] = char_minoff;
     
    
    infos2["timezone"] = salvo_timezone;
    infos2["dias_cultivo"] = diascultivo;
    infos2["cidade"] = char_cidade2;
    infos2["pais"] = char_pais2;
    infos2["password"] =pass;
    
    serializeJsonPretty(infos2, file);
    file.close();
    //page_salvo.show();
    //delay(1500);
   // pageconfig2.show();
   
    ler_memoria();
   // WiFi.close();
    //wifi();
}
pagina_senha_wifi.show();
m_descanso = m + 2;
}

void salvar_senhaCallback(void *ptr)
{
  conf_senha.getText(char_pass, 15);
  conf_senha.getText(char_pass, 15);

  StaticJsonDocument<768> infos2;
    File file = SPIFFS.open("/infos2.json", "w+");
    if (file) {
    infos2["password"] = char_pass; // pega do teclado
    // Atribui valores ao JSON e grava
    infos2["hora_on"]  = char_horaon;
    infos2["hora_off"] = char_horaoff;
    infos2["min_on"] = char_minon;
    infos2["min_off"] = char_minoff;
    infos2["ssid"] = ssid; 
    
    infos2["timezone"] = salvo_timezone;
    infos2["dias_cultivo"] = diascultivo;
    infos2["cidade"] = char_cidade2;
    infos2["pais"] = char_pais2;
    
    serializeJsonPretty(infos2, file);
    file.close();
    page_salvo.show();
    delay(1500);
    
//    pageconfig2.show();
    ler_memoria();
//    WiFi.close();
    wifi();
    
    
}
navegacao_conf_1.show();
m_descanso = m + 2;
}


void salvar_memoria_conf2Callback(void *ptr)
{
    ler_memoria();
    wifi(); 
    m_descanso = m + 2;
}

void proxima_confCallback(void *ptr)
{
//  pageconfig2.show();
  m_descanso = m + 2;
}

void avanca_conf3Callback(void *ptr)
{
//  pageconfig3.show();
  m_descanso = m + 2;
}

void volta_conf1Callback(void *ptr)
{
 // pageconfig1.show();
  m_descanso = m + 2;
}

void home_conf2Callback(void *ptr)
{
  page1b=true;
  pagemodosb = false;
  page2b=false;
  pagemeumodob=false;
  pageconfig1b=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
}
/*
void volta_conf2Callback(void *ptr)
{
  pageconfig2.show();
  m_descanso = m + 2;
}
*/
void page2Callback(void *ptr)
{
  page2b = false;
  page1b=true;
  pagemodosb = false;
  page2b=false;
  pagemeumodob=false;
  pageconfig1b=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
}
void conf_nomeCallback(void *ptr)
{
  get_nome.getText(nome_usuario, 20);
  get_nome.getText(nome_usuario, 20);

  StaticJsonDocument<768> infos3;
    File file2 = SPIFFS.open("/dadosusuario.json", "w+");
    if (file2){
      infos3["nome"] = nome_usuario;
      infos3["telefone"] =char_telefone;
      infos3["canal"] = char_canal;

    serializeJsonPretty(infos3, file2);
    file2.close();
    page_salvo.show();
   delay(1500);
   // pageconfig3.show();
    ler_memoria2();
    }
    navegacao_conf_2.show();
    navegacao_conf_2_bool = true;
  m_descanso = m + 2;
}
void conf_numeroCallback(void *ptr)
{
  get_numero.getText(telefone, 15);
  get_numero.getText(telefone, 15);
  StaticJsonDocument<768> infos3;
    File file2 = SPIFFS.open("/dadosusuario.json", "w+");
    if (file2){
      
      infos3["telefone"] =telefone;
      infos3["canal"] = char_canal;
      infos3["nome"] = char_nome_usuario;

    serializeJsonPretty(infos3, file2);
    file2.close();
    page_salvo.show();
    delay(1500);
   // pageconfig3.show();
    ler_memoria2();
    }
    navegacao_conf_2.show();
    navegacao_conf_1_bool = false;
    navegacao_conf_2_bool = true;
    navegacao_conf_3_bool = false;
  m_descanso = m + 2;
}



void envia_zapCallback(void *ptr)
{
  zap_link();
  m_descanso = m + 2;
}

void editar_wifiCallback(void *ptr)
{
  pagina_wifi.show();
  m_descanso = m + 2;
}

void editar_localCallback(void *ptr)
{
  pagina_cidade.show();
  m_descanso = m + 2;
}

void avanca_navegacao_conf_1Callback(void *ptr)
{
  navegacao_conf_2.show();

  m_descanso = m + 2;
   navegacao_conf_1_bool = false;
  navegacao_conf_2_bool = true;
  navegacao_conf_3_bool = false;
  tela2();
}
void volta_navegacao_conf_1Callback(void *ptr)
{
  navegacao_conf_3.show();
  m_descanso = m + 2;
   navegacao_conf_1_bool = false;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = true;
  tela3();
}
void home_navegacao_conf_1Callback(void *prt)
{
  page1b=true;
  pagemodosb = false;
  page2b=false;
  pagemeumodob=false;
  pageconfig1b=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
   navegacao_conf_1_bool = false;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = false;
}
void editar_nomeCallback(void *ptr)
{
  page_apelido.show();
  m_descanso = m + 2;
}
void editar_telefoneCallback(void *ptr)
{
  pagina_telefone.show();
  m_descanso = m + 2;
}
void avanca_navegacao_conf_2Callback(void *ptr)
{
  navegacao_conf_3.show();
  m_descanso = m + 2;
   navegacao_conf_1_bool = false;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = true;
  tela3();
}

void volta_navegacao_conf_2Callback(void *ptr)
{
  navegacao_conf_1.show();
  m_descanso = m + 2;
   navegacao_conf_1_bool = true;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = false;
  tela1();
}
void home_navegacao_conf_2Callback(void *ptr)
{
  page1b=true;
  pagemodosb = false;
  page2b=false;
  pagemeumodob=false;
  pageconfig1b=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
   navegacao_conf_1_bool = false;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = false;
}

void editar_cultivoCallback(void *ptr)
{
  page_cultivo.show();
  m_descanso = m + 2;
}
void atualiza_horaCallback(void *ptr)
{
  verifica_conexao();
  if(wifi_on==1){
    NTP();
  }
  m_descanso = m + 2;
  tela3();
}

void avanca_navegacao_conf_3Callback(void *ptr)
{
  navegacao_conf_1.show();
  m_descanso = m + 2;
   navegacao_conf_1_bool = true;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = false;
  tela1();
}

void volta_navegacao_conf_3Callback(void *ptr)
{
  navegacao_conf_2.show();
  m_descanso = m + 2;
   navegacao_conf_1_bool = false;
  navegacao_conf_2_bool = true;
  navegacao_conf_3_bool = false;
  tela2();
}

void home_navegacao_conf_3Callback(void *ptr)
{
  page1b=true;
  pagemodosb = false;
  page2b=false;
  pagemeumodob=false;
  pageconfig1b=false;
  page1.show();
  pagina_home();
  m_descanso = m + 2;
   navegacao_conf_1_bool = false;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = false;
}

void back_ssidCallback(void *ptr) // botao de voltar da pagina de adicionar rede wifi
{
  navegacao_conf_1.show();
  navegacao_conf_1_bool = true;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = false;
  m_descanso = m + 2;
}
void back_cidadeCallback(void *ptr)
{
  navegacao_conf_1.show();
  navegacao_conf_1_bool = true;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = false;
  m_descanso = m + 2;
}
void back_apelidoCallback(void *ptr)
{
  navegacao_conf_2.show();
  navegacao_conf_1_bool = false;
  navegacao_conf_2_bool = true;
  navegacao_conf_3_bool = false;
  m_descanso = m + 2;
}
void back_telefoneCallback(void *ptr)
{
  navegacao_conf_2.show();
  navegacao_conf_1_bool = false;
  navegacao_conf_2_bool = true;
  navegacao_conf_3_bool = false;
  m_descanso = m + 2;
}
void back_cultivoCallback(void *ptr)
{
  navegacao_conf_3.show();
  navegacao_conf_1_bool = false;
  navegacao_conf_2_bool = false;
  navegacao_conf_3_bool = true;
  m_descanso = m + 2;
}
void wifi_modCallback(void *ptr)
{
 // page1b=false;
  wifim();
  m_descanso = m + 2;
}
void edita_ligarCallback(void *ptr)
{
  pagemeumodob=false;
  pagina_hon.show();
  m_descanso = m + 2;
}
void edita_desligarCallback(void *ptr)
{
   pagemeumodob=false;
   pagina_hoff.show();
   m_descanso = m + 2;
}
