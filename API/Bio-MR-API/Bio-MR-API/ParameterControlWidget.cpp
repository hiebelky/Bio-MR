#include "ParameterControlWidget.h"

#include <math.h>
#include <algorithm>

#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QHBoxLayout>

ParameterControlWidget::ParameterControlWidget(QStringList createInfo, QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	// Parse the input strings
	m_parameterName = createInfo.at(0);
	bool isButton = createInfo.at(1).compare("True", Qt::CaseInsensitive) == 0;
	double minVal = createInfo.at(3).toDouble();
	double startVal = createInfo.at(4).toDouble();
	double maxVal = createInfo.at(5).toDouble();
	double range = maxVal - minVal;

	if (createInfo.at(2).compare("Float", Qt::CaseInsensitive) == 0) {
		m_type = InputType::k_Float;
	}
	if (createInfo.at(2).compare("Int", Qt::CaseInsensitive) == 0) {
		m_type = InputType::k_Int;
	}


	// Create the horizontal layout for this widget
	QHBoxLayout* pLayout = new QHBoxLayout(this);
	pLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(pLayout);


	// Create the name either as a button, or a label
	if (isButton) {
		QPushButton* pTempButton = new QPushButton(m_parameterName, this);
		connect(pTempButton, &QPushButton::pressed, this, &ParameterControlWidget::ConstructDatagram);

		m_pParameterLabel = pTempButton;
	}
	else {
		QLabel* pTempLabel = new QLabel(m_parameterName, this);

		m_pParameterLabel = pTempLabel;
	}

	// Add the label to the layout
	pLayout->addWidget(m_pParameterLabel);
	pLayout->addStretch();


	// Create the spin box accordingly
	switch (m_type) {
		case InputType::k_Float:
		{
			// Setup the spinbox using float parameters
			QDoubleSpinBox* pTempSpinBox = new QDoubleSpinBox(this);
			pTempSpinBox->setMinimum(minVal);
			pTempSpinBox->setMaximum(maxVal);
			pTempSpinBox->setValue(startVal);
			pTempSpinBox->setSingleStep(CalcSingleStep(range));

			// Only connect changes to this spinbox if it is NOT a button type
			if (!isButton) {
				connect(pTempSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ParameterControlWidget::OnValueChanged);
			}

			m_pSpinBox = pTempSpinBox;
		}
		break;

		case InputType::k_Int:
		{
			// Setup the spinbox using integer parameters
			QSpinBox* pTempSpinBox = new QSpinBox(this);
			pTempSpinBox->setMinimum((int)round(minVal));
			pTempSpinBox->setMaximum((int)round(maxVal));
			pTempSpinBox->setValue((int)round(startVal));

			// Set the single step to a power of 10, with a minimum of 1
			int singleStep = std::max((int)round(CalcSingleStep(range)), 1);
			pTempSpinBox->setSingleStep(singleStep);

			// Only connect changes to this spinbox if it is NOT a button type
			if (!isButton) {
				connect(pTempSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ParameterControlWidget::OnValueChanged);
			}

			m_pSpinBox = pTempSpinBox;
		}
		break;
	}
	
	pLayout->addWidget(m_pSpinBox);
}

double ParameterControlWidget::CalcSingleStep(double range)
{
	// We want roughly 100 steps in the range
	double target = range / 100.0;

	// Round to the nearest power of 10
	double roundedPower = round(log10(target));

	// Convert back to a step size
	double singleStep = pow(10.0, roundedPower);
	return singleStep;
}

void ParameterControlWidget::OnValueChanged(double val)
{
	ConstructDatagram();
}

void ParameterControlWidget::ConstructDatagram()
{
	// Constructs a datagram which appends parameter name and the spinbox value
	// Ex: m_parameterName;spinBoxValue;
	QString datagram;
	switch (m_type) {
		case InputType::k_Float: {
			QDoubleSpinBox* pTempSpinBox = dynamic_cast<QDoubleSpinBox*>(m_pSpinBox);
			datagram = QString("%1;%2;").arg(m_parameterName).arg(pTempSpinBox->value());
		}
		break;

		case InputType::k_Int: {
			QSpinBox* pTempSpinBox = dynamic_cast<QSpinBox*>(m_pSpinBox);
			datagram = QString("%1;%2;").arg(m_parameterName).arg(pTempSpinBox->value());
		}
		break;
	}

	emit DatagramReady(datagram);
}