#include "open_file.h"
#include "ui_open_file.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <qcustomplot.h>

static QString File_name;

static QString Sampling_Rate;

static QVector <double> all_data;

static int Fs;

static QVector <double> data_to_filter;

static QVector <double> average_signal;

static int counter_glob;

Open_file::Open_file(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Open_file)
{
    ui->setupUi(this);
    ui->listWidget_filter->addItem("Bandpass 1 - 35 Butterworth");
    ui->listWidget_filter->addItem("Bandpass 0.7-40 Butterworth");
    ui->listWidget_filter->addItem("Bandpass 5-25 Butterworth");
    ui->listWidget_filter->addItem("Bandpass 10 - 20 Butterworth filtfilt");
}

void Open_file::setupProgram()
{
    setupRealtime(ui->customplot_raw,ui->customplot_2,ui->customplot_average,ui->customplot_analysis,ui->customplot_changing);
}

void Open_file::setupRealtime(QCustomPlot *customplot_raw, QCustomPlot *customplot_2,QCustomPlot *customplot_average,QCustomPlot *customplot_analysis,QCustomPlot *customplot_changing)
{
    const int sampling_rate = Sampling_Rate.toInt();
    Fs = sampling_rate;
    const int delay_time = (8000 * (1.0/Fs));


    QCPItemText *textLabel = new QCPItemText(customplot_raw);
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.5, 0);
    textLabel->setText("Raw Data");

    customplot_raw->addGraph(); // blue line
    customplot_raw->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    customplot_raw->axisRect()->setupFullAxesBox();
    ui->customplot_raw->yAxis->setRange(0 ,50);

    QCPItemText *textLabel_2 = new QCPItemText(customplot_2);
    textLabel_2->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    textLabel_2->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel_2->position->setCoords(0.5, 0);
    textLabel_2->setText("Butterworth Band pass filter [10 20]");

    customplot_2->addGraph();
    customplot_2->graph(0)->setPen(QPen(QColor(1,1,1)));
    customplot_2->addGraph();
    customplot_2->graph(1)->setPen(QPen(QColor(255,0,0)));
    customplot_2->graph(1)->setScatterStyle(QCPScatterStyle::ssStar);
    ui->customplot_2->graph(1)->setLineStyle(QCPGraph::lsNone);

    QCPItemText *textLabel_3 = new QCPItemText(customplot_average);
    textLabel_3->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    textLabel_3->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel_3->position->setCoords(0.5, 0);
    textLabel_3->setText("Average Shape");

    ui->customplot_average->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->customplot_average->legend->setFont(legendFont);
    ui->customplot_average->legend->setSelectedFont(legendFont);
    ui->customplot_average->legend->setSelectableParts(QCPLegend::spItems);

    customplot_average->addGraph();
    customplot_average->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    customplot_average->addGraph();
    customplot_average->graph(1)->setPen(QColor(50, 255, 50));
    customplot_average->graph(1)->setLineStyle(QCPGraph::lsNone);
    customplot_average->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar,10));

    customplot_average->addGraph();
    customplot_average->graph(2)->setPen(QColor(255, 50, 255));
    customplot_average->graph(2)->setLineStyle(QCPGraph::lsNone);
    customplot_average->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar,10));

    customplot_average->addGraph();
    customplot_average->graph(3)->setPen(QColor(200, 100, 50));
    customplot_average->graph(3)->setLineStyle(QCPGraph::lsNone);
    customplot_average->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar,10));

    customplot_average->addGraph();
    customplot_average->graph(4)->setPen(QColor(110, 50, 110));
    customplot_average->graph(4)->setLineStyle(QCPGraph::lsNone);
    customplot_average->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssStar,10));

    ui->customplot_average->graph(0)->setName("ECG");
    ui->customplot_average->graph(1)->setName("Q point");
    ui->customplot_average->graph(2)->setName("S point");
    ui->customplot_average->graph(3)->setName("T wave peak");
    ui->customplot_average->graph(4)->setName("P wave peak");

    customplot_average->axisRect()->setupFullAxesBox();

    QCPItemText *textLabel_4 = new QCPItemText(customplot_analysis);
    textLabel_4->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    textLabel_4->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel_4->position->setCoords(0.5, 0);
    textLabel_4->setText("Analysis window");

    customplot_analysis->addGraph();
    customplot_analysis->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    customplot_analysis->addGraph(customplot_analysis->xAxis2 ,customplot_analysis->yAxis2);
    customplot_analysis->graph(1)->setPen(QColor(255, 50, 50));

    customplot_analysis->axisRect()->setupFullAxesBox();
    customplot_analysis->yAxis->setRange(-100, 100);
    customplot_analysis->yAxis2->setRange(0,6);
    customplot_analysis->xAxis2->setVisible(true);
    customplot_analysis->yAxis2->setVisible(true);
    customplot_analysis->yAxis2->setTickLabels(true);
    customplot_analysis->yAxis2->setLabel(" Motion Artifact point");


    QCPItemText *textLabel_5 = new QCPItemText(customplot_changing);
    textLabel_5->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    textLabel_5->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel_5->position->setCoords(0.5, 0);
    textLabel_5->setText("User choice filtered Signal");

    customplot_changing->addGraph();
    customplot_changing->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    customplot_changing->axisRect()->setupFullAxesBox();

    connect(ui->customplot_raw->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customplot_raw->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->customplot_raw->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customplot_raw->yAxis, SLOT(setRange(QCPRange)));

    connect(ui->customplot_2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customplot_2->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->customplot_2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customplot_2->yAxis, SLOT(setRange(QCPRange)));

    connect(customplot_analysis->xAxis, SIGNAL(rangeChanged(QCPRange)), customplot_analysis->xAxis, SLOT(setRange(QCPRange)));
    connect(customplot_analysis->yAxis, SIGNAL(rangeChanged(QCPRange)), customplot_analysis->yAxis, SLOT(setRange(QCPRange)));

    connect(customplot_changing->xAxis, SIGNAL(rangeChanged(QCPRange)), customplot_changing->xAxis, SLOT(setRange(QCPRange)));
    connect(customplot_changing->yAxis, SIGNAL(rangeChanged(QCPRange)), customplot_changing->yAxis, SLOT(setRange(QCPRange)));

    QSharedPointer<QCPAxisTicker> count(new QCPAxisTicker);
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(delay_time);

}

