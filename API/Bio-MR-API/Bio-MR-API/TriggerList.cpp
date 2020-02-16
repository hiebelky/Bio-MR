#include "TriggerList.h"

#include "Types.h"

#include <QListView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>


QString ComparisonFucntionToQString(ComparisonType type) {
	switch (type) {
	case ComparisonType::k_less: return "<";
	case ComparisonType::k_lessEqual: return "<=";
	case ComparisonType::k_equal: return "==";
	case ComparisonType::k_greaterEqual: return ">=";
	case ComparisonType::k_greater: return ">";
	default: return "";
	}
}


TriggerList::TriggerList(StorageManager* sm, QWidget* parent) : QWidget(parent), m_pStorageManager(sm)
{
	// Create the main layout
	QVBoxLayout* m_pMainLayout = new QVBoxLayout();
	setLayout(m_pMainLayout);

	// Add the list of view items
	m_pTriggerView = new QListView(this);
	m_pMainLayout->addWidget(m_pTriggerView);

	// Create the item model
	m_pTriggerModel = new QStandardItemModel(0, 1, this);
	m_pTriggerView->setModel(m_pTriggerModel);

	// Remove later
	QStandardItem* testRow = new TriggerItem<int>("Shimmer", "GSR", "Heart Rate", 0, ComparisonType::k_greaterEqual, 80, "Rain Intensity", "0.50");
	m_pTriggerModel->appendRow(testRow);

	// Create the add/remove buttons
	QWidget* pAddRemoveRow = new QWidget(this);
	QPushButton* pAddButton = new QPushButton("Add", this);
	QPushButton* pRemoveButton = new QPushButton("Remove", this);

	QHBoxLayout* pAddRemoveRowLayout = new QHBoxLayout(this);
	pAddRemoveRowLayout->setContentsMargins(0, 0, 0, 0);
	pAddRemoveRow->setLayout(pAddRemoveRowLayout);
	pAddRemoveRowLayout->addWidget(pAddButton);
	pAddRemoveRowLayout->addWidget(pRemoveButton);
	m_pMainLayout->addWidget(pAddRemoveRow);

	// Set up button actions
	connect(pRemoveButton, &QPushButton::pressed, this, [&] {
		for (QModelIndex& index : m_pTriggerView->selectionModel()->selectedIndexes()) {
			m_pTriggerModel->removeRow(index.row());
		}
	});

	SetUpTriggerWindow();

	connect(pAddButton, &QPushButton::pressed, this, [&] {
		if (m_pAddTriggerWindow) {
			m_pAddTriggerWindow->show();
		}
	});
}

