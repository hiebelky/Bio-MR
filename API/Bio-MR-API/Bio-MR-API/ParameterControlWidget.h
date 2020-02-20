#pragma once

#include "Types.h"

#include <QWidget>

class QHBoxLayout;
class MultipleInputBox;
class QTimer;

class ParameterControlWidget : public QWidget {
	Q_OBJECT
public:
	ParameterControlWidget(GameEngineRegisterCommandDatagram& createInfo, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

	// Called when a trigger wants to change the value corresponding to this widget
	void UpdateValueExtern(QString& value);

signals:
	void DatagramReady(QString& message);

private slots:
	void ConstructDatagram();
	void UpdateBackgroundColor();

private:

	QString m_parameterName;
	QWidget* m_pParameterLabel = nullptr;
	MultipleInputBox* m_pInputBox = nullptr;
	QHBoxLayout* m_pLayout = nullptr;


	// Values for changing the background color
	const QColor bgColor = { 120, 205, 245, 255 };
	QColor startingBgColor;
	const int bgTotalTime = 1000; // msec
	const int bgTimerInterval = 25; // msec
	double blendPercent = 0;
	QTimer* m_pTimer;
};

