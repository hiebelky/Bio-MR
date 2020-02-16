#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Biomr.h"
#include "Types.h"

// Forward Declare
class NetworkManager;
class StorageManager;


class Biomr : public QMainWindow
{
	Q_OBJECT

public:
	Biomr(QWidget *parent = nullptr);

public slots:
	void AddParameterControlWidget(GameEngineRegisterCommandDatagram& params);

private:
	Ui::BiomrClass ui;

	NetworkManager* m_pNetworkManager = nullptr;
	StorageManager* m_pStorageManager = nullptr;
};
