#ifndef CAMERAS_H
#define CAMERAS_H

#include <QThread>
#include <QtGui/qpixmap.h>
#include <opencv2/opencv.hpp>


class Cameras : public QThread
{
    Q_OBJECT

public:
    void stop();
    explicit Cameras(QObject *parent = nullptr);

    int camera_index;
    bool camera1_enable;
    bool camera2_enable;

    cv::VideoCapture cv_cap1;
    cv::Mat cv_src1;

    cv::VideoCapture cv_cap2;
    cv::Mat cv_src2;

    cv::Mat cv_images; // Whole images from cameras

    QImage mat_to_qimage(const cv::Mat &src);


protected:
    void run();

private:
    volatile bool stopped;

signals:
    void send_image(const cv::Mat&);
    void send_image2(const cv::Mat&);


};

#endif // CAMERAS_H
