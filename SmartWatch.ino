/*
TO DO LIST:
- relogio piscou como se estivesse sem sincronia por um tempo, porem a sincronia estava sendo executada normalmente. corrigido. testando (deixar 4 dias rodando)

- fazer o core da automacao (acionar os reles) + parte de HTML e variaveis salvando na eeprom. OK 

- ao falhar a conexao wifi, ficar no modo ap somente, por 5 minutos, depois tentar de novo (repete o ciclo). OK

- memorizar as 3 a 5 ultimas redes wifi conectadas? para tentar alguma delas a conexao ao ligar...

- Escolher uma imagem/logo e embutir no html via base64 encode
- colocar um background listrado para facilitar a visualizacao na tabela do grafico usando (exemplo) background="/wp-content/uploads/wov.png" (ok, colocar o arquivo em lucatoweb)
- ^colocar a imagem acima e possivelmente o logo que iria colocar em base64 no servidor hostinger, para economizar RAM do esp

- Verificar modulos gps para possivel obtencao de horario em outros projetos.
seconds offset utc:
-10800  -3 //padrao BR
-14400  -4
-7200   -2 //padrao BR como se tivesse no horario de verao
-3600   -1
*/
#include "LEDMatrixDriver.h"
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <TimeLib.h>
#include "fonts.h"
#include <OneWire.h>
#include <DallasTemperature.h>
//#include <WiFiClient.h> 

//#include "ESP8266WiFiGeneric.h"
//#include "ESP8266WiFiAP.h"

//extern "C"{   
  #include "user_interface.h"
  //}

#define rele1 D1
#define rele2 D4
#define botao D2 

//------------------------------------------
//DS18B20
#define ONE_WIRE_BUS D3 //Pin to which is attached a temperature sensor (Default: D3)
#define ONE_WIRE_MAX_DEV 1 //The maximum number of devices

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
int numberOfDevices; //Number of temperature devices found
DeviceAddress devAddr[ONE_WIRE_MAX_DEV];  //An array device temperature sensors
//float tempDev[ONE_WIRE_MAX_DEV]; //Saving the last measurement of temperature
//float tempDevLast[ONE_WIRE_MAX_DEV]; //Previous temperature measurement

unsigned long tempo=0;
unsigned long tempoSnapshot=0;

float temperature=0;
float temperatureMaxEver=-99;
float temperatureMinEver=99;
int tempCount=0;
int diaMaxTempEver=0;
int mesMaxTempEver=0;
int anoMaxTempEver=0;
int horaMaxTempEver=0;
int minutoMaxTempEver=0;
int diaMinTempEver=0;
int mesMinTempEver=0;
int anoMinTempEver=0;
int horaMinTempEver=0;
int minutoMinTempEver=0;

int diaTemp24h[48];
int mesTemp24h[48];
int anoTemp24h[48];
int horaTemp24h[48];
int minutoTemp24h[48];
float temp24h[48];

int diaMaxTemp24h=0;
int mesMaxTemp24h=0;
int anoMaxTemp24h=0;
int horaMaxTemp24h=0;
int minutoMaxTemp24h=0;
float tempMax24h=-99;
int diaMinTemp24h=0;
int mesMinTemp24h=0;
int anoMinTemp24h=0;
int horaMinTemp24h=0;
int minutoMinTemp24h=0;
float tempMin24h=99;

boolean gravou1aVez24h=false;

int diaInicioHverao=0;
int mesInicioHverao=0;
int diaTerminoHverao=0;
int mesTerminoHverao=0;
int horarioVeraoAtivo=0;

int faseExibicaoApIp=0;  //é incrementado a cada 3 segundos. 0= primeira metade do ip; 1=segunda metade
int faseExibicaoWlanIp=0; 
int freezeDisplay=0; //contador para nao atualizar as informacoes normais do display por um tempo (1 step a cada meio segundo)

String pagHtml ="";

boolean estadoRele1 = false;
boolean estadoRele2 = false;
int estadoRelesEeprom = 0; // rele1 ligado: soma 1; rele2 ligado: soma 2.

//variaves de automacao
int autoTemp1 = 0;
int temp1 = 0;
int autoClock1 = 0;
int hrInicio1 = 0;
int minInicio1 = 0;
int hrTermino1 = 0;
int minTermino1 = 0;
int autoTemp2 = 0;
int temp2 = 0;
int autoClock2 = 0;
int hrInicio2 = 0;
int minInicio2 = 0;
int hrTermino2 = 0;
int minTermino2 = 0;

//------------------------------------------
//Convert device id to String
String GetAddressToString(DeviceAddress deviceAddress){
  String str = "";
  for (uint8_t i = 0; i < 8; i++){
    if( deviceAddress[i] < 16 ) str += String(0, HEX);
    str += String(deviceAddress[i], HEX);
  }
  return str;
}

