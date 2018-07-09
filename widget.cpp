#include "widget.h"
#include "ui_widget.h"
#include <QList>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>


/*********************************************************************************************/
/*初始化*/
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //初始化标题
    setWindowTitle("串口调试助手");
    //初始化地图 需要使用绝对路径
    ui->webView->load(QUrl("file:///D:/serialPort/walkroute.html"));
    //初始化端口
    serialport = new QSerialPort(this);
    portFind();
    //初始化按键
    initButton();
    //初始化视频
    videoIsopen = false;
    mPorcess = new QProcess(this);
    videoUpDown = 150;
    videoLeftRight = 150;
}
//析构函数
Widget::~Widget()
{
    serialport->clear();
    serialport->close();
    delete ui;
}


/*********************************************************************************************/
/*初始化按键*/
void Widget::initButton()
{
    ui->send_button->setEnabled(false);
    ui->close_port->setEnabled(true);
    ui->clear_send_button->setEnabled(false);
    ui->clear_receive_button->setEnabled(false);
    ui->send_state->setEnabled(false);
    ui->receive_state->setEnabled(false);
    ui->fore_button->setEnabled(false);
    ui->back_button->setEnabled(false);
    ui->left_button->setEnabled(false);
    ui->right_button->setEnabled(false);
    ui->stop_button->setEnabled(false);
    ui->vup_button->setEnabled(false);
    ui->vdown_button->setEnabled(false);
    ui->vleft_button->setEnabled(false);
    ui->vright_button->setEnabled(false);
    ui->video_button->setEnabled(false);
}
/*开启按键*/
void Widget::openButton()
{
    ui->send_button->setEnabled(true);
    ui->close_port->setEnabled(true);
    ui->open_port->setEnabled(false);
    ui->clear_send_button->setEnabled(true);
    ui->clear_receive_button->setEnabled(true);
    ui->send_state->setEnabled(true);
    ui->receive_state->setEnabled(true);
    ui->com->setEnabled(false);
    ui->baud->setEnabled(false);
    ui->databit->setEnabled(false);
    ui->checkbit->setEnabled(false);
    ui->stopbit->setEnabled(false);
    ui->fore_button->setEnabled(true);
    ui->back_button->setEnabled(true);
    ui->left_button->setEnabled(true);
    ui->right_button->setEnabled(true);
    ui->stop_button->setEnabled(true);
    ui->vup_button->setEnabled(true);
    ui->vdown_button->setEnabled(true);
    ui->vleft_button->setEnabled(true);
    ui->vright_button->setEnabled(true);
    ui->video_button->setEnabled(true);
}
/*关闭按键*/
void Widget::closeButton()
{
    ui->send_button->setEnabled(false);
    ui->open_port->setEnabled(true);
    ui->close_port->setEnabled(true);
    ui->clear_send_button->setEnabled(false);
    ui->clear_receive_button->setEnabled(false);
    ui->send_state->setEnabled(false);
    ui->receive_state->setEnabled(false);
    ui->fore_button->setEnabled(false);
    ui->back_button->setEnabled(false);
    ui->left_button->setEnabled(false);
    ui->right_button->setEnabled(false);
    ui->com->setEnabled(true);
    ui->baud->setEnabled(true);
    ui->databit->setEnabled(true);
    ui->checkbit->setEnabled(true);
    ui->stopbit->setEnabled(true);
    ui->stop_button->setEnabled(false);
    ui->vup_button->setEnabled(false);
    ui->vdown_button->setEnabled(false);
    ui->vleft_button->setEnabled(false);
    ui->vright_button->setEnabled(false);
    ui->video_button->setEnabled(false);
    ui->com->clear();
}


/*********************************************************************************************/
/*自动查询串口*/
void Widget::portFind()
{
    //检查可用的端口，添加到下拉框以供选择
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial_num;
        serial_num.setPort(info);
        if(serial_num.open(QIODevice::ReadWrite))
        {
            ui->com->addItem(serial_num.portName());
            serial_num.close();
        }
    }
}


/*********************************************************************************************/
/*打开串口*/
void Widget::on_open_port_clicked()
{

    //初始化界面并初始化端口
    update();
    //初始化串口的各项参数
    serialport->setPortName(ui->com->currentText());
    //如果成功打开串口则允许设置参数，否则报错提示
    if(serialport->open(QIODevice::ReadWrite))
    {
        //设置波特率
        serialport->setBaudRate(ui->baud->currentText().toInt());
        //设置数据位
        switch (ui->databit->currentIndex()) {
        case 5:
            serialport->setDataBits(QSerialPort::Data5);
            break;
        case 6:
            serialport->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            serialport->setDataBits(QSerialPort::Data7);
            break;
        case 8:
            serialport->setDataBits(QSerialPort::Data8);
            break;
        default:
            break;
        }
        //设置奇偶校验位
        switch (ui->checkbit->currentIndex()) {
        case 0:
            serialport->setParity(QSerialPort::NoParity);
            break;
        default:
            break;
        }
        //设置停止位
        switch (ui->stopbit->currentIndex()) {
        case 1:
            serialport->setStopBits(QSerialPort::OneStop);
            break;
        case 2:
            serialport->setStopBits(QSerialPort::TwoStop);
            break;
        default:
            break;
        }
        //设置流控制
        serialport->setFlowControl(QSerialPort::NoFlowControl);
        //串口设置完成，监听数据
        connect(serialport, &QSerialPort::readyRead, this, &Widget::read_data);
        //重置按键状态
        openButton();
    }
    else
    {QMessageBox::information(this, tr("Warning"), tr("Cannot open this serialport!"), QMessageBox::Ok);}
}
/*关闭串口*/
void Widget::on_close_port_clicked()
{
    //关闭串口，重置按键，重查串口，清空文本框
    serialport->clear();
    serialport->close();
    closeButton();
    portFind();
    ui->Send_text_win->clear();
    ui->Receive_text_win->clear();
}


