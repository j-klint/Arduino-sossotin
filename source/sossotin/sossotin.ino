#include <LiquidCrystal.h>
#include "katakana.h"
#include "circbuff.h"
#include "morse.h"
#include "filter.h"

using duration_t = unsigned long;

void HandleBoop(duration_t len);
void HandlePause(duration_t len);
void HandleSerialInput();
unsigned ParseUInt(const uint8_t* p);
void ExecuteSerialCmd();
template<typename StrT1, typename StrT2> void PrintQuantity(StrT1 name, long value, StrT2 unit);
void ResetArrowRow(uint8_t row = 1);
void SwitchWabunMode(bool yesWabun);
void BeepSerialMessage(const uint8_t* msg);
void BeepSingleLetter(uint8_t c, CircularBuffer<uint8_t, 16>& morseBuf);

constexpr int buttonPin = 12;
constexpr int ledPin = 13;
constexpr int buzzerPin = 2;
constexpr duration_t timeStep{ 5 };

constexpr int serialBufferSize{ 1501 };
uint8_t serialBuffer[serialBufferSize]{ 0 };
int serialCursor{ 0 };

duration_t dahDurMin{ 155 }; // input rate
//duration_t letterPause { dahDurMin };
//duration_t wordPause{ dahDurMin*7/2 };
duration_t letterPause{ 240 };
duration_t wordPause{ 1000 };

duration_t ditDur{ 1200 / 12 }; // output rate, dit = 1200ms / WPM
unsigned buzzerFreq = 800;

bool pohjassa = false;
bool started = false;
bool soundOn = true;
bool wabunMode = false;
uint8_t autoPauses = 0;
duration_t boopStart{ 0 };
duration_t pauseStart{ 0 };
duration_t nextFrameTime{ 0 };


FadingMemFilter<7> filter(20);
//CircularBuffer<char, 16> textBuf;
CircularBuffer<uint8_t, 16> textBuf;
LiquidCrystal lcd(10, 9, 4, 5, 6, 7);
MorseTree morseDecoder;
WabunDecoder wabunDecoder;
TelegraphDecoder* decoder{ &morseDecoder };

void setup()
{
	Serial.begin(9600);

	lcd.begin(16, 2);
	lcd.clear();
	ResetArrowRow();

	pinMode(buttonPin, INPUT);
	pinMode(ledPin, OUTPUT);

	uint8_t temp[8]
	{
		0b10001,
		0b11011,
		0b10101,
		0b10101,
		0b10001,
		0b10001,
		0b10001,
		0b00000
	};
	for ( int i = 0; i < 8; ++i )
		temp[i] ^= 0b11111;
	lcd.createChar(14 - 8, temp);

	temp[0] = 0b01110;
	temp[1] = 0b01110;
	temp[2] = 0b01110;
	temp[3] = 0b01110;
	temp[4] = 0b01010;
	temp[5] = 0b01010;
	temp[6] = 0b10001;
	temp[7] = 0b11111;
	lcd.createChar(15 - 8, temp);

	temp[0] = 0b00010;
	temp[1] = 0b00100;
	temp[2] = 0b01110;
	temp[3] = 0b10001;
	temp[4] = 0b11111;
	temp[5] = 0b10000;
	temp[6] = 0b01110;
	temp[7] = 0b00000;
	lcd.createChar(UTF16_to_1602(L'É'), temp);
	
	//while ( !Serial ) {}
}

/*
void ToistaViesti()
{
  uint8_t count = 0;
  delay(100);
  Serial.println();
  while ( Serial.available() )
  {
	//auto c = UTF16_to_1602( ReadUTF8Serial() );
	auto c = ReadUTF8Serial();
	if ( c > 31 )
	{
	  //textBuf.push(c);
	  //textBuf.print(lcd);
	}

	Serial.print(count++);
	Serial.print(": \"");
	Serial.print(c,HEX);
	Serial.println("\"");
  }
  textBuf.print(lcd);
}
*/

