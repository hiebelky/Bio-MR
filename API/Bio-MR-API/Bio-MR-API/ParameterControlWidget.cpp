#include "ParameterControlWidget.h"

#include "Types.h"
#include "MultipleInputBox.h"

#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QTimer>

ParameterControlWidget::ParameterControlWidget(GameEngineRegisterCommandDatagram& createInfo, QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	setAutoFillBackground(true);
	startingBgColor = palette().color(QPalette::Background);
	m_pTimer = new QTimer(this);
	m_pTimer->setInterval(bgTimerInterval);
	connect(m_pTimer, &QTimer::timeout, this, &ParameterControlWidget::UpdateBackgroundColor);


	// Parse the parameter name and button type
	m_parameterName = createInfo.m_parameterName;
	bool isButton = createInfo.m_isButton.compare("True", Qt::CaseInsensitive) == 0;

	// Create the horizontal layout for this widget
	m_pLayout = new QHBoxLayout(this);
	m_pLayout->setContentsMargins(3, 0, 3, 0);
	setLayout(m_pLayout);

	// Create the name either as a button, or a label
	if (isButton) {
		QPushButton* pTempButton = new QPushButton(m_parameterName, this);
		m_pParameterLabel = pTempButton;

		// If this is a button type, only send data upon the button being pressed
		connect(pTempButton, &QPushButton::pressed, this, &ParameterControlWidget::ConstructDatagram);
	}
	else {
		m_pParameterLabel = new QLabel(m_parameterName, this);
	}

	// Add the label to the layout
	m_pLayout->addWidget(m_pParameterLabel);


	// Determine the type of the parameter, and create a templated widget using that type
	if (createInfo.m_type.compare("Int", Qt::CaseInsensitive) == 0) {
		m_pInputBox = new MultipleInputBox(InputType::k_int, this);
	}
	else if (createInfo.m_type.compare("Float", Qt::CaseInsensitive) == 0 || createInfo.m_type.compare("Double", Qt::CaseInsensitive) == 0) {
		m_pInputBox = new MultipleInputBox(InputType::k_double, this);
	}
	else {
		m_pInputBox = new MultipleInputBox(InputType::k_string, this);
	}

	// Setup the input box
	m_pInputBox->SetMinValue(createInfo.m_minVal.toDouble());
	m_pInputBox->SetValue(createInfo.m_startVal);
	m_pInputBox->SetMaxValue(createInfo.m_maxVal.toDouble());

	// Setup the signal only if this is not a button widget
	if (!isButton) {
		connect(m_pInputBox, &MultipleInputBox::ValueChanged, this, &ParameterControlWidget::ConstructDatagram);
	}

	// Add the input box to the layout
	m_pLayout->addStretch();
	m_pLayout->addWidget(m_pInputBox);
}


void ParameterControlWidget::ConstructDatagram()
{
	QString datagram = QString("%1;%2;").arg(m_parameterName).arg(m_pInputBox->GetValue());
	emit DatagramReady(datagram);
}

void ParameterControlWidget::UpdateValueExtern(QString& value)
{
	// Block signals to prevent duplicate datagrams
	m_pInputBox->blockSignals(true);
	m_pInputBox->SetValue(value);
	m_pInputBox->blockSignals(false);
	ConstructDatagram();

	blendPercent = 1.f;
	m_pTimer->start();
}


void ParameterControlWidget::UpdateBackgroundColor()
{
	// Adjust the blend %
	blendPercent -= (double)bgTimerInterval / (double)bgTotalTime;
	if (blendPercent <= 0) {
		blendPercent = 0;
		m_pTimer->stop();
	}

	// Find the blending
	int r = startingBgColor.red() * (1. - blendPercent) + bgColor.red() * blendPercent;
	int g = startingBgColor.green() * (1. - blendPercent) + bgColor.green() * blendPercent;
	int b = startingBgColor.blue() * (1. - blendPercent) + bgColor.blue() * blendPercent;

	QColor blendedColor = QColor(r, g, b);

	QPalette p = palette();
	p.setColor(QPalette::Background, blendedColor);
	setPalette(p);
}