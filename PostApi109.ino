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
*/



const int heurecouche = 18;
const int heureleve = 8;
const int frequencenuit = 5400;  //5400 soit 1h30
const int frequencejour = 1200;  //1200 soit 20min


String FirmwareVer = {"109"};
#define URL_fw_Version "https://raw.githubusercontent.com/Yogre39/SmartRuche/master/bin_version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/Yogre39/SmartRuche/master/fw.bin"
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"


#include "OneWire.h"
#include "DallasTemperature.h"
OneWire oneWire(32);
DallasTemperature ds(&oneWire);

     int quisuisje=888;
     char* smartruche;
     String key;
     DeviceAddress insideThermometer;
     DeviceAddress outsideThermometer;


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
const unsigned long interval = 843200000;  //vérif MAJ Firmware toutes les 12h

   
#include "EEPROM.h"



#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;



#include "MAX1704X.h"
MAX1704X _fuelGauge = MAX1704X(MAX17043_mV);


#include <CuteBuzzerSounds.h>
#define BUZZER_PIN 2//13

RTC_DATA_ATTR bool miseenmarche=LOW;
RTC_DATA_ATTR bool apibeepsucces=HIGH;

void identite () {

    byte byte1 = EEPROM.read(40);
    byte byte2 = EEPROM.read(41);
  quisuisje = (byte1 << 8) + byte2;
  if (quisuisje == 3){
    smartruche = "Smartruche3";
    key = "kdcr4c67h1nntx0t";
    insideThermometer[0] = 0x28;//numéro de sonde 1
    insideThermometer[1] = 0x04;//Pour trouver l'adresse, scanner avec l'exemple OneWireSearch dans les exemple des Dallastemperature, sans oublier d'activer l'alim des capteur sur la pin18
    insideThermometer[2] = 0x1C;
    insideThermometer[3] = 0x48;
    insideThermometer[4] = 0xF6;
    insideThermometer[5] = 0x7F;
    insideThermometer[6] = 0x3C;
    insideThermometer[7] = 0x8C;
    outsideThermometer[0] = 0x28;//numéro de sonde 2
    outsideThermometer[1] = 0x48;
    outsideThermometer[2] = 0x97;
    outsideThermometer[3] = 0x48;
    outsideThermometer[4] = 0xF6;
    outsideThermometer[5] = 0x44;
    outsideThermometer[6] = 0x3C;
    outsideThermometer[7] = 0x31;
  }
  else if (quisuisje == 2){
    smartruche = "Smartruche2";
    key = "wwh5maya7hcgv4gf";
    insideThermometer[0] = 0x28;//numéro de sonde 3
    insideThermometer[1] = 0x87;
    insideThermometer[2] = 0x2F;
    insideThermometer[3] = 0x87;
    insideThermometer[4] = 0x27;
    insideThermometer[5] = 0x19;
    insideThermometer[6] = 0x01;
    insideThermometer[7] = 0x57;
    outsideThermometer[0] = 0x28;//numéro de sonde 4
    outsideThermometer[1] = 0x4C;
    outsideThermometer[2] = 0x51;
    outsideThermometer[3] = 0x96;
    outsideThermometer[4] = 0x27;
    outsideThermometer[5] = 0x19;
    outsideThermometer[6] = 0x01;
    outsideThermometer[7] = 0x48;
  }
   else if (quisuisje == 6){
    smartruche = "Smartruche6";
    key = "aza7ssxqp4zrnwbr";
    insideThermometer[0] = 0x28;//numéro de sonde 5
    insideThermometer[1] = 0x62;
    insideThermometer[2] = 0xFC;
    insideThermometer[3] = 0xD5;
    insideThermometer[4] = 0x27;
    insideThermometer[5] = 0x19;
    insideThermometer[6] = 0x01;
    insideThermometer[7] = 0x5A;
    outsideThermometer[0] = 0x28;//numéro de sonde 6
    outsideThermometer[1] = 0x7B;
    outsideThermometer[2] = 0x3B;
    outsideThermometer[3] = 0xDD;
    outsideThermometer[4] = 0x27;
    outsideThermometer[5] = 0x19;
    outsideThermometer[6] = 0x01;
    outsideThermometer[7] = 0xF8; 
  } 
     else if (quisuisje == 4){
    smartruche = "Smartruche4";
    key = "wwh5maya7hcgv4qy";
    insideThermometer[0] = 0x28;//numéro de sonde 7
    insideThermometer[1] = 0x1D;
    insideThermometer[2] = 0x8C;
    insideThermometer[3] = 0xCE;
    insideThermometer[4] = 0x27;
    insideThermometer[5] = 0x19;
    insideThermometer[6] = 0x01;
    insideThermometer[7] = 0x27;
    outsideThermometer[0] = 0x28;//numéro de sonde 8
    outsideThermometer[1] = 0x4C;
    outsideThermometer[2] = 0x35;
    outsideThermometer[3] = 0x93;
    outsideThermometer[4] = 0x27;
    outsideThermometer[5] = 0x19;
    outsideThermometer[6] = 0x01;
    outsideThermometer[7] = 0xC7; 
  } 
   else if (quisuisje == 5){
    smartruche = "Smartruche5";
    key = "a1fnacqulgkgrkhp";
    insideThermometer[0] = 0x28;//numéro de sonde 7
    insideThermometer[1] = 0x1D;
    insideThermometer[2] = 0x8C;
    insideThermometer[3] = 0xCE;
    insideThermometer[4] = 0x27;
    insideThermometer[5] = 0x19;
    insideThermometer[6] = 0x01;
    insideThermometer[7] = 0x27;
    outsideThermometer[0] = 0x28;//numéro de sonde 8
    outsideThermometer[1] = 0x4C;
    outsideThermometer[2] = 0x35;
    outsideThermometer[3] = 0x93;
    outsideThermometer[4] = 0x27;
    outsideThermometer[5] = 0x19;
    outsideThermometer[6] = 0x01;
    outsideThermometer[7] = 0xC7; 
  } 
  else {smartruche = "SmartrucheX";}  
}


