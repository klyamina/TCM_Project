#include "serialporttransmitter.h"
#include <QTimer>
#include <QBitArray>

namespace TCM {

serialPortTransmitter::serialPortTransmitter(QObject *parent) : QObject(parent) {

    dataToSend = new TCM_Data();
    //memset(dataToSend,0,sizeof(dataToSend));
    commands.append("h\r");
    commands.append("ec=e\r");
    commands.append("ep=e\r");
    commands.append("er=e\r");
    commands.append("em=e\r");
    commands.append("go\r");

    QVector <QVariant> dataSeparators, dataValues(9);
    dataSeparators<<'C'<<'P'<<'R'<<'X'<<'Y'<<'Z'<<'E'<<'*'<<'/';
    recVec<<dataSeparators<<dataValues;

    packetsInvalid=packetsReceived=packetsSended=0;


    timeout=200;
    int timeoutDiagn=500;
    opened=false;
    state=Standby;

    timeDiagn.start(timeoutDiagn);
    time.start(timeout);

    connect(&com, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(&timeDiagn, SIGNAL(timeout()),SLOT(sendPortInfo()));
    //connect(&time,SIGNAL(timeout()),SLOT(testSend()));
}

serialPortTransmitter::serialPortTransmitter(const QString & config , const QString & name , QObject *parent) : QObject(parent){


    dataToSend = new TCM_Data();

    commands.append("h\r");
    commands.append("ec=e\r");
    commands.append("ep=e\r");
    commands.append("er=e\r");
    commands.append("em=e\r");
    commands.append("go\r");

    QVector <QVariant> dataSeparators, dataValues(9);
    dataSeparators<<'C'<<'P'<<'R'<<'X'<<'Y'<<'Z'<<'E'<<'*'<<'/';
    recVec<<dataSeparators<<dataValues;

    packetsInvalid=packetsReceived=packetsSended=0;
    opened=false;
    state=Standby;
    int timeoutDiagn=500;


    QPIConfig conf(config, QIODevice::ReadOnly);
    QPIConfig::Entry & e(conf.getValue(name));

    QString com_name = e.getValue("com_name").value();
    int baudRate = e.getValue("baudRate").value().toInt();
    int freq = e.getValue("frequency").value().toInt();
    if (freq)timeout=1000/freq;
    else timeout=100;

    timeDiagn.start(timeoutDiagn);
    time.start(timeout);
    //time.singleShot(timeout,this,SLOT(sendPortInfo()));
    time.start(timeout);
    qDebug()<<"com_name"<<com_name;
    qDebug()<<"baudRate"<<baudRate;
    openPort(com_name, baudRate);

    connect(&timeDiagn, SIGNAL(timeout()),SLOT(sendPortInfo()));
    connect(&com, SIGNAL(readyRead()), this, SLOT(readData()));
    //connect(&time,SIGNAL(timeout()),SLOT(testSend()));

}

void serialPortTransmitter::testSend(){
    dataToSend->yaw=90;
    dataToSend->pitch=89;
    dataToSend->roll=45;
    dataToSend->Mx=15;
    dataToSend->My=23;
    dataToSend->Mz=45;
    dataToSend->errorInclOutOfRange=1;
    dataToSend->errorMagnDistortion=0;
    dataToSend->errorMagnOutOfRange=1;
    emit info_send_for_show(*dataToSend);
}

serialPortTransmitter::~serialPortTransmitter(){

}

void serialPortTransmitter::sendActionCommand(QByteArray commandToSend, int time_ms) {
    qDebug()<<commandToSend;
    int bytesWritten=com.write(commandToSend);
    if (bytesWritten) packetsSended++;
   QTimer::singleShot(time_ms, this, SLOT(checkReceivedData()));
}

void serialPortTransmitter::checkReceivedData(){
    if (((int)state<ContiniousMode) && dataTCM.contains(":")) {
        qDebug()<<"Received";
        dataTCM.clear();
        state++;
        sendActionCommand(commands[state],500);
    }
    else if ((int)state==ContiniousMode) {
        qDebug()<<"Continious mode started";
        connect (&time, SIGNAL(timeout()),this,SLOT(parseData()));
    }
    else sendActionCommand(commands[state],500);
}


void serialPortTransmitter::openPort(QString name, qint32 baudRate){
    com.setPortName(name);
    com.setBaudRate(baudRate);
    if (com.open(QIODevice::ReadWrite)) {
        sendActionCommand(commands[state], timeout);
        opened=true;
    }
    else {
        opened=false;
    }
    emit portOpened(opened);
    emit portInfo(com.portName(), com.baudRate(), opened);
}

void serialPortTransmitter::closePort(QString name){
    state=Standby;
    if(com.portName() ==name){
        disconnect (&time, SIGNAL(timeout()),this,SLOT(parseData()));
     //   disconnect(&com, SIGNAL(readyRead()), this, SLOT(readData()));
        com.close();
    }
}

void serialPortTransmitter::readData(){
    dataTCM.append(com.readAll());
}

void serialPortTransmitter::parseData(){
    QByteArray temp;
    int begin, end;

    begin=dataTCM.indexOf('$');
    end=dataTCM.indexOf('\n',begin);
    while (begin!=-1 && end!=-1){
//        qDebug() << "Изначальный массив"<< dataTCM;
        end=dataTCM.indexOf('\n',begin);
        if ((dataTCM.indexOf('\r',begin)==(dataTCM.indexOf('*',begin)+3))
                && ((dataTCM.indexOf("\n",begin)==(dataTCM.indexOf('*',begin)+4)))) {
            for (int i=begin; i<=end; i++){
                  temp.append(dataTCM.at(i));
                  if (i==end) dataTCM.remove(0,end+1);
               }

            if(checkSum(temp)){
                packetsReceived++;
                temp.chop(4);
                parsePacket(recVec, temp);
                temp.clear();
            }
            else packetsInvalid++;
        }
        begin=dataTCM.indexOf('$');
    }
    emit info_send_for_show(*dataToSend);
    qDebug()<<"Принято пакетов:"<<packetsReceived;
    qDebug()<<"Принято инвалидов"<<packetsInvalid;

}

bool serialPortTransmitter::checkSum(QByteArray packet){
    char sum=0;
    QByteArray chSumRec, chSumTemp;
    for(int i=packet.indexOf('$')+1; i<packet.indexOf('*'); ++i){
       sum= sum ^ packet.at(i);
    }
    chSumRec.append(packet.at(packet.indexOf('*')+1));
    chSumRec.append(packet.at(packet.indexOf('*')+2));
    chSumTemp.append(sum);
    chSumTemp=chSumTemp.toHex().toUpper();
    if (chSumTemp==chSumRec) return true;
    else return false;
}

void serialPortTransmitter::parsePacket( QVector <QVector < QVariant > > packetTemplate, QByteArray packet){
    QByteArray temp;
//    qDebug()<<"размер вектора:"<<packetTemplate[0].size();
    for(int i=0;i<packetTemplate[0].size()-1;i++){
        QChar currentKey=packetTemplate[0].at(i).toChar();
        int currentID=packet.indexOf(currentKey);
        QChar nextKey=packetTemplate[0].at(i+1).toChar();
        int nextID=packet.indexOf(nextKey);
        if(nextKey=='E'&& nextID<0){
            nextKey=packetTemplate[0].at(i+2).toChar();
            nextID=packet.indexOf(nextKey);
        }
//        qDebug()<<"массив для обработки"<<packet;
//        qDebug()<<"текущий ключ:"<<currentKey<<"следующий ключ:"<<nextKey;
//        qDebug()<<"текущий ID"<<currentID<<"следующий ID"<<nextID;
        if (currentID<0 || currentID > packet.size()) break;

        if(packet.at(currentID)=='E'){
            //packet.at(currentID+3);
            if (packet.at(currentID+3) & 0b00000001) dataToSend->errorMagnDistortion=1;
            else dataToSend->errorMagnDistortion=0;
            if (packet.at(currentID+3) & 0b00000010) dataToSend->errorInclOutOfRange=1;
            else dataToSend->errorInclOutOfRange=0;
            if (packet.at(currentID+3) & 0b00000100) dataToSend->errorMagnOutOfRange=1;
            else dataToSend->errorMagnOutOfRange=0;
        }
        else if (currentID>=0 && nextID>=0 && currentID<nextID) {
            for(int j=currentID+1; j<nextID ;j++){
                temp.append(packet.at(j));
            }
            packetTemplate[1][i] = temp.toDouble();
            qDebug()<<"Принятое значение параметра"<<packetTemplate[0][i].toChar()<<"="<<temp.toDouble();
            temp.clear();
            if(packetTemplate[0][i].toChar()=='C') dataToSend->yaw=packetTemplate[1][i].toDouble();
            else if (packetTemplate[0][i].toChar()=='P') dataToSend->pitch=packetTemplate[1][i].toDouble();
            else if (packetTemplate[0][i].toChar()=='R') dataToSend->roll=packetTemplate[1][i].toDouble();
            else if (packetTemplate[0][i].toChar()=='X') dataToSend->Mx=packetTemplate[1][i].toDouble();
            else if (packetTemplate[0][i].toChar()=='Y') dataToSend->My=packetTemplate[1][i].toDouble();
            else if (packetTemplate[0][i].toChar()=='Z') dataToSend->Mz=packetTemplate[1][i].toDouble();
        }
   }
        dataToSend->packetsReceived=packetsReceived;
        dataToSend->packetsInvalid=packetsInvalid;
        dataToSend->packetsSended=packetsSended;

        qDebug()<<"Выходные данные:";
        qDebug()<<"Курс"<<dataToSend->yaw;
        qDebug()<<"Крен"<<dataToSend->roll;
        qDebug()<<"Дифферент"<<dataToSend->pitch;
        qDebug()<<"Mx="<<dataToSend->Mx <<"My="<<dataToSend->My<<"Mz="<<dataToSend->Mz;
        qDebug()<<"Наличие магнитного возмущения"<<dataToSend->errorMagnDistortion;
        qDebug()<<"Магнитометры в насыщении:"<<dataToSend->errorMagnOutOfRange;
        qDebug()<<"Инклинометры в насыщении:"<<dataToSend->errorInclOutOfRange;

}

void serialPortTransmitter::getTCMData(TCM_Data &data){
    memcpy(&data,dataToSend,sizeof(data));
}

void serialPortTransmitter::sendPortInfo(){
    emit portInfo(com.portName(), com.baudRate(), opened);
    emit portOpened(opened);
   // emit info_send_for_show(*dataToSend);
}


/*
TCM_Data serialPortTransmitter::getTCMDataValues(){
    return dataToSend;

}
*/
} //namspace TCM
