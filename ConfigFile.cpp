#include "ConfigFile.h"
#include "StringConstants.h"

#ifdef TEST
#define CONFIG_FILE "/config-test.json"
#else
#define CONFIG_FILE "/config.json"
#endif

ConfigFile::ConfigFile(): JSONFileBase(CONFIG_FILE){
  if(!readFile()){
    Serial.printf(String(FAILED_TO_READ).c_str(), fileName.c_str());
  }
}

void ConfigFile::getJson(JsonObject & root){

  root[WIFI_AP           ]      = wifi_ap          ;
  root[WIFI_PASSWORD     ]      = wifi_password    ;
  root[HOSTNAME          ]      = hostname         ;
  root[IPADDRESS         ]      = ipaddress        ;
  root[DNSSERVER         ]      = dnsserver        ;
  root[SUBNET            ]      = subnet           ;
  root[GATEWAY           ]      = gateway          ;
  root[INFLUX_HOST       ]      = influx_host      ;
  root[INFLUX_PORT       ]      = influx_port      ;
  root[INFLUX_USERNAME   ]      = influx_username  ;
  root[INFLUX_PASSWORD   ]      = influx_password  ;
  root[INFLUX_DATABASE   ]      = influx_database  ;
  root[INFLUX_LINE_TEMPERATURE] = influx_line_temperature;
  root[INFLUX_LINE_VOLTAGE]     = influx_line_voltage;
  root[UPDATE_INTERVAL   ]      = update_interval;
}

void ConfigFile::setJson(const JsonObject &json){
  wifi_ap                 = json[WIFI_AP           ].as<String>();
  wifi_password           = json[WIFI_PASSWORD     ].as<String>();
  hostname                = json[HOSTNAME          ].as<String>();
  ipaddress               = json[IPADDRESS         ].as<String>();
  subnet                  = json[SUBNET            ].as<String>();
  dnsserver               = json[DNSSERVER         ].as<String>();
  gateway                 = json[GATEWAY           ].as<String>();
  influx_host             = json[INFLUX_HOST       ].as<String>();
  influx_port             = json[INFLUX_PORT       ].as<String>();
  influx_username         = json[INFLUX_USERNAME   ].as<String>();
  influx_password         = json[INFLUX_PASSWORD   ].as<String>();
  influx_database         = json[INFLUX_DATABASE   ].as<String>();
  influx_line_temperature = json[INFLUX_LINE_TEMPERATURE].as<String>();
  influx_line_voltage     = json[INFLUX_LINE_VOLTAGE].as<String>();
  update_interval         = json[UPDATE_INTERVAL   ].as<String>();


}

String ConfigFile::getInfluxUrl(){
  char buffer[200];
  snprintf(buffer, 200, String(INFLUX_URL).c_str(),
    influx_host.c_str(), influx_port.c_str(), influx_database.c_str(),
    influx_username.c_str(), influx_password.c_str());
  String r = String(buffer);
  if(influx_username.length() > 0 && influx_password.length() > 0){
    r += "&u=" + influx_username + "&p=" + influx_password;
  }
  return r;
}

bool ConfigFile::influxOk(){
  return influx_host.length() > 0 && influx_database.length() > 0
    && influx_port.length() > 0;
}