/*********************************************************************************************/
/*接收数据*/
void Widget::read_data()
{
    //qDebug()<<"read";
    //字符串或者十六进制接收
    QByteArray buffer;
    buffer = serialport->readAll();   
    if(!buffer.isEmpty())
    {
        QString str = ui->Receive_text_win->toPlainText();
        if(ui->receive_state->currentText() == "string")
        {
            str = tr(buffer);
            ui->Receive_text_win->append(str);
        }
        else
        {
           QByteArray temp = buffer.toHex().toUpper();
           str = tr(temp);
           ui->Receive_text_win->append(str);
        }
    }
    buffer.clear();
}
/*发送数据*/
void Widget::on_send_button_clicked()
{
    //qDebug()<<"send";
    //字符串或者十六进制发送
    QString str = ui->Send_text_win->toPlainText();
    if(!str.isEmpty())
    {
        if(ui->send_state->currentText() == "string")
        {
            serialport->write(str.toLatin1());
        }
        else
        {
            int num = str.toInt();
            str = str.setNum(num, 16);
            serialport->write(str.toLatin1());
        }
     }
}


/*********************************************************************************************/
/*清空接收窗口*/
void Widget::on_clear_receive_button_clicked()
{
    ui->Receive_text_win->clear();
}
/*清空发送窗口*/
void Widget::on_clear_send_button_clicked()
{
    ui->Send_text_win->clear();
}


/*********************************************************************************************/
/*小车微调控制*/
//需要先打开串口才能使用
//停止按钮
void Widget::on_stop_button_clicked()
{
    QString str = "k04";
    serialport->write(str.toLatin1());
}
//前进按钮
void Widget::on_fore_button_clicked()
{
    QString str = "k00";
    serialport->write(str.toLatin1());
    carStop();
}
//后退按钮
void Widget::on_back_button_clicked()
{
    QString str = "k01";
    serialport->write(str.toLatin1());
    carStop();
}
//右转按钮
void Widget::on_right_button_clicked()
{
    QString str = "k02";
    serialport->write(str.toLatin1());
    carStop();
}
//左转按钮
void Widget::on_left_button_clicked()
{
    QString str = "k03";
    serialport->write(str.toLatin1());
    carStop();
}
//定时停止
void Widget::carStop()
{
    mysleep(600);
    QString str = "k04";
    serialport->write(str.toLatin1());
}


/*********************************************************************************************/
/*云台微调控制*/
//需要先打开串口才能使用
//上抬按钮
void Widget::on_vup_button_clicked()
{
    videoUpDown += 10;
    if(videoUpDown >= 60 && videoUpDown <= 240)
    {
        QString str = "u0" + QString("%1").arg(videoUpDown, 3, 10, QLatin1Char('0'));
        serialport->write(str.toLatin1());
    }
    else
    {
        videoUpDown = 240;
        QMessageBox::information(this, tr("Warning"), tr("Out of video control range!"), QMessageBox::Ok);
    }
}
//下降按钮
void Widget::on_vdown_button_clicked()
{
    videoUpDown -= 10;
    if(videoUpDown >= 60 && videoUpDown <= 240)
    {
        QString str = "u0" + QString("%1").arg(videoUpDown, 3, 10, QLatin1Char('0'));
        serialport->write(str.toLatin1());
    }
    else
    {
        videoUpDown = 60;
        QMessageBox::information(this, tr("Warning"), tr("Out of video control range!"), QMessageBox::Ok);
    }
}
//左偏按钮
void Widget::on_vleft_button_clicked()
{
    videoLeftRight += 10;
    if(videoLeftRight >= 60 && videoLeftRight <= 240)
    {
        QString str = "r0" + QString("%1").arg(videoLeftRight, 3, 10, QLatin1Char('0'));
        serialport->write(str.toLatin1());
    }
    else
    {
        videoLeftRight = 240;
        QMessageBox::information(this, tr("Warning"), tr("Out of video control range!"), QMessageBox::Ok);
    }
}
//右偏按钮
void Widget::on_vright_button_clicked()
{
    videoLeftRight -= 10;
    if(videoLeftRight >= 60 && videoLeftRight <= 240)
    {
        QString str = "r0" + QString("%1").arg(videoLeftRight, 3, 10, QLatin1Char('0'));
        serialport->write(str.toLatin1());
    }
    else
    {
        videoLeftRight = 60;
        QMessageBox::information(this, tr("Warning"), tr("Out of video control range!"), QMessageBox::Ok);
    }
}


/*********************************************************************************************/
/*视频画面开关控制*/
//需要先打开串口才能使用
void Widget::on_video_button_clicked()
{
    if(videoIsopen == false)
    {
        mPorcess->start("notepad");
        ui->video_button->setText("关闭视频");
        videoIsopen = true;
    }
    else
    {
        mPorcess->close();
        ui->video_button->setText("打开视频");
        videoIsopen = false;
    }
}


/*********************************************************************************************/
/*延时函数*/
void Widget::mysleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
