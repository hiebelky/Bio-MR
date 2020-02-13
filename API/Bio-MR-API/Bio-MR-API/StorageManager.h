#pragma once

#include <vector>
#include <set>

#include <QString>

struct DataField {
	QString m_name;
};

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

template<>
struct std::less<DataField*>
{
	bool operator()(const DataField&& lhs, const DataField&& rhs) const
	{
		return lhs.m_name.compare(rhs.m_name) > 0;
	}
};

template <class T>
struct GameEngineParameter : DataField {
	T minVal;
	T startVal;
	T maxVal;
	bool isButton;
};

template <class T>
struct SensorDataField : DataField {
	int indexInRawString;
	T minVal;
	T maxVal;
};

struct SampleName : DataField {
	// Actually std::vector<SensorDataField*>
	std::set<DataField*> m_fields;
};

struct EventSource : DataField {
	std::set<SampleName> m_sampleNames;
};

class StorageManager {
public:
	StorageManager();

	// Add/retrieve
	//template <class T>
	//void AddSensorDataField(QString& eventSource, QString& sampleName, QString& dataField, int dataFieldIndexInRawData, T minVal, T maxVal);

private:
	// Actually std::vector<EventSource*>
	std::set<DataField*> m_gameEngineParameters;
	std::set<EventSource> m_eventSources;
};