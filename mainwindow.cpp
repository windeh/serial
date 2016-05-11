#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

QT_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    fillPortPara();
    ui->sendButton->setText(tr("Open"));
    setEnableAll(true);
}

MainWindow::~MainWindow()
{
    if(serial != NULL){
        if(serial->isOpen())
            serial->close();
        delete serial;
    }
    delete ui;
}

void MainWindow::fillPortPara()
{
    ui->portNameBox->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->portNameBox->addItem(info.portName());
    }
 //   ui->portNameBox->addItem(QStringLiteral("Custom"));

    ui->baudRateBox->addItem(QStringLiteral("9600"),QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("115200"),QSerialPort::Baud115200);
    ui->baudRateBox->addItem(QStringLiteral("921600"),921600);
    ui->baudRateBox->addItem(tr("Custom"));\
    ui->baudRateBox->setCurrentIndex(1);

    ui->dataBitBox->addItem(QStringLiteral("7"),QSerialPort::Data7);
    ui->dataBitBox->addItem(QStringLiteral("8"),QSerialPort::Data8);
    ui->dataBitBox->setCurrentIndex(1);

    ui->parityBox->addItem(tr("None"),QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);

    ui->stopBitBox->addItem(QStringLiteral("1"),QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitBox->addItem(tr("1.5"),QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitBox->addItem(QStringLiteral("2"),QSerialPort::TwoStop);

    ui->flowcontrlBox->addItem(tr("None"),QSerialPort::NoFlowControl);
    ui->flowcontrlBox->addItem(tr("RTS/CTS"),QSerialPort::HardwareControl);
}

void MainWindow::setEnableAll(bool status)
{
    ui->portNameBox->setEnabled(status);
    ui->baudRateBox->setEnabled(status);
    ui->dataBitBox->setEnabled(status);
    ui->parityBox->setEnabled(status);
    ui->stopBitBox->setEnabled(status);
    ui->flowcontrlBox->setEnabled(status);

    ui->actionConnect->setEnabled(status);

    ui->actionDisconnect->setEnabled(!status);
    //   ui->sendButton->setEnabled(!status);
    ui->echoSendCheckBox->setEnabled(!status);
}

void MainWindow::on_actionConnect_triggered()
{
    openSerialPort();
}

void MainWindow::on_actionDisconnect_triggered()
{
    if(serial->isOpen())
        serial->close();
    ui->statusBar->showMessage(serial->portName()+tr(" Disconnected"));
    setEnableAll(true);
    ui->sendButton->setText(tr("Open"));

}
void MainWindow::openSerialPort()
{
    serial->setPortName(ui->portNameBox->currentText());
    if(ui->baudRateBox->currentIndex()>1){
        serial->setBaudRate(ui->baudRateBox->currentText().toInt());
    }
    else{
        QSerialPort::BaudRate baudrate = static_cast<QSerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
        serial->setBaudRate(baudrate);
    }
    serial->setDataBits(static_cast<QSerialPort::DataBits>(ui->dataBitBox->itemData(ui->dataBitBox->currentIndex()).toInt()));
    serial->setParity(static_cast<QSerialPort::Parity>(ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt()));
    serial->setStopBits(static_cast<QSerialPort::StopBits>(ui->stopBitBox->itemData(ui->stopBitBox->currentIndex()).toInt()));
    serial->setFlowControl(static_cast<QSerialPort::FlowControl>(ui->flowcontrlBox->itemData(ui->flowcontrlBox->currentIndex()).toInt()));

    if(serial->open(QIODevice::ReadWrite)){
        //open succeed
        connect(serial,&QSerialPort::readyRead,this,&MainWindow::readData);
        setEnableAll(false);
        ui->sendButton->setText(tr("Send"));
        ui->actionConnect->setEnabled(false);
        //   QMessageBox::information(this,tr("Open successfully"),tr("Connected"));
        ui->statusBar->showMessage(serial->portName()+tr(" Connected"));
    }
    else{
        QMessageBox::critical(this,tr("ERROR"),serial->errorString());


        ui->statusBar->showMessage(tr("Open Error"));
    }
}
void MainWindow::readData()
{
    //ui->statusBar->showMessage(tr("Recieved data from ")+serial->portName());
    QByteArray   rxData = serial->readAll();
    QString rxbuf;
    if(!rxData.isEmpty()){
        if(ui->rxAsciiChecked->isChecked())
            rxbuf = rxData;
        else if(ui->rxHexChecked->isChecked()){
//            for(int i = 0; i < rxData.size(); i++){
//                QString s;
//                s.sprintf("%02X ",(unsigned char)rxData.at(i));
//                rxbuf += s;
//            }
            foreach (char c, rxData) {
                QString s;
                s.sprintf("%02X ",c);
                rxbuf += s;
            }

        }
        ui->textBrowser->setText(ui->textBrowser->document()->toPlainText()+rxbuf);
        QTextCursor cursor = ui->textBrowser->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textBrowser->setTextCursor(cursor);
    }

}

void MainWindow::writeData()
{
    QByteArray txbuf;

    if(ui->txHexChecked->isChecked()){
        QString str;
        bool ok;
        char data;
        QStringList list;
        str = ui->dataToSendEdit->toPlainText();
        list = str.split(" ");
        foreach (const QString &l, list) {
            if(l == " " || l.isEmpty())
                continue;
            else {
                data = (char)l.toInt(&ok,16);
            }
            if(!ok){
                ui->statusBar->showMessage(tr("Data send ERROR"));
                QMessageBox::warning(this,tr("Warning"),tr("Data format error!"),QMessageBox::Cancel);
               return;
            }
            txbuf.append(data);
        }
    }
    else {
        txbuf = ui->dataToSendEdit->toPlainText().toLocal8Bit();
    }

    serial->write(txbuf);

    if(ui->echoSendCheckBox->isChecked()){
        ui->textBrowser->setText(ui->textBrowser->document()->toPlainText()+ui->dataToSendEdit->toPlainText());
        QTextCursor cursor = ui->textBrowser->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textBrowser->setTextCursor(cursor);
    }
    ui->statusBar->showMessage(tr("Data send complete"));

}

void MainWindow::on_sendButton_clicked()
{
    if(serial->isOpen())
        writeData();
    else
        openSerialPort();
}

void MainWindow::on_actionClear_triggered()
{
    ui->textBrowser->clear();
}

void MainWindow::on_baudRateBox_currentIndexChanged(int index)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(index).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if(isCustomBaudRate)
        ui->baudRateBox->clearEditText();
}

void MainWindow::on_actionSave_triggered()
{
    if(ui->textBrowser->toPlainText().isEmpty()){
        QMessageBox::information(this,tr("No data"),tr("No data to save!"),QMessageBox::Ok);
        return;
    }
    QString filename = QFileDialog::getSaveFileName(this,tr("Save as:"),tr("noname.txt"));
    QFile file(filename);
    if(!file.open(QFile::WriteOnly|QIODevice::Text)){
        QMessageBox::warning(this,tr("Save file"),
                             tr("Cannot open file %1 ,save failed\n%2").arg(filename).arg(file.errorString()),
                             QMessageBox::Ok);
        return;
    }
    QTextStream out(&file);
    out << ui->textBrowser->toPlainText();
    file.close();
    ui->statusBar->showMessage(tr("Saved: %1").arg(filename));
}


void MainWindow::on_actionQuit_triggered()
{
    MainWindow::close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,tr("About author"),tr("Released on github by <b>wind</b> "
                                                  "<a href=\"https://github.com/windeh/serial\">https://github.com/windeh/serial</a>"));
}
