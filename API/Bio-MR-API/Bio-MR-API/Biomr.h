#pragma once

#include "Types.h"

#include <QMainWindow>

// Forward Declare
class NetworkManager;
class StorageManager;

class QGridLayout;
class QVBoxLayout;


class Biomr : public QMainWindow
{
	Q_OBJECT

public:
	Biomr(QWidget *parent = nullptr);

public slots:
	void AddParameterControlWidget(GameEngineRegisterCommandDatagram& params);
	void HandleAutomaticTriggers(IMotionsDatagram& datagram);

protected:
	NetworkManager* m_pNetworkManager = nullptr;
	StorageManager* m_pStorageManager = nullptr;


	QGridLayout* m_pMainLayout = nullptr;
	QVBoxLayout* m_pAutomaticTriggerLayout = nullptr;
	QVBoxLayout* m_pManualControlLayout = nullptr;
};
