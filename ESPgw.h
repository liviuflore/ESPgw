#ifndef _ESPGW_H_
#define _ESPGW_H_

#include "EEPROMconfig.h"

#define ESPconfig_version 1
#define ESPconfig_string_max_size 32

struct ESPgw_config {

	char ssid[ESPconfig_string_max_size];
	char password[ESPconfig_string_max_size];
};




extern ESPgw_config			cfg;
//extern ESP8266WebServer		server;

class ESPgw {
	static void Publish(unsigned int id, int type, unsigned int val) {};
	//static void Advertise(unsigned int id, int type, unsigned int val) {};
};

#endif /* _ESPGW_H_ */