#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui/qpixmap.h>
#include <opencv2/opencv.hpp>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_cameraPushButton_clicked();

    void get_image(cv::Mat image);
    void get_image2(cv::Mat image);

    void on_coilsPushButton_clicked();

    void on_mp285SpeedPushButton_clicked();

    void on_mp285PositionPushButton_clicked();



    void on_pushButtonSetOriginMP285_clicked();

    void on_pushButtonGoPositionMP285_clicked();

    void on_pushButtonMP285SpeedYp_pressed();

    void on_pushButtonMP285SpeedYp_released();

    void on_pushButtonMP285SpeedXp_pressed();

    void on_pushButtonMP285SpeedXp_released();

    void on_pushButtonMP285SpeedYn_pressed();

    void on_pushButtonMP285SpeedYn_released();

    void on_pushButtonMP285SpeedXn_pressed();

    void on_pushButtonMP285SpeedXn_released();

    void on_pushButtonMP285SpeedZp_pressed();

    void on_pushButtonMP285SpeedZp_released();

    void on_pushButtonMP285SpeedZn_pressed();

    void on_pushButtonMP285SpeedZn_released();

    void on_pushButtonCoilsSin_clicked();

    void on_pushButtonSetVelocity_clicked();

    void on_pushButtonMP285Reset_clicked();

private:
    Ui::MainWindow *ui;

    bool camera_satate;
    bool mp285_speed_state;


};
#endif // MAINWINDOW_H
