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
	pAddButton->setToolTip("Add a new automatic trigger");
	QPushButton* pRemoveButton = new QPushButton("Remove", this);
	pRemoveButton->setToolTip("Remove the highlighted automatic trigger");

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

	connect(pTabWidget, &QTabWidget::currentChanged, this, [&](int index) {
		// Keep track of which window we're on
		m_currentTab = index;
	});

	// Left tab (Preset Sensor)
	{
		m_currentTab = 0;

		// Create the sensor level inputs 
		QLabel* pEventSourceLabel = new QLabel("Event Source:", m_pAddTriggerWindow);
		m_pPresetEventSourceInput = new QComboBox(m_pAddTriggerWindow);
		QLabel* pSampleNameLabel = new QLabel("Sample Name:", m_pAddTriggerWindow);
		m_pPresetSampleNameInput = new QComboBox(m_pAddTriggerWindow);
		QLabel* pFieldIdLabel = new QLabel("Field Id:", m_pAddTriggerWindow);
		m_pPresetFieldIndexInput = new QComboBox(m_pAddTriggerWindow);

		QLabel* pComparisonFunctionLabel = new QLabel("Comparison:", m_pAddTriggerWindow);
		m_pPresetComparisonFunctionInput = new QComboBox(m_pAddTriggerWindow);
		QStringList comparisonOptions;
		for (ComparisonType x = (ComparisonType)0; x < ComparisonType::k_count; x = (ComparisonType)((int)x + 1)) {
			comparisonOptions << ComparisonFucntionToQString(x);
		}
		m_pPresetComparisonFunctionInput->addItems(comparisonOptions);

		QLabel* pComparisonValueLabel = new QLabel("Threshold Value:", m_pAddTriggerWindow);
		m_pPresetComparisonValueInput = new MultipleInputBox(InputType::k_int, m_pAddTriggerWindow);

		UpdateEventSourceComboBox();
		connect(m_pPresetEventSourceInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdateSampleNameComboBox);
		connect(m_pPresetSampleNameInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdateFieldIndexComboBox);
		connect(m_pPresetFieldIndexInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdatePresetComarisonValueInput);


		QLabel* pParameterNameLabel = new QLabel("Change Parameter:", m_pAddTriggerWindow);
		m_pParameterNameInputs[m_currentTab] = new QComboBox(m_pAddTriggerWindow);
		QLabel* pParameterValueLabel = new QLabel("Change Value:", m_pAddTriggerWindow);
		m_pParameterValueInputs[m_currentTab] = new MultipleInputBox(InputType::k_int, m_pAddTriggerWindow);

		UpdateGameEngineParameterList(m_currentTab);
		UpdateGameEngineParameterValue(-1, m_currentTab);

		connect(m_pParameterNameInputs[m_currentTab], QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&](int index) {
			UpdateGameEngineParameterValue(index, 0);
		});


		// Layout the widget
		QGridLayout* pGridLayout = new QGridLayout(pPresetTab);
		pPresetTab->setLayout(pGridLayout);

		pGridLayout->addWidget(pEventSourceLabel, 0, 0);
		pGridLayout->addWidget(m_pPresetEventSourceInput, 0, 1);
		pGridLayout->addWidget(pSampleNameLabel, 1, 0);
		pGridLayout->addWidget(m_pPresetSampleNameInput, 1, 1);
		pGridLayout->addWidget(pFieldIdLabel, 2, 0);
		pGridLayout->addWidget(m_pPresetFieldIndexInput, 2, 1);
		pGridLayout->addWidget(pComparisonFunctionLabel, 3, 0);
		pGridLayout->addWidget(m_pPresetComparisonFunctionInput, 3, 1);
		pGridLayout->addWidget(pComparisonValueLabel, 4, 0);
		pGridLayout->addWidget(m_pPresetComparisonValueInput, 4, 1);

		// Spacing
		pGridLayout->setRowMinimumHeight(5, 10);

		pGridLayout->addWidget(pParameterNameLabel, 6, 0);
		pGridLayout->addWidget(m_pParameterNameInputs[m_currentTab], 6, 1);
		pGridLayout->addWidget(pParameterValueLabel, 7, 0);
		pGridLayout->addWidget(m_pParameterValueInputs[m_currentTab], 7, 1);


		pGridLayout->setColumnStretch(1, 1);
		pGridLayout->setRowStretch(99, 1);
	}

	// Right tab (Manual Entry)
	{
		m_currentTab = 1;

		// Create the sensor level inputs 
		QLabel* pEventSourceLabel = new QLabel("Event Source:", m_pAddTriggerWindow);
		m_pCustomEventSourceInput = new QLineEdit(m_pAddTriggerWindow);
		m_pCustomEventSourceInput->setText("Source");
		QLabel* pSampleNameLabel = new QLabel("Sample Name:", m_pAddTriggerWindow);
		m_pCustomSampleNameInput = new QLineEdit(m_pAddTriggerWindow);
		m_pCustomSampleNameInput->setText("Name");
		QLabel* pFieldIdLabel = new QLabel("Field Id:", m_pAddTriggerWindow);
		m_pCustomFieldIndexInput = new QSpinBox(m_pAddTriggerWindow);

		QLabel* pComparisonFunctionLabel = new QLabel("Comparison:", m_pAddTriggerWindow);
		m_pCustomComparisonFunctionInput = new QComboBox(m_pAddTriggerWindow);
		QStringList comparisonOptions;
		for (ComparisonType x = (ComparisonType)0; x < ComparisonType::k_count; x = (ComparisonType)((int)x + 1)) {
			comparisonOptions << ComparisonFucntionToQString(x);
		}
		m_pCustomComparisonFunctionInput->addItems(comparisonOptions);

		QLabel* pComparisonValueLabel = new QLabel("Threshold Value:", m_pAddTriggerWindow);
		m_pCustomComparisonValueInput = new QLineEdit(m_pAddTriggerWindow);
		m_pCustomComparisonValueInput->setText("0");


		QLabel* pParameterNameLabel = new QLabel("Change Parameter:", m_pAddTriggerWindow);
		m_pParameterNameInputs[m_currentTab] = new QComboBox(m_pAddTriggerWindow);
		QLabel* pParameterValueLabel = new QLabel("Change Value:", m_pAddTriggerWindow);
		m_pParameterValueInputs[m_currentTab] = new MultipleInputBox(InputType::k_int, m_pAddTriggerWindow);\

		UpdateGameEngineParameterList(m_currentTab);
		UpdateGameEngineParameterValue(-1, m_currentTab);

		connect(m_pParameterNameInputs[m_currentTab], QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&](int index) {
			UpdateGameEngineParameterValue(index, 1);
		});


		// Layout the widget
		QGridLayout* pGridLayout = new QGridLayout(pCustomTab);
		pCustomTab->setLayout(pGridLayout);

		pGridLayout->addWidget(pEventSourceLabel, 0, 0);
		pGridLayout->addWidget(m_pCustomEventSourceInput, 0, 1);
		pGridLayout->addWidget(pSampleNameLabel, 1, 0);
		pGridLayout->addWidget(m_pCustomSampleNameInput, 1, 1);
		pGridLayout->addWidget(pFieldIdLabel, 2, 0);
		pGridLayout->addWidget(m_pCustomFieldIndexInput, 2, 1);
		pGridLayout->addWidget(pComparisonFunctionLabel, 3, 0);
		pGridLayout->addWidget(m_pCustomComparisonFunctionInput, 3, 1);
		pGridLayout->addWidget(pComparisonValueLabel, 4, 0);
		pGridLayout->addWidget(m_pCustomComparisonValueInput, 4, 1);

		// Spacing
		pGridLayout->setRowMinimumHeight(5, 10);

		pGridLayout->addWidget(pParameterNameLabel, 6, 0);
		pGridLayout->addWidget(m_pParameterNameInputs[m_currentTab], 6, 1);
		pGridLayout->addWidget(pParameterValueLabel, 7, 0);
		pGridLayout->addWidget(m_pParameterValueInputs[m_currentTab], 7, 1);


		pGridLayout->setColumnStretch(1, 1);
		pGridLayout->setRowStretch(99, 1);
	}

	// Make sure to update the parameter lists when the storage manager gets new game engine parameters
	connect(m_pStorageManager, &StorageManager::NewGameEngineParameter, this, [&] {
		UpdateGameEngineParameterList(0);
		UpdateGameEngineParameterList(1);
	});

	// Reset the current tab
	m_currentTab = pTabWidget->currentIndex();


	// Create the preview widget which updates when any of the boxes are changed
	m_pPreviewTrigger = new QLabel(m_pAddTriggerWindow);
	m_pPreviewTrigger->setAlignment(Qt::AlignCenter);
	m_pPreviewTrigger->setFont(smallBold);


	connect(pTabWidget, &QTabWidget::currentChanged, this, &TriggerList::UpdatePreviewText);

	connect(m_pPresetEventSourceInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pPresetSampleNameInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pPresetFieldIndexInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pPresetComparisonFunctionInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pPresetComparisonValueInput, &MultipleInputBox::ValueChanged, this, &TriggerList::UpdatePreviewText);

	connect(m_pCustomEventSourceInput, &QLineEdit::textChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pCustomSampleNameInput, &QLineEdit::textChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pCustomFieldIndexInput, QOverload<int>::of(&QSpinBox::valueChanged), this, &TriggerList::UpdatePreviewText);
	connect(m_pCustomComparisonFunctionInput, &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pCustomComparisonValueInput, &QLineEdit::textChanged, this, &TriggerList::UpdatePreviewText);

	connect(m_pParameterNameInputs[0], &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pParameterValueInputs[0], &MultipleInputBox::ValueChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pParameterNameInputs[1], &QComboBox::currentTextChanged, this, &TriggerList::UpdatePreviewText);
	connect(m_pParameterValueInputs[1], &MultipleInputBox::ValueChanged, this, &TriggerList::UpdatePreviewText);
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



