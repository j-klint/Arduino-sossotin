#ifndef FADING_MEMORY_FILTER_H
#define FADING_MEMORY_FILTER_H

template<int FracBits>
class FadingMemFilter
{
public:
	int gain, value{ 0 };
	int pow = 1 << FracBits;
	int filterThreshold = 1 << (FracBits - 1);

	explicit FadingMemFilter(int filterGain) : gain{ filterGain } {}

	void input(int next)
	{
		value += gain * (next * pow - value) / pow;
	}

	bool yes() const
	{
		return value > filterThreshold;
	}
};

#endif //FADING_MEMORY_FILTER_H