void loop()
{
	duration_t currTime{ millis() };

	if ( Serial.available() )
	{
		HandleSerialInput();
		//ToistaViesti();
		return;
	}

	if ( currTime < nextFrameTime )
		return;
	else
		nextFrameTime = currTime + timeStep;

	filter.input(digitalRead(buttonPin) == HIGH);
	bool btnState = filter.yes();

	if ( !pohjassa && btnState ) // nappi kiinni
	{
		pohjassa = true;
		digitalWrite(ledPin, HIGH);
		if ( soundOn )
			tone(buzzerPin, buzzerFreq);
		boopStart = currTime;
		if ( started )
		{
			HandlePause(currTime - pauseStart);
			autoPauses = 0;
		}
		else
			started = true;
	}
	else if ( pohjassa && !btnState ) // nappi auki
	{
		pohjassa = false;
		digitalWrite(ledPin, LOW);
		noTone(buzzerPin);
		pauseStart = currTime;
		HandleBoop(currTime - boopStart);
		autoPauses = 0;
	}
	else if ( started && !pohjassa && !btnState ) // pause jatkuu
	{
		if ( autoPauses == 0 && (currTime > pauseStart + letterPause) )
		{
			++autoPauses;
			HandlePause(currTime - pauseStart);
		}
		else if ( autoPauses == 1 && (currTime > pauseStart + wordPause) )
		{
			++autoPauses;
			started = false;
			HandlePause(currTime - pauseStart);
		}
	}
}

void HandleBoop(duration_t len)
{
	uint8_t c;
	Boop b;

	if ( len < dahDurMin )
	{
		c = 0b10100101;
		b = Boop::dit;
	}
	else
	{
		c = '-';
		b = Boop::dah;
	}

	if ( decoder->codeLength == 0 )
		ResetArrowRow();

	lcd.setCursor(decoder->codeLength, 1);
	lcd.write(c);

	if ( !decoder->step(b) )
	{
		uint8_t letter = 0xff;
		decoder->reset();
		lcd.setCursor(9, 1);
		lcd.write(letter);
		textBuf.push(letter);
		textBuf.print(lcd);
	}
}

void HandlePause(duration_t len)
{
	if ( len > letterPause && decoder->codeLength > 0 )
	{
		auto letter = decoder->read();
		decoder->reset();
		lcd.setCursor(9, 1);
		lcd.write(letter);
		textBuf.push(letter);
		textBuf.print(lcd);
		if ( letter == 14 )
			SwitchWabunMode(false);
		else if ( letter == 15 )
			SwitchWabunMode(true);
	}

	if ( wabunMode )
		return;

	if ( len > wordPause )
	{
		textBuf.push(' ');
		textBuf.print(lcd);
	}
}

void ResetArrowRow(uint8_t row)
{
	lcd.setCursor(0, row);
	lcd.print(F("       \176        "));
}

template<typename StrT1, typename StrT2>
void PrintQuantity(StrT1 name, long value, StrT2 unit)
{
	lcd.clear();
	textBuf.set(name);
	textBuf.push(':');
	textBuf.push(' ');
	textBuf.print(lcd);
	lcd.setCursor(0, 1);
	lcd.print(value, DEC);
	lcd.print(" ");
	lcd.print(unit);
}

void HandleSerialInput()
{
	while ( Serial.available() && Serial.available() >= UTF8Bytes(Serial.peek()) )
	{
		uint16_t c = ReadUTF8Serial();

		if ( c == '\r' || c == '\n' || (serialCursor == serialBufferSize - 1) )
		{
			started = false;
			serialBuffer[serialCursor] = 0;
			ExecuteSerialCmd();
			serialCursor = 0;
			serialBuffer[0] = 0;
			while ( Serial.available() ) Serial.read();
			return;
		}
		else
		{
			serialBuffer[serialCursor++] = UTF16_to_1602(c);
		}
	}
}

