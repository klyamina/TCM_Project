#include "consolecontrol.h"
#include <QDebug>
#include <stdlib.h>
#include "protocol_TCM2.h"

ConsoleControl::ConsoleControl(QObject *parent) :
    QObject(parent) {
    packetsSended=count=0;
    COM_opened = false;
    memset(&data_show,0,sizeof(data_show));
    Eth_receiverIP.clear();
    Eth_senderIP.clear();
    GANS_senderIP.clear();
    Eth_frequency=Eth_receiverPort=Eth_senderPort=0;
    GANSPacketsSended=GANS_senderPort=0;
}

void ConsoleControl::infoCollect()
{
    //Очистка окна
    system("cls");//для Win

    cout << "COM-connection\n";
    if (COM_opened) cout << "COM opened.\n";
    else  cout << "COM failed.\n";
    cout << "COM-port: " << COM_port.toStdString() << " COM rate: " << COM_speed << "\n";

    cout << "\n-----------------------------------\n\n";

    cout << "TCM data\n";
    cout << "Heading: " << data_show.yaw << "\n";
    cout << "Pitch: " << data_show.pitch << "\n";
    cout << "Roll: " << data_show.roll << "\n";
    cout << "Mx = " << data_show.Mx << " My = " << data_show.My << " Mz = " << data_show.Mz << "\n";
    cout << "Incl out of range: " << data_show.errorInclOutOfRange << "\n";
    cout << "Magn out of range: " << data_show.errorMagnOutOfRange << "\n";
    cout << "Magn dist: " << data_show.errorMagnDistortion << "\n";
    cout << "Sended " << data_show.packetsSended << " packets to TCM2 \n";
    cout << "Received " << data_show.packetsReceived << " packets from TCM2\n";
    cout << "Received " << data_show.packetsInvalid << " invalid packets\n";


    cout << "\n-----------------------------------\n\n";

    cout << "Ethetnet-connection\n";
    cout << "Ethetnet-sender Port: " << Eth_senderPort << " Ethetnet-sender IP: " << Eth_senderIP.toStdString() << "\n";
    cout << "Ethetnet-receiver Port: " << Eth_receiverPort << " Ethetnet-receiver IP: " << Eth_receiverIP.toStdString() << "\n";
    cout << "Ethernet-receiver port state: " << recPortState.toStdString() << "\n";
    cout << "Ethernet - receiver error: " << recPortError.toStdString() << "\n";
    cout << "Ethetnet frequency: " << Eth_frequency << "\n";
    cout << "Sended " << packetsSended <<" packets\n";
    cout << "GANS Ethetnet-connection information\n";
    cout << "Ethetnet-sender Port: " << GANS_senderPort << " Ethetnet-sender IP: " << GANS_senderIP.toStdString() << "\n";
    cout << "Sended " << GANSPacketsSended <<" packets\n";




}

void ConsoleControl::info_COM_collect(QString port, int speed, bool opened)
{
    COM_port = port;
    COM_speed = speed;
    COM_opened = opened;
}

void ConsoleControl::info_Eth_collect(QString IPs, QString IPr, QString IPg, int senderPort, int receiverPort,int gansSenderPort,  int frequency)
{
    Eth_senderIP = IPs;
    Eth_receiverIP = IPr;
    GANS_senderIP=IPg;
    Eth_senderPort = senderPort;
    GANS_senderPort = gansSenderPort;
    Eth_receiverPort = receiverPort;
    Eth_frequency = frequency;

}

void ConsoleControl::info_data_collect(TCM_Data datas)
{
    data_show = datas;
    count++;
}

void ConsoleControl::portOpened(QHostAddress IP,int port, bool opened, int freq){
    Eth_receiverIP=IP.toString();
    Eth_receiverPort=port;
    Eth_frequency=freq;
}
void ConsoleControl::senderInfo(QHostAddress IP, int port){
    Eth_senderIP=IP.toString();
    Eth_senderPort=port;
}
void ConsoleControl::gansSenderInfo(QHostAddress IP, int port){
    GANS_senderIP=IP.toString();
    GANS_senderPort=port;
}
void ConsoleControl::errorInfo (QHostAddress IP, int port, QString error){
    recPortError=error;
}
void ConsoleControl::diagnInfo(int sended, int testSended, int packetsReceived, int packetsInvalid){
    packetsSended=sended;
    GANSPacketsSended=testSended;
}

void ConsoleControl::tetsDataReceive(TCM_Data data){
    testData=data;
}