void TriggerList::SetUpTriggerWindow() {
	m_pAddTriggerWindow = new QWidget();

	m_pMainLayout = new QGridLayout(m_pAddTriggerWindow);
	m_pAddTriggerWindow->setLayout(m_pMainLayout);

	// Create a bold font
	QFont boldFont = font();
	boldFont.setBold(true);
	boldFont.setPointSize(12);

	// Create the sensor level section header
	QLabel* pSensorLevel = new QLabel("Sensor Level", m_pAddTriggerWindow);
	pSensorLevel->setFont(boldFont);

	// Create the sensor level inputs 
	QLabel* pEventSourceLabel = new QLabel("Event Source:", m_pAddTriggerWindow);
	m_pEventSourceInput = new QLineEdit(m_pAddTriggerWindow);
	QLabel* pSampleNameLabel = new QLabel("Sample Name:", m_pAddTriggerWindow);
	m_pSampleNameInput = new QLineEdit(m_pAddTriggerWindow);
	QLabel* pFieldIdLabel = new QLabel("Field Id:", m_pAddTriggerWindow);
	m_pFieldIndexInput = new QSpinBox(m_pAddTriggerWindow);

	QLabel* pComparisonFunctionLabel = new QLabel("Comparison Function:", m_pAddTriggerWindow);
	m_pComparisonFunctionInput = new QComboBox(m_pAddTriggerWindow);
	QStringList comparisonOptions;
	for (ComparisonType x = (ComparisonType)0; x < ComparisonType::k_count; x = (ComparisonType)((int)x + 1)) {
		comparisonOptions << ComparisonFucntionToQString(x);
	}
	m_pComparisonFunctionInput->addItems(comparisonOptions);

	QLabel* pComparisonValueLabel = new QLabel("Comparison Value:", m_pAddTriggerWindow);
	m_pComparisonValueInput = new QLineEdit(m_pAddTriggerWindow);


	// Create the game engine response
	QLabel* pGameEngineCommandLabel = new QLabel("Response", m_pAddTriggerWindow);
	pGameEngineCommandLabel->setFont(boldFont);

	QLabel* pParameterNameLabel = new QLabel("Parameter Name", m_pAddTriggerWindow);
	m_pParameterNameInput = new QComboBox(m_pAddTriggerWindow);
	QLabel* pParameterValueLabel = new QLabel("Parameter Value", m_pAddTriggerWindow);

	UpdateGameEngineParameterList();
	UpdateGameEngineParameterValue(-1);

	connect(m_pParameterNameInput, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TriggerList::UpdateGameEngineParameterValue);
	connect(m_pStorageManager, &StorageManager::NewGameEngineParameter, this, &TriggerList::UpdateGameEngineParameterList);


	m_pMainLayout->addWidget(pSensorLevel, 0, 0, 1, -1);
	m_pMainLayout->addWidget(pEventSourceLabel, 1, 0);
	m_pMainLayout->addWidget(m_pEventSourceInput, 1, 1);
	m_pMainLayout->addWidget(pSampleNameLabel, 2, 0);
	m_pMainLayout->addWidget(m_pSampleNameInput, 2, 1);
	m_pMainLayout->addWidget(pFieldIdLabel, 3, 0);
	m_pMainLayout->addWidget(m_pFieldIndexInput, 3, 1);
	m_pMainLayout->addWidget(pComparisonFunctionLabel, 4, 0);
	m_pMainLayout->addWidget(m_pComparisonFunctionInput, 4, 1);
	m_pMainLayout->addWidget(pComparisonValueLabel, 5, 0);
	m_pMainLayout->addWidget(m_pComparisonValueInput, 5, 1);

	m_pMainLayout->addWidget(pGameEngineCommandLabel, 6, 0, 1, -1);
	m_pMainLayout->addWidget(pParameterNameLabel, 7, 0);
	m_pMainLayout->addWidget(m_pParameterNameInput, 7, 1);
	m_pMainLayout->addWidget(pParameterValueLabel, 8, 0);


	m_pMainLayout->setColumnStretch(1, 1);
	m_pMainLayout->setRowStretch(99, 1);

}

void TriggerList::UpdateGameEngineParameterList()
{
	auto allParams = m_pStorageManager->GetGameEngineParameters();

	// Update the paramater name combo box
	QStringList names;
	for (auto it : allParams) {
		names << it.m_parameterName;
	}
	
	m_pParameterNameInput->clear();
	m_pParameterNameInput->addItems(names);
}

void TriggerList::UpdateGameEngineParameterValue(int index)
{
	// Update the parameter value spinbox
	if (m_pParameterValueInput) {
		delete m_pParameterValueInput;
	}

	auto allParams = m_pStorageManager->GetGameEngineParameters();

	if (index < 0 || index > allParams.size())
	{
		// default
		m_pParameterValueInput = new QSpinBox();
	}
	else {
		GameEngineRegisterCommandDatagram& selectedParameter = allParams[index];

		if (selectedParameter.m_type.compare("Int", Qt::CaseInsensitive) == 0) {
			QSpinBox* pTempSpinBox = new QSpinBox();
			pTempSpinBox->setMinimum(selectedParameter.m_minVal.toInt());
			pTempSpinBox->setValue(selectedParameter.m_startVal.toInt());
			pTempSpinBox->setMaximum(selectedParameter.m_maxVal.toInt());
			m_pParameterValueInput = pTempSpinBox;
		}
		else if (selectedParameter.m_type.compare("Float", Qt::CaseInsensitive) == 0 || selectedParameter.m_type.compare("Double", Qt::CaseInsensitive) == 0) {
			QDoubleSpinBox* pTempSpinBox = new QDoubleSpinBox();
			pTempSpinBox->setMinimum(selectedParameter.m_minVal.toDouble());
			pTempSpinBox->setValue(selectedParameter.m_startVal.toDouble());
			pTempSpinBox->setMaximum(selectedParameter.m_maxVal.toDouble());
			m_pParameterValueInput = pTempSpinBox;
		}
	}


	m_pMainLayout->addWidget(m_pParameterValueInput, 8, 1);
}