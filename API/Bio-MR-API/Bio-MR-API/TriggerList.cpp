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
#include <QDialogButtonBox>


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
	QVBoxLayout* pMainLayout = new QVBoxLayout();
	setLayout(pMainLayout);

	// Add the list of view items
	m_pTriggerView = new QListView(this);
	pMainLayout->addWidget(m_pTriggerView);

	// Create the item model
	m_pTriggerModel = new QStandardItemModel(0, 1, this);
	m_pTriggerView->setModel(m_pTriggerModel);

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
	connect(pRemoveButton, &QPushButton::clicked, this, [&] {
		for (QModelIndex& index : m_pTriggerView->selectionModel()->selectedIndexes()) { 
			QStandardItem* pRemovedItem = m_pTriggerModel->itemFromIndex(index);
			TriggerItem* pRemovedTriggerItem = static_cast<TriggerItem*>(pRemovedItem);
			m_pStorageManager->RemoveTrigger(pRemovedTriggerItem->GetTriggerDescription());
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

	QFont smallBold = font();
	smallBold.setBold(true);

	// Create the sensor level section header
	QLabel* pTitle = new QLabel("Create a New Trigger", m_pAddTriggerWindow);
	pTitle->setFont(boldFont);

	QLabel* pDescription = new QLabel("Specify which sensor to watch, a threshold value, and a command to send to the game engine once the threshold is exceeded.", m_pAddTriggerWindow);
	pDescription->setWordWrap(true);

	// Create the sensor level inputs 
	QLabel* pEventSourceLabel = new QLabel("Event Source:", m_pAddTriggerWindow);
	m_pEventSourceInput = new QLineEdit(m_pAddTriggerWindow);
	m_pEventSourceInput->setText("Source");
	QLabel* pSampleNameLabel = new QLabel("Sample Name:", m_pAddTriggerWindow);
	m_pSampleNameInput = new QLineEdit(m_pAddTriggerWindow);
	m_pSampleNameInput->setText("Name");
	QLabel* pFieldIdLabel = new QLabel("Field Id:", m_pAddTriggerWindow);
	m_pFieldIndexInput = new QSpinBox(m_pAddTriggerWindow);

	QLabel* pComparisonFunctionLabel = new QLabel("Comparison:", m_pAddTriggerWindow);
	m_pComparisonFunctionInput = new QComboBox(m_pAddTriggerWindow);
	QStringList comparisonOptions;
	for (ComparisonType x = (ComparisonType)0; x < ComparisonType::k_count; x = (ComparisonType)((int)x + 1)) {
		comparisonOptions << ComparisonFucntionToQString(x);
	}
	m_pComparisonFunctionInput->addItems(comparisonOptions);

	QLabel* pComparisonValueLabel = new QLabel("Threshold Value:", m_pAddTriggerWindow);
	m_pComparisonValueInput = new QLineEdit(m_pAddTriggerWindow);
	m_pComparisonValueInput->setText("0");


	QLabel* pParameterNameLabel = new QLabel("Change Parameter:", m_pAddTriggerWindow);
	m_pParameterNameInput = new QComboBox(m_pAddTriggerWindow);
	QLabel* pParameterValueLabel = new QLabel("Change Value:", m_pAddTriggerWindow);
	m_pParameterValueInput = new QDoubleSpinBox();

	UpdateGameEngineParameterList();
	UpdateGameEngineParameterValue(-1);

	connect(m_pParameterNameInput, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TriggerList::UpdateGameEngineParameterValue);
	connect(m_pStorageManager, &StorageManager::NewGameEngineParameter, this, &TriggerList::UpdateGameEngineParameterList);

	// Create a preview window which updates on any changes
	m_pPreviewTrigger = new QLabel(m_pAddTriggerWindow);
	m_pPreviewTrigger->setAlignment(Qt::AlignCenter);
	m_pPreviewTrigger->setFont(smallBold);
	connect(m_pEventSourceInput, &QLineEdit::textChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pSampleNameInput, &QLineEdit::textChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pFieldIndexInput, QOverload<int>::of(&QSpinBox::valueChanged), this, &TriggerList::UpdatePreviewText);
	connect(m_pComparisonFunctionInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pComparisonValueInput, &QLineEdit::textChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pParameterNameInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pParameterValueInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &TriggerList::UpdatePreviewText);
	UpdatePreviewText();


	// Create the buttons
	QDialogButtonBox* pButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(pButtonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, [&] {
		TriggerItem* pItem = CreateNewTriggerItem();
		m_pTriggerModel->appendRow(pItem);
		m_pStorageManager->AddTrigger(pItem->GetTriggerDescription());
		m_pAddTriggerWindow->hide();
	});
	connect(pButtonBox->button(QDialogButtonBox::Cancel), &QPushButton::pressed, this, [&] {
		m_pAddTriggerWindow->hide();
	});


	// Layout the widget
	QGridLayout* pGridLayout = new QGridLayout(m_pAddTriggerWindow);
	m_pAddTriggerWindow->setLayout(pGridLayout);
	pGridLayout->addWidget(pTitle, 0, 0, 1, -1);
	pGridLayout->addWidget(pDescription, 1, 0, 1, -1);

	// Spacing
	pGridLayout->setRowMinimumHeight(2, 10);

	//pGridLayout->addWidget(pPreviewLabel, 4, 0);
	pGridLayout->addWidget(m_pPreviewTrigger, 4, 0, 1, -1);

	// Spacing
	pGridLayout->setRowMinimumHeight(5, 10);

	pGridLayout->addWidget(pEventSourceLabel, 10, 0);
	pGridLayout->addWidget(m_pEventSourceInput, 10, 1);
	pGridLayout->addWidget(pSampleNameLabel, 11, 0);
	pGridLayout->addWidget(m_pSampleNameInput, 11, 1);
	pGridLayout->addWidget(pFieldIdLabel, 12, 0);
	pGridLayout->addWidget(m_pFieldIndexInput, 12, 1);
	pGridLayout->addWidget(pComparisonFunctionLabel, 13, 0);
	pGridLayout->addWidget(m_pComparisonFunctionInput, 13, 1);
	pGridLayout->addWidget(pComparisonValueLabel, 14, 0);
	pGridLayout->addWidget(m_pComparisonValueInput, 14, 1);

	// Spacing
	pGridLayout->setRowMinimumHeight(20, 10);

	pGridLayout->addWidget(pParameterNameLabel, 30, 0);
	pGridLayout->addWidget(m_pParameterNameInput, 30, 1);
	pGridLayout->addWidget(pParameterValueLabel, 31, 0);
	pGridLayout->addWidget(m_pParameterValueInput, 31, 1);


	pGridLayout->setColumnStretch(1, 1);
	pGridLayout->setRowStretch(99, 1);

	// Button Box
	pGridLayout->addWidget(pButtonBox, 100, 0, 1, -1);

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
	auto allParams = m_pStorageManager->GetGameEngineParameters();

	if (index < 0 || index > allParams.size())
	{
		m_pParameterValueInput->setMinimum(0);
		m_pParameterValueInput->setValue(0);
		m_pParameterValueInput->setMaximum(99);
	} else {
		GameEngineRegisterCommandDatagram& selectedParameter = allParams[index];

		if (selectedParameter.m_type.compare("Int", Qt::CaseInsensitive) == 0) {
			m_pParameterValueInput->setMinimum(selectedParameter.m_minVal.toInt());
			m_pParameterValueInput->setValue(selectedParameter.m_startVal.toInt());
			m_pParameterValueInput->setMaximum(selectedParameter.m_maxVal.toInt());
			m_pParameterValueInput->setDecimals(0);
		}
		else if (selectedParameter.m_type.compare("Float", Qt::CaseInsensitive) == 0 || selectedParameter.m_type.compare("Double", Qt::CaseInsensitive) == 0) {
			m_pParameterValueInput->setMinimum(selectedParameter.m_minVal.toDouble());
			m_pParameterValueInput->setValue(selectedParameter.m_startVal.toDouble());
			m_pParameterValueInput->setMaximum(selectedParameter.m_maxVal.toDouble());
			m_pParameterValueInput->setDecimals(2);
		}
	}
}


TriggerItem* TriggerList::CreateNewTriggerItem() {
	TriggerDescription* pDescription = new TriggerDescription();

	// Sensor input
	pDescription->m_eventSource = m_pEventSourceInput->text();
	pDescription->m_sampleName = m_pSampleNameInput->text();
	pDescription->m_fieldIndex = m_pFieldIndexInput->value();

	pDescription->m_comparisionFunction = (ComparisonType)m_pComparisonFunctionInput->currentIndex();
	pDescription->m_comparisonValue = m_pComparisonValueInput->text();

	// Game engine input
	pDescription->m_parameterName = m_pParameterNameInput->currentText();
	pDescription->m_parameterValue = QString("%1").arg(m_pParameterValueInput->value());

	return new TriggerItem(pDescription);
}

void TriggerList::UpdatePreviewText()
{
	TriggerItem* pItem = CreateNewTriggerItem();
	m_pPreviewTrigger->setText(pItem->text());
	delete pItem;
}


TriggerItem::TriggerItem(TriggerDescription* desc)
	: QStandardItem(), m_pDescription(desc)
{
	QString objectText = QString("If %1::%2[%3] %4 %5, set %6 to %7")
		.arg(desc->m_eventSource)
		.arg(desc->m_sampleName)
		.arg(desc->m_fieldIndex)
		.arg(ComparisonFucntionToQString(desc->m_comparisionFunction))
		.arg(desc->m_comparisonValue)
		.arg(desc->m_parameterName)
		.arg(desc->m_parameterValue);
	setText(objectText);
}

TriggerItem::~TriggerItem() {
	delete m_pDescription;
}