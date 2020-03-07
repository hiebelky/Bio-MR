#pragma once

#include "Types.h"

#include <vector>

#include <QString>
#include <QObject>

// Forward declare
class ParameterControlWidget;


struct SensorDataField  {
	QString m_name;
	InputType m_type;
	int m_index;
	QString m_minVal;
	QString m_maxVal;
};

struct SampleName {
	QString m_name;
	std::vector<SensorDataField> m_fields;
};

struct EventSource {
	QString m_name;
	std::vector<SampleName> m_sampleNames;
};

class StorageManager : public QObject{
	Q_OBJECT
public:
	StorageManager();

	// Sensor information interface
	void AddSensorDataField(QString& eventSource, QString& sampleName, QString& dataField, QString& type, int dataFieldIndexInRawData, QString& minVal, QString& maxVal);
	std::vector<EventSource>& GetEventSources();

	// Game Engine parameter interface
	void AddGameEngineParameter(GameEngineRegisterCommandDatagram& parameter, ParameterControlWidget* widget);
	std::vector<std::pair<GameEngineRegisterCommandDatagram, ParameterControlWidget*>>& GetGameEngineParameters();

	// Trigger Interface
	void AddTrigger(TriggerDescription* desc);
	void RemoveTrigger(TriggerDescription* desc);
	std::vector<TriggerDescription*>& GetAllTriggers();

signals:
	void NewGameEngineParameter();

private:
	// Stores all the current triggers
	std::vector<TriggerDescription*> m_triggers;

	// Stores all the game engine parameters and their creation info
	std::vector<std::pair<GameEngineRegisterCommandDatagram, ParameterControlWidget*>> m_gameEngineParameters;

	// Stores all the preset sensor information
	std::vector<EventSource> m_eventSources;
};