#pragma once

#include <QString>
#include <QStringList>
class ParameterControlWidget;

struct IMotionsDatagram {
	QString m_seqNumber;
	QString m_eventSource;
	QString m_sampleName;
	QString m_timestamp;
	QString m_mediaTime;
	QStringList m_rawData;
};

struct GameEngineDatagram {
	QString m_commandName;
	QStringList m_args;
};

struct GameEngineRegisterCommandDatagram {
	QString m_parameterName;
	QString m_isButton;
	QString m_type;
	QString m_minVal;
	QString m_startVal;
	QString m_maxVal;
};

enum class ComparisonType {
	k_less,
	k_lessEqual,
	k_equal,
	k_greaterEqual,
	k_greater,
	k_count,
};

enum class InputType {
	k_int,
	k_double,
	k_string,
	k_count,
};

struct TriggerDescription {
	// Sensor Values
	QString m_eventSource;
	QString m_sampleName;
	int m_fieldIndex;

	ComparisonType m_comparisionFunction;
	QString m_comparisonValue;

	// Game Engine Values
	QString m_parameterName;
	QString m_parameterValue;

	// The widget that is controlled by this trigger
	ParameterControlWidget* m_controlWidget;
};