void Open_file::realtimeDataSlot()
{
    static QTime time(QTime::currentTime());

    static int counter = 0;
    static QVector <double> data_to_peaks;
    static QVector <double> R_peaks_location;
    static QVector <double> data_to_moving_filter;
    static QVector <double> data_to_motion_artifact;
    static int internal_counter = 0;
    static int bpm = 0;
    static int counter_for_check_length = 0;

    const int Buffer_size = 8;

    QVector <double> data_to_plot;
    QVector <double> filtered_signal;
    QVector <double> filtered_signal_changed_by_list;
    QVector <double> bandpass_40;
    QVector <double> keys;

    double check = time.elapsed() - internal_counter * (Fs / 3.0);

    int peak_check = 0;

    data_to_plot = Get_data_from_csv();

    for (int var = counter; var < data_to_plot.size() + counter; ++var)
    {
        keys.append(var);
        counter_glob++;
    }

    data_to_filter.append(data_to_plot);

    data_to_motion_artifact.append(data_to_plot);

    CheckDataLength_motion(data_to_motion_artifact);
    CheckDataLength_filter(data_to_filter);
    CheckDataLength_peaks(data_to_peaks,counter_for_check_length);

    filtered_signal_changed_by_list = Check_Which_Filter(keys);

    ui->customplot_changing->graph(0)->addData(keys,filtered_signal_changed_by_list,true);

    ui->customplot_analysis->graph(0)->addData(keys,data_to_plot,true);

    ui->customplot_raw->graph(0)->addData(keys,data_to_plot,true);

    filtered_signal = ApplyFilt_bandpass_1(data_to_filter,keys);

    bandpass_40 = ApplyFilt_bandpass_1_35(data_to_filter,keys);

    average_signal.append(bandpass_40);

    data_to_peaks.append(filtered_signal);

    if(check > (Fs / 4.0))
    {
        peak_check = Check_Peaks(data_to_peaks, R_peaks_location, bpm,counter_for_check_length);
        Motion_Artifact_Check(data_to_motion_artifact, bpm);
        internal_counter++;
    }

    if( peak_check != 0 && peak_check !=1)
    {
        Average_Shape(peak_check);
    }


    ui->customplot_2->graph(0)->addData(keys,filtered_signal);

    ui->customplot_raw->graph(0)->rescaleValueAxis(false,true);
    ui->customplot_2->graph(0)->rescaleValueAxis(false,true);
    ui->customplot_analysis->graph(0)->rescaleValueAxis(false,true);
    ui->customplot_changing->graph(0)->rescaleValueAxis(false,true);
    ui->customplot_average->graph(0)->rescaleValueAxis(false,true);
    ui->customplot_analysis->graph(1)->rescaleKeyAxis();

    ui->customplot_raw->xAxis->setRange(keys.last(),(2*Fs), Qt::AlignRight);
    ui->customplot_2->xAxis->setRange(keys.last(), (2*Fs),Qt::AlignRight);
    ui->customplot_analysis->xAxis->setRange(keys.last(),(3 * Fs), Qt::AlignRight);
    ui->customplot_analysis->xAxis2->setRange(keys.last(),(3 * Fs), Qt::AlignRight);
    ui->customplot_changing->xAxis->setRange(keys.last(), (2 * Fs),Qt::AlignRight);
    ui->customplot_average->xAxis->setRangeLower(0);
    ui->customplot_average->xAxis->setRangeUpper(400);

    ui->customplot_raw->replot();
    ui->customplot_2->replot();
    ui->customplot_average->replot();
    ui->customplot_analysis->replot();
    ui->customplot_changing->replot();

    counter = counter + Buffer_size;

}

