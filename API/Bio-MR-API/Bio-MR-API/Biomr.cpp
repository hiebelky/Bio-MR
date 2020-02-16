#include "Biomr.h"

#include "NetworkManager.h"
#include "ParameterControlWidget.h"
#include "StorageManager.h"
#include "TriggerList.h"

Biomr::Biomr(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Set up the network manager
	m_pNetworkManager = new NetworkManager(this);
	connect(m_pNetworkManager, &NetworkManager::registerGameEngineCommand, this, &Biomr::AddParameterControlWidget);

	// Set up the storage manager
	m_pStorageManager = new StorageManager();

	// Set up the automatic trigger reader
	TriggerList* pTriggerList = new TriggerList(this);
	int lastIndex = ui.triggerLayout->count() - 1;
	ui.triggerLayout->insertWidget(lastIndex, pTriggerList);
}


void Biomr::AddParameterControlWidget(GameEngineRegisterCommandDatagram& params)
{
	ParameterControlWidgetBase* pTempControlWidget = nullptr;

	// Determine the type of the parameter, and create
	// a templated widget using that type
	if (params.m_type.compare("Int", Qt::CaseInsensitive) == 0) {
		pTempControlWidget = new ParameterControlWidget<int>(params, this);
	}
	else if (params.m_type.compare("Float", Qt::CaseInsensitive) == 0 || params.m_type.compare("Double", Qt::CaseInsensitive) == 0) {
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
