#ifndef _EEPROMCONFIG_H_
#define _EEPROMCONFIG_H_

#include <Arduino.h>
#include <inttypes.h>
#include <EEPROM.h>
#include <MD5Builder.h>

#include "Debug.h"


#define MD5_FMT "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x"
#define MD5_VAL(m) m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]

#define EEPROM_COMFIG_MD5_LEN   16

#define EEPROM_start_address	0
#define EEPROM_md5_offset		EEPROM_start_address
#define EEPROM_data_offset		EEPROM_COMFIG_MD5_LEN + EEPROM_md5_offset

class EEPROMconfig {
  public:
	template <class T> static bool Load(const T& cfg);
	template <class T> static bool Save(const T& cfg);
  private:
	static void computeMD5(uint8_t *input, int len, uint8_t *md5_out) {
		MD5Builder md5;
		md5.begin();
		md5.add(input, len);
		md5.calculate();
		md5.getBytes(md5_out);
	}
};

template <class T> bool EEPROMconfig::Load(const T& cfg)
{
	uint32_t evers = 0;
	uint32_t elen = 0;
	uint8_t  emd5[EEPROM_COMFIG_MD5_LEN] = { 0 };
	uint8_t  cmd5[EEPROM_COMFIG_MD5_LEN] = { 0 };

	EEPROM.begin(EEPROM_COMFIG_MD5_LEN + sizeof(T));

	EEPROM.get(EEPROM_md5_offset, emd5);
	EEPROM.get(EEPROM_data_offset, cfg);
	computeMD5((uint8_t *)(&cfg), sizeof(T), (uint8_t*)(&cmd5));

	for (int i = 0; i < EEPROM_COMFIG_MD5_LEN; i++) {
		if (emd5[i] != cmd5[i]) {
			LOG("invalid config MD5 " MD5_FMT " / " MD5_FMT "\n", MD5_VAL(emd5), MD5_VAL(cmd5));
			return false;
		}
	}

	return true;
}

template <class T> bool EEPROMconfig::Save(const T& cfg)
{
	uint8_t  cmd5[EEPROM_COMFIG_MD5_LEN] = { 0 };

	computeMD5((uint8_t *)(&cfg), sizeof(T), (uint8_t *)(&cmd5));

	EEPROM.put(EEPROM_md5_offset, cmd5);
	EEPROM.put(EEPROM_data_offset, cfg);

	EEPROM.commit();
	return true;
}


#endif /* _EEPROMCONFIG_H_ */
