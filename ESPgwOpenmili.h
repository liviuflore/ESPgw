#ifndef _ESPGW_OPENMILI_H_
#define _ESPGW_OPENMILI_H_

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "openmili/PL1167_nRF24.h"
#include "openmili/MiLightRadio.h"

#define CE_PIN 8
#define CSN_PIN 53


class ESPgwOpenmili {
private:
	static RF24 radio;
	static PL1167_nRF24 prf;
	static MiLightRadio mlr;
	static bool receiving;
	static bool printDups;

public:
	static void Setup();
	static void Loop();
	//static void LoopSerial();
	static void Send(uint8_t *packet, size_t len, int resend = 0);
	static void Send(uint8_t data[8]);
	static void Send(uint8_t color, uint8_t bright, uint8_t key,
		uint8_t remote = 0x01, uint8_t remote_prefix = 0x00,
		uint8_t prefix = 0xB8, uint8_t seq = 0x00, uint8_t resends = 10);
	static void EnableReceive(bool enable, bool printDuplicates = false) { 
		receiving = enable; printDups = printDuplicates; 
	}
};



#endif /* _ESPGW_OPENMILI_H_ */