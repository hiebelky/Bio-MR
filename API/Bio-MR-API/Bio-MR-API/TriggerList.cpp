#include "TriggerList.h"


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


TriggerList::TriggerList(QWidget* parent) : QWidget(parent)
{
	// Creaate the main layout
	QVBoxLayout* pMainLayout = new QVBoxLayout();
	setLayout(pMainLayout);

	// Add the list of view items
	m_pTriggerView = new QListView(this);
	pMainLayout->addWidget(m_pTriggerView);

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
	pMainLayout->addWidget(pAddRemoveRow);

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





	QGridLayout* pMainLayout = new QGridLayout(m_pAddTriggerWindow);
	m_pAddTriggerWindow->setLayout(pMainLayout);
	pMainLayout->addWidget(pSensorLevel, 0, 0, 1, -1);
	pMainLayout->addWidget(pEventSourceLabel, 1, 0);
	pMainLayout->addWidget(m_pEventSourceInput, 1, 1);
	pMainLayout->addWidget(pSampleNameLabel, 2, 0);
	pMainLayout->addWidget(m_pSampleNameInput, 2, 1);
	pMainLayout->addWidget(pFieldIdLabel, 3, 0);
	pMainLayout->addWidget(m_pFieldIndexInput, 3, 1);
	pMainLayout->addWidget(pComparisonFunctionLabel, 4, 0);
	pMainLayout->addWidget(m_pComparisonFunctionInput, 4, 1);
	pMainLayout->addWidget(pComparisonValueLabel, 5, 0);
	pMainLayout->addWidget(m_pComparisonValueInput, 5, 1);

	pMainLayout->addWidget(pGameEngineCommandLabel, 6, 0, 1, -1);


	pMainLayout->setColumnStretch(1, 1);
	pMainLayout->setRowStretch(7, 1); // 1 larger than max

}
