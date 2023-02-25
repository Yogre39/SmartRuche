  /* 90
   *  Modif du menu du wifimanager, ajoute d'un champ pour renseigner le numéro de de smartruche sur 3 digits, pour enregistrer, saisir eeprom dans le champs paramètres.
   *  Déclenchement de la tare de la ballance par le wifimanager en saisissant tare dans le champs paramètre.
   *  Désormais un seul programme pour toutes les ruches.
   *  
   *  
   *  100 OTA via github
   *  101 [HARD] Ajout buzzer sur pin13
   *      Ajout mélodie pour début tare balance, tempo et fin tempo après taretoutes les 24h.
   *  102 Effectue la vérif de version du fw en fonction de millis toutes les 24h
   *      Ajout de SmartRuche4 et param des DS1820 6 7 et 8
   *  103 Ajoute parametre "update" dans le portail wifimanager
   *  104 Ajout musique de démarrage
   *  105 MAJ au lancement après upload data et musique qd upload OK
   *  107 envoi un whatsapp si erreur apibeep et au retour à la normal
   *  108 alerte whatsapp poids basé sur poids-2 du 15 mars au 15 octobre
   *       arrondi temperature à 0.5°C
   *  109   Modif numérotation SmartRuche 1-->3, 7-->5
   *  110  Test sur MAX17043 est à l'@ 0x32 ou 0x36
   *  111  [HARD]séparation des capteur de température, ajout une R4.7k et fil vers entrée 33, code en fonction
*/
String FirmwareVer = {"109"};


const int heurecouche = 18;
const int heureleve = 8;
const int frequencenuit = 5400;  //5400 soit 1h30
const int frequencejour = 1200;  //1200 soit 20min



#define URL_fw_Version "https://raw.githubusercontent.com/Yogre39/SmartRuche/master/bin_version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/Yogre39/SmartRuche/master/fw.bin"
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"


#include "OneWire.h"
#include "DallasTemperature.h"
unsigned int WireInt = 32;unsigned int WireExt = 33;

OneWire oneWireInt(WireInt);OneWire oneWireExt(WireExt);
DallasTemperature dsInt(&oneWireInt);DallasTemperature dsExt(&oneWireExt);

     unsigned int quisuisje=888;
     char* smartruche;
     String key;
     String numphone;
     String NomRuche;
     DeviceAddress insideThermometer;
     DeviceAddress outsideThermometer;
     uint8_t Scanaddress[8];


#include <WiFiManager.h>
#include <WiFi.h>
#include <HTTPClient.h>


String serverName = "https://api.beep.nl/api/sensors?key=";

//deepsleep
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
int TIME_TO_SLEEP;

const int AlimCapteurs = 18;
#include "HX711.h"
const int BALLANCE_DOUT_PIN = 16;
const int BALLANCE_SCK_PIN = 17 ;
RTC_DATA_ATTR float poids_0=0;
RTC_DATA_ATTR float poids_1=0;
RTC_DATA_ATTR float poids_2=0;
RTC_DATA_ATTR unsigned long TotalMillis = 0;
const unsigned long interval = 36000000;  //vérif MAJ Firmware toutes les 10h

   
#include "EEPROM.h"



#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;



#include "MAX1704X.h"
MAX1704X _fuelGauge = MAX1704X(MAX17043_mV);


#include <CuteBuzzerSounds.h>
#define BUZZER_PIN 13

RTC_DATA_ATTR bool miseenmarche=LOW;
RTC_DATA_ATTR bool apibeepsucces=HIGH;

void identite () {
  quisuisje = (EEPROM.read(0)<< 8) + EEPROM.read(1);
  numphone = read_String(25);
  key = read_String(37);
  NomRuche = read_String(12);
   //NomRuche = "SmartRuche5";
   
//uint8_t
read_addext;
read_addint;

//smartruche = "SmartrucheX";
}


