#include "ParameterControlWidget.h"

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
	m_isButton = createInfo.at(1).compare("True", Qt::CaseInsensitive) == 0;
	m_minVal = createInfo.at(3).toDouble();
	m_startVal = createInfo.at(4).toDouble();
	m_maxVal = createInfo.at(5).toDouble();

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


	// Create the widgets accordingly
	if (m_isButton) {
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
			QDoubleSpinBox* pTempSpinBox = new QDoubleSpinBox(this);
			pTempSpinBox->setMinimum(m_minVal);
			pTempSpinBox->setMaximum(m_maxVal);
			pTempSpinBox->setValue(m_startVal);

			if (!m_isButton) {
				connect(pTempSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ParameterControlWidget::OnValueChanged);
			}

			m_pSpinBox = pTempSpinBox;
		}
		break;

		case InputType::k_Int:
		{
			QSpinBox* pTempSpinBox = new QSpinBox(this);
			pTempSpinBox->setMinimum((int)m_minVal);
			pTempSpinBox->setMaximum((int)m_maxVal);
			pTempSpinBox->setValue((int)m_startVal);

			if (!m_isButton) {
				connect(pTempSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ParameterControlWidget::OnValueChanged);
			}

			m_pSpinBox = pTempSpinBox;
		}
		break;
	}
	
	pLayout->addWidget(m_pSpinBox);
}

void ParameterControlWidget::OnValueChanged(double val)
{
	ConstructDatagram();
}

void ParameterControlWidget::ConstructDatagram()
{
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