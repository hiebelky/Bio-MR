#pragma once

#include <QWidget>

// Forward declare Qt classes
class QString;
class QAbstractSpinBox;

enum class InputType {
	k_Float,
	k_Int,
};

class ParameterControlWidget : public QWidget {
	Q_OBJECT
public:
	ParameterControlWidget(QStringList createInfo, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

signals:
	void DatagramReady(QString& message);

private slots:
	void OnValueChanged(double);
	void ConstructDatagram();

private:
	double CalcSingleStep(double range);

	QWidget* m_pParameterLabel;
	QAbstractSpinBox* m_pSpinBox;

	QString m_parameterName;
	InputType m_type = InputType::k_Float;
};