#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "Debug.h"

#include "openmili/PL1167_nRF24.h"
#include "openmili/MiLightRadio.h"
#include "ESPgwOpenmili.h"

#define CE_PIN   16 // D0
#define CSN_PIN  15 // D8


RF24 ESPgwOpenmili::radio(CE_PIN, CSN_PIN);
PL1167_nRF24 ESPgwOpenmili::prf(ESPgwOpenmili::radio);
MiLightRadio ESPgwOpenmili::mlr(ESPgwOpenmili::prf);
bool ESPgwOpenmili::receiving = false;
bool ESPgwOpenmili::printDups = false;

void ESPgwOpenmili::Setup()
{
	LOG("OpenMiLight Receiver/Transmitter starting\n");
	LOG("Free Heap: %ld\n", ESP.getFreeHeap());
	mlr.begin();
}

void ESPgwOpenmili::Loop()
{
	static int dupesPrinted = 0;
	if (ESPgwOpenmili::receiving) {
		if (mlr.available()) {
			uint8_t packet[7];
			size_t packet_length = sizeof(packet);
			mlr.read(packet, packet_length);

			LOG("\n2.4GHz recv: ");
			for (int i = 0; i < packet_length; i++) {
				LOG("%02X ", packet[i]);
			}
		}
		if (ESPgwOpenmili::printDups) {
			int dupesReceived = mlr.dupesReceived();
			for (; dupesPrinted < dupesReceived; dupesPrinted++) {
				LOG(".");
			}
		}
	}
}

void ESPgwOpenmili::Send(uint8_t *data, size_t len, int resend)
{
#ifdef __DEBUG__
	LOG("2.4GHz send: ");
	for (int i = 0; i < len; i++) {
		LOG("%02X ", data[i]);
	}
	LOG("%d time(s)\n", resend + 1);
#endif
	mlr.write(data, len);

	if (resend > 0) {
		for (int i = 0; i < resend; i++) {
			mlr.resend();
			delay(1);
		}
	}
}

void ESPgwOpenmili::Send(uint8_t data[8])
{
	static uint8_t seq = 1;

	uint8_t resends = data[7];
	if (data[6] == 0x00) {
		data[6] = seq;
		seq++;
	}

	ESPgwOpenmili::Send(data, 7, resends);
}

void ESPgwOpenmili::Send(uint8_t color, uint8_t bright, uint8_t key,
	uint8_t remote, uint8_t remote_prefix,
	uint8_t prefix, uint8_t seq, uint8_t resends)
{
	uint8_t data[8];
	data[0] = prefix;
	data[1] = remote_prefix;
	data[2] = remote;
	data[3] = color;
	data[4] = bright;
	data[5] = key;
	data[6] = seq;
	data[7] = resends;

	ESPgwOpenmili::Send(data);
}

