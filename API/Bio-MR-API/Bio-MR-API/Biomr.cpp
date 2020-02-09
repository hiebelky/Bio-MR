#include "Biomr.h"

#include "NetworkManager.h"
#include "ParameterControlWidget.h"
#include "labeledSlider.h"


Biomr::Biomr(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Set up the network manager
	m_pNetworkManager = new NetworkManager(this);
	connect(m_pNetworkManager, &NetworkManager::registerCommand, this, &Biomr::AddParameterControlWidget);

	// Set up sliders with min/max values
	/*ui.RainIntensitySlider->SetMaxValueDouble(1.0);
	ui.RainIntensitySlider->SetMinValueDouble(0.0);
	ui.RainIntensitySlider->SetNumTicks(100);
	connect(ui.RainIntensitySlider, &LabeledSlider::valueChanged, this, &Biomr::SetRainIntensity);

	ui.DayNightCycleSlider->SetMaxValueDouble(30);
	ui.DayNightCycleSlider->SetMinValueDouble(0.05);
	ui.DayNightCycleSlider->SetNumTicks(1000);
	ui.DayNightCycleSlider->UpdateSliderPosition();
	connect(ui.DayNightCycleSlider, &LabeledSlider::valueChanged, this, &Biomr::SetDayNightCycle);*/
}


void Biomr::AddParameterControlWidget(QStringList& params) 
{
	ParameterControlWidget* pTempControlWidget = new ParameterControlWidget(params, this);

	connect(pTempControlWidget, &ParameterControlWidget::DatagramReady, m_pNetworkManager, &NetworkManager::SendGameEngineDatagram);

	// Insert at the end
	int lastIndex = ui.manualControlLayout->count() - 1;
	ui.manualControlLayout->insertWidget(lastIndex, pTempControlWidget);
}

void Biomr::SetRainIntensity(double val)
{
	QString command = QString("RainIntensity;%1;").arg(val);
	m_pNetworkManager->SendGameEngineDatagram(command);
}

void Biomr::SetDayNightCycle(double val)
{
	QString command = QString("DayLength;%1;").arg(val);
	m_pNetworkManager->SendGameEngineDatagram(command);
}

