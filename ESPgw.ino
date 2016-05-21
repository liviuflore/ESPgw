#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <SPI.h>

#include "EEPROMconfig.hxx"

#define ErrorEeprom 0
#define ErrorWifi 1
#define ErrorUpgrade 2
int ErrorList[] = {0, 0, 0};
#define ERR(val, ...)       { ErrorList[val] = 1; Serial.printf(__VA_ARGS__); }
#define LOG(...)			Serial.printf(__VA_ARGS__)

#define ESPconfig_version 1
const char* ESPconfig_APssid = "ESPgw";
const char* ESPconfig_APpassword = "espgw";

struct ESPconfig {

    char ssid[32];
    char password[32];
};


EEPROMconfig<ESPconfig> cfg(ESPconfig_version);

void setup_wifi()
{
	LOG("ssid:   %s\n", cfg.data.ssid);
	LOG("pass:   %s\n", cfg.data.password);
	if (cfg.data.ssid[0] == 0 || cfg.data.password[0] == 0) {
		LOG("No wifi configuration found, starting in AP mode\n");
		WiFi.mode(WIFI_AP);
		WiFi.softAP(ESPconfig_APssid, ESPconfig_APpassword);
		LOG("Please connect to:\n");
		LOG("ssid:   %s\n", ESPconfig_APssid);
		LOG("pass:   %s\n", ESPconfig_APpassword);
		LOG("ip:     "); Serial.println(WiFi.softAPIP());
	}
	else {
		int retry_count = 30;
		LOG("Connecting to: %s\n", cfg.data.ssid);
		WiFi.mode(WIFI_STA);
		WiFi.begin(cfg.data.ssid, cfg.data.password);
		while (WiFi.status() != WL_CONNECTED && retry_count-- > 0) {
			delay(1000);
			LOG(".");
		}
		if (WiFi.status() != WL_CONNECTED) { 
			WiFi.disconnect();
			delay(1000);
			LOG("Couldn't connect to network, starting in AP mode\n");
			WiFi.mode(WIFI_AP);
			WiFi.softAP(ESPconfig_APssid, ESPconfig_APpassword);
			LOG("Please connect to:\n");
			LOG("ssid:   %s\n", ESPconfig_APssid);
			LOG("pass:   %s\n", ESPconfig_APpassword);
			LOG("ip:     "); Serial.println(WiFi.softAPIP());
		}
		else {
			LOG("Connected to %s\n", cfg.data.ssid);
			LOG("local ip: "); Serial.println(WiFi.localIP());
		}

	}
}

void setup()
{
    Serial.begin(74800);
    delay(500);
    
    if (!cfg.load()) {
        ERR(ErrorEeprom, "EEPROM config load failed, will load defaults\n");
//		cfg.save();
    }
	Serial.printf("EEPROM config load done\n");
	
	setup_wifi();
}

void loop() {
  // put your main code here, to run repeatedly:

}

