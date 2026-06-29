#include "canvas.h"
#include <QDebug>
#include <QKeyEvent>



Canvas::Canvas(QWidget *parent) :
    QGraphicsView(parent)
{
    current_scale = 1.0;
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);//基于鼠标点缩放
}

void Canvas::setup_ui(Ui::MainWindow *ui)
{
    ui_from_main = ui;
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
    current_key = event->key();
    QGraphicsView::keyPressEvent(event);
}

void Canvas::keyReleaseEvent(QKeyEvent *event)
{
    current_key = NULL;
    QGraphicsView::keyReleaseEvent(event);
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    // control + wheel 控制缩放
    QPoint sroll=event->angleDelta();
    if(current_key == Qt::Key_Control)
    {
        if(sroll.y()>0)
        {
            if(current_scale > 10)
                return;
            current_scale*=1.1;
            scale(1.1,1.1);
        }
        else
        {
            if(current_scale < 0.1)
                return;
            current_scale/=1.1;
            scale(1/1.1,1/1.1);
        }
    }
    QGraphicsView::wheelEvent(event);
}



void Canvas::mousePressEvent(QMouseEvent *event)
{
    current_mouse_key = event->buttons();
    mouse_press_point = event->pos();

    // 鼠标中键平移
    if (event->button() == Qt::MiddleButton)
    {
        return panBeginImpl(event);
    }

    if (event->button() == Qt::LeftButton)
    {
        qDebug() << "position:" << event->position() <<"global position: "<<event->globalPosition()<<"mapToscence: "<<mapToScene(event->pos());
    }

    QGraphicsView::mousePressEvent(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    current_mouse_key = NULL;

    // 鼠标中键释放平移
    if (event->button() == Qt::MiddleButton)
    {
        return panEndImpl(event);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void Canvas::panBeginImpl(QMouseEvent *event)
{
    auto releaseEvent = new QMouseEvent(QEvent::MouseButtonRelease, event->position(), event->globalPosition(),
                                        Qt::LeftButton, Qt::NoButton, event->modifiers());
    QGraphicsView::mouseReleaseEvent(releaseEvent);
    //切换到滚动模式来完成平移动作
    setDragMode(QGraphicsView::ScrollHandDrag);
    auto fakeEvent = new QMouseEvent(event->type(), event->position(), event->globalPosition(), Qt::LeftButton,
                                     event->buttons() | Qt::LeftButton, event->modifiers());
    QGraphicsView::mousePressEvent(fakeEvent);
}
void Canvas::panEndImpl(QMouseEvent *event)
{
    //鼠标中键释放后切换回原有模式
    auto fakeEvent = new QMouseEvent(event->type(), event->position(), event->globalPosition(), Qt::LeftButton,
                                     event->buttons() & ~Qt::LeftButton, event->modifiers());
    QGraphicsView::mouseReleaseEvent(fakeEvent);
    setDragMode(QGraphicsView::RubberBandDrag);
}

