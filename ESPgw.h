#ifndef _ESPGW_H_
#define _ESPGW_H_

#define ESPconfig_version 1
#define ESPconfig_string_max_size 32

struct ESPgw_config {

	char ssid[32];
	char password[32];
};

enum ESPgw_wifi_mode {
	ESPgw_wifi_mode_NONE,
	ESPgw_wifi_mode_AP,
	ESPgw_wifi_mode_STA
};

extern ESPgw_config cfg;
extern ESP8266WebServer server;

#endif /* _ESPGW_H_ */