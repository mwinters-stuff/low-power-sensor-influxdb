#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <RemoteDebug.h>
#include <WString.h>

#include "ConfigFile.h"
#include "HTTPHandler.h"
#include "HardwareConfig.h"
#include "StringConstants.h"

extern RemoteDebug Debug;

HTTPHandler::HTTPHandler(ConfigFile* configFile, DataStorage* dataStorage)
    : httpServer(80), configFile(configFile), dataStorage(dataStorage), dnsServer(NULL), pokedmillis(millis()) {}

void HTTPHandler::update() {
  httpServer.handleClient();
  ArduinoOTA.handle();
  if (dnsServer) {
    dnsServer->processNextRequest();
  }
}

void HTTPHandler::handleAll() {
  pokedmillis = millis();
  Debug.println(F("Handling ALL"));
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root[HEAP] = ESP.getFreeHeap();
  root[TEMPERATURE] = dataStorage->temperature;
  root[VOLTAGE] = dataStorage->voltage;

  root.prettyPrintTo(Debug);
  Debug.println();
  String response;
  root.printTo(response);

  httpServer.sendHeader(ACCESS_CONTROL_HEADER, "*");
  httpServer.send(200, F("application/json"), response);
}

void HTTPHandler::handleConfig() {
  pokedmillis = millis();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  Debug.println(F("Get Config"));

  configFile->getJson(root);

  String response;
  root.prettyPrintTo(response);
  Debug.println(response);
  Debug.println();

  httpServer.sendHeader(ACCESS_CONTROL_HEADER, "*");
  httpServer.send(200, F("application/json"), response);
}

void HTTPHandler::handleConfigSave() {
  pokedmillis = millis();
  if (httpServer.hasArg(F("plain"))) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parse(httpServer.arg(F("plain")));

    Debug.println(F("Save Config"));

    root.prettyPrintTo(Debug);
    Debug.println();

    configFile->setJson(root);
    configFile->saveFile();

    httpServer.sendHeader(ACCESS_CONTROL_HEADER, "*");
    httpServer.send(200, TEXT_PLAIN, OKRESULT);
  } else {
    httpServer.sendHeader(ACCESS_CONTROL_HEADER, "*");
    httpServer.send(500, TEXT_PLAIN, INVALID_ARGUMENTS);
  }
}

void HTTPHandler::handleNotFound() {
  Debug.print(F("handleNotFound uri "));
  Debug.println(httpServer.uri());
  pokedmillis = millis();
  if (!handleFileRead(httpServer.uri())) {
    httpServer.send(404, TEXT_PLAIN, F("Not found"));
  }
}

void HTTPHandler::setupBaseHandlers() {
  httpServer.on(F("/all"), HTTP_GET, std::bind(&HTTPHandler::handleAll, this));
  httpServer.on(F("/config"), HTTP_GET, std::bind(&HTTPHandler::handleConfig, this));
  httpServer.on(F("/reboot"), HTTP_GET, std::bind(&HTTPHandler::handleReboot, this));
  httpServer.on(String(F("/save-config")).c_str(), HTTP_POST, std::bind(&HTTPHandler::handleConfigSave, this));
  httpServer.onNotFound(std::bind(&HTTPHandler::handleNotFound, this));
}

void HTTPHandler::setupServer() {
  SPIFFS.begin();

  setupBaseHandlers();

  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
}

void HTTPHandler::setupPortalServer() {
  dnsServer = new DNSServer();
  delay(500);  // Without delay I've seen the IP address blank
  Debug.print(F("AP IP address: "));
  Debug.println(WiFi.softAPIP());

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());

  httpServer.on(F("/generate_204"), std::bind(&HTTPHandler::handleRedirect, this));
  httpServer.on(F("/gen_204"), std::bind(&HTTPHandler::handleRedirect, this));
  SPIFFS.begin();

  setupBaseHandlers();

  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
}

String HTTPHandler::getContentType(String filename) {
  if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

bool HTTPHandler::handleFileRead(String path) {  // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);

  if (path.endsWith("/")) {
    path += "index.html";  // If a folder is requested, send the index file
  }
  String contentType = getContentType(path);  // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {  // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz)) {                       // If there's a compressed version available
      path += ".gz";                                       // Use the compressed version
    }
    File file = SPIFFS.open(path, "r");                      // Open the file
    size_t sent = httpServer.streamFile(file, contentType);  // Send it to the client
    file.close();                                            // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false;  // If the file doesn't exist, return false
}

void HTTPHandler::setupOTA() {
  // setup arduino ArduinoOTA
  ArduinoOTA.setPassword(ARDUINO_OTA_PASSWORD);
  ArduinoOTA.onStart([]() {
    String type;
    Debug.print(F("Start updating "));
    if (ArduinoOTA.getCommand() == U_FLASH)
      Debug.println(F("sketch"));
    else  // U_SPIFFS
      Debug.println(F("filesystem"));

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    SPIFFS.end();
  });
  ArduinoOTA.onEnd([]() {
    SPIFFS.begin();
    Debug.println(F("\nEnd"));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { Debug.printf_P(PSTR("Progress: %u%%\r"), (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error) {
    String error_s;
    switch (error) {
      case OTA_AUTH_ERROR:
        error_s = F("Auth");
        break;
      case OTA_BEGIN_ERROR:
        error_s = F("Begin");
        break;
      case OTA_CONNECT_ERROR:
        error_s = F("Connect");
        break;
      case OTA_RECEIVE_ERROR:
        error_s = F("Receive");
        break;
      case OTA_END_ERROR:
        error_s = F("End");
        break;
    }
    Debug.printf(String(ERROR_FAILED).c_str(), error, error_s.c_str());
  });
  ArduinoOTA.begin();
}

void HTTPHandler::handleRedirect() {
  String redirect = String(F("http://")) + WiFi.softAPIP().toString() + String("/");
  Debug.print(F("handleRedirect "));
  Debug.println(redirect);
  httpServer.sendHeader(F("Location"), redirect);
  httpServer.send(302, TEXT_PLAIN, INVALID_ARGUMENTS);
  httpServer.client().stop();
}

void HTTPHandler::handleReboot() {
  httpServer.send(200, TEXT_PLAIN, F("Rebooting soon"));

  Debug.println(F("Rebooting"));

  delay(5000);
  ESP.reset();
  delay(2000);
}
