#pragma once

#include <vector>

#include <QString>

// Pure virtual
struct DataField {
	QString m_name;
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
	std::vector<DataField*> m_fields;
};

struct EventSource : DataField {
	std::vector<SampleName> m_sampleNames;
};

class StorageManager {

	// Add/retrieve


private:
	// Actually std::vector<EventSource*>
	std::vector<DataField*> m_gameEngineParameters;
	std::vector<EventSource> m_eventSources;
};