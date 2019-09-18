#include "JSONFileBase.h"
#include <FS.h>

JSONFileBase::JSONFileBase(const String& fileName):
      fileName(fileName){
}


bool JSONFileBase::readFile()
{
  bool result = false;
  if (SPIFFS.begin())
  {
    Serial.println(F("mounted file system"));
    if (SPIFFS.exists(fileName))
    {
      //file exists, reading and loading
      Serial.print(F("reading file "));
      Serial.println(fileName);
      File configFile = SPIFFS.open(fileName, "r");
      if (configFile)
      {
        DynamicJsonDocument doc(1024);
        auto error = deserializeJson(doc, configFile);

        if (!error)
        {
          serializeJsonPretty(doc, Serial);
          Serial.println();
          setJson(doc);
          Serial.println(F("Set"));
          result = true;
        }
        else
        {
          Serial.println(F("failed to load json "));
          Serial.println(error.c_str());
        }
      }
      configFile.close();
    }
  }else{
    Serial.println(F("SPIFFS Failed"));
  }
  return result;
}

void JSONFileBase::saveFile(){
  Serial.print(F("saving file "));
  Serial.println(fileName);
  DynamicJsonDocument doc(1024);

  getJson(doc);

  File configFile = SPIFFS.open(fileName, "w");
  if (!configFile) {
    Serial.println(F("failed to open config file for writing"));
    return;
  }

  serializeJson(doc, configFile);
  
  configFile.close();
}
