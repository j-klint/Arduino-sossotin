#include <HardwareSerial.h> // Serial.read()
#include "katakana.h"

int UTF8Bytes(uint8_t firstChar)
{
	int ones{ 0 };
	uint8_t mask{ 0b10000000 };

	while ( (mask & firstChar) == mask )
	{
		++ones;
		mask = mask >> 1 | 0b10000000;
	}

	if ( ones < 1 || ones > 3 )
		return 1;
	else
		return ones;
}

uint16_t ReadUTF8Serial()
{
	uint8_t ones{ 0 };
	uint8_t mask{ 0b10000000 };
	uint8_t firstChar = Serial.read();

	while ( (mask & firstChar) == mask )
	{
		++ones;
		mask = mask >> 1 | 0b10000000;
	}

	if ( ones == 0 || ones > 3 )
		return firstChar;

	uint16_t code(firstChar & ~mask);

	for ( int i = 1; i < ones; ++i )
	{
		uint8_t nextChar = Serial.read();

		if ( (nextChar & 0b11000000) != 0b10000000 )
			return firstChar;

		code = (code << 6) | (nextChar & 0b00111111);
	}

	return code;
}
