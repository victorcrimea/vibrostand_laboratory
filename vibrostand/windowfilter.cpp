#include "windowfilter.h"

WindowFilter::WindowFilter(filterType type, int level, int cutoffFrequency, int samplingFrequency)
	: highCutoffFrequecy(cutoffFrequency),
	  samplingFrequency(samplingFrequency),
	  level(level),
	  type(type) {
	filt = nullptr;
	reloadFilter();
}

WindowFilter::~WindowFilter() {
	delete filt;
}

QVector<double> WindowFilter::filter(const QVector<double> &signal) {
	QVector<double> out;

	//Reinit filter
	filt->init();

	//Actual filtering of all samples
	for (auto &sample : signal) {
		out.push_back(filt->do_sample(sample));
	}
	return out;
}

int WindowFilter::getHighCutoffFrequency() const {
	return highCutoffFrequecy;
}

void WindowFilter::setHighCutoffFrequency(int value) {
	highCutoffFrequecy = value;
	reloadFilter();
}

int WindowFilter::getSamplingFrequency() const {
	return samplingFrequency;
}

void WindowFilter::setSamplingFrequency(int value) {
	samplingFrequency = value;
	reloadFilter();
}

int WindowFilter::getLevel() const {
	return level;
}

void WindowFilter::setLevel(int value) {
	level = value;
	reloadFilter();
}

filterType WindowFilter::getType() const {
	return type;
}

void WindowFilter::setType(const filterType &value) {
	type = value;
	reloadFilter();
}

int WindowFilter::getLowCutoffFrequecy() const {
	return lowCutoffFrequency;
}

void WindowFilter::setLowCutoffFrequency(int value) {
	lowCutoffFrequency = value;
	reloadFilter();
}

void WindowFilter::reloadFilter() {
	//In order to reinitialize filter we need to delete old one
	if (filt != nullptr) {
		delete filt;
	}
	//Create new filter
	if (type == LPF) {
		filt = new Filter(type, level, (double)samplingFrequency / 1000.0, (double)highCutoffFrequecy / 1000.0);
	} else if (type == HPF) {
		filt = new Filter(type, level, (double)samplingFrequency / 1000.0, (double)lowCutoffFrequency / 1000.0);
	} else if (type == BPF) {
		filt = new Filter(type, level, (double)samplingFrequency / 1000.0, (double)lowCutoffFrequency / 1000.0, (double)highCutoffFrequecy / 1000.0);
	}

	if (filt->get_error_flag() != 0) {
		auto i = 0;
	}
}
