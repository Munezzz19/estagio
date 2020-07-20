#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void writeData(const QString &data);
    void readData();
    void calibracao(const QByteArray &data_hexa);


private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *arduino;
    QSerialPort *serial = nullptr;
    QByteArray data2;
    int flag_tara = 0;
    int flag_calib = 0;
    int flag_calib_enc = 0;


    float ganho = 0;
    float peso_g = 0;

    static const quint16 arduino_micro_vendor_id = 9025;
    static const quint16 arduino_micro_product_id = 32823;

    QString arduino_port_name;
    bool arduino_is_available;
};
#endif // MAINWINDOW_H