//Setting the temperature sensor
void SetupDS18B20(){
  DS18B20.begin();

  Serial.print("Parasite power is: "); 
  if( DS18B20.isParasitePowerMode() ){ 
    Serial.println("ON");
  }else{
    Serial.println("OFF");
  }
  
  numberOfDevices = DS18B20.getDeviceCount();
  Serial.print( "Device count: " );
  Serial.println( numberOfDevices );

  DS18B20.requestTemperatures();

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if( DS18B20.getAddress(devAddr[i], i) ){
      //devAddr[i] = tempDeviceAddress;
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: " + GetAddressToString(devAddr[i]));
      Serial.println();
    }else{
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }

    //Get resolution of DS18b20
    Serial.print("Resolution: ");
    Serial.print(DS18B20.getResolution( devAddr[i] ));
    Serial.println();

    //Read temperature from DS18b20
    float tempC = DS18B20.getTempC( devAddr[i] );
    Serial.print("Temp inicial (C): ");
    Serial.println(tempC);
  }
}

os_timer_t mTimer;
bool       _timeout = false;
 
//Nunca execute nada na interrupcao, apenas setar flags!
void tCallback(void *tCall){   _timeout = true; }
 
void usrInit(void){
    os_timer_setfn(&mTimer, tCallback, NULL);
    os_timer_arm(&mTimer, 500, true); //uma chamada a cada 500ms
}

/*
//dados fixos para teste, da rede a se conectar
char ssid[] = "homewifi_E9E";  //  your network SSID (name)
char pass[] = "gremilim0";       // your network password
*/

//dados do ponto de acesso:
const char *ssid2 = "SmartWatchDeParede";
const char *password = "20182019";

//variaveis que recebem o ssid e pass salvos na eeprom:
String esid="";
String epass = "";
//String ipObtidoInternet="--.--.--.--";

ESP8266WebServer server(80);

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServerIP; // time.nist.gov NTP server address
//const char* ntpServerName = "time.nist.gov";
const char* ntpServerName = "a.st1.ntp.br";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;


// Define the ChipSelect pin for the led matrix (Dont use the SS or MISO pin of your Arduino!)
// Other pins are arduino specific SPI pins (MOSI=DIN of the LEDMatrix and CLK) see https://www.arduino.cc/en/Reference/SPI
const uint8_t LEDMATRIX_CS_PIN = D0;

// Define LED Matrix dimensions (0-n) - eg: 32x8 = 31x7
const int LEDMATRIX_WIDTH = 32;  //32
const int LEDMATRIX_HEIGHT = 8;
const int LEDMATRIX_SEGMENTS = 4; //quantidade de modulos (CIs)

// The LEDMatrixDriver class instance
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

int x=0, y=0;   // start top left

int dig1=0;
int dig2=0;
int dig3=0;
int dig4=0;

int hora=0;
int minuto=0;
int segundos=0;

int dia=0;
int mes=0;
int ano=0;

int diaSemana=0;

int count=0; //conta os intervalos de 1 segundo
int ciclosSemObterTempo=0;

int wifiConCount=27; //contador para exibicao dos 3 pontos em "con. wifi..." quando estiver conectando
int wifiTentativaSemSucessoCount=0;
//boolean wifiTentandoConectar=true;
boolean wifiConectou1aVez=false;
boolean relogioSincronizado = false;

boolean chamadaPar = true; //alterna entre true e false para as chamadas da funcao callback atrelada ao timer (a cada 500ms) para alternar a piscada dos 2 pontos

boolean obteveTempo1avez = false;

int dia1aSincronia=0; int mes1aSincronia=0; int ano1aSincronia=0; int hora1aSincronia=0; int minuto1aSincronia=0; 
int diaUltimaSincronia=0; int mesUltimaSincronia=0; int anoUltimaSincronia=0; int horaUltimaSincronia=0; int minutoUltimaSincronia=0; 

#include "pagina_html.h"

// Marquee speed
//const int ANIM_DELAY = 20;

// Marquee text 
//char text[] = "** LED MATRIX DEMO! ** (1234567890) ++ \"ABCDEFGHIJKLMNOPQRSTUVWXYZ\" ++ <$%/=?'.@,> --";

//int len = strlen(text);
int len = 4;

