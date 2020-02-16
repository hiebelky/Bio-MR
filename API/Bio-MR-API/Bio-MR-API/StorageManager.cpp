#include "StorageManager.h"

#include <fstream>
#include "json.h"

const char* SENSOR_CONFIGURATION_PATH = "SensorConfiguration.json";

StorageManager::StorageManager() : QObject()
{
	// Read the entire file
	std::ifstream file(SENSOR_CONFIGURATION_PATH);
	Json::Value root;
	file >> root;



	// Loop through all the event sources
	Json::Value eventSources = root.get("EventSources", Json::arrayValue);
	for (int i = 0; i < eventSources.size(); ++i) {
		Json::Value eventSource = eventSources.get(i, Json::objectValue);
		eventSource.get("Name", "Unnamed").asString();

		// Loop through all the sample names
		Json::Value sampleNames = eventSource.get("SampleNames", Json::arrayValue);
		for (int j = 0; j < sampleNames.size(); ++j) {
			Json::Value sampleName = sampleNames.get(j, Json::objectValue);
			sampleName.get("Name", "Unnamed").asString();


			// Loop through all the fields
			Json::Value fields = sampleName.get("Fields", Json::arrayValue);
			for (int k = 0; k < fields.size(); ++k) {
				Json::Value field = fields.get(k, Json::objectValue);
				field.get("Name", "Unnamed").asString();
				field.get("Index", "0").asInt();

				QString type = QString(field.get("Type", "String").asString().c_str());

				if (type.compare("Int", Qt::CaseInsensitive) == 0) {
					field.get("Min", "0").asInt();
					field.get("Max", "0").asInt();
				} else if (type.compare("Float", Qt::CaseInsensitive) == 0 || type.compare("Double", Qt::CaseInsensitive) == 0) {
					field.get("Min", "0").asDouble();
					field.get("Max", "0").asDouble();
				} else if (type.compare("String", Qt::CaseInsensitive) == 0) {
					field.get("Min", "0").asString();
					field.get("Max", "0").asString();
				}

			}
		}



	}
}

void StorageManager::AddGameEngineParameter(GameEngineRegisterCommandDatagram& parameter)
{
	m_gameEngineParameters.push_back(parameter);
	emit NewGameEngineParameter();
}

std::vector<GameEngineRegisterCommandDatagram>& StorageManager::GetGameEngineParameters()
{
	return m_gameEngineParameters;
}


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