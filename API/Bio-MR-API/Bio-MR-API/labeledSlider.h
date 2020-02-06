#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>

class LabeledSlider : public QWidget {
	Q_OBJECT
public:
	LabeledSlider(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

	inline void SetMaxValueDouble(double val) { maxValue = val; }
	inline void SetMinValueDouble(double val) { minValue = val; }
	inline void SetNumTicks(int val) { m_pSlider->setMaximum(val); }
	inline void UpdateSliderPosition() { OnSliderValueChanged(m_pSlider->value()); }
	double GetDoubleValue();

signals:
	void valueChanged(double value);


private:
	void OnSliderValueChanged(int value);

	QSlider* m_pSlider;
	QLabel* m_pLabel;
	double maxValue = 1.;
	double minValue = 0.;
};