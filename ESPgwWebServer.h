#ifndef _ESPGW_WEBSERVER_H_
#define _ESPGW_WEBSERVER_H_

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "ESPgw.h"

#define ESPGW_WEBSERVER_PORT	80


class ESPgwWebServer
{
public:
	static void Setup();
	static void Loop();

private:
	static ESP8266WebServer server;

private:
	/* handles */
	static bool handleFileRead(String path);
	static void handleNotFound();
	/* static content */
	static void handleHtmlHome();
	static void handleHtmlWifi();
	/* actions */
	static void handleAjaxWifi();
};

#endif /* _ESPGW_WEBSERVER_H_ */