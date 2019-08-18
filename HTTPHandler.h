#ifndef _HTTPHANDLER_H
#define _HTTPHANDLER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include "ConfigFile.h"
#include "DataStorage.h"

class HTTPHandler{
  public:
    HTTPHandler(ConfigFile *configFile, DataStorage *dataStorage);
    void setupServer();
    void setupOTA();
    void update();
    void setupPortalServer();
    uint32_t getPokedMillis(){
      return pokedmillis;
    };
  private:
    // static HTTPHandler* m_instance;
    uint32_t pokedmillis;
    ESP8266WebServer httpServer;
    ConfigFile *configFile;
    DataStorage *dataStorage;
    DNSServer *dnsServer;
    bool otaBegun;
    // DNS server
    const byte    DNS_PORT = 53;

    String getContentType(String filename);
    bool handleFileRead(String path);

    String formatUnknownAction(const String &what, const String &action);

    void setupBaseHandlers();
    void handleAll();
    void handleConfig();
    void handleConfigSave();
    void handleNotFound();
    void handleRedirect();
    void handleReboot();
};



#endif
