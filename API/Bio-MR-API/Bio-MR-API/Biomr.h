#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Biomr.h"
#include "NetworkManager.h"

class Biomr : public QMainWindow
{
	Q_OBJECT

public:
	Biomr(QWidget *parent = nullptr);

public slots:
	void SetRainIntensity(double val);
	void SetDayNightCycle(double val);

private:
	Ui::BiomrClass ui;

	NetworkManager* m_pNetworkManager = nullptr;
};