void Open_file::Average_Shape(int peak_location)
{
    static QVector <int> peaks_location;
    static int counter_for_raw_signal;
    static int sum_counter;

    int window_width = 300;

    static QVector <double> signal_average(window_width);

    QVector <double> signal_average_raw(window_width);
    QVector <double> signal_section(window_width);
    QVector <double> signal_section_key(window_width);

    for (int var = 0; var < window_width; ++var)
    {
        signal_section_key[var] = var;

    }

    peaks_location.append(peak_location);


    if(peaks_location.first() < 150)
    {
        peaks_location.remove(0);
    }
    else
    {
        if(peaks_location.size() > 2)
        {
            int first_border = peaks_location.first() - 150 - counter_for_raw_signal;

            for (int var = 0 ; var < window_width ; ++var)
            {
                signal_section[var] = average_signal[var + first_border];
                signal_average[var] = ((signal_average[var] + signal_section[var]) / 2) + 10;
            }

            ui->customplot_average->graph(0)->setData(signal_section_key,signal_average,true);
            sum_counter++;
            Get_Parameters(signal_average);

            for (int var = 0; var < peaks_location.first() - 20 - counter_for_raw_signal; ++var)
            {
                average_signal.removeFirst();
                counter_for_raw_signal++;
            }

            peaks_location.removeFirst();

        }

    }

    if(sum_counter == 4)
    {
        sum_counter = 0;
        signal_average.swap(signal_average_raw);
    }

}

void Open_file::Get_Parameters(QVector <double> signal)
{
    int peak_location;

    peak_location = Give_me_my_peaks(signal,0);

    int window_for_Q_point = Fs / 10;
    int window_for_S_point = Fs / 9;

    double S_point_location;
    double S_point_value;

    double Q_point_location;
    double Q_point_value;

    double T_point_location;
    double T_point_value;

    double P_point_location;
    double P_point_value;

    QVector <double> possible_Q_points;
    QVector <double> possible_S_points;


    if( (peak_location - window_for_Q_point > 0) && (peak_location < 250))
    {

        int temp = 1000;
        for (int var = (peak_location - window_for_Q_point); var < peak_location; ++var)
        {
            if( (signal[var] <= signal[var+1]) && (signal[var - 1] >= signal[var]) && (signal[var] <= temp) )
            {
                possible_Q_points.append(var);
                temp = signal[var];
            }
        }

        if(!(possible_Q_points.isEmpty()))
        {
            Q_point_location = possible_Q_points.last();
            Q_point_value = signal[Q_point_location];

            ui->customplot_average->graph(1)->data()->clear();
            ui->customplot_average->graph(1)->addData(Q_point_location,Q_point_value);

        }
    }

    if( (peak_location + window_for_S_point < 250) )
    {

        int temp = 1000;
        for (int var = peak_location; var < (peak_location + window_for_S_point); ++var)
        {
            if( (signal[var] <= signal[var+1]) && (signal[var - 1] >= signal[var]) && (signal[var] <= temp) )
            {
                possible_S_points.append(var);
                temp = signal[var];
            }
        }

       if(!(possible_S_points.isEmpty()))
       {
           S_point_location = possible_S_points.last();
           S_point_value = signal[S_point_location];


           ui->customplot_average->graph(2)->data()->clear();
           ui->customplot_average->graph(2)->addData(S_point_location,S_point_value);

       }
    }

    QVector <double> T_wave_section;

    for (int var = S_point_location; var < signal.size() - (Fs / 10); ++var)
    {
        T_wave_section.append(signal[var]);
    }

    T_point_location = Give_me_my_peaks(T_wave_section,0) + S_point_location;
    T_point_value = signal[T_point_location];

    ui->customplot_average->graph(3)->data()->clear();
    ui->customplot_average->graph(3)->addData(T_point_location,T_point_value);

    QVector <double> P_wave_section;

    for (int var = 40; var < Q_point_location; ++var)
    {
        P_wave_section.append(signal[var]);
    }

    P_point_location = (Give_me_my_peaks(P_wave_section,0) + 40);

    P_point_value = signal[P_point_location];

    ui->customplot_average->graph(4)->data()->clear();
    ui->customplot_average->graph(4)->addData(P_point_location,P_point_value);

    if( !(possible_S_points.isEmpty()) && !(possible_Q_points.isEmpty()))
    {
        int QRS_duration;
        QRS_duration = 1000 * (( possible_S_points.last() - possible_Q_points.last()) / Fs );

        ui->lcdNumber_QRS->display(QRS_duration);
        if(QRS_duration >= 200)
        {
            ui->lcdNumber_QRS->setPalette(Qt::red);
        }
        if(QRS_duration < 200)
        {
            ui->lcdNumber_QRS->setPalette(Qt::black);
        }

        int QT_duration;
        QT_duration = 1000 * ((-possible_Q_points.last() + T_point_location) / Fs );

        ui->lcdNumber_QT->display(QT_duration);
        if(QT_duration >= 400)
        {
            ui->lcdNumber_QT->setPalette(Qt::red);
        }
        if(QT_duration < 400)
        {
            ui->lcdNumber_QT->setPalette(Qt::black);
        }


        int PR_duration;
        PR_duration = 1000 * ((peak_location - P_point_location) / Fs );

        ui->lcdNumber_PR->display(PR_duration);
        if(PR_duration >= 200)
        {
            ui->lcdNumber_PR->setPalette(Qt::red);
        }
        if(PR_duration < 200)
        {
            ui->lcdNumber_PR->setPalette(Qt::black);
        }

    }

}

