#include "noise.h"

Noise::Noise()
{
}

shared_ptr<QVector<double>> Noise::getWhiteNoise(int length)
{
	random_device rd;
	auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
	mt19937 gen(seed);
	auto result = make_shared<QVector<double>>();
	normal_distribution<double> d(0, 1);
	for (int i = 0; i < length; ++i) {
		result->append(d(gen));
	}
	return result;
}
