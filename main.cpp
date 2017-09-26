#include <QCoreApplication>
#include <QTimer>
#include "consolecontrol.h"
#include "protocol_TCM2.h"
#include "serialporttransmitter.h"
#include "tcm_ethernet.h"
#include "qpiconfig.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int frequency = 30;
    ConsoleControl console_control;

    TCM::serialPortTransmitter com_tcm("protocols.conf", "tcm");
    TcmEthernet tcm_eth("protocols.conf", "tcm");

    QTimer time_write;//таймер, по которому обновляется консоль
    time_write.start(1000/frequency);


    QObject::connect(&com_tcm, SIGNAL(portInfo(QString,int,bool)),
                     &console_control, SLOT(info_COM_collect(QString,int,bool)));
    QObject::connect(&tcm_eth, SIGNAL(info_Eth_console(QString,QString,QString,int,int,int,int)),
                     &console_control, SLOT(info_Eth_collect(QString,QString,QString,int,int,int,int)));
    QObject::connect(&tcm_eth, SIGNAL(portOpened(QHostAddress,int,bool,int)),
                     &console_control, SLOT(portOpened(QHostAddress,int,bool,int)));
    QObject::connect(&tcm_eth, SIGNAL(senderInfo(QHostAddress,int)),
                     &console_control, SLOT(senderInfo(QHostAddress,int)));
    QObject::connect(&tcm_eth, SIGNAL(gansSenderInfo(QHostAddress,int)),
                     &console_control, SLOT(gansSenderInfo(QHostAddress,int)));
    QObject::connect(&tcm_eth, SIGNAL(errorInfo(QHostAddress,int,QString)),
                     &console_control, SLOT(errorInfo(QHostAddress,int,QString)));
    QObject::connect (&tcm_eth, SIGNAL(diagnInfo(int,int,int,int)),
                     &console_control, SLOT(diagnInfo(int,int,int,int)));

    QObject::connect(&time_write, SIGNAL(timeout()), &console_control, SLOT(infoCollect()));

    QObject::connect(&com_tcm, SIGNAL(info_send_for_show(TCM_Data)),
                     &console_control, SLOT(info_data_collect(TCM_Data)));
    QObject::connect(&com_tcm, SIGNAL(info_send_for_show(TCM_Data)),
                     &tcm_eth, SLOT(setTCMDataToSend(TCM_Data)));


    return a.exec();
}