void TriggerList::UpdateGameEngineParameterList(int tab)
{
	auto& allParams = m_pStorageManager->GetGameEngineParameters();

	// Update the paramater name combo box
	QStringList names;
	for (auto& it : allParams) {
		names << it.first.m_parameterName;
	}

	// Update the name input combo for this tab
	m_pParameterNameInputs[tab]->clear();
	m_pParameterNameInputs[tab]->addItems(names);
}

void TriggerList::UpdateGameEngineParameterValue(int index, int tab)
{
	auto& allParams = m_pStorageManager->GetGameEngineParameters();

	if (index < 0 || index > allParams.size())
	{
		m_pParameterValueInputs[tab]->SetMinValue(0);
		m_pParameterValueInputs[tab]->SetValue(0);
		m_pParameterValueInputs[tab]->SetMaxValue(99);
	} else {
		GameEngineRegisterCommandDatagram& selectedParameter = allParams[index].first;

		if (selectedParameter.m_type.compare("Int", Qt::CaseInsensitive) == 0) {
			m_pParameterValueInputs[tab]->SetType(InputType::k_int);
			m_pParameterValueInputs[tab]->SetMinValue(selectedParameter.m_minVal.toDouble());
			m_pParameterValueInputs[tab]->SetValue(selectedParameter.m_startVal);
			m_pParameterValueInputs[tab]->SetMaxValue(selectedParameter.m_maxVal.toDouble());
		}
		else if (selectedParameter.m_type.compare("Float", Qt::CaseInsensitive) == 0 || selectedParameter.m_type.compare("Double", Qt::CaseInsensitive) == 0) {
			m_pParameterValueInputs[tab]->SetType(InputType::k_double);
			m_pParameterValueInputs[tab]->SetMinValue(selectedParameter.m_minVal.toDouble());
			m_pParameterValueInputs[tab]->SetValue(selectedParameter.m_startVal);
			m_pParameterValueInputs[tab]->SetMaxValue(selectedParameter.m_maxVal.toDouble());
		}
		else if (selectedParameter.m_type.compare("String", Qt::CaseInsensitive) == 0)
		{
			m_pParameterValueInputs[tab]->SetType(InputType::k_string);
			m_pParameterValueInputs[tab]->SetValue(selectedParameter.m_startVal);
		}
	}
}






