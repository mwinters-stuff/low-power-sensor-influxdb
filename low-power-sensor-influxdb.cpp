#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <user_interface.h>
#include <chrono>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266HTTPClient.h>
#include <cfloat>

#include "ConfigFile.h"
#include "HTTPHandler.h"
#include "RemoteDebug.h"
#include "StringConstants.h"
#include "HardwareConfig.h"

RemoteDebug Debug;
ConfigFile *configFile;
HTTPHandler *httpHandler;
DataStorage *dataStorage;
// ADC_MODE(ADC_VCC);
ADC_MODE(ADC_TOUT_3V3);

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature DS18B20(&oneWire);
bool wokeUp;
bool rebooted;
bool readingTaken = false;
std::chrono::milliseconds fiveMinutes(std::chrono::minutes(5));


void setupAP() {
  Serial.println(F("Setting soft-AP ... "));
  String apName = String("ESP_") + String(ESP.getChipId());
  boolean result = WiFi.softAP((apName).c_str());
  if (result == true) {
    Debug.begin(apName);
    Debug.setResetCmdEnabled(true);
    Debug.setSerialEnabled(true);

    Debug.println(F("Ready Setting Up Portal"));
    httpHandler = new HTTPHandler(configFile, dataStorage);
    httpHandler->setupPortalServer();
    httpHandler->setupOTA();
  } else {
    Debug.begin(apName);
    Debug.setResetCmdEnabled(true);
    Debug.setSerialEnabled(true);
    Serial.println("Failed!");
  }
}

void connectWiFi(bool rebooted) {
  if(rebooted || (!WiFi.getAutoConnect())){
    Serial.println(F("Configuring WiFi"));
    WiFi.mode(WIFI_STA);
    WiFi.hostname(configFile->hostname);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    if (configFile->ipaddress.length() > 0) {
      Serial.println(F("Static Wifi"));
      IPAddress ipaddress;
      IPAddress gateway;
      IPAddress subnet;
      IPAddress dnsserver;
      ipaddress.fromString(configFile->ipaddress);
      gateway.fromString(configFile->gateway);
      subnet.fromString(configFile->subnet);
      dnsserver.fromString(configFile->dnsserver);
      Serial.printf("IP %s GW %s SN %s DN %s\n", ipaddress.toString().c_str(), gateway.toString().c_str(), subnet.toString().c_str(),
                    dnsserver.toString().c_str());

      WiFi.config(ipaddress, gateway, subnet, dnsserver);
    }
    WiFi.begin(configFile->wifi_ap.c_str(), configFile->wifi_password.c_str());
  }
  Serial.print(F("Connecting WiFi"));
  while (WiFi.isConnected()) {
    Serial.print(".");
    delay(5);
  }
  Serial.println();

  Serial.print(F("Connected, IP address: "));
  Serial.println(WiFi.localIP());

}

void setupReboot() {
  rebooted = true;
  // trackMem("setup start");
  Serial.println(F("Rebooted"));

  if (configFile->wifi_ap.length() == 0) {
    setupAP();
  } else {
    connectWiFi(true);

    MDNS.begin(configFile->hostname.c_str());
    httpHandler = new HTTPHandler(configFile, dataStorage);
    httpHandler->setupServer();
    httpHandler->setupOTA();

    Debug.begin(configFile->hostname);
    Debug.setResetCmdEnabled(true);
    Debug.setSerialEnabled(true);
  }
}



void setupWake() {
  Serial.println(F("Woke Up"));
  connectWiFi(false);
  Debug.begin(configFile->hostname);
  Debug.setResetCmdEnabled(true);
  Debug.setSerialEnabled(true);
  wokeUp = true;
}

void setup() {
  wokeUp = false;
  rebooted = false;
  Serial.begin(115200);
  for (int i = 0; i < 10; i++) {
    Serial.println();
  }

  dataStorage = new DataStorage();
  configFile = new ConfigFile();

  rst_info *reset_info = system_get_rst_info();
  if (reset_info->reason == REASON_DEEP_SLEEP_AWAKE) {
    setupWake();
  } else {
    setupReboot();
  }
}


void readTemperature(){
  DS18B20.begin();
  dataStorage->temperature = DBL_MAX;
  if(DS18B20.getDS18Count() > 0){
    ESP.wdtDisable();
    DS18B20.requestTemperatures(); 
    float temp = DS18B20.getTempCByIndex(0);
    if(temp != 85.0 && temp != (-127.0)){
      dataStorage->temperature = temp;
    }else{
      Debug.println(F("Temperature Reading Failed"));
    }
    ESP.wdtEnable((uint32_t)0);
    Debug.print(TEMPERATURE);
    Debug.println(String(dataStorage->temperature,3));
  }else{
    Debug.println(F("No DS18B20 found"));
  }
}

#define NUM_SAMPLES 20
#define MVRANGE 5563.2896
void readVoltage(){
  // dataStorage->voltage = ESP.getVcc() / 1000.0;
  uint32_t sum = 0;
  for(int i = 0; i < NUM_SAMPLES; i++){
    sum += analogRead(A0);
  }
  sum /= NUM_SAMPLES;
  Debug.printf("Analog Read Sum %d\n", sum);
  dataStorage->voltage = ((MVRANGE / 1023.0 ) * sum) / 1000;
  Debug.print(VOLTAGE);
  Debug.println(String(dataStorage->voltage,2));
}


void sendInflux(const String &body){
  if(configFile->influxOk() && body.length() > 0){
    HTTPClient http;
    http.setTimeout(10000);
    String url = configFile->getInfluxUrl();

    Debug.print(F("Sending to influx url: "));
    Debug.print(url);
    Debug.print(F(" body "));
    Debug.println(body);

    for(int i =0; i < 5; i++){
      http.begin(url);
      
      int result = http.POST(body);
      http.end();
      Debug.print(F("HTTP Result: "));
      String last_http_reponse_str = String(result) + String(" - ") + http.errorToString(result);
      Debug.println(last_http_reponse_str);
      if(result == 204){
        break;
      }
      delay(250);
    }
  }

}

void sendData(){
  if(configFile->influxOk()){
    String body(configFile->influx_line);
    body += String(" ");
    if(dataStorage->temperature < 1000.0){
      body += String(TEMPERATURE) + String("=") + String(dataStorage->temperature,3) + String(",");
    }

    body += String(VOLTAGE) + String("=") + String(dataStorage->voltage,3);
    sendInflux(body);
  }
}

void takeReading(){
  if(readingTaken){
    return;
  }
  Debug.println(F("Take Reading"));
  readTemperature();
  readVoltage();
  sendData();

  readingTaken = true;
}

void goToSleep(){
  ESP.deepSleep(std::chrono::microseconds(std::chrono::minutes(configFile->update_interval.toInt())).count());
}

void loop() {
  takeReading();

  if (httpHandler) {
    httpHandler->update();
    if(rebooted){
      if(millis() - httpHandler->getPokedMillis() > fiveMinutes.count()){
        Debug.println(F("Timeout from reboot, sleeping"));
        goToSleep();
      }
    }
  }

  if(wokeUp){
    Debug.println(F("Wokeup going back to sleep"));
    goToSleep();
  }

}