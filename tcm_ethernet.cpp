#include "tcm_ethernet.h"
#include "qpiconfig.h"


TcmEthernet::TcmEthernet(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    sender = new QUdpSocket(this);
    memset(&dataToSend, 0, sizeof(dataToSend));
    dataToSend.head.len=sizeof(dataToSend);
    dataToSend.head.type=2;
    dataToSend.head.start_byte[0]=173;
    packetsSended=packetsReceived=packetsInvalid=0;
    opened=false;

}

TcmEthernet::TcmEthernet(const QString & config , const QString & name , QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    sender = new QUdpSocket(this);
    memset(&dataToSend, 0, sizeof(dataToSend));
    dataToSend.head.len=sizeof(dataToSend);
    dataToSend.head.type=2;
    dataToSend.head.start_byte[0]=173;
    packetsSended=packetsReceived=packetsInvalid=testPacketsSended=0;
    opened=false;

    QPIConfig conf(config, QIODevice::ReadOnly);
    QPIConfig::Entry & e(conf.getValue(name));
    senderIP = e.getValue("sender.ip").value();
    receiverIP = e.getValue("receiver.ip").value();
    senderPort = e.getValue("sender.port", 0);
    receiverPort = e.getValue("receiver.port", 0);
    frequency = e.getValue("receiver.frequency", 20);
    gansSenderIP=e.getValue("testSender.ip").value();
    gansSenderPort=e.getValue("testSender.port", 0);

    qDebug()<<"senderIP"<<senderIP;
    qDebug()<<"receiverIP"<<receiverIP;
    qDebug()<<"senderPort"<<senderPort;
    qDebug()<<"receiverPort"<<receiverPort;
    qDebug()<<"frequency"<<frequency;
    qDebug()<<"testSenderPort"<<gansSenderPort;
    qDebug()<<"testSenderIP"<<gansSenderIP;

    connect(sender, SIGNAL(stateChanged(QAbstractSocket::SocketState)),SLOT(socketState(QAbstractSocket::SocketState)));
    connect(sender, SIGNAL(error(QAbstractSocket::SocketError)),SLOT(socketError(QAbstractSocket::SocketError)));
    connect(timer, SIGNAL(timeout()), SLOT(sendInfo()));
    open_port(receiverIP, receiverPort, senderIP, senderPort, frequency);

}

TcmEthernet::TcmEthernet(QHostAddress recIP, int recPort, QHostAddress sendIP, int sendPort, int freq=20){

    timer = new QTimer(this);
    sender = new QUdpSocket(this);
    memset(&dataToSend, 0, sizeof(dataToSend));
    dataToSend.head.len=sizeof(dataToSend);
    dataToSend.head.type=2;
    dataToSend.head.start_byte[0]=173;
    packetsSended=packetsReceived=packetsInvalid=0;
    opened=false;

    receiverIP=recIP; receiverPort=recPort; senderIP=sendIP; senderPort=sendPort; frequency=freq;
    open_port(receiverIP, receiverPort, senderIP, senderPort, frequency);
}

void TcmEthernet::open_port(QHostAddress recIP, int recPort, QHostAddress sendIP, int sendPort, int freq=20){
    senderIP=sendIP;
    senderPort=sendPort;
    if (sender->bind(recIP,recPort)) {
        opened=true;
    }
    else {
        opened=false;
         //emit portOpened(sender->bind(recIP,recPort));
    }
    emit portOpened(receiverIP, receiverPort, opened, frequency);
    connect(timer,SIGNAL(timeout()), this, SLOT(writeData()));
    if(freq) timer->setInterval(int(1000/freq));
    else timer->setInterval(50);
    timer->start();
}


void TcmEthernet::writeData(){
    dataToSend.head.count++;
    dataToSend.checksum=checksum(&dataToSend,(sizeof(dataToSend)-sizeof(dataToSend.checksum)));
    if (sender->writeDatagram((char *)&dataToSend,sizeof(dataToSend), senderIP, senderPort)) {
        packetsSended++;
    }
    if (sender->writeDatagram((char *)&dataToSend,sizeof(dataToSend), gansSenderIP, gansSenderPort)) {
        testPacketsSended++;
    }

    emit diagnInfo(packetsSended,testPacketsSended, packetsReceived, packetsInvalid);
//    qDebug()<<"Sended:"<<"Yaw="<<dataToSend.data.yaw<<"Pitch="<<dataToSend.data.pitch<<"Roll="<<dataToSend.data.roll;
//    qDebug()<<"Mx="<<dataToSend.data.Mx<<"My"<<dataToSend.data.My<<"Mz"<<dataToSend.data.Mz;

}

void TcmEthernet::setTCMDataToSend (TCM_Data data){
    memcpy(&dataToSend.data, &data, sizeof(data));
}

unsigned int TcmEthernet::checksum(const void *data, int size){
        uint c = 0;
        for (int i = 0; i < size; ++i)
            c += ((const uchar*)data)[i];
        return ~c;
}

void TcmEthernet::socketState(QAbstractSocket::SocketState state){
    QMap <int, QString> states;
    states[QAbstractSocket::UnconnectedState]="UnconnectedState";
    states[QAbstractSocket::HostLookupState]="HostLookupState";
    states[QAbstractSocket::ConnectingState]= "ConnectingState";
    states[QAbstractSocket::ConnectedState]= "ConnectedState";
    states[QAbstractSocket::BoundState] = "BoundState";
    states[QAbstractSocket::ClosingState] = "ClosingState";
    states[QAbstractSocket::ListeningState] = "ListeningState";
    emit socketInfo(receiverIP, receiverPort, states.value(state));
    qDebug()<<states.value(state);
}

void TcmEthernet::socketError(QAbstractSocket::SocketError socketError){
    emit errorInfo(receiverIP, receiverPort, sender->errorString());
    qDebug() << sender->errorString();
}
void TcmEthernet::sendInfo(){
    emit portOpened(receiverIP, receiverPort, opened, frequency);
    emit senderInfo(senderIP, senderPort);
    emit gansSenderInfo(gansSenderIP, gansSenderPort);
    emit errorInfo (receiverIP, receiverPort, sender->errorString());
    emit diagnInfo(packetsSended, testPacketsSended, packetsReceived, packetsInvalid);
}