void TriggerList::UpdateEventSourceComboBox() {

	// Read all the names from the storage manager
	auto& allEventSources = m_pStorageManager->GetEventSources();

	QStringList names;
	for (auto& it : allEventSources) {
		names << it.m_name;
	}

	// Update the name input combo for this tab
	m_pPresetEventSourceInput->clear();
	m_pPresetEventSourceInput->addItems(names);

	// Always finish by updating the sample name box
	UpdateSampleNameComboBox();
}
void TriggerList::UpdateSampleNameComboBox() {
	// Get the current index of the event sources
	int eventSourceIndex = m_pPresetEventSourceInput->currentIndex();

	if (eventSourceIndex < 0) {
		return;
	}

	// Read all the names from the storage manager
	auto& allSampleNames = m_pStorageManager->GetEventSources().at(eventSourceIndex).m_sampleNames;

	QStringList names;
	for (auto& it : allSampleNames) {
		names << it.m_name;
	}

	// Update the name input combo for this tab
	m_pPresetSampleNameInput->clear();
	m_pPresetSampleNameInput->addItems(names);

	// Always finish by updating the field index box
	UpdateFieldIndexComboBox();
}
void TriggerList::UpdateFieldIndexComboBox() {
	// Get the current index of the event sources and sample names
	int eventSourceIndex = m_pPresetEventSourceInput->currentIndex();
	int sampleNameIndex = m_pPresetSampleNameInput->currentIndex();

	if (eventSourceIndex < 0 || sampleNameIndex < 0) {
		return;
	}

	// Read all the names from the storage manager
	auto& allFieldIndices = m_pStorageManager->GetEventSources().at(eventSourceIndex).m_sampleNames.at(sampleNameIndex).m_fields;

	QStringList names;
	for (auto& it : allFieldIndices) {
		names << it.m_name;
	}

	// Update the name input combo for this tab
	m_pPresetFieldIndexInput->clear();
	m_pPresetFieldIndexInput->addItems(names);

	// Always finish by updating the field index box
	UpdatePresetComarisonValueInput();
}
void TriggerList::UpdatePresetComarisonValueInput() {
	// Get the current index of the event sources and sample names
	int eventSourceIndex = m_pPresetEventSourceInput->currentIndex();
	int sampleNameIndex = m_pPresetSampleNameInput->currentIndex();
	int fieldIndex = m_pPresetFieldIndexInput->currentIndex();

	if (eventSourceIndex < 0 || sampleNameIndex < 0 || fieldIndex < 0) {
		return;
	}

	// Read all the names from the storage manager
	SensorDataField& currentField = m_pStorageManager->GetEventSources().at(eventSourceIndex).m_sampleNames.at(sampleNameIndex).m_fields.at(fieldIndex);

	// Update the input widget
	m_pPresetComparisonValueInput->SetType(currentField.m_type);
	m_pPresetComparisonValueInput->SetMinValue(currentField.m_minVal.toDouble());
	m_pPresetComparisonValueInput->SetMaxValue(currentField.m_maxVal.toDouble());
}






