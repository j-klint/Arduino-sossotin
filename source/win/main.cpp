#include <iostream>
#include <string>
#include <io.h>
#include <fcntl.h>
#include "SimpleSerial.h"

using namespace std;

struct UCS2Input
{
	UCS2Input() : oldmode{ _setmode(_fileno(stdin), _O_WTEXT) }
	{
		if ( oldmode < 0 )
			std::cerr << "Failed to set file translation mode for stdin.\n";
	}
	~UCS2Input()
	{
		if ( oldmode >= 0 )
			_setmode(_fileno(stdin), oldmode);
	}
private:
	int oldmode;
};

std::basic_string<unsigned char> UTF8Encode(const wchar_t* wstr)
{
	auto numBytes = [](unsigned c)
	{
		if ( c <= 0x007f ) return 1;
		if ( c <= 0x07ff ) return 2;
		if ( c <= 0xffff ) return 3;
		return 4;
	};

	std::basic_string<unsigned char> output;

	for ( ; *wstr; ++wstr )
	{
		unsigned c = *wstr;
		if ( c <= 0x007f )
		{
			output += static_cast<unsigned char>(*wstr);
			continue;
		}

		unsigned char buffer[4]{ 0,0,0,0 };
		int bytes = numBytes(c);
		int ones = 0;
		for ( int i = bytes - 1; i >= 0; --i )
		{
			buffer[i] = (c & 0b0011'1111) | 0b1000'0000;
			c >>= 6;
			ones = (ones >> 1) | 0b1000'0000;
		}
		buffer[0] |= ones;
		output.append(buffer);
	}
	return output;
}

void PrintInstructions()
{
	wcout << "Komennot:\n"
	         " _p playback WPM\n"
	         " _d input dah min duration\n"
	         " _l input letter space\n"
	         " _w input word space\n"
	         " _s sound on/off\n"
	         " _b buzzer pitch\n"
	         " _r re-print these instructions\n\n";
}

int main(int argc, char** argv)
{
	UCS2Input engage;

	int n = 3;

	if ( argc < 2 )
	{
		wcout << "Ei argumenttia. Default portti COM3.\n";
	}
	else
	{
		try
		{
			n = atoi(argv[1]);
		}
		catch ( ... )
		{
			wcout << "En tajunnut lukua!\n";
			return 1;
		}
		wcout << "Com-portti " << n << '\n';
	}

	char portti[30];
	sprintf_s(portti, "\\\\.\\COM%d", n);

	DWORD COM_BAUD_RATE = CBR_9600;
	SimpleSerial Serial(portti, COM_BAUD_RATE);

	if ( Serial.connected_ )
	{
		wcout << "Tuli yhteys. ";
		PrintInstructions();

		std::wstring input;

		do
		{
			input.clear();
			wcout << "> ";
			getline(wcin, input);

			if ( input.size() > 1 && input[0] == '_' && input[1] == 'r' )
			{
				wcout << '\n';
				PrintInstructions();
				continue;
			}

			auto writee = UTF8Encode(&input[0]);
			writee.push_back('\n');
			writee.push_back(0);
			Serial.WriteSerialPort((char*)&writee[0]);

#ifndef NDEBUG
			wcout << "input:  ";
			for (auto c : input)
			{
				wcout << std::hex << ((int)c) << ' ';
			}
			wcout << "\noutput: ";
			for (auto c : writee)
			{
				wcout << std::hex << ((int)c) << ' ';
			}
			wcout << '\n';
#endif // !NDEBUG
		} while ( !input.empty() && input[0] != '\n' && input[0] != '\r' );

		Serial.CloseSerialPort();
	}
	else
	{
		wcout << "Yhteys ei onnistunut\n";
	}

	wcout << "Loppu\n";
}
