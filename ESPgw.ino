#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <SPI.h>
#include <FS.h>
#include <Stream.h>

#include "ESPgw.h"
#include "RFControl.h"
#include "CommandParser.h"

#include "EEPROMconfig.h"

#include "ESPgwRF433.h"
#include "ESPgwOpenmili.h"
//#include "rf24_fn.h"
#include "ESPgwWebServer.h"
#include "ESPgwCmdParser.h"

#include "Debug.h"


int ErrorList[ErrorCOUNT] = { 0 };

/* default values */
const char* ESPconfig_APssid = "ESPgw";
const char* ESPconfig_APpassword = "espgw";

ESPgw_config cfg = { 0 };
FSInfo fs_info = { 0 };



void setup_config()
{
	if (!EEPROMconfig::Load(cfg)) {
		ERR(ErrorEeprom, "EEPROM config load failed, will load defaults\n");
		cfg = { 0 };
	}
	LOG("EEPROM config setup done\n");
}

void setup_SPIFFS()
{
	// Check if SPIFFS is OK
	if (!SPIFFS.begin()) {
		LOG("SPIFFS failed, needs formatting\n");
		if (SPIFFS.format()) {
			if (!SPIFFS.begin()) {
				ERR(ErrorSPIFFS, "Format SPIFFS failed\n");
			}
		}
		else {
			ERR(ErrorSPIFFS, "Format SPIFFS failed\n");
		}
		LOG("SPIFFS formatted\n");
		delay(500);
		ESP.restart();
	}
	else {
		LOG("SPIFFS mounted\n");
		if (!SPIFFS.info(fs_info)) {
			ERR(ErrorSPIFFS, "fs_info failed\n");
		}
	}
	LOG("SPIFFS setup done\n");
}

void setup_wifi()
{
	LOG("ssid:   %s\n", cfg.ssid);
	LOG("pass:   %s\n", cfg.password);
	if (cfg.ssid[0] == 0 || cfg.password[0] == 0) {
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
		LOG("Connecting to: %s\n", cfg.ssid);
		WiFi.mode(WIFI_STA);
		WiFi.begin(cfg.ssid, cfg.password);
		while (WiFi.status() != WL_CONNECTED && retry_count-- > 0) {
			delay(1000);
			LOG(".");
		}
		if (WiFi.status() != WL_CONNECTED) {
			WiFi.disconnect();
			delay(1000);
			LOG("\nCouldn't connect to network, starting in AP mode\n");
			WiFi.mode(WIFI_AP);
			WiFi.softAP(ESPconfig_APssid, ESPconfig_APpassword);
			LOG("Please connect to:\n");
			LOG("ssid:   %s\n", ESPconfig_APssid);
			LOG("pass:   %s\n", ESPconfig_APpassword);
			LOG("ip:     "); Serial.println(WiFi.softAPIP());
		}
		else {
			LOG("Connected to %s\n", cfg.ssid);
			LOG("local ip: "); Serial.println(WiFi.localIP());
		}
	}
	LOG("WiFi setup done\n");
}


void setup()
{
    Serial.begin(74800);
    delay(500);
    
	setup_config();
	setup_SPIFFS();
	setup_wifi();

	/* setup services */
	ESPgwCmdParser::Setup();
	ESPgwWebServer::Setup();
	delay(500);

	/* setup hardware */
	ESPgwRF433::Setup(false);
	ESPgwOpenmili::Setup();
	ESPgwOpenmili::EnableReceive(true, true);

	/* setup sensors & actuators */

	LOG("SETUP done\n");
}

void loop() {
	ESPgwCmdParser::Loop();
	ESPgwWebServer::Loop();
	//ESPgwRF433::Loop();
	ESPgwOpenmili::Loop();
}