void setup() {
  if (miseenmarche==LOW){
    cute.init(BUZZER_PIN);
    cute.play(S_CONNECTION);
  }
  Serial.begin(115200); 
  EEPROM.begin(53);

  setCpuFrequencyMhz(80);
  Serial.print("Frequence du CPU :");Serial.println(getCpuFrequencyMhz());
  
   pinMode(AlimCapteurs, OUTPUT);
  digitalWrite(AlimCapteurs, HIGH);
  
  identite ();  
  
  wifiportail ();

    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      Serial.print("Heure en mémoire");Serial.println(QuelleHeure());
      if (QuelleHeure()==0){configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);Serial.print("Heure synchronisée");Serial.println(QuelleHeure());}
      
      
      HTTPClient http;
      poids_0 = balance();
      float Vbatterie = getBattery ();
      float VoutsideThermometer = tempExt();
      float VinsideThermometer = tempInt();
      digitalWrite(AlimCapteurs, LOW);
      String forcesignal = "rssi=" + String (WiFi.RSSI());
      String poids = "weight_kg=" + String(poids_0);
      String tempexte = "t=" + String(VoutsideThermometer);
      String tempinte = "t_i=" + String(VinsideThermometer);
      String batterie = "bv=" + String(Vbatterie);
      String lux = "l=20000";
      String gpslat = "lat=46.5047";
      String gpslon = "lon=5.6289";      
      String donnees = "&" + forcesignal + "&"/* +  lux+ "&"*/ +  gpslat+ "&" +  gpslon;
      if (poids_0 > 12) {donnees += + "&" + poids;}
      if (Vbatterie < 4.3) {donnees += + "&" + batterie;}
      if ((VoutsideThermometer != -127)&&(VoutsideThermometer < 55)){donnees += +  "&" +  tempexte;}
      if ((VinsideThermometer != -127)&&(VinsideThermometer < 55)){donnees += +  "&" +  tempinte;}



      
  /*
      if (VinsideThermometer < 7){
      String callmebot = "https://api.callmebot.com/whatsapp.php?phone=" + numphone + "&text=Température+faible"+ NomRuche ""&apikey=6074632";
     Serial.println("callmebot alert");
      http.begin(callmebot.c_str());      
    String httpRequestData1 = "";          
      // Send HTTP POST request
      int httpResponseCode1 = http.POST(httpRequestData1);
      
      if (httpResponseCode1>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode1);
        String payload1 = http.getString();
        Serial.println(payload1);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode1);
      }
      http.end();}

*/      
      String serverPath = serverName + key + donnees;
      Serial.println();Serial.println();
      Serial.print("Numéro téléphone Whatsapp:  ");Serial.println(numphone);
      Serial.print("Key:  ");Serial.println(key);
      Serial.print("Force du signal wifi:  ");Serial.print(forcesignal);;Serial.println("dBm");
      Serial.print("Poids:  ");Serial.println(poids);
      Serial.print("Tension batterie:  ");Serial.println(batterie);
      Serial.print("GPS: LAT,LON ");Serial.print(gpslat);Serial.print(",");Serial.println(gpslon);
      Serial.print("Temperature exterieure ");Serial.println(tempexte);
      Serial.print("Temperature interieure ");Serial.println(tempinte);
      Serial.println();Serial.println();
      Serial.println(serverPath);
      digitalWrite(AlimCapteurs, LOW);
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
     String httpRequestData = "";          
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString();
       
      if ((httpResponseCode>=200)&&(httpResponseCode<=299)) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.println(payload);
        if (apibeepsucces==LOW){WhatsappErreurApiBeep(httpResponseCode);apibeepsucces=HIGH;}
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        Serial.println(payload);
        if (apibeepsucces==HIGH){WhatsappErreurApiBeep(httpResponseCode);apibeepsucces=LOW;}
      }

      
      
      http.end();
       AlertePoids();
      if ((QuelleHeure()>heurecouche)||(QuelleHeure()<heureleve)){TIME_TO_SLEEP = frequencenuit;}
      else {TIME_TO_SLEEP = frequencejour;}
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

    
     
     TotalMillis += millis();
     //Serial.print("miseenmarche = ");Serial.println(miseenmarche);Serial.print("TotalMillis = ");Serial.println(TotalMillis);Serial.print("interval = ");Serial.println(interval);Serial.print("QuelleHeure() = ");Serial.println(QuelleHeure());Serial.print("heureleve = ");Serial.println(heureleve);
     //Serial.println("if ((miseenmarche==LOW)||((TotalMillis >= interval)&&(QuelleHeure()<heureleve)))");
     if ((miseenmarche==LOW)||((TotalMillis >= interval)&&(QuelleHeure()<heureleve))) {
             TotalMillis = 0;
             if (FirmwareVersionCheck()) firmwareUpdate();
             if (miseenmarche==LOW){
                    cute.init(BUZZER_PIN);
                    cute.play(S_DISCONNECTION);
                    miseenmarche=HIGH;}
             }
     TotalMillis += (TIME_TO_SLEEP*1000); 
     Serial.print("TotalMillis = ");Serial.println(TotalMillis);

    
     
      Serial.print("Mode deep sleep pour ");Serial.print(TIME_TO_SLEEP/60);Serial.println(" min");
       
        esp_deep_sleep_start();
    
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}



