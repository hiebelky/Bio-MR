#pragma once
#include <QWidget>

// Forward declare
class QListView;
class QStandardItemModel;

class TriggerList : public QWidget
{
	Q_OBJECT
public:
	TriggerList(QWidget* parent = nullptr);
private:
	QListView* m_pTriggerView = nullptr;
	QStandardItemModel* m_pTriggerModel = nullptr;
};

