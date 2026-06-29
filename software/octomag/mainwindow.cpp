
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui/qpixmap.h>
#include <QThread>
#include <QTime>
#include <QDebug>

#include "cameras.h"
#include "coils.h"
#include "ni6343.h"
#include "mp285.h"


#include <QSerialPort>
#include <QGraphicsItem>

using namespace std;


QGraphicsScene* camera_scene;
QGraphicsPixmapItem* camera_pixmap;

QGraphicsScene* camera_scene2;
QGraphicsPixmapItem* camera_pixmap2;

Cameras* cameras;
Coils* coils;
NI6343* ni6343;
MP285* mp285;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "Main thread address:  " << QThread::currentThread();

    // 创建摄像头子线程
    //不是QT的类型要注册信号
    qRegisterMetaType<cv::Mat>("cv::Mat");
    camera_scene = new QGraphicsScene;//新建场景
    camera_pixmap = new QGraphicsPixmapItem;//新建场景中的摄像头显示pixmap item
    camera_scene->addItem(camera_pixmap); //将摄像头item放入scence中
    ui->graphicsView->setScene(camera_scene);//在graphicview中放置scence
    ui->graphicsView->show();//显示

    camera_scene2 = new QGraphicsScene;//新建场景
    camera_pixmap2 = new QGraphicsPixmapItem;//新建场景中的摄像头显示pixmap item
    camera_scene2->addItem(camera_pixmap2); //将摄像头item放入scence中
    ui->graphicsView2->setScene(camera_scene2);//在graphicview中放置scence
    ui->graphicsView2->show();//显示


    camera_satate = false;


    // 线圈 thread
    coils = new Coils();
    coils->set_output(0x01,-1.0);
    coils->set_current_mode(0x01);
    coils->open_output(0x01);
    coils->close_output(0x01);

    QStringList serial_port_list = coils->get_port_list();
    ui->coilsComboBox->addItems(serial_port_list);


    // mp285 ni6343 speed thread
    mp285_speed_state = false;
    ni6343 = new NI6343();


    // mp285 position thread
    mp285 = new MP285();
    serial_port_list = mp285->get_port_list();
    ui->mp285ComboBox->addItems(serial_port_list);

    // 处理摄像头界面事件， ui.graphicsView的控件被重写为Canvas, 图像界面处理事件请到canvas.c
    ui->graphicsView->setup_ui(ui);
    ui->graphicsView2->setup_ui(ui);

}

// 摄像头 使能 按钮
void MainWindow::on_cameraPushButton_clicked()
{
    if (camera_satate == true)
    {
        if (cameras->isRunning())
        {
            cameras->stop();
            cameras->destroyed();
        }
        camera_satate = false;
        ui->cameraPushButton->setIcon(QIcon(":/resources/play-fill.png"));
    }
    else
    {
        cameras = new Cameras();
        cameras->camera1_enable = ui->checkBoxCamera1->isChecked();
        cameras->camera2_enable = ui->checkBoxCamera2->isChecked();
        connect(cameras, SIGNAL(send_image(cv::Mat)), this, SLOT(get_image(cv::Mat)));
        connect(cameras, SIGNAL(send_image2(cv::Mat)), this, SLOT(get_image2(cv::Mat)));

        cameras->start();
        camera_satate = true;
        ui->cameraPushButton->setIcon(QIcon(":/resources/stop-fill.png"));
    }
}


/************************************************* 摄像头界面刷新 *************************************************/
void MainWindow::get_image(cv::Mat image)
{
    QImage qt_image = cameras->mat_to_qimage(image);
    //qDebug() << "Received qimage:  " << qt_image;
    QPixmap qt_pixmap = QPixmap::fromImage(qt_image);

    camera_pixmap->setPixmap(qt_pixmap);
    camera_scene->update();
}