int QuelleHeure() {
 struct tm timeinfo;
 getLocalTime(&timeinfo);
 char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  String stringhour = String (timeHour);
  int intheure = stringhour.toInt();
  return intheure;
}

int QuelJourdanslannee() {
 struct tm timeinfo;
 getLocalTime(&timeinfo);
 char numerojour[3];
  strftime(numerojour,3, "%j", &timeinfo);
  String stringjour = String (numerojour);
  int intjour = stringjour.toInt();
  return intjour;
}

void wifiportail (){
  Serial.println("Fonction wifi manager");
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    WiFiManager wm;

float zeroeeprom; EEPROM.get(8, zeroeeprom);  Serial.print("Tare en eeprom  ");  Serial.println(zeroeeprom);
char chtare[17] = "";
sprintf(chtare, "%f", zeroeeprom);
Serial.print("chtare  ");  Serial.println(chtare);
char cstr[16];
char charNomRuche[12];NomRuche.toCharArray(charNomRuche, 13);
Serial.print("charNomRuche  ");  Serial.println(charNomRuche);
char charnumphone[11];numphone.toCharArray(charnumphone, 12);
char charkey[16];key.toCharArray(charkey, 17);
char charint[8];//= char (insideThermometer);
char charext[8];//= char (outsideThermometer);

itoa(quisuisje, cstr, 10);

String qui;
//WiFiManagerParameter custom_qui("SmartRuche", "NuméroSR", cstr, 3);
WiFiManagerParameter custom_NomRuche("NomRuche", "Nom_De_La_Ruche", charNomRuche, 12);
WiFiManagerParameter custom_phone("phone", "Numéro_de_Téléphone_WhatsApp", charnumphone, 12);
WiFiManagerParameter custom_key("keybeep", "Key_BeepNL", charkey, 16);
//WiFiManagerParameter custom_adressds1820int("interieure", "Adresse_capteur_température_intérieure", charint, 39);
//WiFiManagerParameter custom_adressds1820ext("exterieure", "Adresse_capteur_température_extérieure", charext, 39);
WiFiManagerParameter custom_tare("Tare", "tare_en_mémoire", chtare , 16);
WiFiManagerParameter custom_save("parametres", "parametres", "", 6);
//wm.addParameter(&custom_qui);
wm.addParameter(&custom_NomRuche);
wm.addParameter(&custom_key);
wm.addParameter(&custom_phone);
//wm.addParameter(&custom_adressds1820int);
//wm.addParameter(&custom_adressds1820ext);
wm.addParameter(&custom_tare);
wm.addParameter(&custom_save);

    wm.setConnectRetries(5);
    wm.setConfigPortalTimeout(200); //180
    //wm.resetSettings();

    const char* SmartRuche = NomRuche.c_str();

    bool res = wm.autoConnect(SmartRuche,"");

 

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
       TIME_TO_SLEEP = 3600;
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      Serial.print("Mode deep sleep pour ");Serial.print(TIME_TO_SLEEP/60);Serial.println(" min");

        esp_deep_sleep_start();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }
 String save = custom_save.getValue();
//Serial.print("save: ");  Serial.println(save); 
    if (save.startsWith("eeprom", 0)) {   
//qui=custom_qui.getValue();
NomRuche=custom_NomRuche.getValue();
numphone=custom_phone.getValue();
key=custom_key.getValue();
//charint= custom_adressds1820int.getValue();
//charext= custom_adressds1820ext.getValue();
quisuisje = qui.toInt();
 Serial.print("quisuisje : ");  Serial.println(quisuisje);
  writeString(12,NomRuche); 
 writeString(25,numphone);
  writeString(37,key);
//  write_char(54,charint);
//  write_char(93,charext);

  EEPROM.write(0, quisuisje >> 8);
  EEPROM.write(1, quisuisje & 0xFF);
  EEPROM.commit();
  identite ();
}
   if (save.startsWith("tare", 0)) {   
zerobalance();
   }
