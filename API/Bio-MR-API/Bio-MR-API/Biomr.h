#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Biomr.h"

// Forward Declare
class NetworkManager;
class StorageManager;

class Biomr : public QMainWindow
{
	Q_OBJECT

public:
	Biomr(QWidget *parent = nullptr);

public slots:
	void SetRainIntensity(double val);
	void SetDayNightCycle(double val);

	void AddParameterControlWidget(QStringList& params);

private:
	Ui::BiomrClass ui;

	NetworkManager* m_pNetworkManager = nullptr;
	StorageManager* m_pStorageManager = nullptr;
};
