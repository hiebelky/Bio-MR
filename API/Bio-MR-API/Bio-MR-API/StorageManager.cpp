#include "StorageManager.h"

#include <fstream>
#include "json.h"

const char* SENSOR_CONFIGURATION_PATH = "SensorConfiguration.json";

StorageManager::StorageManager()
{
	// Read the entire file
	std::ifstream file(SENSOR_CONFIGURATION_PATH);
	Json::Value root;
	file >> root;
}