void Open_file::on_Browse_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,"Open a csv file","C://");
    File_name = file_name;
}

QVector <double> Open_file::Get_data_from_csv()
{
    static int counter = 0;
    const int Buffer_size = 8;

    QVector <double> data_to_plot;

    if( (counter + Buffer_size) < all_data.size() )
    {
        for (int var = counter; var < (counter + Buffer_size); ++var)
        {
               data_to_plot.append(all_data[var]);
        }

        counter = counter + Buffer_size;
    }

    else
    {
        counter = 0;

        for (int var = counter; var < (counter + Buffer_size); ++var)
        {
               data_to_plot.append(all_data[var]);
        }

        counter = counter + Buffer_size;
    }


    return data_to_plot;
}

QVector <double> Open_file::ApplyFilt_bandpass_1(QVector <double> data_to_filter,QVector <double> data_counter_vector)
{

    int length_of_signal = data_to_filter.size();
    int naxpy;
    int filtered_signal_tmp;
    QVector <double> filtered_signal(length_of_signal);
    QVector <double> filtered_signal_last(data_counter_vector.size());


    static const double dv0[7] = { 0.00072570895160154868, -0.0,
      -0.0021771268548046456, -0.0, 0.0021771268548046456, -0.0,
      -0.00072570895160154868 };

    double as;
    static const double dv1[7] = { 1.0, -5.4143040199969077, 12.409518868546026,
      -15.404517279178839, 10.922127801362153, -4.194632661109563,
      0.682124952012655 };

    //  cuttoff low frequency to get rid of baseline wander
    //  cuttoff frequency to discard high frequency noise
    //  cutt off based on fs
    //  order of 3 less processing


    for (int var = 0; var < length_of_signal; ++var)
    {
        if(length_of_signal - var < 7)
        {
            naxpy = length_of_signal - 1 - var;
        }
        else
        {
            naxpy = 6;

        }

        for (int dummy = 0;  dummy <= naxpy; dummy++)
        {
            filtered_signal_tmp = dummy + var;
            filtered_signal[filtered_signal_tmp] += data_to_filter[var] * dv0[dummy];
        }

        if (length_of_signal - 1 - var < 6)
        {
            naxpy = length_of_signal - 2 - var;
        }

        else
        {
            naxpy = 5;

        }

        as = -filtered_signal[var];

        for (int temp = 0; temp <= naxpy; temp++)
        {
            filtered_signal_tmp = (temp + var) + 1;
            filtered_signal[filtered_signal_tmp] += as * dv1[temp + 1];
        }

    }

    // Take only plotting data for filter
    for (int var = 0; var < data_counter_vector.size()  ; ++var)
    {
       filtered_signal_last[var] =  filtered_signal[filtered_signal.size() + var - data_counter_vector.size()];
    }

    return filtered_signal_last;

}

