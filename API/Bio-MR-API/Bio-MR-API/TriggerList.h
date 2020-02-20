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


	TriggerItem* CreateNewTriggerItem();

	StorageManager* m_pStorageManager = nullptr;
	QListView* m_pTriggerView = nullptr;
	QStandardItemModel* m_pTriggerModel = nullptr;

	QWidget* m_pAddTriggerWindow = nullptr;

	// Sensor input
	QLineEdit* m_pEventSourceInput = nullptr;
	QLineEdit* m_pSampleNameInput = nullptr;
	QSpinBox* m_pFieldIndexInput = nullptr;
	QComboBox* m_pComparisonFunctionInput = nullptr;
	QLineEdit* m_pComparisonValueInput = nullptr;

	// Command to game engine input
	QComboBox* m_pParameterNameInput = nullptr;
	QDoubleSpinBox* m_pParameterValueInput = nullptr;

	// Preview
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