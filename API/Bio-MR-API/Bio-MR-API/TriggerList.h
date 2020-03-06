#pragma once
#include <QWidget>
#include <QStandardItem>

#include "StorageManager.h"

// Forward declare
class QListView;
class QStandardItemModel;
class QDoubleSpinBox;
class QLineEdit;
class QSpinBox;
class QLabel;
class QComboBox;
class QGridLayout;
class TriggerItem;
class ParameterControlWidget;
class MultipleInputBox;

class TriggerList : public QWidget
{
	Q_OBJECT
public:
	TriggerList(StorageManager* sm, QWidget* parent = nullptr);
private:
	void SetUpTriggerWindow();

	void UpdateGameEngineParameterList();
	void UpdateGameEngineParameterValue(int index);
	std::pair<GameEngineRegisterCommandDatagram, ParameterControlWidget*>& GetSelectedGameEngineParameters();
	void UpdatePreviewText();

	// Returns a trigger item created with the settings displayed in the preview
	TriggerItem* CreateNewTriggerItem();

	// MVC paradigm for displaying triggers
	StorageManager* m_pStorageManager = nullptr;
	QListView* m_pTriggerView = nullptr;
	QStandardItemModel* m_pTriggerModel = nullptr;

	// Window for adding a new trigger
	QWidget* m_pAddTriggerWindow = nullptr;

	// Keeps track of which tab we are on
	bool m_isCustom = false;

	// *************************
	// Preset sensor input
	//**************************
	QComboBox* m_pPresetEventSourceInput = nullptr;
	QComboBox* m_pPresetSampleNameInput = nullptr;
	QComboBox* m_pPresetFieldIndexInput = nullptr;
	QComboBox* m_pPresetComparisonFunctionInput = nullptr;
	MultipleInputBox* m_pPresetComparisonValueInput = nullptr;
	QComboBox* m_pPresetParameterNameInput = nullptr;
	MultipleInputBox* m_pPresetParameterValueInput = nullptr;

	// *************************
	// Custom sensor input
	//**************************
	QLineEdit* m_pCustomEventSourceInput = nullptr;
	QLineEdit* m_pCustomSampleNameInput = nullptr;
	QSpinBox* m_pCustomFieldIndexInput = nullptr;
	QComboBox* m_pCustomComparisonFunctionInput = nullptr;
	QLineEdit* m_pCustomComparisonValueInput = nullptr;
	QComboBox* m_pCustomParameterNameInput = nullptr;
	MultipleInputBox* m_pCustomParameterValueInput = nullptr;


	// Preview window
	QLabel* m_pPreviewTrigger = nullptr;
};


class TriggerItem : public QStandardItem
{
public:
	TriggerItem(TriggerDescription* desc);
	~TriggerItem();

	inline TriggerDescription* GetTriggerDescription() { return m_pDescription; }
private:
	TriggerDescription* m_pDescription = nullptr;
};