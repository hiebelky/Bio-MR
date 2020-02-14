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
	m_pTriggerModel->appendRow(new QStandardItem("Test"));

	// Create the add/remove buttons
	QWidget* pAddRemoveRow = new QWidget(this);
	QPushButton* pAddButton = new QPushButton("Add", this);
	QPushButton* pRemoveButton = new QPushButton("Remove", this);

	QHBoxLayout* pAddRemoveRowLayout = new QHBoxLayout(this);
	pAddRemoveRowLayout->setContentsMargins(0, 0, 0, 0);
	pAddRemoveRow->setLayout(pAddRemoveRowLayout);
	pAddRemoveRowLayout->addWidget(pAddButton);
	pAddRemoveRowLayout->addWidget(pRemoveButton);

	// Add the buttons to the layout
	pMainLayout->addWidget(pAddRemoveRow);

	for (QModelIndex& index : m_pTriggerView->selectionModel()->selectedIndexes()) {
		m_pTriggerModel->removeRow(index.row());
	}
}