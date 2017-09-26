#ifndef CONSOLECONTROL_H
#define CONSOLECONTROL_H

#include <QObject>
#include <string>
#include <QUdpSocket>
#include <protocol_TCM2.h>
#include <iostream>
#include "serialporttransmitter.h"
#include "tcm_ethernet.h"

using namespace std;

class ConsoleControl : public QObject
{
    Q_OBJECT
public:
    explicit ConsoleControl(QObject *parent = 0);

signals:

public slots:
    void infoCollect();
    void info_COM_collect(QString port, int speed, bool opened);
    void info_Eth_collect(QString IPs, QString IPr, QString IPg, int senderPort, int receiverPort,int gansSenderPort,  int frequency);
    void info_data_collect(TCM_Data datas);
    void portOpened(QHostAddress IP,int port, bool opened, int freq); //сигнал с IP, портом +получилось открыть порт или нет
    void senderInfo(QHostAddress IP, int port);//сигнал с информацией об отправителе (из .conf файла)
    void gansSenderInfo (QHostAddress IP, int port);//сигнал с информацией об отправителе ГАНС (из .conf файла)
    void errorInfo (QHostAddress IP, int port, QString error);
    void diagnInfo(int packetsSended, int GANSPacketsSended, int packetsReceived, int packetsInvalid);
    void tetsDataReceive(TCM_Data);

private:
    QString COM_port;
    int COM_speed;
    bool COM_opened;//true - открыт, false - закрыт
    QString Eth_senderIP, Eth_receiverIP, GANS_senderIP;
    int Eth_senderPort, Eth_receiverPort;
    int GANS_senderPort;
    int Eth_frequency;
    int packetsSended, GANSPacketsSended;
    TCM_Data data_show, testData;
    QString recPortState, recPortError;
    int count;
};

#endif // CONSOLECONTROL_H
