#ifndef KATAKANA_KONVERTTERI_H
#define KATAKANA_KONVERTTERI_H

#include <stdint.h> // uint8_t jne.
#include <stddef.h> // size_t

int UTF8Bytes(uint8_t firstChar);
uint16_t ReadUTF8Serial();

// A function for determining the size of an array at compile time
template<typename T, size_t N> constexpr size_t ArraySize(const T(&)[N]) { return N; }

// Converts half-width katakana and a handful of other symbols from UCS-2/UTF-16 to 1602 printable values
constexpr uint8_t UTF16_to_1602(uint16_t c)
{
	// C++11 allows only one return statement per constexpr function and no ifs.
	// That's why we need to spam the ternary operator.
	return (0xff61 <= c && c <= 0xff9f) ? (c - 0xff61 + 0b10100001) : //ascii lookalikes
	       (0xff01 <= c && c <= 0xff5d) ? (c - 0xff61 + 0b10100001) : //katakana proper
	       (c == 0x4e07)                ? 0b11111011 :                //man
	       (c == 0x5186)                ? 0b11111100 :                //en
	       (c == 0x5343)                ? 0b11111010 :                //sen
	       (c == 0xffe5)                ? 0b01011100 :                //Y-yen
	       (c == 0xffe9)                ? 0b01111111 :                //vas. nuoli
	       (c == 0xffeb)                ? 0b01111110 :                //oik. nuoli
	       (c == 0x00c4 || c == 0x00e4) ? 0b11100001 :                //Ää
	       (c == 0x00d6 || c == 0x00f6) ? 0b11101111 :                //Öö
	       (c == 0x00dc || c == 0x00fc) ? 0b11110101 :                //Üü
	       (c == 0x00d1 || c == 0x00f1) ? 0b11101110 :                //Ññ
	       (c == L'É'   || c == L'é'  ) ? 5          :                //Éé //TODO: draw an É symbol for the LCD display to display
	       (c == L'´')                  ? '\''       :
	       (c < 128)                    ? c          :                //non-ASCII chars could be falsely interpreted as katakana
	       0xff;
}

#endif //KATAKANA_KONVERTTERI_H
