#ifndef _DEBUG_H_
#define _DEBUG_H_

#define __DEBUG__

enum {
	ErrorEeprom = 0,
	ErrorSPIFFS,
	ErrorWifi,
	ErrorUpgrade,
	ErrorCOUNT
};
extern int ErrorList[ErrorCOUNT];


#define ERR(val, ...)       { ErrorList[val] = 1; Serial.printf(__VA_ARGS__); }
#define LOG(...)			Serial.printf(__VA_ARGS__)
#ifdef __DEBUG__
#define DBG(...)			Serial.printf(__VA_ARGS__)
#else
#define DBG(...)
#endif

class FTraceC {
private:
	char* f;
public:
	FTraceC(const char* function) { f = (char*)function; LOG("--> %s\n", f); }
	~FTraceC() { LOG("<-- %s\n", f); }
};

#define FTRACE() FTraceC ft(__FUNCTION__)
#define FINTRO() LOG("--> %s:%d\n", __FUNCTION__, __LINE__)
#define FEXIT() LOG("<-- %s:%d\n", __FUNCTION__, __LINE__)

#endif /* _DEBUG_H_ */
