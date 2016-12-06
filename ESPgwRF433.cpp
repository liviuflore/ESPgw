#include <ESP8266WiFi.h>

#include "RFControl.h"
#include "ESPgwRF433.h"
#include "Debug.h"

#define RF433_RX_PIN 5  // D1
#define RF433_TX_PIN 4  // D2

bool ESPgwRF433::rfc_raw_mode = false;
int ESPgwRF433::rfc_interrupt_pin = -1;
int ESPgwRF433::rfc_transmit_pin = -1;

bool ESPgwRF433::getBytes(unsigned int *timings, unsigned int len, unsigned char *outBytes)
{
	int bit = -1;
	for (int i = 0; i < len; i+=2) {
		if (i % 16 == 0) {
			bit++;
			outBytes[bit] = 0;
		}
		if (timings[i] != 0) {
			return false;
		}
		outBytes[bit] = (outBytes[bit] << 1) + (timings[i + 1] - 1);
	}
	outBytes[bit] = (outBytes[bit] << (len % 16) / 2);
	return true;
}

void ESPgwRF433::Setup(bool raw_mode)
{
	rfc_interrupt_pin = RF433_RX_PIN;
	rfc_transmit_pin = RF433_TX_PIN;
	rfc_raw_mode = raw_mode;
	RFControl::startReceiving(rfc_interrupt_pin);
	LOG("RFControl setup done\n");
}

void ESPgwRF433::Loop()
{
	if (rfc_raw_mode) {
		if (RFControl::existNewDuration()) {
			Serial.print(RFControl::getLastDuration() * RFControl::getPulseLengthDivider());
			Serial.print(", ");
			static byte line = 0;
			line++;
			if (line >= 16) {
				line = 0;
				Serial.write('\n');
			}
		}
	}
	else {
		if (RFControl::hasData()) {
			unsigned int *timings;
			unsigned int timings_size;
			RFControl::getRaw(&timings, &timings_size);
			unsigned int buckets[8];
			unsigned int pulse_length_divider = RFControl::getPulseLengthDivider();
			RFControl::compressTimings(buckets, timings, timings_size);
			if (timings_size >= 72) {
				unsigned char bytes[5];
				if (getBytes(timings, 72, bytes)) {
					unsigned long ts = micros() / 1000;
					// AOK-2026 temperature sensor data parsing
					unsigned int id = ((unsigned int)bytes[0] << 4) + ((unsigned int)(bytes[1] >> 4) & 0x0F);
					unsigned int temp = ((unsigned int)(bytes[1] & 0x0F) << 8) + bytes[2];
					unsigned int hum = ((unsigned int)(bytes[3] & 0x0F) << 4) + ((unsigned int)(bytes[4] >> 4) & 0x0F);
					Serial.printf("[%ld] %02lX %02lX %02lX %02lX %02lX\n", ts, bytes[0], bytes[1], bytes[2], bytes[3], bytes[4]);
					Serial.printf("[%ld] ID:%04lX temp:%lu hum:%ld\n", ts, id, temp, hum);
					//ESPgw::publish(id, S_TEMP, temp);
					//ESPgw::publish(id, S_HUM, hum);
				}
			}
			RFControl::continueReceiving();
		}
	}
}

void ESPgwRF433::Close()
{
	RFControl::stopReceiving();
}

void ESPgwRF433::SetRXMode(bool mode)
{
	rfc_raw_mode = mode ? true : false;
}

void ESPgwRF433::Send(unsigned long *buckets, char *seq, unsigned int repeats)
{
	RFControl::sendByCompressedTimings(rfc_transmit_pin, buckets, seq, repeats);
}

// TODO: add send function
