#include "StringConstants.h"
#include <Arduino.h>

#include <WString.h>

const char _OKRESULT[] PROGMEM              = "OK";
const char _ACCESS_CONTROL_HEADER[] PROGMEM = "Access-Control-Allow-Origin";
const char _TEXT_PLAIN[] PROGMEM            = "text/plain";
const char _INVALID_ARGUMENTS[] PROGMEM     = "Invalid Arguments";
const char _TEMPERATURE[] PROGMEM           = "temperature";
const char _VOLTAGE[] PROGMEM               = "voltage";
const char _LAST_HTTP_RESPONSE[] PROGMEM    = "last_http_response";
const char _HEAP[] PROGMEM                  = "heap";

const char _WIFI_AP[] PROGMEM              = "wifi_ap";
const char _WIFI_PASSWORD[] PROGMEM        = "wifi_password";
const char _HOSTNAME[] PROGMEM             = "hostname";
const char _IPADDRESS[] PROGMEM            = "ipaddress";
const char _DNSSERVER[] PROGMEM            = "dnsserver";
const char _SUBNET[] PROGMEM               = "subnet";
const char _GATEWAY[] PROGMEM               = "gateway";

const char _UPDATE_INTERVAL[] PROGMEM      = "update_interval";
const char _INFLUX_HOST[] PROGMEM          = "influx_host";
const char _INFLUX_PORT[] PROGMEM          = "influx_port";
const char _INFLUX_USERNAME[] PROGMEM      = "influx_username";
const char _INFLUX_PASSWORD[] PROGMEM      = "influx_password";
const char _INFLUX_DATABASE[] PROGMEM      = "influx_database";

const char _INFLUX_LINE_TEMPERATURE[] PROGMEM   = "influx_line_temperature";
const char _INFLUX_LINE_VOLTAGE[]     PROGMEM   = "influx_line_voltage";


const char _YES[] PROGMEM    = "YES";
const char _NO[] PROGMEM     = "NO";

const char _ERROR_FAILED[] PROGMEM = "Error[%u]: %s Failed\n";

const char _FAILED_TO_READ[] PROGMEM              = "Failed to read %s\n";
const char _INFLUX_URL[] PROGMEM = "http://%s:%s/write?db=%s";