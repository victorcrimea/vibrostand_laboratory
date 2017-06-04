#ifndef FILTER_H
#define FILTER_H

#include <QObject>
#include <QVector>

#include "filter/filt.h"

class WindowFilter : public QObject {
	Q_OBJECT
public:
	WindowFilter(filterType type = LPF, int level = 50, int highCutoffFrequecy = 1000, int samplingFrequency = 40000);
	~WindowFilter();

	/**
	 * @brief Method to filter entire realisation
	 * @param signal - signal to be filtered
	 * @return filtered signal
	 */
	QVector<double> filter(const QVector<double> &signal);

	//Getters
	int getHighCutoffFrequency() const;
	int getSamplingFrequency() const;
	int getLevel() const;
	filterType getType() const;
	int getLowCutoffFrequecy() const;

	//Setters
	void setHighCutoffFrequency(int value);
	void setSamplingFrequency(int value);
	void setLevel(int value);
	void setType(const filterType &value);
	void setLowCutoffFrequency(int value);

private:
	/** In Hz */
	int highCutoffFrequecy;

	/** In Hz */
	int lowCutoffFrequency;

	/** In Hz */
	int samplingFrequency;

	int level;

	filterType type;

	Filter *filt;

	void reloadFilter();
};

#endif // FILTER_H
