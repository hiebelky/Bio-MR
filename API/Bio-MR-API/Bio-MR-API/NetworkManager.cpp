#include "NetworkManager.h"

#include <QNetworkDatagram>
#include <QUdpSocket>
#include <QString>

NetworkManager::NetworkManager(QObject* parent) : QObject(parent)
{
	// Set up sockets
	m_pIMotionsListener = new QUdpSocket();
	m_pIMotionsListener->bind(QHostAddress::LocalHost, RECEIVE_FROM_IMOTIONS_PORT);
	connect(m_pIMotionsListener, &QUdpSocket::readyRead, this, &NetworkManager::ReadImotionsDatagrams);

	m_pGameEngineSender = new QUdpSocket();
}

// Reading from Imotions
void NetworkManager::ReadImotionsDatagrams()
{
	while (m_pIMotionsListener->hasPendingDatagrams())
	{
		QNetworkDatagram datagram = m_pIMotionsListener->receiveDatagram();
		ProcessImotionsDatagram(datagram);
	}
}
void NetworkManager::ProcessImotionsDatagram(QNetworkDatagram& datagram)
{
	QByteArray& rawData = datagram.data();
	QString dataString = QString::fromUtf8(rawData);
	QStringList splitData = dataString.split(";");

	// Add Qiu's command parsing code

	//SetRainIntensity(0.5);
}


// Sending to game engine
void NetworkManager::SendGameEngineDatagram(QString& datagram)
{
	QByteArray& rawData = datagram.toUtf8();
	m_pGameEngineSender->writeDatagram(rawData, QHostAddress::LocalHost, SEND_TO_GAME_ENGINE_PORT);
}
