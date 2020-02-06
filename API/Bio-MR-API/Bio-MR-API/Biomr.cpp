#include "Biomr.h"

#include "labeledSlider.h"
#include <QNetworkDatagram>

Biomr::Biomr(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Set up sliders with min/max values
	ui.RainIntensitySlider->SetMaxValueDouble(1.0);
	ui.RainIntensitySlider->SetMinValueDouble(0.0);
	ui.RainIntensitySlider->SetNumTicks(100);
	connect(ui.RainIntensitySlider, &LabeledSlider::valueChanged, this, &Biomr::SetRainIntensity);

	ui.DayNightCycleSlider->SetMaxValueDouble(30);
	ui.DayNightCycleSlider->SetMinValueDouble(0.05);
	ui.DayNightCycleSlider->SetNumTicks(1000);
	ui.DayNightCycleSlider->UpdateSliderPosition();
	connect(ui.DayNightCycleSlider, &LabeledSlider::valueChanged, this, &Biomr::SetDayNightCycle);


	// Set up sockets
	m_pIMotionsListener = new QUdpSocket();
	m_pIMotionsListener->bind(QHostAddress::LocalHost, RECEIVE_FROM_IMOTIONS_PORT);
	connect(m_pIMotionsListener, &QUdpSocket::readyRead, this, &Biomr::ReadImotionsDatagrams);

	m_pGameEngineSender = new QUdpSocket();
}




// Reading from Imotions
void Biomr::ReadImotionsDatagrams()
{
	while (m_pIMotionsListener->hasPendingDatagrams()) 
	{
		QNetworkDatagram datagram = m_pIMotionsListener->receiveDatagram();
		ProcessImotionsDatagram(datagram);
	}
}
void Biomr::ProcessImotionsDatagram(QNetworkDatagram& datagram)
{
	QByteArray& rawData = datagram.data();
	QString dataString = QString::fromUtf8(rawData);
	QStringList splitData = dataString.split(";");

	// Add Qiu's command parsing code

	SetRainIntensity(0.5);
}


// Sending to game engine
void Biomr::SendGameEngineDatagram(QString& datagram)
{
	QByteArray& rawData = datagram.toUtf8();
	m_pGameEngineSender->writeDatagram(rawData, QHostAddress::LocalHost, SEND_TO_GAME_ENGINE_PORT);
}

void Biomr::SetRainIntensity(double val)
{
	QString command = QString("RainIntensity;%1;").arg(val);
	SendGameEngineDatagram(command);
}

void Biomr::SetDayNightCycle(double val)
{
	QString command = QString("DayLength;%1;").arg(val);
	SendGameEngineDatagram(command);
}