QVector <double> Open_file::ApplyFilt_bandpass_1_35(QVector <double> data_to_filter, QVector <double> data_counter_vector)
{
//    f1 = 1;
//    f2 = 35;
//    Fs = 330;
//    Wn=[f1 f2] * 2 / Fs ;
//    N = 3;
//    [b, a] = butter(N,Wn,'bandpass');


    int length_of_signal = data_to_filter.size();
    int naxpy;
    int filtered_signal_tmp;
    QVector <double> filtered_signal(length_of_signal);
    QVector <double> filtered_signal_last(data_counter_vector.size());

    static const double dv0[7] = { 0.0195244532080388, -0.0, -0.058573359624116396,
      -0.0, 0.058573359624116396, -0.0, -0.0195244532080388 };

    double as;
    static const double dv1[7] = { 1.0, -4.6923136391650129, 9.21715350373313,
      -9.7580414797826034, 5.9010267641834275, -1.9348516388277224,
      0.26702766217982671 };

    //  cuttoff low frequency to get rid of baseline wander
    //  cuttoff frequency to discard high frequency noise
    //  cutt off based on fs
    //  order of 3 less processing
    //

    for (int var = 0; var < length_of_signal; ++var)
    {
        if(length_of_signal - var < 7)
        {
            naxpy = length_of_signal - 1 - var;
        }
        else
        {
            naxpy = 6;

        }

        for (int dummy = 0;  dummy <= naxpy; dummy++)
        {
            filtered_signal_tmp = dummy + var;
            filtered_signal[filtered_signal_tmp] += data_to_filter[var] * dv0[dummy];
        }

        if (length_of_signal - 1 - var < 6)
        {
            naxpy = length_of_signal - 2 - var;
        }

        else
        {
            naxpy = 5;

        }

        as = -filtered_signal[var];

        for (int temp = 0; temp <= naxpy; temp++)
        {
            filtered_signal_tmp = (temp + var) + 1;
            filtered_signal[filtered_signal_tmp] += as * dv1[temp + 1];
        }

    }

    // Take only plotting data for filter
    for (int var = 0; var < data_counter_vector.size()  ; ++var)
    {
       filtered_signal_last[var] =  filtered_signal[filtered_signal.size() + var - data_counter_vector.size()];
    }

    return filtered_signal_last;
}

QVector <double> Open_file::ApplyFilt_bandpass_07_40(QVector<double> data_to_filter, QVector<double> data_counter_vector)
{
//    f1 = 0.7;
//    f2 = 40;
//    Fs = 330;
//    Wn=[f1 f2] * 2 / Fs ;
//    N = 3;
//    [b, a] = butter(N,Wn,'bandpass');


    int length_of_signal = data_to_filter.size();
    int naxpy;
    int filtered_signal_tmp;
    QVector <double> filtered_signal(length_of_signal);
    QVector <double> filtered_signal_last(data_counter_vector.size());

    static const double dv0[7] = { 0.02809822517670249, -0.0,
      -0.084294675530107471, -0.0, 0.084294675530107471, -0.0,
      -0.02809822517670249 };

    double as;
    static const double dv1[7] = { 1.0, -4.50570708124942, 8.4841478261756222,
      -8.6265471258874982, 5.0382845408450931, -1.6048993348301228,
      0.21472173464013 };

    //  cuttoff low frequency to get rid of baseline wander
    //  cuttoff frequency to discard high frequency noise
    //  cutt off based on fs
    //  order of 3 less processing
    //

    for (int var = 0; var < length_of_signal; ++var)
    {
        if(length_of_signal - var < 7)
        {
            naxpy = length_of_signal - 1 - var;
        }
        else
        {
            naxpy = 6;

        }

        for (int dummy = 0;  dummy <= naxpy; dummy++)
        {
            filtered_signal_tmp = dummy + var;
            filtered_signal[filtered_signal_tmp] += data_to_filter[var] * dv0[dummy];
        }

        if (length_of_signal - 1 - var < 6)
        {
            naxpy = length_of_signal - 2 - var;
        }

        else
        {
            naxpy = 5;

        }

        as = -filtered_signal[var];

        for (int temp = 0; temp <= naxpy; temp++)
        {
            filtered_signal_tmp = (temp + var) + 1;
            filtered_signal[filtered_signal_tmp] += as * dv1[temp + 1];
        }

    }

    // Take only plotting data for filter
    for (int var = 0; var < data_counter_vector.size()  ; ++var)
    {
       filtered_signal_last[var] =  filtered_signal[filtered_signal.size() + var - data_counter_vector.size()];
    }

    return filtered_signal_last;
}

