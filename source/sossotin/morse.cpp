#include <avr/pgmspace.h>
#include "katakana.h"
#include "morse.h"

namespace
{
	class MorseTreeNode
	{
	public:
		uint8_t letter;
		uint8_t left, right;
		uint8_t operator[](Boop b) const
		{
			if ( b == Boop::dit )
				return left;
			else
				return right;
		}
	};

	struct WabunCode
	{
		uint8_t kana;
		uint8_t code, len;
	};



	constexpr MorseTreeNode MorseTreeData[] PROGMEM {
		{  0,  1, 2 },
		{'E',  3, 4 },
		{'T', 31,32 },
		{'I',  5, 6 },
		{'A', 18,19 },
		{'S',  7, 8 },
		{'U',  9,10 },
		{'H', 11,12 },
		{'V', 60,13 },
		{'F', 14, 0 },
		{0b11110101,15,16 }, //Ü
		{'5',  0, 0 },
		{'4',  0, 0 },
		{'3',  0, 0 },
		{UTF16_to_1602(L'É'),  0, 0 }, //É, the LCD doesn't have this symbol by default
		{0xff,17, 0 }, //Eth
		{'2',  0, 0 },
		{'?',  0, 0 },
		{'R', 20,21 },
		{'W', 23,24 },
		{'L',  0,22 },
		{0b11100001,27, 0 }, //Ä
		{0xff,59, 0 }, //È
		{'P',  0,25 },
		{'J',  0,29 },
		{0xff,26, 0 }, //À
		{'@',  0, 0 },
		{'+',  0,28 },
		{'.',  0, 0 },
		{'1', 30, 0 },
		{'\'', 0, 0 },
		{'N', 33,34 },
		{'M', 45,46 },
		{'D', 35,36 },
		{'K', 41,42 },
		{'B', 37,38 },
		{'X', 40,61 },
		{'6',  0,39 },
		{'=',  0, 0 },
		{'-',  0, 0 },
		{'/',  0, 0 },
		{'C',  0, 0 },
		{'Y', 43, 0 },
		{'(',  0,44 },
		{')',  0, 0 },
		{'G', 47,48 },
		{'O', 52,53 },
		{'Z', 49,50 },
		{'Q',  0,58 },
		{'7',  0, 0 },
		{0xff, 0,51 }, //^Z
		{',',  0, 0 },
		{0b11101111,54,0 },
		{0xff,56,57 }, //CH
		{'8', 55, 0 },
		{':',  0, 0 },
		{'9',  0, 0 },
		{'0',  0, 0 },
		{0b11101110, 0, 0 }, //ñ
		{'\"', 0, 0 },
		{  14, 0, 0 }, // shift out, intl 
		{0xff, 0,62 }, // matkalla shiftiin
		{  15, 0, 0 }  // shift in, wabun
	};

	constexpr MorseCode morseCodes[] PROGMEM {
		{6,0b010010} // \"
		,{0,0}//#
		,{0,0}//$
		,{0,0}//%
		,{0,0}//&
		,{6,0b011110}// \'
		,{5,0b10110}//(
		,{6,0b101101}//)
		,{0,0}//*
		,{5,0b01010}//+
		,{6,0b110011}//,
		,{6,0b100001}//-
		,{6,0b010101}//.
		,{5,0b10010}// /
		,{5,0b11111}//0
		,{5,0b01111}
		,{5,0b00111}
		,{5,0b00011}
		,{5,0b00001}
		,{5,0b00000}
		,{5,0b10000}
		,{5,0b11000}
		,{5,0b11100}
		,{5,0b11110}//9
		,{6,0b111000}//:
		,{0,0}//;
		,{0,0}//<
		,{5,0b10001}//=
		,{0,0}//>
		,{6,0b001100}//?
		,{6,0b011010}//@
		,{2,0b01}//A
		,{4,0b1000}//B
		,{4,0b1010}//C
		,{3,0b100}//D
		,{1,0b0}//E
		,{4,0b0010}//F
		,{3,0b110}//G
		,{4,0b0000}//H
		,{2,0b00}//I
		,{4,0b0111}//J
		,{3,0b101}//K
		,{4,0b0100}//L
		,{2,0b11}//M
		,{2,0b10}//N
		,{3,0b111}//O
		,{4,0b0110}//P
		,{4,0b1101}//Q
		,{3,0b010}//R
		,{3,0b000}//S
		,{1,0b1}//T
		,{3,0b001}//U
		,{4,0b0001}//V
		,{3,0b011}//W
		,{4,0b1001}//X
		,{4,0b1011}//Y
		,{4,0b1100}//Z
		,{0,0}//[
		,{0,0}// takakeno
		,{0,0}//]
		,{0,0}//^
		,{0,0}//_
		,{6,0b011110}//\'
	};

