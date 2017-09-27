#ifndef TCM_ETHERNET_H
#define TCM_ETHERNET_H

#include <QObject>
#include "protocol_TCM2.h"
#include <QUdpSocket>
#include <QTimer>



class TcmEthernet : public QObject {
    Q_OBJECT
public:
    TcmEthernet(QObject *parent = 0);
    TcmEthernet(const QString & config , const QString & name , QObject *parent = 0);
    TcmEthernet(QHostAddress recIP, int recPort, QHostAddress sendIP, int sendPort, int freq);
signals:
    void portOpened(QHostAddress IP,int port, bool opened, int freq); //сигнал с IP, портом +получилось открыть порт или нет
    void socketInfo (QHostAddress IP, int port, QString state); //состояние порта (выводить отдельно)
    void senderInfo(QHostAddress IP, int port);//сигнал с информацией об отправителе (из .conf файла)
    void errorInfo (QHostAddress IP, int port, QString error);
    void diagnInfo(int packetsSended, int packetsReceived, int packetsInvalid);
    void info_Eth_collect(QString IPs, QString IPr, int senderPort, int receiverPort, int frequency, bool opened);

private:
    QTimer *timer;
    QUdpSocket *sender;
    TCM2_protocol_in dataToSend;
    QHostAddress senderIP, receiverIP;
    int senderPort, receiverPort;
    int frequency;
    int packetsSended, packetsReceived, packetsInvalid;
    bool opened;


    unsigned int checksum(const void *data, int size);


public slots:
    void open_port(QHostAddress recIP, int recPort, QHostAddress sendIP, int sendPort, int freq);
    void setTCMDataToSend(TCM_Data data);
    void writeData();
    void socketState(QAbstractSocket::SocketState state);
    void socketError(QAbstractSocket::SocketError socketErro);
    void sendInfo();
};

#endif // TCM_ETHERNET_H
