#include "mp285.h"
#include <QDebug>

MP285::MP285(QObject *parent)
    : QThread{parent}
{
    is_open = false;
    stopped = false;

    serial_mp285 = new QSerialPort();

    get_position_run_once = false;
    goto_position_run_once = false;
    set_velocity_run_once = false;
    update_panel_run_once = false;
    set_origin_run_once = false;
    send_reset_run_once = false;
    get_status_run_once = false;
}

QStringList MP285::get_port_list()
{
    QStringList m_serialPortName;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_serialPortName << info.portName();
        qDebug()<<"get serialPortName:"<<info.portName();
    }
    return m_serialPortName;
}

void MP285::serial_open(QString port, int baudrate)
{
    if (is_open==false)
    {

        if(serial_mp285->isOpen())
        {
            serial_mp285->clear();
            serial_mp285->close();
        }

        serial_mp285->setPortName(port);

        if(!serial_mp285->open(QIODevice::ReadWrite))
        {
            qDebug()<<port<<"open failed!";
            return;
        }
        else
        {
            qDebug()<<port<<"open success!";
        }

        //打开成功
        serial_mp285->setBaudRate(baudrate,QSerialPort::AllDirections);//设置波特率和读写方向
        serial_mp285->setDataBits(QSerialPort::Data8);              //数据位为8位
        serial_mp285->setFlowControl(QSerialPort::NoFlowControl);   //无流控制
        serial_mp285->setParity(QSerialPort::NoParity);             //无校验位
        serial_mp285->setStopBits(QSerialPort::OneStop);            //一位停止位

        //connect(serial_mp285,SIGNAL(readyRead()),this,SLOT(serial_received()));
        is_open=true;
    }
}

void MP285::serial_close()
{
    serial_mp285->close();
    is_open=false;
}

void MP285::run()
{
    qDebug()<<"run():";
    while (!stopped)
    {
        // 串行顺序执行，避免mp285飞车
        if(is_open == true)
        {
            if(get_position_run_once == true)
            {
                get_position_run_once = false;
                get_position();
            }
            if(goto_position_run_once == true)
            {
                goto_position_run_once = false;
                goto_position(desired_x,desired_y,desired_z);
            }
            if(set_velocity_run_once == true)
            {
                set_velocity_run_once = false;
                set_velocity(desired_vel,desired_v_scalf);
            }
            if(update_panel_run_once == true)
            {
                update_panel_run_once = false;
                update_panel();
            }
            if(set_origin_run_once == true)
            {
                set_origin_run_once = false;
                set_origin();
            }
            if(send_reset_run_once == true)
            {
                send_reset_run_once = false;
                send_reset();
            }
            if(get_status_run_once == true)
            {
                get_status_run_once = false;
                get_status();
            }
        }
        QThread::msleep(1);
    }
}


void MP285::serial_write(QByteArray data)
{
    serial_mp285->write(data);
}


//void MP285::serial_received()
//{
//    qDebug()<<"serial received():";
//    QByteArray info = serial_mp285->readAll();
//    qDebug()<<info;
//}

void MP285::get_position()
{

    QString str("c\r");
    QByteArray bytes = str.toUtf8(); // QString转QByteArray方法1
    serial_mp285->write(bytes);

    int position[3];
    QByteArray buffer = serial_mp285->read(13);
    memcpy(position, buffer.data(), 12);

    current_x = position[0];
    current_y = position[1];
    current_z = position[2];
}

void MP285::goto_position(int x, int y, int z)
{
    char send_list[14];
    send_list[0]  = 'm';

    x = x * 25;
    y = y * 25;
    z = z * 25;

    memcpy(&send_list[1], &x, 4);
    memcpy(&send_list[5], &y, 4);
    memcpy(&send_list[9], &z, 4);

    send_list[13] = '\r';

    serial_mp285->write(send_list, 14);

    QByteArray buffer = serial_mp285->read(1);

    if (!buffer.isEmpty())
    {
        qDebug() << "Sutter did not finish moving before timeout";
    }
    else
    {
        qDebug() << "sutterMP285: Sutter move completed";
    }
}

void MP285::set_velocity(int16_t vel, int v_scalf)
{

    if (v_scalf==50)
        vel = vel+32768;

    char send_list[4];
    send_list[0]  = 'V';
    memcpy(&send_list[1], &vel, 2);
    send_list[3] = '\r';

    serial_mp285->write(send_list,4);

    QByteArray buffer = serial_mp285->read(1);

}

void MP285::update_panel()
{


    QString str("n\r");
    QByteArray bytes = str.toUtf8(); // QString转QByteArray方法1
    serial_mp285->write(bytes);

    QByteArray buffer = serial_mp285->read(1);
}

void MP285::set_origin()
{

    QString str("o\r");
    QByteArray bytes = str.toUtf8(); // QString转QByteArray方法1
    serial_mp285->write(bytes);

    QByteArray buffer = serial_mp285->read(1);
}


void MP285::send_reset()
{

    QString str("r\r");
    QByteArray bytes = str.toUtf8(); // QString转QByteArray方法1
    serial_mp285->write(bytes);

    QByteArray buffer = serial_mp285->read(1);
}

void MP285::get_status()
{

    QString str("s\r");
    QByteArray bytes = str.toUtf8(); // QString转QByteArray方法1
    serial_mp285->write(bytes);

    QByteArray buffer = serial_mp285->read(33);

    if(buffer.length()>30)
    {
        int16_t step_mult, vscale_factor, current_velocity;

        step_mult = float(buffer[25])*256 + float(buffer[24]);



        if (buffer[29] > 127)
            vscale_factor = 50;
        else
            vscale_factor = 10;

        current_velocity = float(127 & buffer[29])*256+float(buffer[28]);

        qDebug() <<  "step_mul (usteps/um): %g" << step_mult;
        qDebug() <<  "xspeed [velocity] (usteps/sec): %g" << current_velocity;
        qDebug() <<  "velocity scale factor (usteps/step): %g" << vscale_factor;
    }

}

void MP285::stop()
{
    stopped = true;
}
