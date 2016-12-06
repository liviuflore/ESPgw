#include <ESP8266WiFi.h>
#include "CommandParser.h"
#include "ESPgwCmdParser.h"
#include "Debug.h"


CommandParser ESPgwCmdParser::cmd;

void ESPgwCmdParser::Setup()
{
	//	cmd.addCommand("DR", cmd_parse_digital_read_command);
	//	cmd.addCommand("DW", cmd_parse_digital_write_command);
	//	cmd.addCommand("AR", cmd_parse_analog_read_command);
	//	cmd.addCommand("AW", cmd_parse_analog_write_command);
	//	cmd.addCommand("PM", cmd_parse_pin_mode_command);
	//	cmd.addCommand("RF", cmd_parse_rfcontrol_command);
	//	cmd.addCommand("DHT", cmdParse_dht_command);
	cmd.addCommand("ML", cmdOpenmiliSend);
	cmd.addCommand("PING", cmdPing);
	cmd.addCommand("RESET", cmdReset);

	cmd.setDefaultHandler(cmdUnrecognized);

	LOG("CMD parser setup done\n");
}

void ESPgwCmdParser::Loop()
{
	cmd.readStream(Serial);
}

void ESPgwCmdParser::cmdUnrecognized(const char *command) {
	LOG("ERR unknown_command [%s]\n", command);
}

void ESPgwCmdParser::cmdPing() {
	LOG("PONG\n");
}

void ESPgwCmdParser::cmdReset() {
	LOG("will reset ESP\n");
	ESP.restart();
}

void ESPgwCmdParser::cmdOpenmiliSend() {
	//LOG("ERR unknown_command\n");
	char* arg = NULL;
	while ((arg = cmd.next()) != NULL) {
		LOG("ML: 1 %s\n", arg);
	}
	// TODO: compose data[8], send ...
}

