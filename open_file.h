#ifndef OPEN_FILE_H
#define OPEN_FILE_H

#include <QDialog>
#include "qcustomplot.h"

namespace Ui {
class Open_file;
}

class Open_file : public QDialog
{
    Q_OBJECT

public:
    explicit Open_file(QWidget *parent = nullptr);

    void setupProgram();

    void setupRealtime(QCustomPlot *customplot_raw, QCustomPlot *customplot_2,QCustomPlot *customplot_average,QCustomPlot *customplot_anaylsis,QCustomPlot *customplot_changing);

    ~Open_file();

private slots:
    void on_Browse_clicked();

    void on_Read_clicked();

    void realtimeDataSlot();

private:

    Ui::Open_file *ui;

    QVector <double>  Get_data_from_csv();

    void Average_Shape(int peak_location);

    void Get_Parameters(QVector <double> signal);

    double standard_deviation_calculation(QVector <double> signal);

    void Motion_Artifact_Check(QVector <double> signal,int bpm);

    void CheckDataLength_filter(QVector <double> &data_to_filter);

    void CheckDataLength_motion(QVector <double> &data_to_motion);

    QVector <double> ApplyFilt_bandpass_1(QVector <double> data_to_filter,QVector <double> data_counter_vector);

    QVector <double> ApplyFilt_bandpass_1_35(QVector <double> data_to_filter, QVector <double> data_counter_vector);

    QVector <double> ApplyFilt_bandpass_07_40(QVector <double> data_to_filter, QVector <double> data_counter_vector);

    QVector <double> ApplyFilt_bandpass_5_25(QVector <double> data_to_filter, QVector <double> data_counter_vector);

    QVector <double> ApplyFilt_bandpass_filtfilt_10_20(QVector <double> data_to_filter,QVector <double> data_counter_vector);

    QVector <double> ApplyFilt_bandpass_10_20_all(QVector <double> data_to_filter,QVector <double> data_counter_vector);

    void CheckDataLength_peaks(QVector <double> &data_to_filter, int &counter_for_check_length);

    int Check_Peaks(QVector <double> data_to_peaks, QVector <double> &R_peaks_location, int &bpm, int counter_for_length);

    int Give_me_my_peaks(QVector <double> data_to_peaks, int counter_for_length);

    void Give_bpm (QVector <double> R_peaks_location, int &bpm);

    QVector <double> Check_Which_Filter(QVector <double> keys);

    QTimer dataTimer;
};

#endif // OPEN_FILE_H
