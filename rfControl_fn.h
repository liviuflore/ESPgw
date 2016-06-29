#ifndef __rfControl_fn_h__
#define __rfControl_fn_h__

#include "RFControl.h"

bool rfc_raw_mode = false;
int rfc_interrupt_pin = -1;
int rfc_transmit_pin = -1;

void setup_rfcontrol(int irq_pin, int tx_pin)
{
	rfc_interrupt_pin = irq_pin;
	rfc_transmit_pin = tx_pin;
	rfc_raw_mode = false;
	RFControl::startReceiving(rfc_interrupt_pin);
	LOG("RFControl setup done\n");
}

void loop_rfcontrol() {
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
			Serial.print("RF receive ");
			for (unsigned int i = 0; i < 8; i++) {
				unsigned long bucket = buckets[i] * pulse_length_divider;
				Serial.print(bucket);
				Serial.write(' ');
			}
			for (unsigned int i = 0; i < timings_size; i++) {
				Serial.write('0' + timings[i]);
			}
			Serial.print("\r\n");
			RFControl::continueReceiving();
		}
	}
}

void close_rfcontrol()
{
	RFControl::stopReceiving();
}

void rfcontrol_set_receive_mode(int mode)
{
	rfc_raw_mode = mode ? true : false;
}

void rfcontrol_send(unsigned long *buckets, char *seq, unsigned int repeats = 3U)
{
	RFControl::sendByCompressedTimings(rfc_transmit_pin, buckets, seq, repeats);
}

// TODO: add send function

#endif /* __rfControl_fn_h__ */