//measuring the temperature
void TempLoop(){
  
    for(int i=0; i<numberOfDevices; i++){
      float tempC = DS18B20.getTempC( devAddr[i] ); //Measuring temperature in Celsius
      //tempDev[i] = tempC; //Save the measured value to the array
      temperature=tempC;
      if(temperature>temperatureMaxEver){
  temperatureMaxEver=temperature; diaMaxTempEver=dia; mesMaxTempEver=mes; anoMaxTempEver=ano; horaMaxTempEver=hora; minutoMaxTempEver=minuto;
}
      if(temperature<temperatureMinEver){
temperatureMinEver=temperature; diaMinTempEver=dia; mesMinTempEver=mes; anoMinTempEver=ano; horaMinTempEver=hora; minutoMinTempEver=minuto;
}
//      Serial.print("Temp C: ");
//    Serial.println(temperature);
    }
    DS18B20.setWaitForConversion(false); //No waiting for measurement
    DS18B20.requestTemperatures(); //Initiate the temperature measurement

//efetua a automacao dos reles: -------------------------------
  boolean atingiuTemp1=false; boolean atingiuTemp2=false;
  boolean atingiuClock1=false; boolean atingiuClock2=false;
if(temperature>=temp1){ atingiuTemp1=true; }
if(temperature>=temp2){ atingiuTemp2=true; }
if(  (hora>hrInicio1 || (hora==hrInicio1 && minuto>=minInicio1)) && (hora<hrTermino1 || (hora==hrTermino1 && minuto <= minTermino1)) ){atingiuClock1=true;}
if(  (hora>hrInicio2 || (hora==hrInicio2 && minuto>=minInicio2)) && (hora<hrTermino2 || (hora==hrTermino2 && minuto <= minTermino2)) ){atingiuClock2=true;}
//if(hora>=hrInicio2 && minuto>=minInicio2 && hora<= hrTermino2 && minuto <= minTermino2 ){atingiuClock2=true;}

boolean acionarRele1Temp  =  (autoTemp1==1 && atingiuTemp1) || (autoTemp1==2 && (!atingiuTemp1) );        
boolean acionarRele1Clock =  (autoClock1==1 && atingiuClock1) || (autoClock1==2 && (!atingiuClock1) );        

boolean desligarRele1Temp  =  (autoTemp1==1 && (!atingiuTemp1)) || (autoTemp1==2 && atingiuTemp1 );  
boolean desligarRele1Clock =  (autoClock1==1 && (!atingiuClock1)) || (autoClock1==2 && atingiuClock1 );

boolean acionarRele1 =  (acionarRele1Temp || acionarRele1Clock) && (!desligarRele1Temp) && (!desligarRele1Clock);    
boolean desligarRele1=  (desligarRele1Temp || desligarRele1Clock);     //      && (!acionarRele1Temp) && (!acionarRele1Clock);

if(acionarRele1 && (!estadoRele1)){
digitalWrite(rele1, HIGH);
estadoRele1 = true;
atualizaEstadoReleEeprom();
  }

  if(desligarRele1 && estadoRele1){
digitalWrite(rele1, LOW);
estadoRele1 = false;
atualizaEstadoReleEeprom();
  }


boolean acionarRele2Temp  =  (autoTemp2==1 && atingiuTemp2) || (autoTemp2==2 && (!atingiuTemp2) );        
boolean acionarRele2Clock =  (autoClock2==1 && atingiuClock2) || (autoClock2==2 && (!atingiuClock2) );        

boolean desligarRele2Temp  =  (autoTemp2==1 && (!atingiuTemp2)) || (autoTemp2==2 && atingiuTemp2 );  
boolean desligarRele2Clock =  (autoClock2==1 && (!atingiuClock2)) || (autoClock2==2 && atingiuClock2 );

boolean acionarRele2 =  (acionarRele2Temp || acionarRele2Clock) && (!desligarRele2Temp) && (!desligarRele2Clock);    
boolean desligarRele2=  (desligarRele2Temp || desligarRele2Clock);     

if(acionarRele2 && (!estadoRele2)){
digitalWrite(rele2, HIGH);
estadoRele2 = true;
atualizaEstadoReleEeprom();
  }

  if(desligarRele2 && estadoRele2){
digitalWrite(rele2, LOW);
estadoRele2 = false;
atualizaEstadoReleEeprom();
  }
  
}


void scanWifi(){
  Serial.println("scan start");
  pagHtml="";

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0){
    Serial.println("no networks found");
    pagHtml+="Nenhuma rede sem fio encontrada.";
}else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
 //     Serial.print(i + 1);
 //     Serial.print(": ");
 //     Serial.print(WiFi.SSID(i));
      pagHtml+=WiFi.SSID(i)+" ("+WiFi.RSSI(i)+") <br />";
 //     Serial.print(" (");
 //     Serial.print(WiFi.RSSI(i));
 //     Serial.print(")");
 //     Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
 // return retorno;
 // Serial.println("");
  }

void atualizaEstadoReleEeprom(){
estadoRelesEeprom = 0;
if(estadoRele1){estadoRelesEeprom++;}
if(estadoRele2){estadoRelesEeprom=estadoRelesEeprom+2;}
EEPROM.write(101, estadoRelesEeprom );
EEPROM.commit();
}
        
