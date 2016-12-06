#ifndef _ESPGW_CMDPARSER_H_
#define _ESPGW_CMDPARSER_H_

#include "CommandParser.h"


class ESPgwCmdParser {
public:
	static void Setup();
	static void Loop();

private:
	/* commands */
	static void cmdUnrecognized(const char *command);
	static void cmdPing();
	static void cmdReset();
	static void cmdOpenmiliSend();
private:
	static CommandParser cmd;
};

#endif /* _ESPGW_CMDPARSER_H_ */