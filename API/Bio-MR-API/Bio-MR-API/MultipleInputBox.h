#pragma once

#include <QWidget>

#include "Types.h"

// Forward declare
class QDoubleSpinBox;
class QLineEdit;

class MultipleInputBox : public QWidget
{
	Q_OBJECT
public:
	MultipleInputBox(InputType type, QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

	QString GetValue();
	void SetValue(QString in);
	void SetMaxValue(double in);
	void SetMinValue(double in);

	inline InputType GetType() { return m_type; }

signals:
	void ValueChanged(QString val);
private:

	void UpdateSpinBoxRange();

	InputType m_type;

	// Int / Double
	QDoubleSpinBox* m_pSpinBox = nullptr;

	// String
	QLineEdit* m_pLineEdit = nullptr;
};