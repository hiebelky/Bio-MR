#include "TriggerList.h"


#include <QListView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

TriggerList::TriggerList(QWidget* parent) : QWidget(parent)
{
	// Creaate the main layout
	QVBoxLayout* pMainLayout = new QVBoxLayout();
	setLayout(pMainLayout);

	// Add the list of view items
	m_pTriggerView = new QListView(this);
	pMainLayout->addWidget(m_pTriggerView);

	// Create the item model
	m_pTriggerModel = new QStandardItemModel(0, 1, this);
	m_pTriggerView->setModel(m_pTriggerModel);
	QStandardItem* testRow = new TriggerItem<int>("Shimmer", "GSR", "Heart Rate", 0, ComparisonType::k_greaterEqual, 80, "Rain Intensity", "0.50");
	m_pTriggerModel->appendRow(testRow);

	// Create the add/remove buttons
	QWidget* pAddRemoveRow = new QWidget(this);
	QPushButton* pAddButton = new QPushButton("Add", this);
	QPushButton* pRemoveButton = new QPushButton("Remove", this);

	QHBoxLayout* pAddRemoveRowLayout = new QHBoxLayout(this);
	pAddRemoveRowLayout->setContentsMargins(0, 0, 0, 0);
	pAddRemoveRow->setLayout(pAddRemoveRowLayout);
	pAddRemoveRowLayout->addWidget(pAddButton);
	pAddRemoveRowLayout->addWidget(pRemoveButton);
	pMainLayout->addWidget(pAddRemoveRow);

	// Set up button actions
	connect(pRemoveButton, &QPushButton::pressed, this, [&] {
		for (QModelIndex& index : m_pTriggerView->selectionModel()->selectedIndexes()) {
			m_pTriggerModel->removeRow(index.row());
		}
	});
}
