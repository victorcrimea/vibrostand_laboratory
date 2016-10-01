#include "noise.h"

Noise::Noise()
{

}

unique_ptr<QVector<double>> Noise::getWhiteNoise(int length)
{
	random_device rd;
	std::srand(std::time(0));

	mt19937 gen(std::rand());
	auto result = make_unique<QVector<double>>();
	normal_distribution<double> d(0,1);
	for(int i=0; i<length; ++i) {
			result->append(d(gen));
	}
	return result;
}