QVector <double> Open_file::ApplyFilt_bandpass_5_25(QVector<double> data_to_filter, QVector<double> data_counter_vector)
{
//    f1 = 5;
//    f2 = 25;
//    Fs = 330;
//    Wn=[f1 f2] * 2 / Fs ;
//    N = 3;
//    [b, a] = butter(N,Wn,'bandpass');


    int length_of_signal = data_to_filter.size();
    int naxpy;
    int filtered_signal_tmp;
    QVector <double> filtered_signal(length_of_signal);
    QVector <double> filtered_signal_last(data_counter_vector.size());

    static const double dv0[7] = { 0.0048805607452786148, -0.0,
      -0.014641682235835845, -0.0, 0.014641682235835845, -0.0,
      -0.0048805607452786148 };

    double as;
    static const double dv1[7] = { 1.0, -5.1228009966562542, 11.057048672035068,
      -12.88379186657323, 8.5535604662290989, -3.0686424109144586,
      0.46469118833975 };

    //  cuttoff low frequency to get rid of baseline wander
    //  cuttoff frequency to discard high frequency noise
    //  cutt off based on fs
    //  order of 3 less processing
    //

    for (int var = 0; var < length_of_signal; ++var)
    {
        if(length_of_signal - var < 7)
        {
            naxpy = length_of_signal - 1 - var;
        }
        else
        {
            naxpy = 6;

        }

        for (int dummy = 0;  dummy <= naxpy; dummy++)
        {
            filtered_signal_tmp = dummy + var;
            filtered_signal[filtered_signal_tmp] += data_to_filter[var] * dv0[dummy];
        }

        if (length_of_signal - 1 - var < 6)
        {
            naxpy = length_of_signal - 2 - var;
        }

        else
        {
            naxpy = 5;

        }

        as = -filtered_signal[var];

        for (int temp = 0; temp <= naxpy; temp++)
        {
            filtered_signal_tmp = (temp + var) + 1;
            filtered_signal[filtered_signal_tmp] += as * dv1[temp + 1];
        }

    }

    // Take only plotting data for filter
    for (int var = 0; var < data_counter_vector.size()  ; ++var)
    {
       filtered_signal_last[var] =  filtered_signal[filtered_signal.size() + var - data_counter_vector.size()];
    }

    return filtered_signal_last;
}

QVector <double> Open_file::ApplyFilt_bandpass_10_20_all(QVector <double> data_to_filter,QVector <double> data_counter_vector)
{

    int length_of_signal = data_to_filter.size();
    int naxpy;
    int filtered_signal_tmp;
    QVector <double> filtered_signal(length_of_signal);
    QVector <double> filtered_signal_last(data_counter_vector.size());

    static const double dv0[7] = { 0.00072570895160154868, -0.0,
      -0.0021771268548046456, -0.0, 0.0021771268548046456, -0.0,
      -0.00072570895160154868 };

    double as;
    static const double dv1[7] = { 1.0, -5.4143040199969077, 12.409518868546026,
      -15.404517279178839, 10.922127801362153, -4.194632661109563,
      0.682124952012655 };

    //  cuttoff low frequency to get rid of baseline wander
    //  cuttoff frequency to discard high frequency noise
    //  cutt off based on fs
    //  order of 3 less processing


    for (int var = 0; var < length_of_signal; ++var)
    {
        if(length_of_signal - var < 7)
        {
            naxpy = length_of_signal - 1 - var;
        }
        else
        {
            naxpy = 6;

        }

        for (int dummy = 0;  dummy <= naxpy; dummy++)
        {
            filtered_signal_tmp = dummy + var;
            filtered_signal[filtered_signal_tmp] += data_to_filter[var] * dv0[dummy];
        }

        if (length_of_signal - 1 - var < 6)
        {
            naxpy = length_of_signal - 2 - var;
        }

        else
        {
            naxpy = 5;

        }

        as = -filtered_signal[var];

        for (int temp = 0; temp <= naxpy; temp++)
        {
            filtered_signal_tmp = (temp + var) + 1;
            filtered_signal[filtered_signal_tmp] += as * dv1[temp + 1];
        }

    }


    return filtered_signal;

}

QVector <double> Open_file::ApplyFilt_bandpass_filtfilt_10_20(QVector <double> data_to_filter,QVector <double> data_counter_vector)
{

    int length_of_signal = data_to_filter.size();
    int naxpy;
    int filtered_signal_tmp;
    QVector <double> filtered_signal(length_of_signal);
    QVector <double> filtered_signal_last(data_counter_vector.size());
    QVector <double> filtered_signal_reversed(length_of_signal);

    static const double dv0[7] = { 0.00072570895160154868, -0.0,
      -0.0021771268548046456, -0.0, 0.0021771268548046456, -0.0,
      -0.00072570895160154868 };

    double as;
    static const double dv1[7] = { 1.0, -5.4143040199969077, 12.409518868546026,
      -15.404517279178839, 10.922127801362153, -4.194632661109563,
      0.682124952012655 };

    //  cuttoff low frequency to get rid of baseline wander
    //  cuttoff frequency to discard high frequency noise
    //  cutt off based on fs
    //  order of 3 less processing


    for (int var = 0; var < length_of_signal; ++var)
    {
        if(length_of_signal - var < 7)
        {
            naxpy = length_of_signal - 1 - var;
        }
        else
        {
            naxpy = 6;

        }

        for (int dummy = 0;  dummy <= naxpy; dummy++)
        {
            filtered_signal_tmp = dummy + var;
            filtered_signal[filtered_signal_tmp] += data_to_filter[var] * dv0[dummy];
        }

        if (length_of_signal - 1 - var < 6)
        {
            naxpy = length_of_signal - 2 - var;
        }

        else
        {
            naxpy = 5;

        }

        as = -filtered_signal[var];

        for (int temp = 0; temp <= naxpy; temp++)
        {
            filtered_signal_tmp = (temp + var) + 1;
            filtered_signal[filtered_signal_tmp] += as * dv1[temp + 1];
        }

    }

    for (int var = 0; var < filtered_signal.size(); ++var)
    {
        filtered_signal_reversed[var] = filtered_signal[filtered_signal.size() - var - 1];
    }

    filtered_signal = ApplyFilt_bandpass_10_20_all(filtered_signal_reversed,data_counter_vector);

    for (int var = 0; var < filtered_signal.size(); ++var)
    {
        filtered_signal_reversed[var] = filtered_signal[filtered_signal.size() - var -1];
    }

    // Take only plotting data for filter
    for (int var = 0; var < data_counter_vector.size()  ; ++var)
    {
       filtered_signal_last[var] =  filtered_signal_reversed[filtered_signal_reversed.size() + var - data_counter_vector.size()];
    }

    return filtered_signal_last;
}