void handleRoot() {

//String message = "Number of args received:";

//message += server.args();

//for (int i = 0; i < server.args(); i++) {
//message += "Arg nº" + (String)i + " –> ";
//message += server.argName(i) + ": ";
//message += server.arg(i) + "\n";} 

    //aciona os reles:
    if (server.arg("rele1")== "1") {
       digitalWrite(rele1, HIGH);
       estadoRele1 = true;
       atualizaEstadoReleEeprom();
       server.send(200, "text/html", "<html><body></body></html>");return;
      }
          if (server.arg("rele1")== "0") {
       digitalWrite(rele1, LOW);
              estadoRele1 = false;
       atualizaEstadoReleEeprom();
       server.send(200, "text/html", "<html><body></body></html>");return;
      }
          if (server.arg("rele2")== "1") {
       digitalWrite(rele2, HIGH);
              estadoRele2 = true;
       atualizaEstadoReleEeprom();
       server.send(200, "text/html", "<html><body></body></html>");return;
      }
          if (server.arg("rele2")== "0") {
       digitalWrite(rele2, LOW);
              estadoRele2 = false;
       atualizaEstadoReleEeprom();
       server.send(200, "text/html", "<html><body></body></html>");return;
      }

      //salva dados de horario de verao:
         if (server.arg("diaInicioHverao")!= "") {
          diaInicioHverao=server.arg("diaInicioHverao").toInt();
          mesInicioHverao=server.arg("mesInicioHverao").toInt();
          diaTerminoHverao=server.arg("diaTerminoHverao").toInt();
          mesTerminoHverao=server.arg("mesTerminoHverao").toInt();
          EEPROM.write(96, diaInicioHverao);
          EEPROM.write(97, mesInicioHverao);
          EEPROM.write(98, diaTerminoHverao);
          EEPROM.write(99, mesTerminoHverao);
EEPROM.commit();
         /* Serial.print("diaInicioVerao recebido: ");
              Serial.println(diaInicioHverao); 
                Serial.print("mesTerminoVerao recebido: ");
              Serial.println(mesTerminoHverao); */
          }
       if (server.arg("hVerao")!= "") {
        horarioVeraoAtivo=server.arg("hVerao").toInt();
         EEPROM.write(100, horarioVeraoAtivo);
         relogioSincronizado=false; //forca a resincronizacao e recalculo
        EEPROM.commit();
        /*Serial.print("hVerao recebido: ");
              Serial.println(horarioVeraoAtivo); */
         
        }

        //salva os dados de automacao:
       if (server.arg("autoTemp1")!= "") {
        autoTemp1=server.arg("autoTemp1").toInt();
        EEPROM.write(102, autoTemp1);
        temp1=server.arg("temp1").toInt();
        EEPROM.write(103, (temp1+100));
        autoClock1=server.arg("autoClock1").toInt();
        EEPROM.write(104, autoClock1);
        hrInicio1=server.arg("hrInicio1").toInt();
        EEPROM.write(105, hrInicio1);
        minInicio1=server.arg("minInicio1").toInt();
        EEPROM.write(106, minInicio1);
        hrTermino1=server.arg("hrTermino1").toInt();
        EEPROM.write(107, hrTermino1);
        minTermino1=server.arg("minTermino1").toInt();
        EEPROM.write(108, minTermino1);
        autoTemp2=server.arg("autoTemp2").toInt();
        EEPROM.write(109, autoTemp2);
        temp2=server.arg("temp2").toInt();
        EEPROM.write(110, (temp2+100));
        autoClock2=server.arg("autoClock2").toInt();
        EEPROM.write(111, autoClock2);
        hrInicio2=server.arg("hrInicio2").toInt();
        EEPROM.write(112, hrInicio2);
        minInicio2=server.arg("minInicio2").toInt();
        EEPROM.write(113, minInicio2);
        hrTermino2=server.arg("hrTermino2").toInt();
        EEPROM.write(114, hrTermino2);
        minTermino2=server.arg("minTermino2").toInt();
        EEPROM.write(115, minTermino2);
         
        EEPROM.commit();
          }        

  //recebe novo SSID e PASSWORD
    if (server.arg("wlanSsid")!= "") {
    
        esid = server.arg("wlanSsid");
       epass = server.arg("wlanPass");

         
        //teste salvando ssid and pass na EEPROM
Serial.println("clearing eeprom NETWORK DATA");
          for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }

          Serial.println("writing eeprom ssid:");
          for (int i = 0; i < esid.length(); ++i)
            {
              EEPROM.write(i, esid[i]);
              Serial.print("Wrote: ");
              Serial.println(esid[i]); 
            }
          Serial.println("writing eeprom pass:"); 
          for (int i = 0; i < epass.length(); ++i)
            {
              EEPROM.write(32+i, epass[i]);
              Serial.print("Wrote: ");
              Serial.println(epass[i]); 
            }    
          EEPROM.commit();
          Serial.println("{\"Success\":\"saved to eeprom... \"}");
          freezeDisplay=0;
   conectarWifi();
  }

   if (server.arg("showScan")!= "") {
    scanWifi();
    server.send(200, "text/html", "<html><body>"+pagHtml+"</body></html>");
    pagHtml="";
    }else{

  //exibicao pagina normal:
  int contentLenght=0;

htmlPart2(); 
contentLenght=contentLenght+pagHtml.length();

htmlPart3();
contentLenght=contentLenght+pagHtml.length();

htmlPart4();
contentLenght=contentLenght+pagHtml.length();

htmlPart5();
contentLenght=contentLenght+pagHtml.length();

htmlPart6();
contentLenght=contentLenght+pagHtml.length();

htmlPart7();
contentLenght=contentLenght+pagHtml.length();

htmlPart1();
contentLenght=contentLenght+pagHtml.length();


server.setContentLength(contentLenght);  
server.send(200,"text/html",pagHtml);
htmlPart2(); 
server.sendContent(pagHtml);
htmlPart3();
server.sendContent(pagHtml);
htmlPart4();
server.sendContent(pagHtml);
htmlPart5();
server.sendContent(pagHtml);
htmlPart6();
server.sendContent(pagHtml);
htmlPart7();
server.sendContent(pagHtml);
pagHtml="";

//server.send(200, "text/html", paginaHtml);
   }
   
     // if (server.arg("conWifi")== "1") {
     // conectarWifi();
     // }
      
// server.send(200, "text/html", "<h1>Hello from ESP8266 AP!</h1>");
}

