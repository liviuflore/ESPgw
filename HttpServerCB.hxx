#ifndef _HTTPSERVERCB_H_
#define _HTTPSERVERCB_H_

#include <ESP8266WebServer.h>
#include "ESPgw.h"


void handle_wifim_html()
{
	server.send(200, "text/html", "<form method='POST' action='/wifi_ajax'><input type='hidden' name='form' value='wifi'><input type='text' name='ssid'><input type='password' name='password'><input type='submit' value='Submit'></form><br<b>Enter WiFi credentials</b>");
}
void handle_wifi_ajax()
{
	//String form = server.arg("form");
	//if (form != "wifi")
	//{
	//	String ssidStored = HandleEeprom(ssid_Address, "read");
	//	String passStored = HandleEeprom(password_Address, "read");

	//	// Glue everything together and send to client
	//	server.send(200, "text/html", ssidStored + sep + passStored);
	//}
	//if (form == "wifi")
	//{
	//	String ssidArg = server.arg("ssid");
	//	String passArg = server.arg("password");

	//	HandleEeprom(ssid_Address, "write", ssidArg);
	//	HandleEeprom(password_Address, "write", passArg);
	//	server.send(200, "text/html", "OK");
	//	delay(500);
	//	ESP.restart();
	//}
}


#endif /* _HTTPSERVERCB_H_ */