#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <SPI.h>
#include <FS.h>
#include <Stream.h>

#include "ESPgw.h"
#include "EEPROMconfig.h"
#include "RFControl.h"
#include "CommandParser.h"

enum {
	ErrorEeprom = 0,
	ErrorSPIFFS,
	ErrorWifi,
	ErrorUpgrade,
	ErrorCOUNT
};
int ErrorList[ErrorCOUNT] = { 0 };

#define ERR(val, ...)       { ErrorList[val] = 1; Serial.printf(__VA_ARGS__); }
#define LOG(...)			Serial.printf(__VA_ARGS__)

const char* ESPconfig_APssid = "ESPgw";
const char* ESPconfig_APpassword = "espgw";

#include "rfControl_fn.h"
//#include "rf24_fn.h"
#include "HttpServerCB.h"

#include "cmdParse_generic.h"


ESPgw_config cfg = { 0 };
FSInfo fs_info = { 0 };
ESP8266WebServer  server(80);
CommandParser cmd;
#define RF433_RX_PIN 1
#define RF433_TX_PIN 2


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
			LOG("Couldn't connect to network, starting in AP mode\n");
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

void setup_cmd_parser()
{
//	cmd.addCommand("DR", cmd_parse_digital_read_command);
//	cmd.addCommand("DW", cmd_parse_digital_write_command);
//	cmd.addCommand("AR", cmd_parse_analog_read_command);
//	cmd.addCommand("AW", cmd_parse_analog_write_command);
//	cmd.addCommand("PM", cmd_parse_pin_mode_command);
//	cmd.addCommand("RF", cmd_parse_rfcontrol_command);
//	cmd.addCommand("DHT", cmdParse_dht_command);
	cmd.addCommand("PING", cmdParse_ping_command);
	cmd.addCommand("RESET", cmdParse_reset_command);

	cmd.setDefaultHandler(cmdParse_unrecognized);

	LOG("CMD parser setup done\n");
}

void loop_cmd_parser()
{
	cmd.readStream(Serial);
}

void setup_http_server()
{
	if (WiFi.getMode() != WIFI_STA) {
		server.on("/", handle_wifim_html);
	}
	server.on("/wifi_ajax", handle_wifi_ajax);

	server.onNotFound([]() {
		if (!handle_file_read(server.uri()))
			server.send(404, "text/plain", "FileNotFound");
	});

	server.begin();
	LOG("HTTP server setup done\n");
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
	setup_cmd_parser();
	setup_http_server();
	//delay(1000);
	//setup_rfcontrol(RF433_RX_PIN, RF433_TX_PIN);
	LOG("SETUP done\n");
}

void loop() {
	loop_cmd_parser();
	loop_http_server();
	//loop_rfcontrol();
}

