#ifndef _EEPROMCONFIG_H_
#define _EEPROMCONFIG_H_

#include <Arduino.h>
#include <inttypes.h>
#include <EEPROM.h>
#include <MD5Builder.h>

#define DEBUG
#ifdef DEBUG
#define Dlog(...)    Serial.printf("EEPROM: " __VA_ARGS__)
#else
#define Dlog(...)
#endif

#define MD5_FMT "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x"
#define MD5_VAL(m) m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]

#define EEPROM_COMFIG_MD5_LEN   16

#define EEPROM_start_address	0
#define EEPROM_vers_offset		0 + EEPROM_start_address
#define EEPROM_len_offset		4 + EEPROM_start_address
#define EEPROM_md5_offset		8 + EEPROM_start_address
#define EEPROM_data_offset		EEPROM_COMFIG_MD5_LEN + 8 + EEPROM_start_address

template <typename T> class EEPROMconfig {
  public:
	EEPROMconfig(int version);
	bool load();
	bool save();
  private:
	void computeMD5(uint8_t *input, int len, uint8_t *md5_out);

public:
	bool loaded;
	T data;
private:
	uint32_t _vers;
	uint32_t _len;                           // len of data
	uint8_t  _md5[EEPROM_COMFIG_MD5_LEN];    // md5 of data
};

template <typename T> EEPROMconfig<T>::EEPROMconfig(int version)
	: loaded(false)
	, _vers(version)
	, _len(sizeof(T))
{
	_md5[0] = 0;
	memset(&data, 0, sizeof(T));
}

template <typename T> bool EEPROMconfig<T>::load(void)
{
	uint32_t evers = 0;
	uint32_t elen = 0;
	uint8_t  emd5[EEPROM_COMFIG_MD5_LEN] = { 0 };

	if (loaded)
		return true;

	EEPROM.begin(sizeof(_vers) + sizeof(_md5) + sizeof(_len) + sizeof(data));

	EEPROM.get(EEPROM_vers_offset, evers);
	if (evers != _vers) {
		Dlog("invalid config version %d/%d\n", evers, _vers);
		return false;
	}
	EEPROM.get(EEPROM_len_offset, elen);
	if (elen != _len) {
		Dlog("invalid config length %d/%d\n", elen, _len);
		return false;
	}
	EEPROM.get(EEPROM_data_offset, data);
	computeMD5((uint8_t *)(&data), sizeof(data), (uint8_t*)(&_md5));

	EEPROM.get(EEPROM_md5_offset, emd5);
	for (int i = 0; i < EEPROM_COMFIG_MD5_LEN; i++) {
		//if (memcmp(emd5, _md5, EEPROM_COMFIG_MD5_LEN) == 0) {
		if (emd5[i] != _md5[i]) {
			Dlog("invalid config MD5 " MD5_FMT " / " MD5_FMT "\n", MD5_VAL(emd5), MD5_VAL(_md5));
			return false;
		}
	}

	loaded = true;
	return true;
}

template <typename T> bool EEPROMconfig<T>::save(void)
{
	computeMD5((uint8_t *)(&data), sizeof(data), (uint8_t *)(&_md5));

	EEPROM.put(EEPROM_vers_offset, _vers);
	EEPROM.put(EEPROM_len_offset, _len);
	EEPROM.put(EEPROM_md5_offset, _md5);
	EEPROM.put(EEPROM_data_offset, data);

	EEPROM.commit();
	return true;
}

template <typename T> void EEPROMconfig<T>::computeMD5(uint8_t *input, int len, uint8_t *md5_out)
{
	MD5Builder md5;
	md5.begin();
	md5.add(input, len);
	md5.calculate();
	md5.getBytes(md5_out);
}




#endif /*_EEPROMCONFIG_H_*/