void ativarModoAP(){  

  //WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.softAP(ssid2, password);
  WiFi.mode(WIFI_AP);
  delay(100);
  
Serial.println("Modo AP ativo");
Serial.print("Server IP address: ");
Serial.println(WiFi.softAPIP());

server.on("/", handleRoot);
  server.begin();
  
  }

void conectarWifi(){
wifiTentativaSemSucessoCount=0;

 //WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  delay(100);
  
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(esid);

  WiFi.begin(esid.c_str(), epass.c_str());
  
  }
 
  
void setup() {
  EEPROM.begin(128); //tamanho em bytes que será alocado para a "EEPROM" (entre 4 e 4096bytes)
  /*
  0-95  ssid e pass
  96  dia inicio h verao
  97  mes inicio h verao
  98  dia termino h verao
  99  mes termino h verao
  100 horario de verao ativo (0 ou 1)
  101 estado dos reles

  102 rele1 automatizacao por temperatura 
  103 rele1 temp 
  104 rele1 automatizacao por tempo
  105 rele1 hr inicio
  106 rele1 min inicio
  107 rele1 hr termino
  108 rele1 min termino
  109 rele2 automatizacao por temperatura 
  110 rele2 temp 
  111 rele2 automatizacao por tempo
  112 rele2 hr inicio
  113 rele2 min inicio
  114 rele2 hr termino
  115 rele2 min termino

  
  */
  delay(10);

//iniciar a interrupcao
  usrInit();

 //WiFi.mode(WIFI_AP_STA); //estava WiFi.mode(WIFI_STA); //em WIFI_AP_STA ele fica como AP e cliente ao mesmo tempo! (conectado em 2 redes), testar pings simultaneos para ver o comportamento.
 
    // init the display
  lmd.setEnabled(true);
  lmd.setIntensity(2);   // 0 = low, 10 = high (padrao: 2)

drawConWifi(wifiConCount);
  lmd.display();
  
  Serial.begin(115200);
 delay(10);
 
 // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
 
  for (int i = 0; i < 32; ++i){
    if(EEPROM.read(i)!=0){
      esid += char(EEPROM.read(i));
      }
    }
  Serial.print("SSID: ");
   Serial.println(esid);
  Serial.println("Reading EEPROM pass");

  for (int i = 32; i < 96; ++i){
    if(EEPROM.read(i)!=0){
      epass += char(EEPROM.read(i));
      }
    }
  Serial.print("PASS: ");
  //epass.trim();
  Serial.println(epass);  

//le as datas de horario de verao salvas na eeprom:
diaInicioHverao=EEPROM.read(96);
mesInicioHverao=EEPROM.read(97);
diaTerminoHverao=EEPROM.read(98);
mesTerminoHverao=EEPROM.read(99);
horarioVeraoAtivo=EEPROM.read(100);

autoTemp1 = EEPROM.read(102);
temp1 = EEPROM.read(103)-100;
autoClock1 = EEPROM.read(104);
hrInicio1 = EEPROM.read(105);
minInicio1 = EEPROM.read(106);
hrTermino1 = EEPROM.read(107);
minTermino1 = EEPROM.read(108);
autoTemp2 = EEPROM.read(109);
temp2 = EEPROM.read(110)-100;
autoClock2 = EEPROM.read(111);
hrInicio2 = EEPROM.read(112);
minInicio2 = EEPROM.read(113);
hrTermino2 = EEPROM.read(114);
minTermino2 = EEPROM.read(115);
/* Serial.print("diaInicioVerao lido: ");
   Serial.println(diaInicioHverao);

    Serial.print("hVerao lido: ");
   Serial.println(horarioVeraoAtivo);*/


wifiConectou1aVez=false;
ativarModoAP();
conectarWifi();

//Setup DS18b20 temperature sensor
  SetupDS18B20();

pinMode(rele1, OUTPUT);
pinMode(rele2, OUTPUT);

 digitalWrite(rele1, LOW);
 digitalWrite(rele2, LOW);

//lê o estado dos reles na eeprom
estadoRelesEeprom = EEPROM.read(101);
 if(estadoRelesEeprom==1 || estadoRelesEeprom==3){estadoRele1=true;digitalWrite(rele1, HIGH);}
 if(estadoRelesEeprom==2 || estadoRelesEeprom==3){estadoRele2=true;digitalWrite(rele2, HIGH);}
 
drawApIp();
}

