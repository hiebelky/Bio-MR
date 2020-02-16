#pragma once

#include <QString>
#include <QStringList>

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