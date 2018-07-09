#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTime>
#include <QtWebKitWidgets>
#include <QProcess>
#include <QTcpSocket>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void initButton();
    void openButton();
    void closeButton();
    void portFind();
    void read_data();
    void mysleep(int);
    void carStop();

    void on_open_port_clicked();
    void on_close_port_clicked();
    void on_send_button_clicked();

    void on_clear_receive_button_clicked();
    void on_clear_send_button_clicked();

    void on_fore_button_clicked();
    void on_back_button_clicked();
    void on_left_button_clicked();
    void on_right_button_clicked();
    void on_stop_button_clicked();

    void on_video_button_clicked();
    void on_vup_button_clicked();
    void on_vdown_button_clicked();
    void on_vleft_button_clicked();
    void on_vright_button_clicked();


private:
    Ui::Widget *ui;
    QSerialPort *serialport;
    QProcess *mPorcess;
    bool videoIsopen;
    int videoUpDown;
    int videoLeftRight;
};

#endif // WIDGET_H
