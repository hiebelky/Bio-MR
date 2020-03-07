#pragma once

#include <QWidget>

// Forward declare
class NetworkManager;
class QLabel;

class DatagramCounter : public QWidget
{
	Q_OBJECT

public:
	DatagramCounter(NetworkManager* networkManager, QWidget* parent);

public slots:
	void UpdateCounters();

private:
	NetworkManager* m_pNetworkManager;

	QLabel* m_pGameEngineCounter;
	QLabel* m_pIMotionsCounter;
};