void callback(){ //1 chamada a cada 0,5 segundo
  if(WiFi.status()==3){ //WiFi.status()==3 significa que se conectou com sucesso à rede wifi para acesso à internet...
    if(!wifiConectou1aVez){
      Serial.println("WiFi connected");
       Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //ipObtidoInternet=WiFi.localIP().toString();
  

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
    wifiConectou1aVez=true;
freezeDisplay=0; //para sumir o "Con. fail" do display    
    }
  if((!obteveTempo1avez || !relogioSincronizado) && segundos%3==0){ //a cada 3 segundos, tenta obter o tempo novamente, caso nao tenha obtido pela primeira vez ou se tiver fora de sincronia...
   atualizaTempo(); 
    }     wifiTentativaSemSucessoCount=0;}

    if(WiFi.status()==1 || WiFi.status()==4){ //1:ssid nao encontrado; 4:senha incorreta
  wifiTentativaSemSucessoCount++;
  if(wifiTentativaSemSucessoCount>32000){wifiTentativaSemSucessoCount=32000;}
  if(wifiTentativaSemSucessoCount==60){ //depois de 60 tentativas (1 a cada meio segundo) 
//conexao falhou, exibir algo no display
  Serial.println("Tentativa de conexão ao WiFi falhou...");
  drawConFail();
  freezeDisplay=600; //1 unidade para cada 0,5 segundo.

ativarModoAP();
  
    }
    }

  if(chamadaPar){ //1 chamada a cada segundo

 //conta o tempo para funcionar o relogio mesmo sem a sincronia (definir um tempo max para o relogio funcionar assim, para nao ficar impreciso)
tempo++;

//(o ajuste de fuso é feito em atualizaTempo())

tempoSnapshot=tempo;

segundos=second(tempoSnapshot);
hora=hour(tempoSnapshot);
minuto=minute(tempoSnapshot);
diaSemana=weekday(tempoSnapshot);
dia=day(tempoSnapshot);
mes=month(tempoSnapshot);
ano=year(tempoSnapshot);

dig3=minuto/10;
dig4=minuto-(dig3*10);

dig1=hora/10;
dig2=hora-(dig1*10);

// segundos++;
//if(segundos==60){segundos=0;

 if(segundos==0){ //executa 1 vez por minuto

/*
  dig4++;
if(dig4==10){dig4=0;dig3++;
if(dig3==6){dig3=0;dig2++;
if( (dig2==4 && dig1==2) || dig2==10 ){dig2=0;dig1++;
if(dig1==3){dig1=0; //passou "1 dia"
} } } } 
*/

//verifica se atingiu a data de inicio ou termino do horario de verao e ativa ou desativa o horario de verao:
if(horarioVeraoAtivo==0 && dia==diaInicioHverao && mes==mesInicioHverao){
  horarioVeraoAtivo=1;
  EEPROM.write(100, horarioVeraoAtivo);
         relogioSincronizado=false; //forca a resincronizacao e recalculo
        EEPROM.commit();
  }
if(horarioVeraoAtivo==1 && dia==diaTerminoHverao && mes==mesTerminoHverao){
  horarioVeraoAtivo=0;
  EEPROM.write(100, horarioVeraoAtivo);
         relogioSincronizado=false; //forca a resincronizacao e recalculo
        EEPROM.commit();
 }

//se estiver entre 23h55 e 0h05 roda a sincronia a cada minuto para manter a data precisa:
//if( (dig1==2 && dig2==3 && dig3==5 && dig4>4) || (dig1==0 && dig2==0 && dig3==0 && dig4<6)){
//relogioSincronizado = false;}

//se for minutos 00 ou 30, memoriza a temperatura atual para verificacao posterior dos recordes 24h:
if(( (dig3==0 || dig3==3) && dig4==0 ) ||(!(gravou1aVez24h))){

//move os horarios/dias/temps 24h?
for(int i=47; i>0; i-- ){
   diaTemp24h[i]=   diaTemp24h[(i-1)];
   mesTemp24h[i]=   mesTemp24h[(i-1)];
   anoTemp24h[i]=   anoTemp24h[(i-1)];
  horaTemp24h[i]=  horaTemp24h[(i-1)];
minutoTemp24h[i]=minutoTemp24h[(i-1)];
temp24h[i]=temp24h[(i-1)];
  }
//grava a posicao 0 com os dados atuais:
diaTemp24h[0]=dia;
mesTemp24h[0]=mes;
anoTemp24h[0]=ano;
temp24h[0]=temperature;

//if( dig1==0 && dig2==0 && dig3==0 && dig4==5 ){
horaTemp24h[0]=hora;
minutoTemp24h[0]=minuto;
//  }else{
//    int variavelTemporaria=dig1*10;
//    variavelTemporaria=variavelTemporaria+dig2;
//horaTemp24h[0]=(dig1*10)+dig2;
//minutoTemp24h[0]=(dig3*10)+dig4;    
//    }
  
gravou1aVez24h=true;  }

}//fim acao a cada minuto

tempCount++;
if(tempCount==3){tempCount=0;
//uma acao a cada 3 segundos:
TempLoop(); //le a temperatura

//exibe o AP IP
if(faseExibicaoApIp<3){
if(faseExibicaoApIp==0){
drawNumIp(1, 9, 2, 1, 6, 8, 55) ; 
  }
if(faseExibicaoApIp==1){
drawNumIp(0, 0, 4, 0, 0, 1, 10) ; 
  }
  faseExibicaoApIp++;
  }

//exibe o Wlan IP no diplay:
if(faseExibicaoApIp>2 && wifiConectou1aVez && faseExibicaoWlanIp<4){
if(faseExibicaoWlanIp==0){
drawWlanIp();
  }
if(faseExibicaoWlanIp==1){
int tempDig1=WiFi.localIP()[0]/100; 
int tempDig2= (WiFi.localIP()[0] - (tempDig1 * 100)) /10 ;
int tempDig3= WiFi.localIP()[0] - (tempDig1 * 100) - (tempDig2 * 10); 
int tempDig4=WiFi.localIP()[1]/100; 
int tempDig5= (WiFi.localIP()[1] - (tempDig4 * 100)) /10 ;
int tempDig6= WiFi.localIP()[1] - (tempDig4 * 100) - (tempDig5 * 10); 
drawNumIp(tempDig1, tempDig2, tempDig3, tempDig4, tempDig5, tempDig6, 55) ; 
  }
if(faseExibicaoWlanIp==2){
int tempDig1=WiFi.localIP()[2]/100; 
int tempDig2= (WiFi.localIP()[2] - (tempDig1 * 100)) /10 ;
int tempDig3= WiFi.localIP()[2] - (tempDig1 * 100) - (tempDig2 * 10); 
int tempDig4=WiFi.localIP()[3]/100; 
int tempDig5= (WiFi.localIP()[3] - (tempDig4 * 100)) /10 ;
int tempDig6= WiFi.localIP()[3] - (tempDig4 * 100) - (tempDig5 * 10); 
drawNumIp(tempDig1, tempDig2, tempDig3, tempDig4, tempDig5, tempDig6, 10) ; 
  }
  faseExibicaoWlanIp++;
  }
  
} //fim acao a cada 3 segundos

 count++;
if(count==21600){count=0; //atualiza relogio a cada X segundos (valor do count)
   relogioSincronizado=false; ciclosSemObterTempo++; 
   //evita que a variavel estoure:
  if(ciclosSemObterTempo>32000){ciclosSemObterTempo=32000;}
  }
    } //fim acao a cada segundo


//refresh no display:
if(faseExibicaoApIp>2 && freezeDisplay==0 && ( !(wifiConectou1aVez) || faseExibicaoWlanIp>3 ) ){
  if(wifiConectou1aVez){
    if(obteveTempo1avez){
     if(chamadaPar){
        
  //se ficar mais de x ciclos de atualizacao (aprox x minutos) sem conseguir atualizar a hora, exibir o horario piscando para alertar pois pode ter ficado impreciso
  // a variavel ciclosSemObterTempo recebe 1 incremento a cada chamada de atualizacao por NTP, recebendo 0 quando o tempo é obtido com sucesso.
  if(ciclosSemObterTempo>16){ //para 1 sincronia a cada 4 horas, deixa de exibir o relogio se ficar 4 dias sem sincronizar (conforme testes efetuados, após 1 dia sem sincronia o relogio estava correto) 
    
   // drawClock(11,11,12,11,11, x, 0); //exibe --:--  
   drawClock(10,10,12,10,10, x, 0);
  
  }else{
    drawClock(dig1,dig2,10,dig3,dig4, x, 0);
    }
    }else{
       drawClock(dig1,dig2,12,dig3,dig4, x, 0);
    }

   }else{
    //exibir conectando ntp . . .

   wifiConCount++;
    if(wifiConCount>36 || wifiConCount<33){wifiConCount=33;}
        drawConNPT(wifiConCount);
        
    }
  }else{
    //exibir conectando wifi . . .
    wifiConCount++;
    if(wifiConCount>30 || wifiConCount<27){wifiConCount=27;}
    drawConWifi(wifiConCount);
      
    }
    }
   lmd.display();
if(freezeDisplay>0){freezeDisplay--;
if(freezeDisplay==0){
 conectarWifi();
  }
}
if(chamadaPar){chamadaPar=false;}else{chamadaPar=true;}
if(digitalRead(botao)==1){ 
  drawApIp();
faseExibicaoApIp=0;  
faseExibicaoWlanIp=0;   
  }
}
  
