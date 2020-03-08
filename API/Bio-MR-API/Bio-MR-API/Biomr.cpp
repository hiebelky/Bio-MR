#include "Biomr.h"

#include "NetworkManager.h"
#include "ParameterControlWidget.h"
#include "StorageManager.h"
#include "TriggerList.h"
#include "DatagramCounter.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QMenuBar>
#include <QStatusBar>

Biomr::Biomr(QWidget* parent)
	: QMainWindow(parent)
{
	// Set up the network manager
	m_pNetworkManager = new NetworkManager(this);
	connect(m_pNetworkManager, &NetworkManager::registerGameEngineCommand, this, &Biomr::AddParameterControlWidget);
	connect(m_pNetworkManager, &NetworkManager::imotionsDataRecieved, this, &Biomr::HandleAutomaticTriggers);

	// Set up the storage manager
	m_pStorageManager = new StorageManager();


	// Set up the central widget
	QWidget* pCentralWidget = new QWidget(this);
	m_pMainLayout = new QGridLayout(this);
	pCentralWidget->setLayout(m_pMainLayout);
	setCentralWidget(pCentralWidget);

	// Build the other layouts
	m_pAutomaticTriggerLayout = new QVBoxLayout(this);
	m_pManualControlLayout = new QVBoxLayout(this);
	DatagramCounter* pDatagramCounter = new DatagramCounter(m_pNetworkManager, this);
	QFrame* pHorizontalDivider = new QFrame(this);
	pHorizontalDivider->setLineWidth(1);
	pHorizontalDivider->setFrameShape(QFrame::HLine);
	pHorizontalDivider->setFrameShadow(QFrame::Sunken);
	QFrame* pVerticalDivider = new QFrame(this);
	pVerticalDivider->setLineWidth(1);
	pVerticalDivider->setFrameShape(QFrame::VLine);
	pVerticalDivider->setFrameShadow(QFrame::Sunken);

	// Build the main layout
	m_pMainLayout->addLayout(m_pAutomaticTriggerLayout, 0, 0);
	m_pMainLayout->addWidget(pVerticalDivider, 0, 1);
	m_pMainLayout->addLayout(m_pManualControlLayout, 0, 2);
	m_pMainLayout->addWidget(pHorizontalDivider, 1, 0, 1, -1);
	m_pMainLayout->addWidget(pDatagramCounter, 2, 0, 1, -1);


	// Create a bold font
	QFont boldFont = font();
	boldFont.setBold(true);
	boldFont.setPointSize(12);

	// Set up the automatic trigger side
	QLabel* pAutomaticLabel = new QLabel("Automatic Triggers");
	pAutomaticLabel->setFont(boldFont);
	pAutomaticLabel->setAlignment(Qt::AlignCenter);
	TriggerList* pTriggerList = new TriggerList(m_pStorageManager, this);
	m_pAutomaticTriggerLayout->addWidget(pAutomaticLabel);
	m_pAutomaticTriggerLayout->addWidget(pTriggerList);
	m_pAutomaticTriggerLayout->addStretch();

	// Set up the manual control side
	QLabel* pManualLabel = new QLabel("Manual Control");
	pManualLabel->setFont(boldFont);
	pManualLabel->setAlignment(Qt::AlignCenter);
	m_pManualControlLayout->addWidget(pManualLabel);
	m_pManualControlLayout->addStretch();

	// Set up the menu bar
	QMenu* pFileMenu = menuBar()->addMenu("File");
	QMenu* pEditMenu = menuBar()->addMenu("Edit");

	QAction* pPassthroughMode = new QAction("&Passthrough Mode", this);
	pPassthroughMode->setCheckable(true);
	pPassthroughMode->setShortcut(QKeySequence::Print);
	pPassthroughMode->setStatusTip("Forward all packets from iMotions to the Game Engine");
	connect(pPassthroughMode, &QAction::triggered, m_pNetworkManager, &NetworkManager::SetPassthroughMode);
	pEditMenu->addAction(pPassthroughMode);
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
	int lastIndex = m_pManualControlLayout->count() - 1;
	m_pManualControlLayout->insertWidget(lastIndex, pTempControlWidget);
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