void Open_file::CheckDataLength_peaks(QVector <double> &data_to_filter, int &counter_for_length)
{
    int size_of_filter_data = data_to_filter.size();
    if(size_of_filter_data > (3 * Fs))
    {
        for (int var = 0; var < size_of_filter_data - (3 * Fs); ++var)
        {
           data_to_filter.removeFirst();
           counter_for_length++;
        }
    }

}

int Open_file::Check_Peaks(QVector <double> data_to_peaks, QVector <double> &R_peaks_location, int &bpm, int counter_for_length)
{
    int location_of_peak;
    location_of_peak = Give_me_my_peaks(data_to_peaks, counter_for_length);

    if(location_of_peak - counter_for_length < 0)
    {
        ui->label_peak_not_found->setText("<font color='red'>No peaks found for a long time</font = 10>");

    }

    if( location_of_peak != 0 && (R_peaks_location.isEmpty() || (location_of_peak != R_peaks_location.last()) && (abs(location_of_peak - R_peaks_location.last()) > (Fs / 2.5))) )
    {

       R_peaks_location.append(location_of_peak);
       ui->customplot_2->graph(1)->addData(R_peaks_location.last(),data_to_peaks[R_peaks_location.last() - counter_for_length]);

       Give_bpm(R_peaks_location, bpm);
       ui->bpm->display(bpm);
       ui->label_peak_not_found->clear();

       return R_peaks_location.last();
    }

}

void Open_file::Give_bpm (QVector <double> R_peaks_location, int &bpm)
{
    int size_of_RR_interval = R_peaks_location.size();

    if ( size_of_RR_interval > 1)
    {
        double interval = ( R_peaks_location[size_of_RR_interval-1] - R_peaks_location[size_of_RR_interval - 2] ) / Fs;
        bpm = 60 / interval;
    }

    if( size_of_RR_interval > 5)
    {
       double temp = R_peaks_location.last();
       R_peaks_location.clear();
       R_peaks_location.append(temp);
    }
}

int Open_file::Give_me_my_peaks(QVector <double> data_to_peaks, int counter_for_length)
{
    const int length = data_to_peaks.size();

    int threshold = 6;
    QVector <double> peaks_found;
    QVector <double> possible_peaks;
    int result;

    for (int var = 1; var < length - 1; ++var)
    {
        if( data_to_peaks[var] >= data_to_peaks[var + 1]  && data_to_peaks[var] > threshold  && data_to_peaks[var - 1] <= data_to_peaks[var])
            possible_peaks.append(var);
    }

    double window_width = 0.5 * Fs;
    peaks_found = possible_peaks;
    int peaks_length = possible_peaks.size();

    int temp = 0;
    int abbrv;
    for (int var = 0; var < peaks_length - 1; ++var)
    {
        abbrv = var - temp;
        if (peaks_found[abbrv + 1] - peaks_found[abbrv] <= window_width && data_to_peaks[peaks_found[abbrv + 1]] >= data_to_peaks[peaks_found[abbrv]]  && data_to_peaks[peaks_found[abbrv + 1] > threshold])
        {
            threshold = data_to_peaks[peaks_found[abbrv + 1]];
            peaks_found.remove(abbrv);
            temp++;
        }
        else if (peaks_found[abbrv + 1] - peaks_found[abbrv] <= window_width && data_to_peaks[peaks_found[abbrv + 1]] <= data_to_peaks[peaks_found[abbrv]]  && data_to_peaks[peaks_found[abbrv] > threshold])
        {
            threshold = data_to_peaks[peaks_found[abbrv]];
            peaks_found.remove(abbrv + 1);
            temp++;
        }

    }


    if (peaks_found.isEmpty())
    {
        return 0;
    }
    else
    {
       result = peaks_found.last() + counter_for_length;
    }

    return result;

}

