#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QtWidgets>
#include <QString>
#include <QLabel>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <unistd.h>
#include <iostream>
#include <string>
#include <iostream>

//test


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      serial(new QSerialPort(this))
{
    ui->setupUi(this);
    arduino_is_available = false;
    arduino_port_name = "";
    arduino = new QSerialPort;




    qDebug() << "Numero de portos disponiveis : " << QSerialPortInfo::availablePorts().length();

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        qDebug() << "Has vendor ID:" << serialPortInfo.hasVendorIdentifier();
        if(serialPortInfo.hasVendorIdentifier())
        {
            qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
        }

        qDebug() << "Has Product ID:" << serialPortInfo.hasProductIdentifier();
        if(serialPortInfo.hasProductIdentifier())
        {
            qDebug() << "Product  ID: " << serialPortInfo.productIdentifier();
        }


    }

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            qDebug() << "111111111" ;
            if(serialPortInfo.vendorIdentifier() == arduino_micro_vendor_id){
                qDebug() << "22222222222" ;
                if(serialPortInfo.productIdentifier() == arduino_micro_product_id){
                    qDebug() << "333333333333" ;
                    arduino_port_name = serialPortInfo.portName();
                    arduino_is_available = true;
                }
            }
        }

        if(arduino_is_available){
            qDebug() << "444444444444" ;
            arduino->setPortName(arduino_port_name);
            arduino->open(QIODevice::ReadWrite);
            arduino->setBaudRate(QSerialPort::Baud115200);
            arduino->setDataBits(QSerialPort::Data8);
            arduino->setParity(QSerialPort::NoParity);
            arduino->setStopBits(QSerialPort::OneStop);
            arduino->setFlowControl(QSerialPort::NoFlowControl);
            arduino->flush();
            arduino->clear(QSerialPort::AllDirections);
            connect(arduino, SIGNAL(readyRead()),this,SLOT(readData()));

        }else{

            QMessageBox::warning(this, "Port error", "Couldn't find the Arduino");
        }



    }


}

MainWindow::~MainWindow()
{


    if(arduino->isOpen()){

        arduino->close();
    }
    delete ui;
}
void MainWindow::calibracao(const QByteArray &data_hexa)
{

    bool ok;
    uint leitura_adc=0;
    QByteArray b1,b2,b3,b4;

    b1.append(data_hexa[8]);
    b1.append(data_hexa[9]);
    //qDebug()  << b1;
    int hex1 = b1.toInt(&ok, 16);
    //qDebug() << "em int:" << hex1 ;

    b2.append(data_hexa[10]);
    b2.append(data_hexa[11]);
    //qDebug()  << b2;
    int hex2 = b2.toInt(&ok, 16);
    //qDebug() << "em int:" << hex2 ;

    b3.append(data_hexa[12]);
    b3.append(data_hexa[13]);
    //qDebug()  << b3;
    int hex3 = b3.toInt(&ok, 16);
    //qDebug() << "em int:" << hex3 ;

    b4.append(data_hexa[14]);
    b4.append(data_hexa[15]);
    //qDebug()  << b4;
    int hex4 = b4.toInt(&ok, 16);
    //qDebug() << "em int:" << hex4 ;

    leitura_adc = hex1*256*256*256 + hex2*256*256 + hex3*256 + hex4;
    qDebug() << leitura_adc;
    if(flag_calib==1){
        ganho = float(410) / float(leitura_adc);
        flag_calib_enc = 1;
    }
    peso_g = leitura_adc*ganho;
    qDebug() << peso_g;

    //qDebug() << "CALIBRAÇÃO FEITA!";


}


void MainWindow::writeData(const QString &data)
{
   //QByteArray var1 = data.toUtf8();
    // qDebug()<<"READ DATA"<<var1;

    if(arduino->isWritable()&&arduino->isOpen())
    {
        QByteArray test2 = data.toLocal8Bit().constData();

        QByteArray var2 = QByteArray::fromHex(test2);
        arduino->write(var2);
        arduino->waitForBytesWritten();
        qDebug()<<"READ DATA(HEX)"<<var2;
    }

}
void MainWindow::readData()
{
    if(arduino->isReadable()&&arduino->isOpen())
    {

         data2 = arduino->readAll();

         qDebug() << "Response :" << data2;
         QByteArray text1 = data2.toHex();

         if(flag_calib_enc==1)
         {
             calibracao(text1);

         }
         if(flag_calib==1)
         {
            calibracao(text1);
            flag_calib_enc = 1;
         }

         data2.clear();
         arduino->flush();
         arduino->clear(QSerialPort::AllDirections);


    }

}
void MainWindow::on_pushButton_clicked()
{
    QString data = ui->textEdit->toPlainText();
    if(QString::compare(data, "02 00 01 04 f9", Qt::CaseInsensitive)==0)
    {
        qDebug() << "Calibração .... "  ;
        qDebug() << "Tire todos os objetos da balança "  ;
        qDebug() << "3.......";
        sleep(1);
        qDebug() << "2.......";
        sleep(1);
        qDebug() << "1.......";
        sleep(1);
        qDebug() << "Procedemos";
        flag_tara=1;
        writeData(data);
        qDebug() << "Tara calculada!";

    }
    if((QString::compare(data, "02 00 01 03 fa", Qt::CaseInsensitive)==0) && (flag_tara==1))
    {
        flag_calib = 1;
        qDebug() << "Coloque o peso conhecido o peso conhecido(500g)";
        qDebug() << "Tem 3 segundos!";
        qDebug() << "3.......";
        sleep(1);
        qDebug() << "2.......";
        sleep(1);
        qDebug() << "1.......";
        sleep(1);
        flag_tara=0;
        writeData(data);

    }

    if((QString::compare(data, "START", Qt::CaseInsensitive)==0) && (flag_calib_enc == 1) )
    {
        flag_calib = 0;
        qDebug() << "START SENDING";
        qDebug() << ganho ;
        while(!(QString::compare(data, "STOP", Qt::CaseInsensitive)==0)){
            usleep(250000);
            writeData("02 00 01 03 fa");

        }



    }






}