void MainWindow::get_image2(cv::Mat image)
{
    QImage qt_image = cameras->mat_to_qimage(image);
    //qDebug() << "Received qimage:  " << qt_image;
    QPixmap qt_pixmap = QPixmap::fromImage(qt_image);

    camera_pixmap2->setPixmap(qt_pixmap);
    camera_scene2->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_coilsPushButton_clicked()
{
    QStringList serial_port_list = coils->get_port_list();

    if (coils->is_open==false)
    {
        coils = new Coils();
        coils->serial_open(serial_port_list[ui->coilsComboBox->currentIndex()],9600);
        ui->coilsPushButton->setText("Close");
        coils->start();
    }
    else
    {
        coils->serial_close();
        if (coils->isRunning())
        {
            coils->destroyed();
        }
        ui->coilsPushButton->setText("Open");
    }

}

void MainWindow::on_pushButtonCoilsSin_clicked()
{
    QByteArray send_list;
    send_list.resize(5);
    send_list[0] = 0x12;
    send_list[1] = 0x10;
    send_list[2] = 0x20;
    send_list[3] = 0x30;
    send_list[4] = 0x00;

    QString str("hello\r\n");
    QByteArray bytes = str.toUtf8(); // QString转QByteArray方法1

    coils->serial_write(bytes);
}


/************************************************* 位置模式下的按钮操作逻辑*************************************************/

#pragma region "mp285 position mode" {

// MP285 位置模式 串口使能 按钮
void MainWindow::on_mp285PositionPushButton_clicked()
{

    QStringList serial_port_list = mp285->get_port_list();

    if (mp285->is_open==false)
    {
        mp285 = new MP285();
        mp285->serial_open(serial_port_list[ui->mp285ComboBox->currentIndex()],9600);
        ui->mp285PositionPushButton->setText("Close");
        mp285->start();

        mp285->set_velocity(200,10);
        QThread::msleep(100);
        mp285->update_panel();
        QThread::msleep(100);
        mp285->get_status();
        QThread::msleep(100);
    }
    else
    {
        mp285->serial_close();
        if (mp285->isRunning())
        {
            mp285->stop();
            mp285->destroyed();
        }
        ui->mp285PositionPushButton->setText("Open");
    }
}

void MainWindow::on_pushButtonSetOriginMP285_clicked()
{
    mp285->set_origin();
    QThread::msleep(100);
    mp285->update_panel();
    QThread::msleep(100);
}


void MainWindow::on_pushButtonGoPositionMP285_clicked()
{
    mp285->desired_x = ui->doubleSpinBoxSetXPosition->value();
    mp285->desired_y = ui->doubleSpinBoxSetYPosition->value();
    mp285->desired_z = ui->doubleSpinBoxSetZPosition->value();
    mp285->goto_position(mp285->desired_x,mp285->desired_y,mp285->desired_z);
    QThread::msleep(100);
    mp285->update_panel();
    QThread::msleep(100);
}

void MainWindow::on_pushButtonSetVelocity_clicked()
{
    if(ui->spinBoxVelResolution->value() == 0 )
        mp285->set_velocity(ui->spinBoxVelocity->value(),10);
    else
        mp285->set_velocity(ui->spinBoxVelocity->value(),50);
    QThread::msleep(100);
    mp285->update_panel();
}

void MainWindow::on_pushButtonMP285Reset_clicked()
{
    mp285->send_reset();
}

#pragma endregion }

/************************************************* 速度模式下的按钮操作逻辑*************************************************/

#pragma region "mp285 speed mode" {


// MP285 速度模式 使能 按钮
void MainWindow::on_mp285SpeedPushButton_clicked()
{
    if (mp285_speed_state == true)
    {
        if (ni6343->isRunning())
        {
            ni6343->stop();
            ni6343->destroyed();
        }
        mp285_speed_state = false;
        ui->mp285SpeedPushButton->setIcon(QIcon(":/resources/play-fill.png"));
    }
    else
    {
        ni6343 = new NI6343();
        ni6343->start();
        mp285_speed_state = true;
        ui->mp285SpeedPushButton->setIcon(QIcon(":/resources/stop-fill.png"));
    }
}

void MainWindow::on_pushButtonMP285SpeedYp_pressed()
{
    int speed = ui->spinBoxMP285SetSpeed->value();
    ni6343->set_mp285_speed(0,speed,0,true);
}


void MainWindow::on_pushButtonMP285SpeedYp_released()
{
    ni6343->set_mp285_speed(0,0,0,false);
}


void MainWindow::on_pushButtonMP285SpeedXp_pressed()
{
    int speed = ui->spinBoxMP285SetSpeed->value();
    ni6343->set_mp285_speed(speed,0,0,true);
}


void MainWindow::on_pushButtonMP285SpeedXp_released()
{
    ni6343->set_mp285_speed(0,0,0,false);
}


void MainWindow::on_pushButtonMP285SpeedYn_pressed()
{
    int speed = ui->spinBoxMP285SetSpeed->value();
    ni6343->set_mp285_speed(0,-speed,0,true);
}


void MainWindow::on_pushButtonMP285SpeedYn_released()
{
    ni6343->set_mp285_speed(0,0,0,false);
}


void MainWindow::on_pushButtonMP285SpeedXn_pressed()
{
    int speed = ui->spinBoxMP285SetSpeed->value();
    ni6343->set_mp285_speed(-speed,0,0,true);
}


void MainWindow::on_pushButtonMP285SpeedXn_released()
{
    ni6343->set_mp285_speed(0,0,0,false);
}


void MainWindow::on_pushButtonMP285SpeedZp_pressed()
{
    int speed = ui->spinBoxMP285SetSpeed->value();
    ni6343->set_mp285_speed(0,0,speed,true);
}


void MainWindow::on_pushButtonMP285SpeedZp_released()
{
    ni6343->set_mp285_speed(0,0,0,false);
}


void MainWindow::on_pushButtonMP285SpeedZn_pressed()
{
    int speed = ui->spinBoxMP285SetSpeed->value();
    ni6343->set_mp285_speed(0,0,-speed,true);
}


void MainWindow::on_pushButtonMP285SpeedZn_released()
{
    ni6343->set_mp285_speed(0,0,0,false);
}

#pragma endregion }




