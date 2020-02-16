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

class TriggerList : public QWidget
{
	Q_OBJECT
public:
	TriggerList(StorageManager* sm, QWidget* parent = nullptr);
private:
	void SetUpTriggerWindow();

	void UpdateGameEngineParameterList();
	void UpdateGameEngineParameterValue(int index);
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

enum class ComparisonType
{
	k_less,
	k_lessEqual,
	k_equal,
	k_greaterEqual,
	k_greater,
	k_count
};

class TriggerItem : public QStandardItem
{
public:
	TriggerItem(QString eventSource, QString sampleName, int fieldIndex, ComparisonType compareFunc, QString compareValue, QString parameterName, QString parameterValue);

private:

	// Sensor Values
	QString m_eventSource;
	QString m_sampleName;
	int m_fieldIndex;

	ComparisonType m_comparisionFunction;
	QString m_comparisonValue;
	
	// Game Engine Values
	QString m_parameterName;
	QString m_parameterValue;
};