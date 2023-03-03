#ifndef MORSETULKKARI_H
#define MORSETULKKARI_H

#include <stdint.h>

enum struct Boop : uint8_t
{
	dit, dah
};

class TelegraphDecoder
{
public:
	uint8_t codeLength{ 0 };
	virtual void reset() = 0;
	virtual uint8_t read() const = 0;

	// tosi, jos siirto onnistui
	virtual bool step(Boop b) = 0;
};

class MorseTree : public TelegraphDecoder
{
	uint8_t curr{ 0 };

public:
	bool step(Boop b) override;
	void reset() override;
	uint8_t read() const override;
};


struct MorseCode
{
	uint8_t len, code;
};

MorseCode MorseEncode(uint8_t ltr);


class WabunDecoder : public TelegraphDecoder
{
	uint8_t curr;
	uint8_t code;
	void init() { code = codeLength = 0; curr = 8; }

public:
	WabunDecoder() { init(); }
	bool step(Boop b) override;
	void reset() override;
	uint8_t read() const override;
};

#endif //MORSETULKKARI_H
