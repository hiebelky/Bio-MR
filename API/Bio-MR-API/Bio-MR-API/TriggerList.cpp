#include "TriggerList.h"

#include "Types.h"
#include "MultipleInputBox.h"

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
#include <QTabWidget>

namespace{
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



	// Tab Widget - Left tab is combo boxes (Preset Sensor) 
	//            - Right tab is manual entry (Custom Sensor)
	QTabWidget* pTabWidget = new QTabWidget(m_pAddTriggerWindow);
	QWidget* pPresetTab = new QWidget(pTabWidget);
	QWidget* pCustomTab = new QWidget(pTabWidget);
	pTabWidget->addTab(pPresetTab, "Preset Sensor");
	pTabWidget->addTab(pCustomTab, "Custom Sensor");

	// Left tab (Preset Sensor)
	{




	}

	// Right tab (Manual Entry)
	{
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
		m_pParameterValueInput = new MultipleInputBox(InputType::k_int, m_pAddTriggerWindow);

		UpdateGameEngineParameterList();
		UpdateGameEngineParameterValue(-1);

		connect(m_pParameterNameInput, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TriggerList::UpdateGameEngineParameterValue);
		connect(m_pStorageManager, &StorageManager::NewGameEngineParameter, this, &TriggerList::UpdateGameEngineParameterList);


		// Layout the widget
		QGridLayout* pGridLayout = new QGridLayout(pPresetTab);
		pCustomTab->setLayout(pGridLayout);

		pGridLayout->addWidget(pEventSourceLabel, 0, 0);
		pGridLayout->addWidget(m_pEventSourceInput, 0, 1);
		pGridLayout->addWidget(pSampleNameLabel, 1, 0);
		pGridLayout->addWidget(m_pSampleNameInput, 1, 1);
		pGridLayout->addWidget(pFieldIdLabel, 2, 0);
		pGridLayout->addWidget(m_pFieldIndexInput, 2, 1);
		pGridLayout->addWidget(pComparisonFunctionLabel, 3, 0);
		pGridLayout->addWidget(m_pComparisonFunctionInput, 3, 1);
		pGridLayout->addWidget(pComparisonValueLabel, 4, 0);
		pGridLayout->addWidget(m_pComparisonValueInput, 4, 1);

		// Spacing
		pGridLayout->setRowMinimumHeight(5, 10);

		pGridLayout->addWidget(pParameterNameLabel, 6, 0);
		pGridLayout->addWidget(m_pParameterNameInput, 6, 1);
		pGridLayout->addWidget(pParameterValueLabel, 7, 0);
		pGridLayout->addWidget(m_pParameterValueInput, 7, 1);


		pGridLayout->setColumnStretch(1, 1);
		pGridLayout->setRowStretch(99, 1);
	}




	// Create the preview widget which updates when any of the boxes are changed
	m_pPreviewTrigger = new QLabel(m_pAddTriggerWindow);
	m_pPreviewTrigger->setAlignment(Qt::AlignCenter);
	m_pPreviewTrigger->setFont(smallBold);
	connect(m_pEventSourceInput, &QLineEdit::textChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pSampleNameInput, &QLineEdit::textChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pFieldIndexInput, QOverload<int>::of(&QSpinBox::valueChanged), this, &TriggerList::UpdatePreviewText);
	connect(m_pComparisonFunctionInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pComparisonValueInput, &QLineEdit::textChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pParameterNameInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pParameterValueInput, &MultipleInputBox::ValueChanged, this, &TriggerList::UpdatePreviewText);
	UpdatePreviewText();



	// Create the bottom navigation buttons buttons
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


	// Main layout
	QVBoxLayout* pMainLayout = new QVBoxLayout(m_pAddTriggerWindow);
	m_pAddTriggerWindow->setLayout(pMainLayout);
	pMainLayout->addWidget(pTitle);
	pMainLayout->addWidget(pDescription);
	pMainLayout->addSpacing(10);
	pMainLayout->addWidget(m_pPreviewTrigger);
	pMainLayout->addSpacing(10);
	pMainLayout->addWidget(pTabWidget);
	pMainLayout->addStretch();
	pMainLayout->addWidget(pButtonBox);
}

void TriggerList::UpdateGameEngineParameterList()
{
	auto& allParams = m_pStorageManager->GetGameEngineParameters();

	// Update the paramater name combo box
	QStringList names;
	for (auto& it : allParams) {
		names << it.first.m_parameterName;
	}
	
	m_pParameterNameInput->clear();
	m_pParameterNameInput->addItems(names);
}

void TriggerList::UpdateGameEngineParameterValue(int index)
{
	auto& allParams = m_pStorageManager->GetGameEngineParameters();

	if (index < 0 || index > allParams.size())
	{
		m_pParameterValueInput->SetMinValue(0);
		m_pParameterValueInput->SetValue(0);
		m_pParameterValueInput->SetMaxValue(99);
	} else {
		GameEngineRegisterCommandDatagram& selectedParameter = allParams[index].first;

		if (selectedParameter.m_type.compare("Int", Qt::CaseInsensitive) == 0) {
			m_pParameterValueInput->SetType(InputType::k_int);
			m_pParameterValueInput->SetMinValue(selectedParameter.m_minVal.toDouble());
			m_pParameterValueInput->SetValue(selectedParameter.m_startVal);
			m_pParameterValueInput->SetMaxValue(selectedParameter.m_maxVal.toDouble());
		}
		else if (selectedParameter.m_type.compare("Float", Qt::CaseInsensitive) == 0 || selectedParameter.m_type.compare("Double", Qt::CaseInsensitive) == 0) {
			m_pParameterValueInput->SetType(InputType::k_double);
			m_pParameterValueInput->SetMinValue(selectedParameter.m_minVal.toDouble());
			m_pParameterValueInput->SetValue(selectedParameter.m_startVal);
			m_pParameterValueInput->SetMaxValue(selectedParameter.m_maxVal.toDouble());
		}
		else if (selectedParameter.m_type.compare("String", Qt::CaseInsensitive) == 0)
		{
			m_pParameterValueInput->SetType(InputType::k_string);
			m_pParameterValueInput->SetValue(selectedParameter.m_startVal);
		}
	}
}

std::pair<GameEngineRegisterCommandDatagram, ParameterControlWidget*>& TriggerList::GetSelectedGameEngineParameters()
{
	auto& allParams = m_pStorageManager->GetGameEngineParameters();
	int selectedIndex = m_pParameterNameInput->currentIndex();

	if (selectedIndex >= allParams.size() || selectedIndex < 0) {
		return std::make_pair(GameEngineRegisterCommandDatagram(), (ParameterControlWidget*)nullptr);
	}

	return allParams.at(selectedIndex);
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
	pDescription->m_parameterValue = QString("%1").arg(m_pParameterValueInput->GetValue());

	pDescription->m_controlWidget = GetSelectedGameEngineParameters().second;

	return new TriggerItem(pDescription);
}

void TriggerList::UpdatePreviewText()
{
	TriggerItem* pItem = CreateNewTriggerItem();
	m_pPreviewTrigger->setText(pItem->text());
	delete pItem;
}




// --------------------------------------------------------
// Trigger Item
// --------------------------------------------------------
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