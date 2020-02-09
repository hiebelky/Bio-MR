#include "Biomr.h"

#include "NetworkManager.h"
#include "ParameterControlWidget.h"


Biomr::Biomr(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Set up the network manager
	m_pNetworkManager = new NetworkManager(this);
	connect(m_pNetworkManager, &NetworkManager::registerCommand, this, &Biomr::AddParameterControlWidget);
}


void Biomr::AddParameterControlWidget(QStringList& params) 
{
	ParameterControlWidget* pTempControlWidget = new ParameterControlWidget(params, this);

	// Alert the network manager when new data is ready for transmit
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

