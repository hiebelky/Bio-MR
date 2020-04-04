#include "StorageManager.h"

#include <fstream>
#include "json.h"
#include "ParameterControlWidget.h"

const char* SENSOR_CONFIGURATION_PATH = "SensorConfiguration.json";


// ***********************************************
// Sensor Configuration
// ***********************************************
StorageManager::StorageManager() : QObject()
{
	// Read the entire file of configured sensors
	std::ifstream file(SENSOR_CONFIGURATION_PATH);
	Json::Value root;
	file >> root;

	// Loop through all the event sources
	Json::Value eventSources = root.get("EventSources", Json::arrayValue);
	for (int i = 0; i < eventSources.size(); ++i) {
		Json::Value eventSource = eventSources.get(i, Json::objectValue);
		QString eventSourceString = QString::fromStdString(eventSource.get("Name", "Unnamed").asString());

		// Loop through all the sample names
		Json::Value sampleNames = eventSource.get("SampleNames", Json::arrayValue);
		for (int j = 0; j < sampleNames.size(); ++j) {
			Json::Value sampleName = sampleNames.get(j, Json::objectValue);
			QString sampleNameString = QString::fromStdString(sampleName.get("Name", "Unnamed").asString());


			// Loop through all the fields
			Json::Value fields = sampleName.get("Fields", Json::arrayValue);
			for (int k = 0; k < fields.size(); ++k) {
				Json::Value field = fields.get(k, Json::objectValue);
				QString dataFieldString = QString::fromStdString(field.get("Name", "Unnamed").asString());
				int dataFieldIndex = field.get("Index", "0").asInt();

				QString type = QString::fromStdString(field.get("Type", "String").asString());
				QString minValue = QString::fromStdString(field.get("Min", "0").asString());
				QString maxValue = QString::fromStdString(field.get("Max", "0").asString());

				// Store configured sensor information to autofill combo boxes
				AddSensorDataField(eventSourceString, sampleNameString, dataFieldString, type, dataFieldIndex, minValue, maxValue);
			}
		}
	}
}

void StorageManager::AddSensorDataField(QString& eventSource, QString& sampleName, QString& dataField, QString& type, int dataFieldIndexInRawData, QString& minVal, QString& maxVal)
{
	// Resolve the type
	InputType newType;
	if (type.compare("Int", Qt::CaseInsensitive) == 0) {
		newType = InputType::k_int;
	}
	else if (type.compare("Float", Qt::CaseInsensitive) == 0 || type.compare("Double", Qt::CaseInsensitive) == 0) {
		newType = InputType::k_double;
	}
	else {
		newType = InputType::k_string;
	}

	// Construct the data structure
	SensorDataField newDataField = { dataField, newType, dataFieldIndexInRawData, minVal, maxVal };
	SampleName newSampleName = { sampleName, {newDataField} };
	EventSource newEventSource = { eventSource, {newSampleName} };


	// Insert the new nodes without overwriting existing nodes with same name
	for (EventSource& tempEventSource : m_eventSources) {
		if (tempEventSource.m_name.compare(eventSource, Qt::CaseInsensitive) == 0) {
			// Matched event source name

			for (SampleName& tempSampleName : tempEventSource.m_sampleNames) {
				if (tempSampleName.m_name.compare(sampleName, Qt::CaseInsensitive) == 0) {
					// Matched sample name

					// Check if there is already a sensor field at this index
					for (SensorDataField& tempSensorDataField : tempSampleName.m_fields) {
						if (tempSensorDataField.m_index == dataFieldIndexInRawData) {
							// Do not overwrite existing index
							return;
						}
					}

					// No matching field index
					tempSampleName.m_fields.push_back(newDataField);
					return;
				}
			}

			// No matching sample name
			tempEventSource.m_sampleNames.push_back(newSampleName);
			return;
		}
	}

	// No matching event source name
	m_eventSources.push_back(newEventSource);
	return;
}

std::vector<EventSource>& StorageManager::GetEventSources() {
	return m_eventSources;
}

// ***********************************************
// Game Engine Parameters
// ***********************************************
void StorageManager::AddGameEngineParameter(GameEngineRegisterCommandDatagram& parameter, ParameterControlWidget* widget)
{
	m_gameEngineParameters.push_back(std::make_pair(parameter, widget));
	emit NewGameEngineParameter();
}

std::vector<std::pair<GameEngineRegisterCommandDatagram, ParameterControlWidget*>>& StorageManager::GetGameEngineParameters()
{
	return m_gameEngineParameters;
}



// ***********************************************
// Triggers
// ***********************************************
std::vector<TriggerDescription*>& StorageManager::GetAllTriggers()
{
	return m_triggers;
}

void StorageManager::RemoveTrigger(TriggerDescription* desc)
{
	auto elem = std::find(m_triggers.begin(), m_triggers.end(), desc);
	m_triggers.erase(elem);
}
void StorageManager::AddTrigger(TriggerDescription* desc)
{
	m_triggers.push_back(desc);
}