std::pair<GameEngineRegisterCommandDatagram, ParameterControlWidget*> TriggerList::GetSelectedGameEngineParameters(int tab)
{
	auto& allParams = m_pStorageManager->GetGameEngineParameters();
	int selectedIndex = m_pParameterNameInputs[tab]->currentIndex();

	if (selectedIndex >= allParams.size() || selectedIndex < 0) {
		return std::make_pair(GameEngineRegisterCommandDatagram(), (ParameterControlWidget*)nullptr);
	}

	return allParams.at(selectedIndex);
}


TriggerItem* TriggerList::CreateNewTriggerItem() {
	TriggerDescription* pDescription = new TriggerDescription();

	// Sensor input
	if (m_currentTab == 0) {
		int eventSourceIndex = m_pPresetEventSourceInput->currentIndex();
		if (eventSourceIndex >= 0) {
			EventSource& eventSource = m_pStorageManager->GetEventSources().at(eventSourceIndex);
			pDescription->m_eventSource = eventSource.m_name;
		}

		int sampleNameIndex = m_pPresetSampleNameInput->currentIndex();
		if (sampleNameIndex >= 0) {
			SampleName& sampleName = m_pStorageManager->GetEventSources().at(eventSourceIndex).m_sampleNames.at(sampleNameIndex);
			pDescription->m_sampleName = sampleName.m_name;
		}

		int fieldIndex = m_pPresetFieldIndexInput->currentIndex();
		if (fieldIndex >= 0) {
			SensorDataField& currentField = m_pStorageManager->GetEventSources().at(eventSourceIndex).m_sampleNames.at(sampleNameIndex).m_fields.at(fieldIndex);
			pDescription->m_fieldName = currentField.m_name;
			pDescription->m_fieldIndex = currentField.m_index;
		}

		pDescription->m_comparisionFunction = (ComparisonType)m_pPresetComparisonFunctionInput->currentIndex();
		pDescription->m_comparisonValue = m_pPresetComparisonValueInput->GetValue();
	}
	else {
		pDescription->m_eventSource = m_pCustomEventSourceInput->text();
		pDescription->m_sampleName = m_pCustomSampleNameInput->text();
		pDescription->m_fieldName = QString();
		pDescription->m_fieldIndex = m_pCustomFieldIndexInput->value();

		pDescription->m_comparisionFunction = (ComparisonType)m_pCustomComparisonFunctionInput->currentIndex();
		pDescription->m_comparisonValue = m_pCustomComparisonValueInput->text();
	}

	// Game engine input
	pDescription->m_parameterName = m_pParameterNameInputs[m_currentTab]->currentText();
	pDescription->m_parameterValue = QString("%1").arg(m_pParameterValueInputs[m_currentTab]->GetValue());

	pDescription->m_controlWidget = GetSelectedGameEngineParameters(m_currentTab).second;

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
	if (desc->m_fieldName.isEmpty()) {
	QString objectText = QString("If %1::%2[%3] %4 %5, set %6 to %7")
		.arg(desc->m_eventSource)
		.arg(desc->m_sampleName)
		.arg(desc->m_fieldIndex)
		.arg(ComparisonFucntionToQString(desc->m_comparisionFunction))
		.arg(desc->m_comparisonValue)
		.arg(desc->m_parameterName)
		.arg(desc->m_parameterValue);
	setText(objectText);
	setToolTip(objectText);
	} else {
		QString objectText = QString("If %1::%2::%3 %4 %5, set %6 to %7")
			.arg(desc->m_eventSource)
			.arg(desc->m_sampleName)
			.arg(desc->m_fieldName)
			.arg(ComparisonFucntionToQString(desc->m_comparisionFunction))
			.arg(desc->m_comparisonValue)
			.arg(desc->m_parameterName)
			.arg(desc->m_parameterValue);
		setText(objectText);
		setToolTip(objectText);
	}

}

TriggerItem::~TriggerItem() {
	delete m_pDescription;
}