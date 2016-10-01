#ifndef NOISE_H
#define NOISE_H

#include "QVector"
#include <chrono>
#include <cmath>
#include <ctime>
#include <memory>
#include <random>

using namespace std;

using namespace std::chrono;

class Noise
{
public:
	Noise();
	static shared_ptr<QVector<double>> getWhiteNoise(int length);
};

#endif // NOISE_H
