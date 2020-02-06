#pragma once

#include <QtWidgets/QMainWindow>
#include <QUdpSocket>
#include "ui_Biomr.h"

const int RECEIVE_FROM_IMOTIONS_PORT = 60000;
const int SEND_TO_IMOTIONS_PORT = 60001;
const int SEND_TO_GAME_ENGINE_PORT = 60002;
const int RECEIVE_FROM_GAME_ENGINE_PORT = 60003;


class Biomr : public QMainWindow
{
	Q_OBJECT

public:
	Biomr(QWidget *parent = Q_NULLPTR);

	void ReadImotionsDatagrams();
	void ProcessImotionsDatagram(QNetworkDatagram& datagram);

	void SendGameEngineDatagram(QString& datagram);

public slots:
	void SetRainIntensity(double val);
	void SetDayNightCycle(double val);

private:
	Ui::BiomrClass ui;

	// One socket for each communication path
	QUdpSocket* m_pIMotionsListener = nullptr;
	QUdpSocket* m_pIMotionsSender = nullptr;
	QUdpSocket* m_pGameEngineListener = nullptr;
	QUdpSocket* m_pGameEngineSender = nullptr;
};
