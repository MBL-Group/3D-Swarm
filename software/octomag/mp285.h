#ifndef MP285_H
#define MP285_H

#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>

class MP285 : public QThread
{
    Q_OBJECT
public:
    explicit MP285(QObject *parent = nullptr);

    QStringList get_port_list();
    void serial_open(QString port, int baudrate);
    void serial_close();
    void serial_write(QByteArray data);

    void get_position();
    void goto_position(int x, int y, int z);
    void set_velocity(int16_t vel, int v_scalf);
    void update_panel();
    void set_origin();
    void send_reset();
    void get_status();

    void stop();

    int current_x, current_y, current_z;
    int desired_x, desired_y, desired_z;
    int16_t desired_vel, desired_v_scalf;

    bool get_position_run_once;
    bool goto_position_run_once;
    bool set_velocity_run_once;
    bool update_panel_run_once;
    bool set_origin_run_once;
    bool send_reset_run_once;
    bool get_status_run_once;

    bool is_open;

protected:
    void run();

private:
    QSerialPort* serial_mp285;

    volatile bool stopped;


public slots:
    //void serial_received();
};

#endif // MP285_H
