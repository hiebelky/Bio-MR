#pragma once

#include "Types.h"

#include <QWidget>

class QHBoxLayout;
class MultipleInputBox;

class ParameterControlWidget : public QWidget {
	Q_OBJECT
public:
	ParameterControlWidget(GameEngineRegisterCommandDatagram& createInfo, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

signals:
	void DatagramReady(QString& message);

private slots:
	void ConstructDatagram();

private:

	QString m_parameterName;
	QWidget* m_pParameterLabel = nullptr;
	MultipleInputBox* m_pInputBox = nullptr;
	QHBoxLayout* m_pLayout = nullptr;
};

