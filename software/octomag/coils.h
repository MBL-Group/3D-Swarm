#ifndef COILS_H
#define COILS_H

#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>

class Coils : public QThread
{
    Q_OBJECT
public:
    explicit Coils(QObject *parent = nullptr);

    QStringList get_port_list();
    void serial_open(QString port, int baudrate);
    void serial_close();
    void serial_write(QByteArray data);

    void set_output(char address, float current);
    void set_current_mode(char address);
    void open_output(char address);
    void close_output(char address);


    bool is_open;

private:
    QSerialPort* serial_coil;

    uint16_t crc16(uint8_t *buffer, uint16_t buffer_length);

public slots:
    void serial_received();
};

#endif // COILS_H
