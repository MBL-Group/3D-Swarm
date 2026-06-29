#include "cameras.h"
#include <opencv2/opencv.hpp>
#include <QThread>
#include <QDebug>
#include <time.h>


using namespace std;
using namespace cv;


int next_query = 0;
int use_which_frame = 1;



Cameras::Cameras(QObject *parent)
    : QThread{parent}
{
    stopped = false;
    camera1_enable = false;
    camera2_enable = false;
}


void Cameras::run()
{

    qDebug() << "Current thread:" << QThread::currentThreadId();

    if ((!cv_cap1.isOpened()) && (camera1_enable==true))
    {
        cv_cap1.open(0);
    }

    if ((!cv_cap2.isOpened()) && (camera2_enable==true))
    {
        cv_cap2.open(1);
    }

    double time_start = 0;
    double time_finish = 0;

    while (!stopped)
    {

        time_finish = (double)clock();
        //qDebug() << "camera read use (ms):" << (time_finish-time_start);

        if (camera1_enable)
        {
            cv_cap1 >> cv_src1;
            if(!cv_src1.data)
            {
                continue;
            }
            emit send_image(cv_src1);
        }
        if (camera2_enable)
        {
            cv_cap2 >> cv_src2;
            if(!cv_src2.data)
            {
                continue;
            }
            //cv::flip(cv_src2,cv_src2,0);
            emit send_image2(cv_src2);
        }

        cv_src1.release();
        cv_src2.release();
        time_start = (double)clock();
    }
    cv_cap1.release();
    cv_cap2.release();
}

void Cameras::stop()
{
    stopped = true;
}

//Mat转成QImage
QImage Cameras::mat_to_qimage(const cv::Mat &src)
{
    //CV_8UC1 8位无符号的单通道---灰度图片
    if (src.type() == CV_8UC1)
    {
        static QVector<QRgb>  sColorTable;
        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
           for ( int i = 0; i < 256; ++i )
              sColorTable.push_back( qRgb( i, i, i ) );
        }
        QImage image( src.data, src.cols, src.rows, src.step, QImage::Format_Indexed8 );
        image.setColorTable( sColorTable );
        return image;
    }
    //为3通道的彩色图片
    else if (src.type() == CV_8UC3)
    {
        //得到图像的的首地址
        const uchar *pSrc = (const uchar*)src.data;
        //以src构造图片
        QImage qImage(pSrc, src.cols, src.rows, src.step, QImage::Format_RGB888);
        //在不改变实际图像数据的条件下，交换红蓝通道
        return qImage.rgbSwapped();
    }
    //四通道图片，带Alpha通道的RGB彩色图像
    else if (src.type() == CV_8UC4)
    {
        const uchar *pSrc = (const uchar*)src.data;
        QImage qImage(pSrc, src.cols, src.rows, src.step, QImage::Format_ARGB32);
        //返回图像的子区域作为一个新图像
        return qImage.copy();
    }
    else
    {
        return QImage();
    }
}

