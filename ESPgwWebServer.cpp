
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "ESPgw.h"
#include "ESPgwWebServer.h"

String sep = "____";

ESP8266WebServer ESPgwWebServer::server(ESPGW_WEBSERVER_PORT);

void ESPgwWebServer::Setup()
{
	if (WiFi.getMode() != WIFI_STA) {
		server.on("/", handleHtmlWifi);
	}
	else {
		server.on("/", handleHtmlHome);
	}
	/* TODO: add other wifi handlers */
	server.on("/wifi_ajax", handleAjaxWifi);

	server.onNotFound(handleNotFound);

	server.begin();
	LOG("HTTP server setup done\n");
}

void ESPgwWebServer::Loop()
{
	server.handleClient();
}

/* handlers */
bool ESPgwWebServer::handleFileRead(String path)
{
	return false;
}

void ESPgwWebServer::handleNotFound()
{
	if (!handleFileRead(server.uri()))
		server.send(404, "text/plain", "FileNotFound");
}

void ESPgwWebServer::handleHtmlHome() 
{ 
	// TODO: add auto redirect: <meta http-equiv="refresh" content="0; url=/index.html"/>
	server.send(200, "text/html", "<h1>ESP GW - WeMos mini</h1><br/><p><a href=\"/index.html\">Redirect</a></p>");
}

void ESPgwWebServer::handleHtmlWifi()
{
	server.send(200, "text/html", "<form method='POST' action='/wifi_ajax'><input type='hidden' name='form' value='wifi'><input type='text' name='ssid'><input type='password' name='password'><input type='submit' value='Submit'></form><br<b>Enter WiFi credentials</b>");
}

void ESPgwWebServer::handleAjaxWifi() 
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
		EEPROMconfig::Save(cfg);

		server.send(200, "text/html", "OK");
		delay(500);
		ESP.restart();
	}
}
