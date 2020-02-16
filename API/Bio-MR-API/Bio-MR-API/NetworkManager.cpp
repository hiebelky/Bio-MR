#include "NetworkManager.h"

#include <QNetworkDatagram>
#include <QUdpSocket>
#include <QString>


NetworkManager::NetworkManager(QObject* parent) : QObject(parent)
{
	// Set up recieve sockets
	m_pIMotionsListener = new QUdpSocket(this);
	m_pIMotionsListener->bind(QHostAddress::LocalHost, RECEIVE_FROM_IMOTIONS_PORT);
	connect(m_pIMotionsListener, &QUdpSocket::readyRead, this, &NetworkManager::ReadImotionsDatagrams);

	m_pGameEngineListener = new QUdpSocket(this);
	m_pGameEngineListener->bind(QHostAddress::LocalHost, RECEIVE_FROM_GAME_ENGINE_PORT);
	connect(m_pGameEngineListener, &QUdpSocket::readyRead, this, &NetworkManager::ReadGameEngineDatagrams);


	// Set up send sockets
	m_pGameEngineSender = new QUdpSocket(this);
	m_pIMotionsSender = new QUdpSocket(this);
}



// ----------------------------------------------------------------------
// Reading from Imotions
// ----------------------------------------------------------------------
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

	// Must have length 5
	if (splitData.size() < 5) {
		return;
	}

	IMotionsDatagram result;
	result.m_seqNumber = splitData[0];
	result.m_eventSource = splitData[1];
	result.m_sampleName = splitData[2];
	result.m_timestamp = splitData[3];
	result.m_mediaTime = splitData[4];

	splitData.removeAt(0);
	splitData.removeAt(0);
	splitData.removeAt(0);
	splitData.removeAt(0);
	splitData.removeAt(0);
	result.m_rawData = splitData;

	emit imotionsDataRecieved(result);
}



// ----------------------------------------------------------------------
// Reading from Game Engine
// ----------------------------------------------------------------------
void NetworkManager::ReadGameEngineDatagrams()
{
	while (m_pGameEngineListener->hasPendingDatagrams())
	{
		QNetworkDatagram datagram = m_pGameEngineListener->receiveDatagram();
		ProcessGameEngineDatagram(datagram);
	}
}
void NetworkManager::ProcessGameEngineDatagram(QNetworkDatagram& datagram)
{
	QByteArray& rawData = datagram.data();
	QString dataString = QString::fromUtf8(rawData);
	QStringList splitData = dataString.split(";");

	// Command length must be larger than 1
	if (splitData.length() < 1) {
		return;
	}

	// Check if the game engine has requested to register a command
	if (splitData.at(0).compare("RegisterCommand", Qt::CaseInsensitive) == 0)
	{
		// Remove the "RegisterCommand" string
		splitData.removeFirst();

		// This command needs to have 6 or more arguments
		if (splitData.length() < 6) {
			return;
		}

		GameEngineRegisterCommandDatagram command = { splitData[0], splitData[1], splitData[2], splitData[3], splitData[4], splitData[5] };

		emit registerGameEngineCommand(command);
	}

	if (splitData.at(0).compare("PickedUpItem", Qt::CaseInsensitive) == 0)
	{
		// TODO: Forward this to imotions
	}

	if (splitData.at(0).compare("Navigating", Qt::CaseInsensitive) == 0)
	{
		// TODO: Forward this to imotions
	}
}



// ----------------------------------------------------------------------
// Sending to Game Engine
// ----------------------------------------------------------------------
void NetworkManager::SendGameEngineDatagram(QString& datagram)
{
	QByteArray& rawData = datagram.toUtf8();
	m_pGameEngineSender->writeDatagram(rawData, QHostAddress::LocalHost, SEND_TO_GAME_ENGINE_PORT);
}


// ----------------------------------------------------------------------
// Sending to IMotions
// ----------------------------------------------------------------------
void NetworkManager::SendIMotionsDatagram(QString& datagram)
{
	QByteArray& rawData = datagram.toUtf8();
	m_pIMotionsSender->writeDatagram(rawData, QHostAddress::LocalHost, SEND_TO_IMOTIONS_PORT);
}