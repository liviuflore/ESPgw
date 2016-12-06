#ifndef _ESPGW_RF433_H_
#define _ESPGW_RF433_H_


class ESPgwRF433
{
private:
	static bool rfc_raw_mode;
	static int rfc_interrupt_pin;
	static int rfc_transmit_pin;

public:
	static void Setup(bool raw_mode = false);
	static void Loop();
	static void Close();
	static void SetRXMode(bool mode);
	static void Send(unsigned long *buckets, char *seq, unsigned int repeats = 3U);

private:
	static bool getBytes(unsigned int *timings, unsigned int len, unsigned char *outBytes);
};

#endif /* _ESPGW_RF433_H_ */