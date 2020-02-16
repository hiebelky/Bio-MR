#pragma once
#include <QWidget>
#include <QStandardItem>

// Forward declare
class QListView;
class QStandardItemModel;
class QLineEdit;
class QSpinBox;
class QComboBox;
class QAbstractSpinBox;

class TriggerList : public QWidget
{
	Q_OBJECT
public:
	TriggerList(QWidget* parent = nullptr);
private:
	void SetUpTriggerWindow();


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
	QAbstractSpinBox* m_pParameterValueInput = nullptr;
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

template <class T>
class TriggerItem : public QStandardItem
{
public:
	TriggerItem(QString eventSource, QString sampleName, QString fieldName, int fieldIndex,
		ComparisonType compareFunc, T compareValue, QString parameterName, QString parameterValue) : QStandardItem(),
		m_eventSource(eventSource), m_sampleName(sampleName), m_fieldName(fieldName), m_fieldIndex(fieldIndex),
		m_comparisionFunction(compareFunc), m_comparisonValue(compareValue), m_parameterName(parameterName), m_parameterValue(parameterValue)
	{
		QString objectText = QString("If %1::%2::%3 %4 %5, set %6 to %7")
			.arg(m_eventSource)
			.arg(m_sampleName)
			.arg(m_fieldName)
			.arg(ComparisonFucntionToQString(m_comparisionFunction))
			.arg(ComparisonValueToQString(m_comparisonValue))
			.arg(m_parameterName)
			.arg(m_parameterValue);
			setText(objectText);
	}

private:
	QString ComparisonValueToQString(T value) {
		if (std::is_same<T, int>::value) {
			return QString("%1").arg((int)value);
		}
		else if (std::is_same<T, float>::value) {
			return QString("%1").arg((float)value);
		} else if (std::is_same<T, QString>::value) {
			return QString(value);
		}
	}

	// Sensor Values
	QString m_eventSource;
	QString m_sampleName;
	QString m_fieldName;
	int m_fieldIndex;

	ComparisonType m_comparisionFunction;
	T m_comparisonValue;
	
	// Game Engine Values
	QString m_parameterName;
	QString m_parameterValue;
};