unsigned ParseUInt(const uint8_t* p)
{
	unsigned result = 0;
	while ( '0' <= *p && *p <= '9' )
	{
		result = 10 * result + (*p - '0');
		++p;
	}
	return result;
}
  
void ExecuteSerialCmd()
{
	if ( serialBuffer[0] != '_' )
	{
		BeepSerialMessage(serialBuffer);
		return;
	}

	uint8_t c = serialBuffer[1];
	if ( c == 'p' || c == 'P' )
	{
		auto WPM = ParseUInt(serialBuffer + 2);
		if ( 0 < WPM && WPM < 200 )
		{
			ditDur = 1200 / WPM;
			PrintQuantity(F("Playback rate"), WPM, F("WPM"));
		}
	}
	else if ( c == 'd' || c == 'D' )
	{
		auto n = ParseUInt(serialBuffer + 2);
		if ( timeStep < n )
		{
			dahDurMin = n;
			PrintQuantity(F("Dah threshold"), n, F("ms"));
		}
	}
	else if ( c == 's' || c == 'S' )
	{
		uint8_t n = serialBuffer[2];
		if ( n == '0' || n == '1' )
		{
			soundOn = n == '1';
			lcd.clear();
			if ( soundOn )
				textBuf.set(F("Sound on "));
			else
				textBuf.set(F("Sound off "));
			textBuf.print(lcd);
		}
	}
	else if ( c == 'l' || c == 'L' )
	{
		auto n = ParseUInt(serialBuffer + 2);
		if ( n < wordPause )
		{
			letterPause = n;
			PrintQuantity(F("Letter space"), n, F("ms"));
		}
	}
	else if ( c == 'w' || c == 'W' )
	{
		auto n = ParseUInt(serialBuffer + 2);
		if ( letterPause < n )
		{
			wordPause = n;
			PrintQuantity(F("Word space"), n, F("ms"));
		}
	}
	else if ( c == 'B' || c == 'b' )
	{
		auto n = ParseUInt(serialBuffer + 2);
		if ( 50 < n && n < 20001 )
		{
			buzzerFreq = n;
			PrintQuantity(F("Buzzer freq"), n, F("Hz"));
		}
	}
}

void BeepSerialMessage(const uint8_t* msg)
{
	textBuf.clear();
	lcd.clear();
	CircularBuffer<uint8_t, 16> morseBuf;

	while ( *msg )
		BeepSingleLetter(*(msg++), morseBuf);
}

void BeepSingleLetter(uint8_t c, CircularBuffer<uint8_t, 16>& morseBuf)
{
	morseBuf.push(' ');
	MorseCode m = MorseEncode(c);

	if ( c == ' ' )// || c == '\r' || c == '\n' )
	{
		textBuf.push(' ');
		textBuf.print(lcd);
		morseBuf.push(' ');
		morseBuf.print(lcd, 1);
		delay(4 * ditDur);
		return;
	}
	else if ( m.len == 0 )
	{
		return;
	}

	textBuf.push(c);
	textBuf.print(lcd);

	for ( uint8_t mask = 1 << (m.len - 1); mask > 0; mask >>= 1 )
	{
		if ( m.code & mask )
			morseBuf.push('-');
		else
			morseBuf.push(0b10100101);
	}
	morseBuf.print(lcd, 1);

	for ( uint8_t mask = 1 << (m.len - 1); mask > 0; mask >>= 1 )
	{
		if ( soundOn )
			tone(buzzerPin, buzzerFreq);
		digitalWrite(ledPin, HIGH);

		if ( m.code & mask )
		{
			delay(3 * ditDur);
		}
		else
		{
			delay(1 * ditDur);
		}

		noTone(buzzerPin);
		digitalWrite(ledPin, LOW);
		delay(ditDur);
	}

	delay(2 * ditDur);
}

void SwitchWabunMode(bool yesWabun)
{
	wabunMode = yesWabun;

	if ( yesWabun )
		decoder = &wabunDecoder;
	else
		decoder = &morseDecoder;
}
