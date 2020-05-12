#pragma once

#include <QObject>

#include "Types.h"

// Forward declare Qt classes
class QNetworkDatagram;
class QUdpSocket;
class QString;
class QFile;


// These ports are constant throughout the project
const int RECEIVE_FROM_IMOTIONS_PORT = 60000;
const int SEND_TO_IMOTIONS_PORT = 60001;
const int SEND_TO_GAME_ENGINE_PORT = 60002;
const int RECEIVE_FROM_GAME_ENGINE_PORT = 60003;


class NetworkManager : public QObject {
	Q_OBJECT

public:
	NetworkManager(QObject* parent = nullptr);

	// Helpers for the datagram counter
	inline int GetIMotionsRecievedDatagramCount() { return m_iMotionsRecievedDatagramCounter; }
	inline int GetIMotionsSentDatagramCount() { return m_iMotionsSentDatagramCounter; }
	inline int GetGameEngineRecievedDatagramCount() { return m_gameEngineRecievedDatagramCounter; }
	inline int GetGameEngineSentDatagramCount() { return m_gameEngineSentDatagramCounter; }

public slots:
	// Helpers to send datagrams
	void SendGameEngineDatagram(QString& datagram);
	void SendIMotionsDatagram(QString& datagram);

	// Passthrough mode automatically forwards iMotions datagrams to the game engine,
	// thus skipping all data processing in the API
	void SetPassthroughMode(bool checked);

signals:
	// Signal raised when the game engine indicates a parameter which can be modified
	// To respond to this, we must add a new parameter control widget to the manual control
	// and register this commend in the storage manager
	void registerGameEngineCommand(GameEngineRegisterCommandDatagram& newCommand);

	// Signal raised every time data is recieved from iMotions. When this occurs,
	// we must check the data to see if any threshold has been exceeded
	void imotionsDataRecieved(IMotionsDatagram& newDatagram);

	// Signal raised every time a datagram is recieved. Used to update the datagram counter
	void datagramCountChanged();

private:
	// Functions used for reading datagrams from iMotions
	void ReadImotionsDatagrams();
	void ProcessImotionsDatagram(QNetworkDatagram& datagram);

	// Functions used for reading datagrams from a game engine
	void ReadGameEngineDatagrams();
	void ProcessGameEngineDatagram(QNetworkDatagram& datagram);

	// When in this mode, the network manager forwards all packets
	// from iMotions directly to the game engine
	bool m_isPassthroughMode = false;

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