#ifndef __cmdParse_generic_h__
#define __cmdParse_generic_h__

void cmdParse_unrecognized(const char *command) {
	Serial.print("ERR unknown_command\r\n");
}

void cmdParse_ping_command() {
	Serial.print("PING");
	Serial.print("\r\n");
}

void cmdParse_reset_command() {
	Serial.print("will reset ESP\r\n");
	ESP.restart();
}

#endif /* __cmdParse_generic_h__ */