if (save.startsWith("scands", 0)) {ds1820toeeprom ();}
   
   if (save.startsWith("update", 0)) {   
     TotalMillis = 0;
     firmwareUpdate();}
   //}
   
  }

  
float getBattery()
{
 _fuelGauge.begin(DEFER_ADDRESS);
 Wire.beginTransmission(0x32);
   byte error = Wire.endTransmission();
    if (error == 0) {
      _fuelGauge.address(0x32);
       _fuelGauge.reset();
 delay(250);
 _fuelGauge.quickstart();
 delay(125);
 float bat = (_fuelGauge.voltage()/1000)/1.184;
 _fuelGauge.sleep();
  return bat;    
      }
  Wire.beginTransmission(0x36);
    error = Wire.endTransmission();
    if (error == 0) {
      _fuelGauge.address(0x36);  
 _fuelGauge.reset();
 delay(250);
 _fuelGauge.quickstart();
 delay(125);
 float bat = (_fuelGauge.voltage()/1000);
 _fuelGauge.sleep();
  return bat;
  }
  }
  
void AlertePoids()
{
  HTTPClient http;
      if (((poids_2 - poids_0)>0.600)&&((QuelJourdanslannee()>=74)&&(QuelJourdanslannee()<=288))){   //74-288 entre le 15 mars et le 15 octobre
         String callmebot = "https://api.callmebot.com/whatsapp.php?phone=" + numphone + "&text=[" + NomRuche + "]+Perte+poids+" + String(poids_2 - poids_0) + "Kg+essaimage+possible&apikey=6074632";
     Serial.println("callmebot alert");
     Serial.println(callmebot);
      http.begin(callmebot.c_str());      
    String httpRequestData1 = "";          
      // Send HTTP POST request
      int httpResponseCode1 = http.POST(httpRequestData1);
      
      if (httpResponseCode1>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode1);
        String payload1 = http.getString();
        Serial.println(payload1);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode1);
      }
      http.end();
      poids_2=poids_0;
      poids_1=poids_0;
      }
      poids_2 = poids_1;
      poids_1 = poids_0;
      if (poids_2==0)poids_2=poids_1;
}

void WhatsappMAJ(String payload)
{
  HTTPClient http;
     String callmebot = "https://api.callmebot.com/whatsapp.php?phone=" + numphone + "&text=[" + NomRuche + "]+Mise+à+jour+Firmware+V" + payload + "&apikey=6074632";
     Serial.println("callmebot alert");
     Serial.println(callmebot);
      http.begin(callmebot.c_str());      
    String httpRequestData1 = "";          
      // Send HTTP POST request
      int httpResponseCode1 = http.POST(httpRequestData1);
      
      if (httpResponseCode1>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode1);
        String payload1 = http.getString();
        Serial.println(payload1);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode1);
      }
      http.end();
      
}

void WhatsappErreurApiBeep(int erreur)
{
  HTTPClient http2;
     String callmebot = "https://api.callmebot.com/whatsapp.php?phone=" + numphone + "&text=[" + NomRuche + "]+HTTP+Reponse+Code+api+beep+:+" + erreur + "&apikey=6074632";
     Serial.println("callmebot alert");
     Serial.println(callmebot);
      http2.begin(callmebot.c_str());      
    String httpRequestData1 = "";          
      // Send HTTP POST request
      int httpResponseCode1 = http2.POST(httpRequestData1);
      
      if (httpResponseCode1>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode1);
        String payload1 = http2.getString();
        Serial.println(payload1);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode1);
      }
      http2.end();
      
}

float balance() {
float zeroeeprom;
HX711 scale;
scale.begin(BALLANCE_DOUT_PIN, BALLANCE_SCK_PIN);
scale.set_scale(-22.604);       // coef de calibration
EEPROM.get(8, zeroeeprom);
delay(1000);
  //if (scale.is_ready()) {
   float poids = scale.get_units(5);
   poids -= zeroeeprom;
   poids /= 1000;
    if (poids<0) {poids=-poids;}
  return poids;
 // }
}

 void zerobalance() {
   cute.init(BUZZER_PIN);
  Serial.println("tare en cours");
  cute.play(S_CONNECTION);
HX711 scale;
scale.begin(BALLANCE_DOUT_PIN, BALLANCE_SCK_PIN);
scale.set_scale(-22.604);
delay(1500);
  if (scale.is_ready()) {
    Serial.print("lecture pour tare : ");Serial.println(scale.get_units(5));
    float zero = scale.get_units(5);
  EEPROM.put(8, zero);//EEPROM.writeFloat(8, zero);
  EEPROM.commit();
   Serial.println("tare OK");


 //tempo musicale de 3minutes

for (int i=0;i<18;i++){
     cute.play(S_SAD);
  delay(7000);
}
    cute.play(S_DISCONNECTION);

  }
}

