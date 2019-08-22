#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSerialPort>
#include <QMainWindow>
#include "qcustomplot.h"
#include <QtSerialPort/QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupProgram();

    void setupRealtime(QCustomPlot *customplot, QCustomPlot *customplot_2, QCustomPlot *customplot_average,QCustomPlot *customplot_analysis,QCustomPlot *customplot_square,QCustomPlot *customplot_moving);


private slots:
  void realtimeDataSlot();
  void on_Saving_location_clicked();

private:
    QVector <double> readData(QVector <double> &data_counter_vector, quint8 &temp);

    QVector <double> ApplyFilt_bandpass_1(QVector <double> data_to_filter,QVector <double> data_counter_vector);

    QVector <double> ApplyFilt_bandpass_1_all(QVector <double> data_to_filter,QVector <double> data_counter_vector);

    QVector <double> ApplyFilt_bandpass_2(QVector <double> data_to_filter, QVector <double> data_counter_vector);

    QVector <double> ApplyFilt_bandpass_40(QVector <double> data_to_filter, QVector <double> data_counter_vector);

    QVector <double> ApplyFilt_bandpass_40_fir1(QVector <double> data_to_filter, QVector <double> data_counter_vector);

    QVector <double> ApplyFilt_bandpass_filtfilt(QVector <double> data_to_filter,QVector <double> data_counter_vector);

    QVector <double> Take_Square(QVector <double> data_to_square);

    QVector <double> ApplyFilt_Moving_Average(QVector <double> data_to_moving, QVector <double> data_counter_vector);

    void Get_Parameters(QVector <double> signal);

    void Average_Shape(int peak_location);

    void CheckDataLength_peaks(QVector <double> &data_to_filter, int &counter_for_check_length);

    void CheckDataLength_filter(QVector <double> &data_to_filter);

    void CheckDataLength_motion(QVector <double> &data_to_motion);

    int Give_me_my_peaks(QVector <double> data_to_peaks, int counter_for_length);

    void Give_bpm (QVector <double> R_peaks_location, int &bpm);

    int Check_Peaks(QVector <double> data_to_peaks, QVector <double> &R_peaks_location, int &bpm, int counter_for_length);

    double standard_deviation_calculation(QVector <double> signal);

    void Write_Data(QVector <double> data_to_write);

    void Write_QRS(int data_to_write);

    void Write_QT(int data_to_write);

    void Write_PR(int data_to_write);

    void Motion_Artifact_Check(QVector <double> signal,int bpm);


    Ui::MainWindow *ui;
    QSerialPort *m_serial;
    QTimer dataTimer;

};

#endif // MAINWINDOW_H
