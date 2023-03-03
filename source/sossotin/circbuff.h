#ifndef CIRCULAR_STR_BUFFER_H
#define CIRCULAR_STR_BUFFER_H

#include <LiquidCrystal.h>

template<typename T, size_t N>
class CircularBuffer
{
	T buf[N];
	size_t cursor, size;

public:
	CircularBuffer()
	{
		static_assert( sizeof(T) == 1 );
		clear();
	}

	void clear()
	{
		cursor = size = 0;
		for ( size_t i = 0; i < N; ++i )
			buf[i] = T(' ');
	}

	void push(T c)
	{
		buf[cursor] = c;
		size += size != N;
		cursor = (cursor + 1) % N;
	}

	void set(const T* s)
	{
		cursor = size = 0;
		size_t i = 0;
		while ( i++ < N && *s )
			push(*s++);

		while ( i < N )
			buf[i++] = T(' ');
	}

	void set(const __FlashStringHelper* s0)
	{
		//const char* s = reinterpret_cast<const char*>(s0);
		const T* s = reinterpret_cast<const T*>(s0);
		cursor = size = 0;
		size_t i = 0;
		T b{ pgm_read_byte(s) };
		while ( i++ < N && b )
		{
			push(b);
			b = static_cast<T>(pgm_read_byte(++s));
		}

		while ( i < N )
			buf[i++] = T(' ');
	}

	void print(LiquidCrystal& lcd, uint8_t row = 0) const
	{
		lcd.setCursor(0, row);
		if ( size == N )
		{
			for ( size_t i = cursor; i < N; ++i )
				lcd.write(buf[i]);
		}
		for ( size_t i = 0; i < cursor; ++i )
			lcd.write(buf[i]);
	}
};

#endif //CIRCULAR_STR_BUFFER_H
