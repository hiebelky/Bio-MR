#include "MultipleInputBox.h"

#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QGridLayout>

MultipleInputBox::MultipleInputBox(InputType type, QWidget* parent, Qt::WindowFlags flags)
	: QWidget(parent, flags)
{
	// Setup the layout
	SetType(type);
}

void MultipleInputBox::SetType(InputType type)
{
	// Store the type that we are currently showing
	m_type = type;

	// Replace the current layout with a fresh new one
	QLayout* temp = layout();
	setLayout(new QGridLayout(this));
	layout()->setContentsMargins(0, 0, 0, 0);
	if (temp) {
		delete temp;
		temp = nullptr;
	}
	if (m_pSpinBox) {
		delete m_pSpinBox;
		m_pSpinBox = nullptr;
	}
	if (m_pLineEdit) {
		delete m_pLineEdit;
		m_pLineEdit = nullptr;
	}

	// Only show the widget that is needed
	if (m_type == InputType::k_int || m_type == InputType::k_double)
	{
		// Set up the spin box
		m_pSpinBox = new QDoubleSpinBox(this);
		connect(m_pSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [&](double d) {
			QString newValue = QString("%1").arg(d);
			emit ValueChanged(newValue);
		});
	}
	if (m_type == InputType::k_string) {
		// Set up the line edit
		m_pLineEdit = new QLineEdit(this);
		connect(m_pLineEdit, &QLineEdit::textChanged, this, [&](QString text) {
			emit ValueChanged(text);
		});
	}


	switch (m_type) {
	case InputType::k_int:
	{
		layout()->addWidget(m_pSpinBox);
		m_pSpinBox->setDecimals(0);
		break;
	}
	case InputType::k_double:
	{
		layout()->addWidget(m_pSpinBox);
		m_pSpinBox->setDecimals(2);
		break;
	}
	case InputType::k_string:
	{
		layout()->addWidget(m_pLineEdit);
		break;
	}
	}
}



QString MultipleInputBox::GetValue()
{
	switch (m_type) {
	case InputType::k_int:
	case InputType::k_double:
	{
		if (!m_pSpinBox) {
			return QString();
		}

		QString newValue = QString("%1").arg(m_pSpinBox->value());
		return newValue;
	}
	case InputType::k_string:
	{
		if (!m_pLineEdit) {
			return QString();
		}

		return m_pLineEdit->text();
		break;
	}
	default:
		return QString();
	}
}

void MultipleInputBox::SetValue(QString in)
{
	switch (m_type) {
	case InputType::k_int:
	case InputType::k_double:
	{
		if (!m_pSpinBox) {
			return;
		}

		double value = in.toDouble();
		m_pSpinBox->setValue(value);

		break;
	}
	case InputType::k_string:
	{
		if (!m_pLineEdit) {
			return;
		}

		m_pLineEdit->setText(in);

		break;
	}
	}
}


void MultipleInputBox::SetMaxValue(double in)
{
	if (!m_pSpinBox) {
		return;
	}
	m_pSpinBox->setMaximum(in);
	UpdateSpinBoxRange();
}

void MultipleInputBox::SetMinValue(double in)
{
	if (!m_pSpinBox) {
		return;
	}
	m_pSpinBox->setMinimum(in);
	UpdateSpinBoxRange();
}

void MultipleInputBox::UpdateSpinBoxRange() {
	if (!m_pSpinBox) {
		return;
	}

	// Get the range of the spinbox in double
	double range = m_pSpinBox->maximum() - m_pSpinBox->minimum();

	// We want roughly 100 steps in the range
	double target = range / 100.0;

	// Round to the nearest power of 10
	double roundedPower = round(log10(target));

	// Convert back to a step size
	double singleStep = pow(10.0, roundedPower);

	if (m_type == InputType::k_int) {
		// Set the single step to an integer >= 1
		int intSingleStep = std::max((int)round(singleStep), 1);
		m_pSpinBox->setSingleStep(intSingleStep);
	} else {
		m_pSpinBox->setSingleStep(singleStep);
	}
}