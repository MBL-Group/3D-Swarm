#ifndef NI6343_H
#define NI6343_H

#include <QObject>
#include <QThread>

class NI6343: public QThread
{
    Q_OBJECT

public:
    NI6343();
    void set_voltage(uint8_t channel, float voltage);
    void set_mp285_speed(int v1, int v2, int v3, bool enable);
    int find_closest(float arr[], int n, float target);
    void stop();

protected:
    void run();

private:

    volatile bool stopped;

    float voltage1;
    float voltage2;
    float voltage3;
    float voltage4;

    uint8_t update_channel;

};

#endif // NI6343_H