	constexpr WabunCode wabunCodes[] PROGMEM {
		{UTF16_to_1602(L'ｧ'),  0b11011,  5},
		{UTF16_to_1602(L'ｨ'),  0b01,     2},
		{UTF16_to_1602(L'ｩ'),  0b001,    3},
		{UTF16_to_1602(L'ｪ'),  0b10111,  5},
		{UTF16_to_1602(L'ｫ'),  0b01000,  5},
		{UTF16_to_1602(L'ｬ'),  0b011,    3},
		{UTF16_to_1602(L'ｭ'),  0b10011,  4},
		{UTF16_to_1602(L'ｮ'),  0b11,     2},
		{UTF16_to_1602(L'ｯ'),  0b0110,   4},
		{UTF16_to_1602(L'ﾑ'),  0b1,      1},
		{UTF16_to_1602(L'ﾍ'),  0b0,      1},
		{UTF16_to_1602(L'ｲ'),  0b01,     2},
		{UTF16_to_1602(L'ﾀ'),  0b10,     2},
		{UTF16_to_1602(L'ﾖ'),  0b11,     2},
		{UTF16_to_1602(0xff9e),0b00,     2}, //hipsut "dakuten"
		{UTF16_to_1602(L'ｳ'),  0b001,    3},
		{UTF16_to_1602(L'ﾅ'),  0b010,    3},
		{UTF16_to_1602(L'ﾎ'),  0b100,    3},
		{UTF16_to_1602(L'ﾔ'),  0b011,    3},
		{UTF16_to_1602(L'ﾗ'),  0b000,    3},
		{UTF16_to_1602(L'ﾘ'),  0b110,    3},
		{UTF16_to_1602(L'ﾚ'),  0b111,    3},
		{UTF16_to_1602(L'ﾜ'),  0b101,    3},
		{UTF16_to_1602(L'ｶ'),  0b0100,   4},
		{UTF16_to_1602(L'ｸ'),  0b0001,   4},
		{UTF16_to_1602(L'ｹ'),  0b1011,   4},
		{UTF16_to_1602(L'ｺ'),  0b1111,   4},
		{UTF16_to_1602(L'ｿ'),  0b1110,   4},
		{UTF16_to_1602(L'ﾁ'),  0b0010,   4},
		{UTF16_to_1602(L'ﾂ'),  0b0110,   4},
		{UTF16_to_1602(L'ﾆ'),  0b1010,   4},
		{UTF16_to_1602(L'ﾇ'),  0b0000,   4},
		{UTF16_to_1602(L'ﾈ'),  0b1101,   4},
		{UTF16_to_1602(L'ﾉ'),  0b0011,   4},
		{UTF16_to_1602(L'ﾊ'),  0b1000,   4},
		{UTF16_to_1602(L'ﾌ'),  0b1100,   4},
		{UTF16_to_1602(L'ﾏ'),  0b1001,   4},
		{UTF16_to_1602(L'ﾕ'),  0b10011,  4},
		{UTF16_to_1602(L'ﾛ'),  0b0101,   4},
		{UTF16_to_1602(L'ｦ'),  0b0111,   4},
		{UTF16_to_1602(L'ﾋ'),  0b11001,  5},
		{UTF16_to_1602(L'ﾝ'),  0b01010,  5},
		{UTF16_to_1602(L'ｱ'),  0b11011,  5},
		{UTF16_to_1602(L'ｴ'),  0b10111,  5},
		{UTF16_to_1602(L'ｵ'),  0b01000,  5},
		{UTF16_to_1602(L'ｷ'),  0b10100,  5},
		{UTF16_to_1602(L'ｻ'),  0b10101,  5},
		{UTF16_to_1602(L'ｼ'),  0b11010,  5},
		{UTF16_to_1602(L'ｽ'),  0b11101,  5},
		{UTF16_to_1602(L'ｾ'),  0b01110,  5},
		{UTF16_to_1602(L'ﾃ'),  0b01011,  5},
		{UTF16_to_1602(L'ﾄ'),  0b00100,  5},
		{UTF16_to_1602(L'ﾕ'),  0b10011,  5},
		{UTF16_to_1602(L'ﾐ'),  0b00101,  5},
		{UTF16_to_1602(L'ﾒ'),  0b10001,  5},
		{UTF16_to_1602(L'ﾓ'),  0b10010,  5},
		{UTF16_to_1602(L'ﾙ'),  0b10110,  5},
		{UTF16_to_1602(0xff9f),0b00110,  5}, //pallero "handakuten"
		{UTF16_to_1602(L'ｰ'),  0b01101,  5},
		{                 14,  0b00010,  5}, //shift out, international mode
		{                '0',  0b11111,  5}, //0
		{                '1',  0b01111,  5},
		{                '2',  0b00111,  5},
		{                '3',  0b00011,  5},
		{                '4',  0b00001,  5},
		{                '5',  0b00000,  5},
		{                '6',  0b10000,  5},
		{                '7',  0b11000,  5},
		{                '8',  0b11100,  5},
		{                '9',  0b11110,  5}, //9
		{UTF16_to_1602(L'､'),  0b010101, 6},
		{UTF16_to_1602(L'｡'),  0b010100, 6},
		{UTF16_to_1602(L'('),  0b101101, 6},
		{UTF16_to_1602(L')'),  0b010010, 6},
		{                 15,  0b100111, 6}  //shift in,  wabun mode
	};

