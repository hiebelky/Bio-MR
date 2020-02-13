#include "Biomr.h"

#include "NetworkManager.h"
#include "ParameterControlWidget.h"
#include "StorageManager.h"


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
	ParameterControlWidgetBase* pTempControlWidget = nullptr;

	// Determine the type of the parameter, and create
	// a templated widget using that type
	if (params.at(2).compare("Int", Qt::CaseInsensitive) == 0) {
		pTempControlWidget = new ParameterControlWidget<int>(params, this);
	}
	else if (params.at(2).compare("Float", Qt::CaseInsensitive) == 0 || params.at(2).compare("Double", Qt::CaseInsensitive) == 0) {
		pTempControlWidget = new ParameterControlWidget<double>(params, this);
	}

	if (!pTempControlWidget) {
		return;
	}

	// Alert the network manager when new data is ready for transmit
	connect(pTempControlWidget, &ParameterControlWidgetBase::DatagramReady, m_pNetworkManager, &NetworkManager::SendGameEngineDatagram);

	// Add the new widget to the layout.
	// Place it in the 2nd to last position.
	// The last position is a resizable vertical spacer.
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

