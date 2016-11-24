#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QtSerialPort>
#include <QString>
#include <string>
#include <iostream>

class SerialPort
{
public:
    enum Baudrate
    {
        BAUDRATE_4800 = 4800,
        BAUDRATE_9600 = 9600,
        BAUDRATE_19200 = 19200,
        BAUDRATE_38400 = 38400,
        BAUDRATE_57600 = 57600,
        BAUDRATE_115200 = 115200,
        BAUDRATE_230400 = 230400,
        BAUDRATE_460800 = 460800,
        BAUDRATE_921600 = 921600
    };
    struct SerialParam
    {
        int baudrate;
        QString portname;
        QSerialPort::DataBits databits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopbits;
        QSerialPort::FlowControl flowcontrol;

        SerialParam()
        {
            baudrate = BAUDRATE_115200;
            portname = QString("\0");
            databits = QSerialPort::Data8;
            parity = QSerialPort::NoParity;
            stopbits = QSerialPort::OneStop;
            flowcontrol = QSerialPort::NoFlowControl;
        }
    };

public:
    SerialPort();
    SerialPort(QSerialPort *ser);
    SerialPort(std::string portname,int baud = BAUDRATE_115200);
    ~SerialPort();


    bool open();
    bool open(std::string portname,int baud = BAUDRATE_115200);
    bool close();

    bool isConnected();
    QSerialPort * getSerial();

    int read(char *data,int maxSize);
    const char * readAll();
    int write(const char *data,int maxSize);
    bool setSerialParameter(SerialParam &p);
    bool getSerialParameter(SerialParam *p = NULL);

private:
    bool initSerialPort();

private:
    QSerialPort *serial;
    SerialParam param;
    bool connected;
    QByteArray rxBuffer;
    QByteArray txBuffer;

};

#endif // SERIALPORT_H
