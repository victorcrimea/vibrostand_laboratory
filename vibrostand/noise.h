#ifndef NOISE_H
#define NOISE_H

#include "QVector"
#include <random>
#include <cmath>
#include <memory>
#include <ctime>

using namespace std;

class Noise
{
public:
	Noise();
	static unique_ptr<QVector<double>> getWhiteNoise(int length);
};

#endif // NOISE_H
