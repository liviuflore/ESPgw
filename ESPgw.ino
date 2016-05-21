#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <SPI.h>

#include "EEPROMconfig.hxx"

#define ErrorEeprom 0
#define ErrorWifi 1
#define ErrorUpgrade 2
int ErrorList[] = {0, 0, 0};
#define ERR(val, msg)        { ErrorList[val] = 1; Serial.printf(msg); }

#define ESPconfig_version 1
const char* ESPconfig_APssid = "ESPgw";
const char* ESPconfig_APpassword = "espgw";

struct ESPconfig {

    char ssid[32];
    char password[32];
};


EEPROMconfig<ESPconfig> cfg(ESPconfig_version);

void setup() {
    Serial.begin(74800);
    delay(500);
    
    if (!cfg.load()) {
        ERR(ErrorEeprom, "EEPROM config load failed, will load defaults\n");
		cfg.save();
//		while (true);
    }
	Serial.printf("EEPROM config load done\n");

    Serial.printf("loaded: %d\n", cfg.loaded);
    Serial.printf("ssid:   %s\n", cfg.data.ssid);
	Serial.printf("pass:   %s\n", cfg.data.password);

	if (cfg.data.ssid[0] == 0 || cfg.data.password[0] == 0) {
		Serial.printf("No wifi configuration found, starting in AP mode\n");
		WiFi.mode(WIFI_AP);
		WiFi.softAP(ESPconfig_APssid, ESPconfig_APpassword);
		Serial.printf("Please connect to:\n");
		Serial.printf("ssid:   %s\n", ESPconfig_APssid);
		Serial.printf("pass:   %s\n", ESPconfig_APpassword);
		Serial.printf("ip:     "); Serial.println(WiFi.softAPIP());
	}
}

void loop() {
  // put your main code here, to run repeatedly:

}

