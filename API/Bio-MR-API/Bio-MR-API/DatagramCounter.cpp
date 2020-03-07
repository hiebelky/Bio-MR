#include "DatagramCounter.h"

#include "NetworkManager.h"

#include <QGridLayout>
#include <QString>
#include <QLabel>


DatagramCounter::DatagramCounter(NetworkManager* networkManager, QWidget* parent)
	: QWidget(parent), m_pNetworkManager(networkManager)
{
	QGridLayout* pMainLayout = new QGridLayout(this);
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(pMainLayout);

	QLabel* pImotionsLabel = new QLabel("iMotions (rec/send)", this);
	pImotionsLabel->setToolTip("The number of datagrams recieved from and sent to iMotions.");
	m_pIMotionsCounter = new QLabel("0 / 0", this);
	m_pIMotionsCounter->setToolTip("The number of datagrams recieved from and sent to iMotions.");

	QLabel* pGameEngineLabel = new QLabel("Game Engine (rec/send)", this);
	pGameEngineLabel->setToolTip("The number of datagrams recieved from and sent to the game engine.");
	m_pGameEngineCounter = new QLabel("0 / 0", this);
	m_pGameEngineCounter->setToolTip("The number of datagrams recieved from and sent to the game engine.");
	pGameEngineLabel->setAlignment(Qt::AlignRight);
	m_pGameEngineCounter->setAlignment(Qt::AlignRight);

	pMainLayout->addWidget(pImotionsLabel, 0, 0, 1, 1);
	pMainLayout->addWidget(m_pIMotionsCounter, 1, 0, 1, 1);
	pMainLayout->setColumnStretch(0, 99);
	pMainLayout->addWidget(pGameEngineLabel, 0, 1, 1, 1);
	pMainLayout->addWidget(m_pGameEngineCounter, 1, 1, 1, 1);

	connect(networkManager, &NetworkManager::datagramCountChanged, this, &DatagramCounter::UpdateCounters);
}

void DatagramCounter::UpdateCounters() 
{
	int imr = m_pNetworkManager->GetIMotionsRecievedDatagramCount();
	int ims = m_pNetworkManager->GetIMotionsSentDatagramCount();
	int gmr = m_pNetworkManager->GetGameEngineRecievedDatagramCount();
	int gms = m_pNetworkManager->GetGameEngineSentDatagramCount();

	QString iMotionsText = QString("%1 / %2").arg(imr).arg(ims);
	QString gameEngineText = QString("%1 / %2").arg(gmr).arg(gms);

	m_pIMotionsCounter->setText(iMotionsText);
	m_pGameEngineCounter->setText(gameEngineText);
}