void Open_file::on_Read_clicked()
{      
    Sampling_Rate = ui->sample_rate->text();

    QFile file(File_name);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "file cannot open";
    }

    QStringList wordList;
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        wordList.append(line.split(',').first());
    }
    QVector <QString> data;
    QVector <double> data_double;
    data = wordList.toVector();

    for (int var = 0; var < data.size(); ++var)
    {
        all_data.append(data[var].toDouble());
    }

    setupProgram();
}

void Open_file::CheckDataLength_filter(QVector <double> &data_to_filter)
{
    int size_of_filter_data = data_to_filter.size();
    if(size_of_filter_data > (Fs * 2))
    {
        for (int var = 0; var < size_of_filter_data - (Fs * 2); ++var)
        {
           data_to_filter.removeFirst();
        }
    }
}

void Open_file::CheckDataLength_motion(QVector <double> &data_to_motion)
{
    int size_of_motion_data = data_to_motion.size();
    if(size_of_motion_data > (Fs / 2.5))
    {
        for (int var = 0; var < size_of_motion_data - (Fs / 2.5); ++var)
        {
           data_to_motion.removeFirst();
        }
    }
}


double Open_file::standard_deviation_calculation(QVector <double> signal)
{
    double length_of_signal = signal.size();

    double standard_deviation;
    double temp_1 = 0;
    double temp_2 = 0;

    for (int var = 0; var < length_of_signal; ++var)
    {
        temp_1 = temp_1 + signal[var];
    }

    double mean_of_signal = temp_1 / length_of_signal;

    for (int var = 0; var < length_of_signal; ++var)
    {
        temp_2 = temp_2 + (signal[var] - mean_of_signal) * (signal[var] - mean_of_signal);
    }

    standard_deviation = qSqrt(temp_2 / length_of_signal);

    return standard_deviation;

}

void Open_file::Motion_Artifact_Check(QVector <double> signal,int bpm)
{
    static int last_bpm = 0;
    static int artifact_counter = 0;
    int Motion_Artifact_Point = 0;
    double standard_deviation =  standard_deviation_calculation(signal);

    // Check the standard deviation and increment Artifact point by 3

    if( standard_deviation >= 31)
    {
        Motion_Artifact_Point += 3;
    }

    if(bpm > 120)
    {
        Motion_Artifact_Point +=2;
    }

    if( abs(last_bpm - bpm) > 50)
    {
        Motion_Artifact_Point++;
    }

    if(last_bpm != bpm)
    {
        last_bpm = bpm;
    }

    switch (Motion_Artifact_Point)
    {
    case 0: ui->label_color->setText("<font color='black'>Motion Artifact Point</font>");
        break;

    case 1:
        ui->label_color->setText("<font color='blue'>Motion Artifact Point</font>");
        break;

    case 2:
        ui->label_color->setText("<font color='green'>Motion Artifact Point</font>");
        break;

    case 3:
        ui->label_color->setText("<font color='purple'>Motion Artifact Point</font>");
        break;

    case 4:
        ui->label_color->setText("<font color='orange'>Motion Artifact Point</font>");
        break;

    case 5:
        ui->label_color->setText("<font color='red'>Motion Artifact Point</font>");
        break;
    }

    QVector <double> motion_artifact_point;
    QVector <double> motion_artifact_keys;

    for (int var = artifact_counter; var < counter_glob; ++var)
    {
        motion_artifact_point.append(Motion_Artifact_Point);
        motion_artifact_keys.append(var);
    }

    artifact_counter = counter_glob;

    ui->customplot_analysis->graph(1)->addData(motion_artifact_keys,motion_artifact_point);

    ui->lcdNumber_Artifact->display(Motion_Artifact_Point);

}

QVector <double> Open_file::Check_Which_Filter(QVector <double> keys)
{
    int current_row = ui->listWidget_filter->currentRow();
    QVector <double> filtered_signal;

    switch (current_row)
    {
    case -1:
        for (int var = 0; var < keys.size(); ++var)
        {
            filtered_signal.append(0);
        }
        break;

    case 0:
        filtered_signal = ApplyFilt_bandpass_1_35(data_to_filter,keys);
        break;

    case 1:
        filtered_signal = ApplyFilt_bandpass_07_40(data_to_filter,keys);
        break;

    case 2:
        filtered_signal = ApplyFilt_bandpass_5_25(data_to_filter,keys);
        break;

    case 3:
        filtered_signal = ApplyFilt_bandpass_filtfilt_10_20(data_to_filter,keys);
        break;

    case 4:
        ui->label_color->setText("<font color='orange'>Motion Artifact Point</font>");
        break;

    case 5:
        ui->label_color->setText("<font color='red'>Motion Artifact Point</font>");
        break;
    }

    return filtered_signal;

}

Open_file::~Open_file()
{
    delete ui;
}
