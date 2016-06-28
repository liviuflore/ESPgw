#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <SPI.h>
#include <FS.h>

#include "ESPgw.h"
#include "EEPROMconfig.h"
#include "HttpServerCB.h"

#define ErrorEeprom 0
#define ErrorWifi 1
#define ErrorUpgrade 2
int ErrorList[] = {0, 0, 0};
#define ERR(val, ...)       { ErrorList[val] = 1; Serial.printf(__VA_ARGS__); }

#define LOG(...)			Serial.printf(__VA_ARGS__)

const char* ESPconfig_APssid = "ESPgw";
const char* ESPconfig_APpassword = "espgw";


ESPgw_config cfg = { 0 };
FSInfo fs_info = { 0 };
int wifi_mode = ESPgw_wifi_mode_NONE;
ESP8266WebServer  server(80);

void setup_config()
{
	if (!EEPROMconfig::load(cfg)) {
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
				LOG("Format SPIFFS failed\n");
			}
		}
		else {
			LOG("Format SPIFFS failed\n");
		}
		delay(500);
		ESP.restart();
	}
	else {
		LOG("SPIFFS mounted\n");
		if (!SPIFFS.info(fs_info)) {
			LOG("fs_info failed\n");
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
		wifi_mode = ESPgw_wifi_mode_AP;
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
			LOG("Couldn't connect to network, starting in AP mode\n");
			WiFi.mode(WIFI_AP);
			WiFi.softAP(ESPconfig_APssid, ESPconfig_APpassword);
			LOG("Please connect to:\n");
			LOG("ssid:   %s\n", ESPconfig_APssid);
			LOG("pass:   %s\n", ESPconfig_APpassword);
			LOG("ip:     "); Serial.println(WiFi.softAPIP());
		}
		else {
			wifi_mode = ESPgw_wifi_mode_STA;
			LOG("Connected to %s\n", cfg.ssid);
			LOG("local ip: "); Serial.println(WiFi.localIP());
		}
	}
	LOG("WiFi setup done\n");
}

void setup_http_server()
{
	if (wifi_mode != ESPgw_wifi_mode_STA) {
		server.on("/", handle_wifim_html);
	}
	server.on("/wifi_ajax", handle_wifi_ajax);

	server.onNotFound([]() {
		if (!handle_file_read(server.uri()))
			server.send(404, "text/plain", "FileNotFound");
	});

	server.begin();
}

void loop_http_server()
{
	server.handleClient();
}

void setup()
{
    Serial.begin(74800);
    delay(500);
    
	setup_config();
	setup_SPIFFS();
	setup_wifi();
	setup_http_server();
}

void loop() {
	// put your main code here, to run repeatedly:
	loop_http_server();
}

