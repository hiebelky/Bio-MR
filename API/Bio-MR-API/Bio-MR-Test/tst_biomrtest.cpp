#include <QtTest>
#include <QCoreApplication>
#include <QtWidgets/QApplication>
#include <QStandardItemModel>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkDatagram>
#include <QtWidgets/QLabel>

// add necessary includes here
#include "../Bio-MR-API/Biomr.h"
#include "../Bio-MR-API/Types.h"
#include "../Bio-MR-API/StorageManager.h"
#include "../Bio-MR-API/ParameterControlWidget.h"
#include "../Bio-MR-API/TriggerList.h"
#include "../Bio-MR-API/NetworkManager.h"

class BioMRTest : public Biomr
{
    Q_OBJECT

public:
    BioMRTest();
    ~BioMRTest();

private slots:
    void guiTest();

};

BioMRTest::BioMRTest()
{

}

BioMRTest::~BioMRTest()
{
    
}

void BioMRTest::guiTest()
{
    // Create a game engine parameter
    GameEngineRegisterCommandDatagram parameter = { "GameEngineParameter", "false", "int", "0", "1", "100" };
    this->AddParameterControlWidget(parameter);

    // Find the newly created widget
    ParameterControlWidget* controlWidget = this->findChild<ParameterControlWidget*>();
    QVERIFY(controlWidget);

    // Find the tigger list
    QStandardItemModel* triggerModel = this->findChild<QStandardItemModel*>();
    QVERIFY(triggerModel);

    // Set up the trigger
    TriggerDescription* testTrigger = new TriggerDescription();
    testTrigger->m_eventSource = "EventSource";
    testTrigger->m_sampleName = "SampleName";
    testTrigger->m_fieldName = "FieldName";
    testTrigger->m_fieldIndex = 0;
    testTrigger->m_comparisionFunction = ComparisonType::k_greater;
    testTrigger->m_comparisonValue = "50";
    testTrigger->m_parameterName = "GameEngineParameter";
    testTrigger->m_parameterValue = "2";
    testTrigger->m_controlWidget = controlWidget;

    // Add the trigger to the the storage manager so it can respond to events
    triggerModel->appendRow(new TriggerItem(testTrigger));
    this->m_pStorageManager->AddTrigger(testTrigger);

    // Set up the networking ports
    QUdpSocket* recieveSocket = new QUdpSocket();
    recieveSocket->bind(QHostAddress::LocalHost, SEND_TO_GAME_ENGINE_PORT);

    // Set up a signal spy to listen for the datagram recieved event
    QSignalSpy spy(recieveSocket, SIGNAL(readyRead(void)));

    // Create a datagram which should cause a trigger
    QString datagram = "0;EventSource;SampleName;0;0;51;";
    QUdpSocket* sendSocket = new QUdpSocket();
    sendSocket->writeDatagram(datagram.toUtf8(), QHostAddress::LocalHost, RECEIVE_FROM_IMOTIONS_PORT);

    // Wait 100 ms to ensure the datagram was sent and recieved
    QTest::qWait(100);

    // Ensure exactly one datagram was recieved
    QCOMPARE(spy.count(), 1);

    // Read the datagram to ensure the data is correct
    QNetworkDatagram recDatagram = recieveSocket->receiveDatagram();
    QByteArray rawData = recDatagram.data();
    QString dataString = QString::fromUtf8(rawData);
    QCOMPARE(dataString, "GameEngineParameter;2;");





    // Test manually changing a control widget
    QString tempval = "3";
    controlWidget->UpdateValueExtern(tempval);

    // Wait 100 ms to ensure the datagram was sent and recieved
    QTest::qWait(100);

    // Ensure one more datagram was sent
    QCOMPARE(spy.count(), 2);

    // Read the datagram to ensure the data is correct
    recDatagram = recieveSocket->receiveDatagram();
    rawData = recDatagram.data();
    dataString = QString::fromUtf8(rawData);
    QCOMPARE(dataString, "GameEngineParameter;3;");



    // Test the datagram counter
    QLabel* iMotionsCounter = this->findChild<QLabel*>("iMotions Counter");
    QLabel* gameEngineCounter = this->findChild<QLabel*>("Game Engine Counter");
    QVERIFY(iMotionsCounter);
    QVERIFY(gameEngineCounter);
    QCOMPARE(iMotionsCounter->text(), "1 / 0");
    QCOMPARE(gameEngineCounter->text(), "0 / 2");

    //QTest::qWait(10000);
}

int main(int argc, char** argv) {  
    QApplication a(argc, argv);
    BioMRTest w;
    w.show();

    return QTest::qExec(&w, argc, argv);
}

//QTEST_MAIN(BioMRTest)

// Required to link test functions
#include "tst_biomrtest.moc"
