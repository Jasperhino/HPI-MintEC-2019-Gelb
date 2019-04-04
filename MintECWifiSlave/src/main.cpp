#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <string>
#include <WiFi.h>
#include <vector>
#include <utility>
#include <iostream>
#include <HTTPClient.h>
#include "esp_wifi.h"1

// Define led
#define PIN 14
#define NUMPIXELS 25
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int lastK = -1;

// END Define led

// Define sniffer
#define maxCh 13 //max Channel -> US = 11, EU = 13, Japan = 14
int curChannel = 1;
unsigned long timer = 0;
unsigned long wifi_timeout = 30000;
// END Define sniffer

// Define analyser
std::vector<std::pair<String, long>*> macList;
int deviceNum = 0;
// END Define analyser

// Define transmitter
unsigned long timer1 = 0;
//#define serverIP "http://172.16.48.225/update/a1/"
//#define serverIP "http://172.16.48.225/test/"
//#define serverIP "http://192.168.43.154/test/"
#define serverIP "http://192.168.43.154/update/a1/"

#define location "a1"
const char *ssid = "esp-test";//mintec2019";
const char *password = "12345678";
// END Define transmitter

//----------------------------

// funktions analyser
void AddMacToList(std::vector<std::pair<String, long>*> *vec, String mac){
  //std::cout << vec;
  long curTime = millis();
  bool adressFound = false;
  std::pair<String, long>* curAdress = new std::pair<String, long>();
  curAdress->first = mac;
  curAdress->second = curTime;

  //Serial.print("(*vec).size() = ");
  //Serial.println(vec->size());
  heap_caps_check_integrity_all(true);

  for (int i = 0; i < vec->size(); i++)
  {
    /*Serial.print("DEBUG: (*vec)[");
    Serial.print(i);
    Serial.print("].first = ");
    Serial.println((*vec).at(i)->first);
    Serial.print("length: ");
    Serial.println(vec->size());//*/
    if ((*vec).at(i)->first == mac)
    {
      adressFound = true;
      (*vec).at(i)->second = curTime;
    }
  }
  if(!adressFound) (*vec).push_back(curAdress);
}

int UpdateMacList(std::vector<std::pair<String, long>*> *vec){
  //std::cout << vec;
  long curTime = millis();
  int length = (*vec).size();
  std::pair<String, long> curAdress;
  
  //Serial.print("update: (*vec).size() = ");
  //Serial.println(length);
  heap_caps_check_integrity_all(true);
  /*for (int i = length-1; i >= 0; i--){
    Serial.print("update: DEBUG: (*vec)[");
    Serial.print(i);
    Serial.print("].first = ");
    Serial.println((*vec).at(i)->first);
  }//*/
  for (int i = length - 1; i > 0; i--){
    /*Serial.print("update: DEBUG: (*vec)[");
    Serial.print(i);
    Serial.print("].first = ");
    Serial.println((*vec).at(i)->first);//*/
    if (curTime - (*vec).at(i)->second > 60000){
      //Serial.print("erased: ");
      //Serial.println(i);
      (*vec).erase((*vec).begin() + i);
      break;
    }
  }//*/
  return length;
}
// END funktion analyser

//funktions sniffer
const wifi_promiscuous_filter_t filt = {.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};//Idk what this does

typedef struct{ // or this
  uint8_t mac[6];
} __attribute__((packed)) MacAddr;

typedef struct{ // still dont know much about this
  int16_t duration;
  MacAddr da;
  MacAddr sa;
  MacAddr bssid;
  int16_t seqctl;
  unsigned char payload[];
} __attribute__((packed)) WifiMgmtHdr;

void sniffer(void *buf, wifi_promiscuous_pkt_type_t type){                                                            //This is where packets end up after they get sniffed
  wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t *)buf; // Dont know what these 3 lines do
  int len = p->rx_ctrl.sig_len;
  WifiMgmtHdr *wh = (WifiMgmtHdr *)p->payload;
  len -= sizeof(WifiMgmtHdr);
  if (len < 0)
  {
    //Serial.println("Receuved 0");
    return;
  }
  String packet;
  String mac;
  //Serial.println("payload: ");
  for (int i = 8; i <= 8 + 6 + 1; i++)
  { // This reads the first couple of bytes of the packet. This is where you can read the whole packet replaceing the "8+6+1" with "p->rx_ctrl.sig_len"
    packet += String(p->payload[i], HEX);
    //packet += std::cout << std::hex << p->payload[i];
    //Serial.println(p->payload[i]);
  }
  //Serial.println("packet: "+packet);
  for (int i = 4; i <= 15; i++)
  { // This removes the 'nibble' bits from the stat and end of the data we want. So we only get the mac address.
    mac += packet[i];
  }

  //if (mac.find(NULL) != std::string::npos){
  if (mac.charAt(11) == '\0')
  {
    //€Serial.println("Receuved Invalid MAC-Adress: "+mac);
    return;
  } //*/

  mac.toUpperCase();
  //Serial.println("MAC: " + mac);
  AddMacToList(&macList,mac);
}
// END funktions sniffer

// funktions LED
void intensity(int k){
  if(lastK != k){
    lastK = k;
    Serial.print("dN: ");
    Serial.println(deviceNum);

    int l = 0;
    if (k == 0){
      l = pixels.Color(0, 0, 30); // Blue
    } else if (k <= 10){
      l = pixels.Color(0, 30, 0); // Green
    } else if (k <= 20){
      l = pixels.Color(30, 30, 0); // Yellow
    } else {
      l = pixels.Color(30, 0, 0); // Red
    }

    // Set colors
    pixels.clear();
    for (int i = 0; i < k; i++)
    {
      pixels.setPixelColor(i, l);
      delayMicroseconds(10);
    }
    pixels.show();
  }
}
// END funktions LED

void connect(){

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  long conection_start = millis();
  while(WiFi.status() != WL_CONNECTED && millis() - conection_start < wifi_timeout){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected with IP: ");
  Serial.println(WiFi.localIP());

}

// funktions transmitter
void sendPost(int num){
  if(WiFi.status() != WL_CONNECTED){
    connect();
  }
  
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String dest = serverIP;
    dest += String(num);
    
    Serial.print("Posting to: ");
    Serial.println(dest);
    http.setReuse(true);
    http.begin(dest); //Specify destination for HTTP request
    http.addHeader("Content-Type", "text/plain");             //Specify content-type header
    //http.setTimeout(0);

    int httpResponseCode = http.POST(location);   //Send the actual POST request
  
    if(httpResponseCode > 0){
      //String response = http.getString(); //Get the response to the request

      Serial.println(httpResponseCode); //Print return code
      //Serial.println(response);         //Print request answer
  
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }//*/
    http.end();  //Free resources
  }
}
// END funktions transmitter */

//--------------------------------------------------

//===== SETUP =====//
void setup(){
  /* start Serial */
  Serial.begin(9600);

  /* setup LED */
  pinMode(PIN, OUTPUT);
  pixels.begin();
  pixels.clear();
  pixels.show();

  connect();
  /* setup sniffer */
   Serial.println("starting!");
}

//===== LOOP =====//
void loop(){
  //Serial.println(millis());
  if (timer + 2000 < millis()){
    timer = millis();
    //Serial.println("Changed channel:" + String(curChannel));
    curChannel++;
    if (curChannel > maxCh){
      curChannel = 1;
    }
    esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
  }
  deviceNum = UpdateMacList(&macList);
  intensity(deviceNum);

  if (timer1 + 10000 < millis()){
      timer1 = millis();
      sendPost(deviceNum);
  }
}