#pragma once

#include <QObject>

#include "Types.h"

// Forward declare Qt classes
class QNetworkDatagram;
class QUdpSocket;
class QString;
class QFile;


// Define the ports we will be using
const int RECEIVE_FROM_IMOTIONS_PORT = 60000;
const int SEND_TO_IMOTIONS_PORT = 60001;
const int SEND_TO_GAME_ENGINE_PORT = 60002;
const int RECEIVE_FROM_GAME_ENGINE_PORT = 60003;


class NetworkManager : public QObject {
	Q_OBJECT

public:
	NetworkManager(QObject* parent = nullptr);

	inline int GetIMotionsRecievedDatagramCount() { return m_iMotionsRecievedDatagramCounter; }
	inline int GetIMotionsSentDatagramCount() { return m_iMotionsSentDatagramCounter; }
	inline int GetGameEngineRecievedDatagramCount() { return m_gameEngineRecievedDatagramCounter; }
	inline int GetGameEngineSentDatagramCount() { return m_gameEngineSentDatagramCounter; }

public slots:
	void SendGameEngineDatagram(QString& datagram);
	void SendIMotionsDatagram(QString& datagram);

signals:
	void registerGameEngineCommand(GameEngineRegisterCommandDatagram& newCommand);
	void imotionsDataRecieved(IMotionsDatagram& newDatagram);

	void datagramCountChanged();

private:
	void ReadImotionsDatagrams();
	void ProcessImotionsDatagram(QNetworkDatagram& datagram);

	void ReadGameEngineDatagrams();
	void ProcessGameEngineDatagram(QNetworkDatagram& datagram);


	// One socket for each communication path
	QUdpSocket* m_pIMotionsListener = nullptr;
	QUdpSocket* m_pIMotionsSender = nullptr;
	QUdpSocket* m_pGameEngineListener = nullptr;
	QUdpSocket* m_pGameEngineSender = nullptr;

	// One counter for each socket
	int m_iMotionsRecievedDatagramCounter = 0;
	int m_iMotionsSentDatagramCounter = 0;
	int m_gameEngineRecievedDatagramCounter = 0;
	int m_gameEngineSentDatagramCounter = 0;

	// Loggers to write packets
	QFile* m_pIMotionsLog;
	QFile* m_pGameEngineLog;
};