void loop() {

 //  server.handleClient();
   

//verificacao no loop
  if (_timeout){
      callback();
      _timeout = false;
  }
  yield(); //um putosegundo soh pra respirar

    server.handleClient();
  

 // Draw the text to the current position
 //drawClock(dig1,dig2,dig3,dig4, x, 0);
 
   // In case you wonder why we don't have to call lmd.clear() in every loop: The font has a opaque (black) background...
  // Toggle display of the new framebuffer
// lmd.display();

/*
  //Wait to let the human read the display
 delay(ANIM_DELAY+30);
 // Advance to next coordinate
 if( --x < len * -8 )
   x = LEDMATRIX_WIDTH;
*/

//delay(500);


/*
delay(500);


*/

}

void atualizaTempo(){
  if(WiFi.status()==3){ //se está conectado à rede wifi
  
  Serial.println("tentando atualizar o tempo...");

   //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);


int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  } else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);

    tempo=epoch-10800+(3600*horarioVeraoAtivo); //aplica UTC -3
    tempoSnapshot=tempo;

/*
    // print the hour, minute and second:
    Serial.print("The UTC-3 time is ");       // UTC is the time at Greenwich Meridian (GMT)

    hora=(epoch  % 86400L) / 3600;

//aplica o fuso do brasil (UTC-3):
hora=hora-3;
if(hora<0){hora=hora+24;}
    
    dig1=hora/10;
    dig2=hora-(dig1*10);

    minuto=((epoch % 3600) / 60);
    dig3=minuto/10;
    dig4=minuto-(dig3*10);

    ciclosSemObterTempo=0;
    
    Serial.print(hora); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (minuto < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second

   Serial.println(day(epoch));
   Serial.println(month(epoch));
     Serial.println(year(epoch));

     Serial.println(weekday(epoch));
*/
//diaSemana=weekday(epoch);
 diaUltimaSincronia=day(tempoSnapshot);  mesUltimaSincronia=month(tempoSnapshot);  anoUltimaSincronia=year(tempoSnapshot);  horaUltimaSincronia=hour(tempoSnapshot);  minutoUltimaSincronia=minute(tempoSnapshot);

 relogioSincronizado=true;
 ciclosSemObterTempo=0;
 if(!obteveTempo1avez){
  dia1aSincronia=diaUltimaSincronia;  mes1aSincronia=mesUltimaSincronia;  ano1aSincronia=anoUltimaSincronia;  hora1aSincronia=horaUltimaSincronia;  minuto1aSincronia=minutoUltimaSincronia;
  obteveTempo1avez=true;
  } 

         
  }
  }
  }



