#ifndef SERIALPORTTRANSMITTER_H
#define SERIALPORTTRANSMITTER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QDebug>
#include <QTimer>
#include <QMap>
#include <QVariant>
#include "protocol_TCM2.h"
#include "qpiconfig.h"

namespace TCM {

enum {Standby, addPsi, addPitch, addRoll, addXYZ, ContiniousMode};

class serialPortTransmitter : public QObject {
    Q_OBJECT
public:

    serialPortTransmitter(QObject *parent = 0);
    serialPortTransmitter(const QString & config , const QString & name , QObject *parent=0);
    virtual ~serialPortTransmitter();


private:
    QSerialPort com;
    QByteArray dataTCM;
    QTimer time, timeDiagn;

    int state;
    QVector <QByteArray> commands;
    QHash <QChar, double> recData;
    QVector <QVector < QVariant > > recVec;
    TCM_Data *dataToSend;
    TCM_Data testData;

    int timeout;
    int packetsSended;
    int packetsReceived;
    int packetsInvalid;
    bool opened;

    bool checkSum(QByteArray packet);
    void parsePacket(QVector <QVector <QVariant> > packetTemplate, QByteArray packet);

signals:
    void portOpened(bool state);
    void portInfo(QString name, int baudRate, bool state);
    void info_send_for_show(TCM_Data data_for_show);//для вывода данных на экран
    void testSendSignal(TCM_Data);

public slots:
    void sendActionCommand(QByteArray command, int time_ms);
    void checkReceivedData();
    void openPort(QString name, qint32 baudRate);
    void closePort(QString name);
    void readData();
    void parseData();
    void getTCMData(TCM_Data &data);
    void testSend();
    void sendPortInfo();


};

} //namspace TCM
#endif // SERIALPORTTRANSMITTER_H
