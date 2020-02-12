#pragma once

#include <math.h>
#include <algorithm>

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QHBoxLayout>


// --------------------------------------
// Base class
// --------------------------------------
class ParameterControlWidgetBase : public QWidget {
	Q_OBJECT
public:
	ParameterControlWidgetBase(QStringList createInfo, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()) : QWidget(parent, f) {
		// Parse the common input strings
		m_parameterName = createInfo.at(0);
		bool isButton = createInfo.at(1).compare("True", Qt::CaseInsensitive) == 0;

		// Create the horizontal layout for this widget
		m_pLayout = new QHBoxLayout(this);
		m_pLayout->setContentsMargins(0, 0, 0, 0);
		setLayout(m_pLayout);

		// Create the name either as a button, or a label
		if (isButton) {
			m_pParameterLabel = new QPushButton(m_parameterName, this);
		} else {
			m_pParameterLabel = new QLabel(m_parameterName, this);
		}

		// Add the label to the layout
		m_pLayout->addWidget(m_pParameterLabel);
	}

signals:
	void DatagramReady(QString& message);

protected:
	double ParameterControlWidgetBase::CalcSingleStep(double range)
	{
		// We want roughly 100 steps in the range
		double target = range / 100.0;

		// Round to the nearest power of 10
		double roundedPower = round(log10(target));

		// Convert back to a step size
		double singleStep = pow(10.0, roundedPower);
		return singleStep;
	}

	QString m_parameterName;
	QWidget* m_pParameterLabel;
	QHBoxLayout* m_pLayout;

};


// --------------------------------------
// Generic Template
// --------------------------------------
template <class T>
class ParameterControlWidget : public ParameterControlWidgetBase 
{};

// --------------------------------------
// Integer specialization
// --------------------------------------
template <>
class ParameterControlWidget <int> : public ParameterControlWidgetBase {
public:
	ParameterControlWidget(QStringList createInfo, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
		: ParameterControlWidgetBase(createInfo, parent, f)
	{
		// Parse the input strings
		bool isButton = createInfo.at(1).compare("True", Qt::CaseInsensitive) == 0;
		int minVal = createInfo.at(3).toInt();
		int startVal = createInfo.at(4).toInt();
		int maxVal = createInfo.at(5).toInt();
		int range = maxVal - minVal;

		// Setup the spinbox range and values
		m_pSpinBox = new QSpinBox(this);
		m_pSpinBox->setMinimum(minVal);
		m_pSpinBox->setMaximum(maxVal);
		m_pSpinBox->setValue(startVal);

		// Set the single step to a power of 10, with a minimum of 1
		int singleStep = std::max((int)round(CalcSingleStep(range)), 1);
		m_pSpinBox->setSingleStep(singleStep);

		// If this is a button type, connect the button to send the datagram to the game engine
		// If it is not a button type, send the datagram every time the spinbox changes
		if (isButton) {
			QPushButton* pTempButton = dynamic_cast<QPushButton*>(m_pParameterLabel);
			if (pTempButton) {
				connect(pTempButton, &QPushButton::pressed, this, &ParameterControlWidget::ConstructDatagram);
			}
		} else 
		{
			connect(m_pSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ParameterControlWidget::ConstructDatagram);
		}

		m_pLayout->addStretch();
		m_pLayout->addWidget(m_pSpinBox);
	}

private slots:
	void ConstructDatagram() 
	{
		QString datagram = QString("%1;%2;").arg(m_parameterName).arg(m_pSpinBox->value());
		emit DatagramReady(datagram);
	}

private:
	QSpinBox* m_pSpinBox;
};

// --------------------------------------
// Double specialization
// --------------------------------------
template <>
class ParameterControlWidget <double> : public ParameterControlWidgetBase {
public:
	ParameterControlWidget(QStringList createInfo, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
		: ParameterControlWidgetBase(createInfo, parent, f)
	{
		// Parse the input strings
		bool isButton = createInfo.at(1).compare("True", Qt::CaseInsensitive) == 0;
		double minVal = createInfo.at(3).toDouble();
		double startVal = createInfo.at(4).toDouble();
		double maxVal = createInfo.at(5).toDouble();
		double range = maxVal - minVal;

		// Setup the spinbox range and values
		m_pSpinBox = new QDoubleSpinBox(this);
		m_pSpinBox->setMinimum(minVal);
		m_pSpinBox->setMaximum(maxVal);
		m_pSpinBox->setValue(startVal);
		m_pSpinBox->setSingleStep(CalcSingleStep(range));

		// If this is a button type, connect the button to send the datagram to the game engine
		// If it is not a button type, send the datagram every time the spinbox changes
		if (isButton) {
			QPushButton* pTempButton = dynamic_cast<QPushButton*>(m_pParameterLabel);
			if (pTempButton) {
				connect(pTempButton, &QPushButton::pressed, this, &ParameterControlWidget::ConstructDatagram);
			}
		}
		else
		{
			connect(m_pSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ParameterControlWidget::ConstructDatagram);
		}

		m_pLayout->addStretch();
		m_pLayout->addWidget(m_pSpinBox);
	}

private slots:
	void ConstructDatagram()
	{
		QString datagram = QString("%1;%2;").arg(m_parameterName).arg(m_pSpinBox->value());
		emit DatagramReady(datagram);
	}

private:
	QDoubleSpinBox* m_pSpinBox;
};