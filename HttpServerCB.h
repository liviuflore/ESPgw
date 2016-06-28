#ifndef _HTTPSERVERCB_H_
#define _HTTPSERVERCB_H_

#include <ESP8266WebServer.h>
#include "ESPgw.h"

String sep = "____";

bool handle_file_read(String path)
{
	// red the file from SPIFS fs and send it through http socket
	return false;
}

void handle_wifim_html()
{
	server.send(200, "text/html", "<form method='POST' action='/wifi_ajax'><input type='hidden' name='form' value='wifi'><input type='text' name='ssid'><input type='password' name='password'><input type='submit' value='Submit'></form><br<b>Enter WiFi credentials</b>");
}
void handle_wifi_ajax()
{
	String form = server.arg("form");
	if (form != "wifi")
	{
		server.send(200, "text/html", cfg.ssid + sep + cfg.password);
	}
	if (form == "wifi")
	{
		server.arg("ssid").toCharArray(cfg.ssid, 32);
		server.arg("password").toCharArray(cfg.password, 32);
		EEPROMconfig::save(cfg);

		server.send(200, "text/html", "OK");
		delay(500);
		ESP.restart();
	}
}


#endif /* _HTTPSERVERCB_H_ */