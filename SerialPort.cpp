#include "SerialPort.h"

SerialPort::SerialPort():connected(false)
{
    serial = new QSerialPort();
    initSerialPort();
}

SerialPort::SerialPort(QSerialPort *ser):connected(false)
{
    this->serial = ser;
    getSerialParameter();
    initSerialPort();
}

SerialPort::SerialPort(std::string portname,int baud):connected(false)
{
    serial = new QSerialPort();
    param.portname = QString::fromStdString(portname);
    param.baudrate = baud;
    initSerialPort();
}

SerialPort::~SerialPort()
{
    if(serial != NULL)
        delete serial;
}

bool SerialPort::open()
{
    if(serial->open(QIODevice::ReadWrite))
    {
        connected = true;
    }
    else
    {
        connected = false;
        std::cout<<"Open "<<std::string(param.portname.toStdString())<<" Error!"<<std::endl;
    }

    return connected;
}

bool SerialPort::open(std::string portname,int baud)
{
    param.portname = QString::fromStdString(portname);
    param.baudrate = baud;
    initSerialPort();
    return this->open();
}

bool SerialPort::close()
{
    if(serial->isOpen())
        serial->close();
    connected = false;
    return true;
}

bool SerialPort::isConnected()
{
    return connected;
}
QSerialPort * SerialPort::getSerial()
{
    return serial;
}

int SerialPort::read(char *data,int maxSize)
{
    int n = 0;
    if(connected)
        n = serial->read(data,maxSize);
    else
        n = -1;
    return n;
}

const char * SerialPort::readAll()
{
    if(connected)
    {
        rxBuffer = serial->readAll();
        return rxBuffer.constData();
    }
    else
        return NULL;
}

int SerialPort::write(const char *data,int maxSize)
{
    int n = 0;
    if(connected)
        n = serial->write(data,maxSize);
    else
        n = -1;
    return n;
}

bool SerialPort::setSerialParameter(SerialParam &p)
{
    param.baudrate = p.baudrate;
    param.portname = p.portname;
    param.databits = p.databits;
    param.parity = p.parity;
    param.stopbits = p.stopbits;
    param.flowcontrol = p.flowcontrol;
    initSerialPort();
    return true;
}
bool SerialPort::getSerialParameter(SerialParam *p)
{
    param.baudrate = serial->baudRate();
    param.portname = serial->portName();
    param.databits = serial->dataBits();
    param.parity = serial->parity();
    param.stopbits = serial->stopBits();
    param.flowcontrol = serial->flowControl();

    if(p != NULL)
    {
        p->baudrate = param.baudrate;
        p->portname = param.portname;
        p->databits = param.databits;
        p->parity = param.parity;
        p->stopbits = param.stopbits;
        p->flowcontrol = param.flowcontrol;
    }

    return true;
}


bool SerialPort::initSerialPort()
{
    serial->setPortName(param.portname);
    serial->setBaudRate(param.baudrate);
    serial->setDataBits(param.databits);
    serial->setParity(param.parity);
    serial->setStopBits(param.stopbits);
    serial->setFlowControl(param.flowcontrol);

    return true;
}
