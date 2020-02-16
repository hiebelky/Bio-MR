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
	TriggerList* pTriggerList = new TriggerList(m_pStorageManager, this);
	int lastIndex = ui.triggerLayout->count() - 1;
	ui.triggerLayout->insertWidget(lastIndex, pTriggerList);

	// Set up a function to handle automatic triggers
	connect(m_pNetworkManager, &NetworkManager::imotionsDataRecieved, this, &Biomr::HandleAutomaticTriggers);
}


void Biomr::AddParameterControlWidget(GameEngineRegisterCommandDatagram& params)
{
	ParameterControlWidgetBase* pTempControlWidget = nullptr;

	m_pStorageManager->AddGameEngineParameter(params);

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



void Biomr::HandleAutomaticTriggers(IMotionsDatagram& datagram)
{
	auto allTriggers = m_pStorageManager->GetAllTriggers();
	for (auto it : allTriggers) {
		if (datagram.m_eventSource.compare(it->m_eventSource, Qt::CaseInsensitive) != 0) {
			continue;
		}
		if (datagram.m_sampleName.compare(it->m_sampleName, Qt::CaseInsensitive) != 0) {
			continue;
		}
		if (datagram.m_rawData.size() <= it->m_fieldIndex) {
			continue;
		}
		
		QString value = datagram.m_rawData[it->m_fieldIndex];

		bool isNumber;
		it->m_comparisonValue.toDouble(&isNumber);

		bool shouldTrigger = false;
		if (isNumber) {
			double dataValue = datagram.m_rawData[it->m_fieldIndex].toDouble();
			double compareValue = it->m_comparisonValue.toDouble();

			switch (it->m_comparisionFunction) {
			case ComparisonType::k_less:
				shouldTrigger = dataValue < compareValue;
				break;
			case ComparisonType::k_lessEqual:
				shouldTrigger = dataValue <= compareValue;
				break;
			case ComparisonType::k_equal:
				shouldTrigger = dataValue == compareValue;
				break;
			case ComparisonType::k_greaterEqual:
				shouldTrigger = dataValue >= compareValue;
				break;
			case ComparisonType::k_greater:
				shouldTrigger = dataValue > compareValue;
				break;
			}
		}
		else {
			QString dataValue = datagram.m_rawData[it->m_fieldIndex];
			QString compareValue = it->m_comparisonValue;

			switch (it->m_comparisionFunction) {
			case ComparisonType::k_less:
				shouldTrigger = dataValue.compare(compareValue, Qt::CaseInsensitive) < 0;
				break;
			case ComparisonType::k_lessEqual:
				shouldTrigger = dataValue.compare(compareValue, Qt::CaseInsensitive) <= 0;
				break;
			case ComparisonType::k_equal:
				shouldTrigger = dataValue.compare(compareValue, Qt::CaseInsensitive) == 0;
				break;
			case ComparisonType::k_greaterEqual:
				shouldTrigger = dataValue.compare(compareValue, Qt::CaseInsensitive) >= 0;
				break;
			case ComparisonType::k_greater:
				shouldTrigger = dataValue.compare(compareValue, Qt::CaseInsensitive) > 0;
				break;
			}
		}

		if (shouldTrigger) {
			QString triggerMessage = QString("%1;%2;").arg(it->m_parameterName).arg(it->m_parameterValue);
			m_pNetworkManager->SendGameEngineDatagram(triggerMessage);
		}

	}
}