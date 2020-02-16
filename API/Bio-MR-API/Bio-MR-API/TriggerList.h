#pragma once
#include <QWidget>
#include <QStandardItem>

// Forward declare
class QListView;
class QStandardItemModel;

class TriggerList : public QWidget
{
	Q_OBJECT
public:
	TriggerList(QWidget* parent = nullptr);
private:
	QListView* m_pTriggerView = nullptr;
	QStandardItemModel* m_pTriggerModel = nullptr;
};


enum class ComparisonType
{
	k_less,
	k_lessEqual,
	k_equal,
	k_greaterEqual,
	k_greater
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
		QString objectText = QString("If %1::%2::%3 %4 %5, set %6 to %7.")
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