void setup() {
  if (miseenmarche==LOW){
    cute.init(BUZZER_PIN);
    cute.play(S_CONNECTION);
  }
  
  Serial.begin(115200); 
  EEPROM.begin(200);

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
      float VoutsideThermometer = temp(outsideThermometer);
      float VinsideThermometer = temp(insideThermometer);
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
      if (Vbatterie > 2) {donnees += + "&" + batterie;}
      if (VoutsideThermometer != -127){donnees += +  "&" +  tempexte;}
      if (VinsideThermometer != -127){donnees += +  "&" +  tempinte;}



      
  /*
      if (VinsideThermometer < 7){
      String callmebot = "https://api.callmebot.com/whatsapp.php?phone=33685870183&text=Température+faible+SmartRuche3&apikey=6074632";
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
             if (FirmwareVersionCheck()) {firmwareUpdate();}
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
char chtare[30] = "";
sprintf(chtare, "%f", zeroeeprom);
Serial.print("chtare  ");  Serial.println(chtare);
char cstr[16];
itoa(quisuisje, cstr, 10);

String qui;
WiFiManagerParameter custom_qui("SmartRuche", "NuméroSR", cstr, 3);
WiFiManagerParameter custom_tare("Tare", "tare", chtare , 16);
WiFiManagerParameter custom_save("parametres", "parametres", "", 6);
wm.addParameter(&custom_qui);
wm.addParameter(&custom_tare);
wm.addParameter(&custom_save);

    wm.setConnectRetries(5);
    wm.setConfigPortalTimeout(180); //180
    //wm.resetSettings();


    bool res = wm.autoConnect(smartruche,"");

 

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
qui=custom_qui.getValue();
quisuisje = qui.toInt();
 Serial.print("quisuisje : ");  Serial.println(quisuisje);
 //Serial.print("custom_tare.getValue()");  Serial.println(custom_tare.getValue());
//stringtare = custom_tare.getValue(); Serial.print("stringtare");  Serial.println(stringtare);
//zeroeeprom = stringtare.toFloat();Serial.print("zeroeeprom");  Serial.println(zeroeeprom);
 // EEPROM.writeFloat(8, zeroeeprom);
 

  EEPROM.write(40, quisuisje >> 8);
  EEPROM.write(41, quisuisje & 0xFF);
  EEPROM.commit();
  identite ();
}
   if (save.startsWith("tare", 0)) {   
zerobalance();
   }

   
   if (save.startsWith("update", 0)) {   
     TotalMillis = 0;
     if (FirmwareVersionCheck()) {firmwareUpdate();}
   }
   
  }

  
float getBattery()
{
 _fuelGauge.begin(DEFER_ADDRESS);
 _fuelGauge.address(0x32);
 _fuelGauge.reset();
 delay(250);
 _fuelGauge.quickstart();
 delay(125);
 float bat = (_fuelGauge.voltage()/1000)/1.184;
 _fuelGauge.sleep();
  return bat;
  }
  
void AlertePoids()
{
  HTTPClient http;
      if (((poids_2 - poids_0)>0.600)&&((QuelJourdanslannee()>=74)&&(QuelJourdanslannee()<=288))){   //74-288 entre le 15 mars et le 15 octobre
         String callmebot = "https://api.callmebot.com/whatsapp.php?phone=33685870183&text=[SmartRuche" + String (quisuisje) + "]+Perte+poids+" + String(poids_2 - poids_0) + "Kg+essaimage+possible&apikey=6074632";
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
     String callmebot = "https://api.callmebot.com/whatsapp.php?phone=33685870183&text=[SmartRuche" + String (quisuisje) + "]+Mise+à+jour+Firmware+V" + payload + "&apikey=6074632";
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
     String callmebot = "https://api.callmebot.com/whatsapp.php?phone=33685870183&text=[SmartRuche" + String (quisuisje) + "]+HTTP+Reponse+Code+api+beep+:+" + erreur + "&apikey=6074632";
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
  EEPROM.writeFloat(8, zero);
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

float temp(DeviceAddress deviceAddress) {
  ds.begin();
  //ds.reset();
  ds.requestTemperatures();
  float tempC = ds.getTempC(deviceAddress);
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



void loop() {
 
}
