#pragma once

#include "Types.h"

#include <vector>
#include <set>

#include <QString>
#include <QObject>

// Forward declare
class ParameterControlWidget;

/*struct DataField {
	QString m_name;
};*/

/*bool operator== (const DataField* lhs, const DataField* rhs)
{
	return lhs->m_name.compare(rhs->m_name) == 0;
}

bool operator< (const DataField& lhs, const DataField& rhs)
{
	// Maybe wrong
	return lhs.m_name.compare(rhs.m_name) < 0;
}

bool comparison(const DataField* lhs, const DataField* rhs) {
	return lhs->m_name.compare(rhs->m_name) > 0;
}*/

/*template<>
struct std::less<DataField*>
{
	bool operator()(const DataField&& lhs, const DataField&& rhs) const
	{
		return lhs.m_name.compare(rhs.m_name) > 0;
	}
};

template <class T>
struct GameEngineParameterSet : DataField {
	T m_minVal;
	T m_startVal;
	T m_maxVal;
	bool m_isButton;
};

template <class T>
struct SensorDataField : DataField {
	int m_indexInRawString;
	T m_minVal;
	T m_maxVal;
};

struct SampleName : DataField {
	// Actually std::vector<SensorDataField*>
	std::set<DataField*> m_fields;
};

struct EventSource : DataField {
	std::set<SampleName> m_sampleNames;
};*/

class StorageManager : public QObject{
	Q_OBJECT
public:
	StorageManager();

	// Add/retrieve
	//template <class T>
	//void AddSensorDataField(QString& eventSource, QString& sampleName, QString& dataField, int dataFieldIndexInRawData, T minVal, T maxVal);
	void AddGameEngineParameter(GameEngineRegisterCommandDatagram& parameter, ParameterControlWidget* widget);
	std::vector<std::pair<GameEngineRegisterCommandDatagram, ParameterControlWidget*>>& GetGameEngineParameters();

	void AddTrigger(TriggerDescription* desc);
	void RemoveTrigger(TriggerDescription* desc);
	std::vector<TriggerDescription*>& GetAllTriggers();

signals:
	void NewGameEngineParameter();

private:
	std::vector<TriggerDescription*> m_triggers;
	std::vector<std::pair<GameEngineRegisterCommandDatagram, ParameterControlWidget*>> m_gameEngineParameters;

	// Currently unused
	//std::set<EventSource> m_eventSources;
};