#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Canvas : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = 0);
    void setup_ui(Ui::MainWindow *ui);

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void panBeginImpl(QMouseEvent *event);
    void panEndImpl(QMouseEvent *event);


private:
    Ui::MainWindow *ui_from_main;

    int current_key; //记录当前按下的键盘
    qreal current_scale; //记录缩放比例

    int current_mouse_key;
    QPoint mouse_press_point;

signals:

public slots:
private slots:
};

#endif // CANVAS_H