float tempInt() {
  dsInt.begin();
  //ds.reset();
  dsInt.requestTemperatures();
  oneWireInt.reset_search();
  DeviceAddress adress;
  oneWireInt.search(adress);

  float tempC = dsInt.getTempC(adress);
float arron;
int a = (int)tempC;
float dif = tempC - (float)a;
if ( dif < 0.25 ) {arron = a;}
else if ( dif < 0.75 ) {arron = (float)a +0.5;}
else { arron = a+1;}
 return arron;
}
float tempExt() {
  dsExt.begin();
  //ds.reset();
  dsExt.requestTemperatures();
  oneWireExt.reset_search();
  DeviceAddress adress;
  oneWireExt.search(adress);

  float tempC = dsExt.getTempC(adress);
float arron;
int a = (int)tempC;
float dif = tempC - (float)a;
if ( dif < 0.25 ) {arron = a;}
else if ( dif < 0.75 ) {arron = (float)a +0.5;}
else { arron = a+1;}
 return arron;
}


void firmwareUpdate(void) {
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
//  httpUpdate.setLedPin(LED_BUILTIN, LOW);
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

  switch (ret) {
  case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    Serial.println("HTTP_UPDATE_OK");
    break;
  }
}
int FirmwareVersionCheck(void) {
  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_fw_Version;
  fwurl += "?";
  fwurl += String(rand());
  Serial.println(fwurl);
  WiFiClientSecure * client = new WiFiClientSecure;

  if (client) 
  {
    client -> setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
    HTTPClient https;

    if (https.begin( * client, fwurl)) 
    { // HTTPS      
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) // if version received
      {
        payload = https.getString(); // save received version
      } else {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }
      
  if (httpCode == HTTP_CODE_OK) // if version received
  {
    payload.trim();
    if (payload.equals(FirmwareVer)) {
      Serial.printf("\nDevice already on latest firmware version:%s\n", FirmwareVer);
      return 0;
    } 
    else 
    {
      Serial.println(payload);
      Serial.println("New firmware detected");
      WhatsappMAJ(payload);
      return 1;
    }
  } 
  return 0;  
}


void writeString(char add,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
}


String read_String(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

void write_addint()
{
  int _size = 8;
  int i;
  int add=54;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,insideThermometer[i]);
  }

  EEPROM.commit();
}

void write_addext()
{
  int _size = 8;
  int i;
  int add=62;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,outsideThermometer[i]);
  }

  EEPROM.commit();
}

void read_addint()
{
  int i=0;
  int add=54;
  while(i < 8)
  {    
    insideThermometer[i]=EEPROM.read(add+i);
    i++;
  }
}

void read_addext()
{
  int i=0;
  int add=62;
  while(i < 8)
  {    
    outsideThermometer[i]=EEPROM.read(add);
    i++;add++;
  }
}

uint8_t findDevices(int pin)
{
  OneWire ow(pin);
  uint8_t count = 0;
  if (ow.search(Scanaddress))do {count++;} while (ow.search(Scanaddress));
  return count;
}

void ds1820toeeprom ()
{
 int nbds1820 = findDevices(32);
 if (nbds1820==0)Serial.println("// Pas de capteur détecté");
  if (nbds1820>1)Serial.println("// Plusieurs capteurs détectés, merci d'un connecter uniquement un");
  if (nbds1820==1){
    Serial.println("Aquisition capteur int");
    memcpy(insideThermometer, Scanaddress, sizeof(Scanaddress));
   //insideThermometer[]=Scanaddress[];
    write_addint();
    delay (10000);
    if ((findDevices(32))==1){
      Serial.println("Aquisition capteur ext");
      memcpy(outsideThermometer, Scanaddress, sizeof(Scanaddress));
     // outsideThermometer[]=Scanaddress[];
      write_addext();
      delay (10000);
    }
                  }
}




void loop() {
 
}
