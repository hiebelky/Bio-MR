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
	// Create a new parameter control widget
	ParameterControlWidget* pTempControlWidget = new ParameterControlWidget(params, this);

	// When adding a new parameter control widget, store the information about
	// paramter name, type, min, max to populate combo boxes later
	m_pStorageManager->AddGameEngineParameter(params, pTempControlWidget);

	// Alert the network manager when the user modifies a value in a parameter control widget
	// Automatically send the packet
	connect(pTempControlWidget, &ParameterControlWidget::DatagramReady, m_pNetworkManager, &NetworkManager::SendGameEngineDatagram);

	// Add the new widget to the layout.
	// Place it in the 2nd to last position.
	// The last position is a resizable vertical spacer.
	int lastIndex = ui.manualControlLayout->count() - 1;
	ui.manualControlLayout->insertWidget(lastIndex, pTempControlWidget);
}



void Biomr::HandleAutomaticTriggers(IMotionsDatagram& datagram)
{
	auto& allTriggers = m_pStorageManager->GetAllTriggers();
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

		bool isCompareValueNumber;
		it->m_comparisonValue.toDouble(&isCompareValueNumber);

		bool isDataValueNumber;
		datagram.m_rawData[it->m_fieldIndex].toDouble(&isDataValueNumber);

		if (isDataValueNumber != isCompareValueNumber) {
			continue;
		}

		bool shouldTrigger = false;
		if (isDataValueNumber) {
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
			if (it->m_controlWidget) {
				it->m_controlWidget->UpdateValueExtern(it->m_parameterValue);
			}
		}

	}
}