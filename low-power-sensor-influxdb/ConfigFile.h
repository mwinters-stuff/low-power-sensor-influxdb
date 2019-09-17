#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include "JSONFileBase.h"

class ConfigFile: public JSONFileBase{
  public:
    String wifi_ap;
    String wifi_password;
    String hostname;
    String ipaddress;
    String dnsserver;
    String subnet;
    String gateway;
    String influx_database;
    String influx_host;
    String influx_username;
    String influx_password;
    String influx_port;
    String update_interval;
    String influx_line;

    ConfigFile();

    void getJson(JsonObject & root);
    void setJson(const JsonObject &object);
    String getInfluxUrl();
    bool influxOk();

};


#endif