	constexpr uint8_t numWabuns = ArraySize(wabunCodes);
}


void MorseTree::reset() { codeLength = curr = 0; }

uint8_t MorseTree::read() const
{
	if ( curr == 0 )
		return 0xff;

	uint8_t retval;
	memcpy_P(&retval, &MorseTreeData[curr].letter, sizeof(retval));
	return retval;
}

bool MorseTree::step(Boop b)
{
	++codeLength;
	MorseTreeNode tempNode;
	memcpy_P(&tempNode, MorseTreeData + curr, sizeof(tempNode));
	curr = tempNode[b];
	return curr != 0;
}

MorseCode MorseEncode(uint8_t ltr)
{
	if ( 0xa1 <= ltr && ltr <= 0xdf ) //katakanat
	{
		for ( int i = 0; i < numWabuns; ++i )
		{
			WabunCode code;
			memcpy_P(&code, wabunCodes + i, sizeof(code));
			if ( code.kana == ltr )
				return { code.len, code.code };
		}
		return { 0,0 };
	}

	switch ( ltr )
	{
	case 15: return { 6, 0b100111 };
	case 14: return { 5, 0b00010 };
	case UTF16_to_1602(L'ä'): return { 4,0b0101 };
	case UTF16_to_1602(L'ö'): return { 4,0b1110 };
	case UTF16_to_1602(L'ü'): return { 4,0b0011 };
	case UTF16_to_1602(L'é'): return { 5,0b00100};
	}

	if ( ltr < '\"' || ltr >= '{' )
		return { 0,0 };

	if ( 'a' <= ltr && ltr <= 'z' )
		ltr -= 'a' - 'A';

	MorseCode retval;
	memcpy_P(&retval, morseCodes + (ltr - '\"'), sizeof(retval));
	return retval;
}

bool WabunDecoder::step(Boop b)
{
	if ( ++codeLength > 6 )
		return false;

	uint8_t index = curr + 1;
	code = (code << 1) | (b == Boop::dah);
	WabunCode tempCode;

	while ( index < numWabuns )
	{
		memcpy_P(&tempCode.len, &wabunCodes[index].len, sizeof(tempCode.len));
		if ( tempCode.len >= codeLength ) break;
		++index;
	}

	while ( index < numWabuns )
	{
		memcpy_P(&tempCode, &wabunCodes[index], sizeof(tempCode));

		if ( tempCode.len > codeLength ) break;

		if ( tempCode.len == codeLength && tempCode.code == code )
		{
			curr = index;
			return true;
		}

		++index;
	}

	curr = index - 1;

	while ( index < numWabuns )
	{
		memcpy_P(&tempCode, &wabunCodes[index], sizeof(tempCode));
		if ( (tempCode.code >> (tempCode.len - codeLength)) == code )
			return true;

		++index;
	}

	return false;
}

void WabunDecoder::reset() { init(); }

uint8_t WabunDecoder::read() const
{
	WabunCode tempCode;
	memcpy_P(&tempCode, &wabunCodes[curr], sizeof(tempCode));

	if ( tempCode.code == code && tempCode.len == codeLength )
		return tempCode.kana;
	else
		return 0xff;
}