void drawClock(int dig1,int dig2, int pontos, int dig3,int dig4, int x, int y ){

  if(dig1==0){dig1=10;} //apaga o primeiro digito se for zero

  int tempDezena = (temperature/10);
  int tempUnidade = (temperature-((tempDezena)*10))+42; //+13 fonte media, +42 fonte peq
  tempDezena=tempDezena+42;

      drawSprite( font[dig1], 0, y, 5, 8 ); // 8,8,8
      drawSprite( font[dig2], 5, y, 5, 8 );
      drawSprite( font[pontos], 10, y, 5, 8 );
      drawSprite( font[dig3], 12, y, 5, 8 );
      drawSprite( font[dig4], 17, y, 5, 8 );

      drawSprite( font[10], 22, y, 1, 8 );
      drawSprite( font[tempDezena], 23, y, 4, 8 ); //dezena temp
      drawSprite( font[tempUnidade], 27, y, 6, 8 ); //unidade temp
      drawSprite( font[23], 31, y, 1, 1 ); //grau

}

void drawNumIp(int num1, int num2, int num3, int num4, int num5, int num6, int pontoFinal){
  //pontoFinal=55: ponto; =10:nada
num1=num1+42;
num2=num2+42;
num3=num3+42;
num4=num4+42;
num5=num5+42;
num6=num6+42;

      drawSprite( font[num1], 0, 0, 8, 8 ); 
      drawSprite( font[num2], 4, 0, 8, 8 );
      drawSprite( font[num3], 8, 0, 8, 8 );
      drawSprite( font[55],  12, 0, 8, 8 );

      drawSprite( font[num4], 16, 0, 8, 8 ); 
      drawSprite( font[num5], 20, 0, 8, 8 );
      drawSprite( font[num6], 24, 0, 8, 8 );
      drawSprite( font[pontoFinal],   28, 0, 8, 8 );
     
}

void drawConWifi(int valor){

      drawSprite( font[24], 0, 0, 8, 8 ); // 8,8,8
      drawSprite( font[25], 8, 0, 8, 8 );
      drawSprite( font[26], 16, 0, 8, 8 );
      drawSprite( font[valor], 24, 0, 8, 8 ); //27+wifi
     
}

void drawConNPT(int valor){

      drawSprite( font[24], 0, 0, 8, 8 ); // 8,8,8
      drawSprite( font[31], 8, 0, 8, 8 );
      drawSprite( font[32], 16, 0, 8, 8 );
      drawSprite( font[valor], 24, 0, 8, 8 ); //27+wifi
     
}

void drawConFail(){

      drawSprite( font[24], 0, 0, 8, 8 ); 
      drawSprite( font[52], 8, 0, 8, 8 );
      drawSprite( font[53], 16, 0, 8, 8 );
      drawSprite( font[54], 24, 0, 8, 8 ); 
     
}

void drawApIp(){

      drawSprite( font[56], 0, 0, 8, 8 ); 
      drawSprite( font[57], 8, 0, 8, 8 );
      drawSprite( font[58], 16, 0, 8, 8 );
      drawSprite( font[10], 24, 0, 8, 8 );
     
}

void drawWlanIp(){

      drawSprite( font[59], 0, 0, 8, 8 ); 
      drawSprite( font[60], 8, 0, 8, 8 );
      drawSprite( font[61], 16, 0, 8, 8 );
      drawSprite( font[58], 24, 0, 8, 8 );
     
}


/**
 * This function draws a string of the given length to the given position.
 */
 /*
void drawString(char* text, int len, int x, int y )
{
  for( int idx = 0; idx < len; idx ++ )
  {
    int c = text[idx] - 32;

    // stop if char is outside visible area
    if( x + idx * 8  > LEDMATRIX_WIDTH )
      return;

    // only draw if char is visible
    if( 8 + x + idx * 8 > 0 )
      drawSprite( font[c], x + idx * 8, y, 8, 8 );
  }
}
*/

/**
 * This draws a sprite to the given position using the width and height supplied (usually 8x8)
 */
void drawSprite( byte* sprite, int x, int y, int width, int height )
{
  // The mask is used to get the column bit from the sprite row
  byte mask = B10000000;
  
  for( int iy = 0; iy < height; iy++ )
  {
    for( int ix = 0; ix < width; ix++ )
    {
      lmd.setPixel(x + ix, y + iy, (bool)(sprite[iy] & mask ));

      // shift the mask by one pixel to the right
      mask = mask >> 1;
    }

    // reset column mask
    mask = B10000000;
  }
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress& address) {
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

