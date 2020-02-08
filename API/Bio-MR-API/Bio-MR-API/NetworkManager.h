#pragma once

#include <QObject>

// Forward declare Qt classes
class QNetworkDatagram;
class QUdpSocket;
class QString;


// Define the ports we will be using
const int RECEIVE_FROM_IMOTIONS_PORT = 60000;
const int SEND_TO_IMOTIONS_PORT = 60001;
const int SEND_TO_GAME_ENGINE_PORT = 60002;
const int RECEIVE_FROM_GAME_ENGINE_PORT = 60003;


class NetworkManager : public QObject {
	Q_OBJECT

public:
	NetworkManager(QObject* parent = nullptr);

	void SendGameEngineDatagram(QString& datagram);
	void SendIMotionsDatagram(QString& datagram);

signals:
	void registerCommand(QStringList& newCommand);

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
};