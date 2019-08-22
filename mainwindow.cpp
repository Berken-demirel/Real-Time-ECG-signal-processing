#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

static QString File_name;

static int Fs = 329;
static int counter;
static quint8 temp = 0;
static QVector <double> data_to_filter;
static QVector <double> average_signal;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_serial(new QSerialPort(this))
{
    ui->setupUi(this);

    QPixmap pix(":/resources/img/medit_logo.png");
    int w = ui->label_medit->width();
    int h = ui->label_medit->height();
    // set a scaled pixmap to a w x h window keeping its aspect ratio
    ui->label_medit->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

    m_serial->setPortName("COM7");
    m_serial->setBaudRate(QSerialPort::Baud115200);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->open(QIODevice::ReadOnly);
    setupProgram();
}

void MainWindow::setupProgram()
{
    setupRealtime(ui->customplot,ui->customplot_2,ui->customplot_average,ui->customplot_analysis,ui->customplot_square,ui->customplot_moving);
    ui->customplot->replot();
    ui->customplot_2->replot();
}

void MainWindow::setupRealtime(QCustomPlot *customplot, QCustomPlot *customplot_2, QCustomPlot *customplot_average, QCustomPlot *customplot_analysis, QCustomPlot *customplot_square, QCustomPlot *customplot_moving)
{

  // include this section to fully disable antialiasing for higher performance:

//  customplot->setNotAntialiasedElements(QCP::aeAll);
//  QFont font;
//  font.setStyleStrategy(QFont::NoAntialias);
//  customplot->xAxis->setTickLabelFont(font);
//  customplot->yAxis->setTickLabelFont(font);
//  customplot->legend->setFont(font);

  // Title and its position //
  QCPItemText *textLabel = new QCPItemText(customplot);
  textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
  textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
  textLabel->position->setCoords(0.5, 0);
  textLabel->setText("Raw Data");

  customplot->addGraph(); // blue line
  customplot->graph(0)->setPen(QPen(QColor(40, 110, 255)));


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

  QCPItemText *textLabel_4 = new QCPItemText(customplot_analysis);
  textLabel_4->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
  textLabel_4->position->setType(QCPItemPosition::ptAxisRectRatio);
  textLabel_4->position->setCoords(0.5, 0);
  textLabel_4->setText("Analysis window");

  customplot_analysis->addGraph();
  customplot_analysis->graph(0)->setPen(QPen(QColor(40, 110, 255)));

  customplot_analysis->addGraph(customplot_analysis->xAxis2 ,customplot_analysis->yAxis2);
  customplot_analysis->graph(1)->setPen(QColor(255, 50, 50));
  //customplot_analysis->graph(1)->setLineStyle(QCPGraph::lsLine);
  //customplot_analysis->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));


  QCPItemText *textLabel_5 = new QCPItemText(customplot_square);
  textLabel_5->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
  textLabel_5->position->setType(QCPItemPosition::ptAxisRectRatio);
  textLabel_5->position->setCoords(0.5, 0);
  textLabel_5->setText("Squaring signal");

  customplot_square->addGraph();
  customplot_square->graph(0)->setPen(QPen(QColor(40, 110, 255)));

  QCPItemText *textLabel_6 = new QCPItemText(customplot_moving);
  textLabel_6->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
  textLabel_6->position->setType(QCPItemPosition::ptAxisRectRatio);
  textLabel_6->position->setCoords(0.5, 0);
  textLabel_6->setText("Moving Average Filter");

  customplot_moving->addGraph(); // blue line
  customplot_moving->graph(0)->setPen(QPen(QColor(40, 110, 255)));


  QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
  QSharedPointer<QCPAxisTicker> count(new QCPAxisTicker);
  //count->setTickOrigin(counter_for_all);  may be used
  timeTicker->setTimeFormat("%h:%m:%s");
  customplot->xAxis->setTicker(timeTicker);
  customplot->xAxis->setTicker(count);
  customplot->axisRect()->setupFullAxesBox();
  customplot->yAxis->setRange(0, 50);

  customplot_2->axisRect()->setupFullAxesBox();
  customplot_2->yAxis->setRange(-100, 100);

  customplot_average->axisRect()->setupFullAxesBox();
  customplot_average->yAxis->setRange(-100, 100);

  customplot_analysis->axisRect()->setupFullAxesBox();
  customplot_analysis->yAxis->setRange(-100, 100);
  customplot_analysis->yAxis2->setRange(0,6);
  customplot_analysis->xAxis2->setVisible(true);
  customplot_analysis->yAxis2->setVisible(true);
  customplot_analysis->yAxis2->setTickLabels(true);
  customplot_analysis->yAxis2->setLabel(" Motion Artifact point");



  customplot_square->axisRect()->setupFullAxesBox();
  customplot_square->yAxis->setRange(-100, 100);

  customplot_moving->axisRect()->setupFullAxesBox();
  customplot_moving->yAxis->setRange(0, 400);


  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customplot->xAxis, SIGNAL(rangeChanged(QCPRange)), customplot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customplot->yAxis, SIGNAL(rangeChanged(QCPRange)), customplot->yAxis2, SLOT(setRange(QCPRange)));

  connect(customplot_2->xAxis, SIGNAL(rangeChanged(QCPRange)), customplot_2->xAxis2, SLOT(setRange(QCPRange)));
  connect(customplot_2->yAxis, SIGNAL(rangeChanged(QCPRange)), customplot_2->yAxis2, SLOT(setRange(QCPRange)));

  connect(customplot_analysis->xAxis, SIGNAL(rangeChanged(QCPRange)), customplot_analysis->xAxis, SLOT(setRange(QCPRange)));
  connect(customplot_analysis->yAxis, SIGNAL(rangeChanged(QCPRange)), customplot_analysis->yAxis, SLOT(setRange(QCPRange)));

  connect(customplot_analysis->xAxis2, SIGNAL(rangeChanged(QCPRange)), customplot_analysis->xAxis2, SLOT(setRange(QCPRange)));
  connect(customplot_analysis->yAxis2, SIGNAL(rangeChanged(QCPRange)), customplot_analysis->yAxis2, SLOT(setRange(QCPRange)));

  connect(customplot_square->xAxis, SIGNAL(rangeChanged(QCPRange)), customplot_square->xAxis2, SLOT(setRange(QCPRange)));
  connect(customplot_square->yAxis, SIGNAL(rangeChanged(QCPRange)), customplot_square->yAxis2, SLOT(setRange(QCPRange)));

  connect(customplot_moving->xAxis, SIGNAL(rangeChanged(QCPRange)), customplot_moving->xAxis2, SLOT(setRange(QCPRange)));
  connect(customplot_moving->yAxis, SIGNAL(rangeChanged(QCPRange)), customplot_moving->yAxis2, SLOT(setRange(QCPRange)));


  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  //connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::realtimeDataSlot);

}

void MainWindow::realtimeDataSlot()
{
    //Static
    static QTime time(QTime::currentTime());
    static QVector <double> data_to_peaks;
    static QVector <double> R_peaks_location;
    static QVector <double> data_to_moving_filter;
    static QVector <double> data_to_motion_artifact;
    static int internal_counter = 0;
    static int bpm = 0;
    static int counter_for_check_length = 0;
    // Doubles
    double key = time.elapsed() / 1000; // time elapsed since start of demo, in seconds
    double key_try = time.elapsed();
    double check = time.elapsed() - internal_counter * 100;
    //Vectors
    QVector <double> data_to_plot;
    QVector <double> data_counter_vector;
    QVector <double> filtered_signal;
    QVector <double> squared_signal;
    QVector <double> moving_signal;
    QVector <double> bandpass_40;
    QVector <double> filtfilt_signal;
    //Integers
    int peak_check = 0;
    // Read the data
    data_to_plot = readData(data_counter_vector,temp);
    // Write the data
    if(ui->checkBox_raw->isChecked())
    {
     Write_Data(data_to_plot);
    }
    // add data
    ui->customplot->graph(0)->addData(data_counter_vector,data_to_plot,true);
    //ui->customplot->graph(0)->data()->removeBefore(data_counter_vector.first() - 1200);

    data_to_filter.append(data_to_plot);
    //average_signal.append(data_to_plot);

    data_to_motion_artifact.append(data_to_plot);

    CheckDataLength_motion(data_to_motion_artifact);
    CheckDataLength_filter(data_to_filter);
    CheckDataLength_peaks(data_to_peaks, counter_for_check_length);
    CheckDataLength_filter(data_to_moving_filter);

    ui->customplot_analysis->graph(0)->addData(data_counter_vector,data_to_plot,true);

    bandpass_40 = ApplyFilt_bandpass_2(data_to_filter,data_counter_vector);

    average_signal.append(bandpass_40);

    //filtfilt_signal = ApplyFilt_bandpass_filtfilt(data_to_filter,data_counter_vector);

    filtered_signal = ApplyFilt_bandpass_1(data_to_filter,data_counter_vector);

    data_to_peaks.append(filtered_signal);

    squared_signal = Take_Square(filtered_signal);

    data_to_moving_filter.append(squared_signal);

    ui->customplot_square->graph(0)->addData(data_counter_vector,squared_signal,true);

    moving_signal = ApplyFilt_Moving_Average(data_to_moving_filter,data_counter_vector);

    ui->customplot_moving->graph(0)->addData(data_counter_vector,moving_signal,true);

    // Check every 200 ms to data whether new peak has came
    if(check > 100)
    {
        peak_check = Check_Peaks(data_to_peaks, R_peaks_location, bpm,counter_for_check_length);
        Motion_Artifact_Check(data_to_motion_artifact, bpm);
        internal_counter++;
    }
    // Take average shape of last 4 peaks
    if( peak_check != 0 && peak_check !=1)
    {
        Average_Shape(peak_check);
    }


    // Add data
    ui->customplot_2->graph(0)->addData(data_counter_vector,filtered_signal,true);

    //rescale value (vertical) axis to fit the current data:
    ui->customplot->graph(0)->rescaleValueAxis(false,true);
    ui->customplot_2->graph(0)->rescaleValueAxis(false,true);
    ui->customplot_average->graph(0)->rescaleValueAxis();
    ui->customplot_analysis->graph(0)->rescaleValueAxis(false,true);
    ui->customplot_analysis->graph(1)->rescaleKeyAxis();
    ui->customplot_square->graph(0)->rescaleValueAxis(false,true);
    ui->customplot_moving->graph(0)->rescaleValueAxis(false,true);
    //ui->customplot->graph(1)->rescaleValueAxis();


    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customplot->xAxis->setRange(data_counter_vector.last(),1200, Qt::AlignRight);
    ui->customplot_2->xAxis->setRange(data_counter_vector.last(),1200, Qt::AlignRight);
    ui->customplot_square->xAxis->setRange(data_counter_vector.last(),1200, Qt::AlignRight);
    ui->customplot_analysis->xAxis->setRange(data_counter_vector.last(),1500, Qt::AlignRight);
    ui->customplot_analysis->xAxis2->setRange(data_counter_vector.last(),1500, Qt::AlignRight);
    ui->customplot_moving->xAxis->setRange(data_counter_vector.last(),1200, Qt::AlignRight);
    ui->customplot_average->xAxis->setRangeLower(0);
    ui->customplot_average->xAxis->setRangeUpper(400);

//    ui->customplot->replot();
//    ui->customplot_2->replot();
//    ui->customplot_average->replot();
//    ui->customplot_square->replot();
//    ui->customplot_analysis->replot();
//    ui->customplot_moving->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount = 0;
    //++frameCount;
    if ((frameCount/( (key_try-lastFpsKey) / 1000) < 25)) // average fps over 2 seconds
    {
      ui->statusBar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/( (key_try-lastFpsKey) / 1000), 0, 'f', 0)
            .arg(ui->customplot->graph(0)->data()->size())
            , 0);
      lastFpsKey = key_try;
      frameCount = 0;

      ui->customplot->replot();
      ui->customplot_2->replot();
      ui->customplot_average->replot();
      ui->customplot_square->replot();
      ui->customplot_analysis->replot();
      ui->customplot_moving->replot();
      ++frameCount;

    }
}

void MainWindow::CheckDataLength_peaks(QVector <double> &data_to_filter, int &counter_for_length)
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

void MainWindow::CheckDataLength_filter(QVector <double> &data_to_filter)
{
    int size_of_filter_data = data_to_filter.size();
    if(size_of_filter_data > (2 * Fs))
    {
        for (int var = 0; var < size_of_filter_data - (2 * Fs); ++var)
        {
           data_to_filter.removeFirst();
        }
    }
}

void MainWindow::CheckDataLength_motion(QVector <double> &data_to_motion)
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

QVector <double> MainWindow::readData(QVector <double> &data_counter_vector,quint8 &temp)
{
    QVector <quint8> first_pack;
    QVector <quint8> second_pack;
    quint8 check;
    QVector <double> ADCVal;
    QByteArray check_byte;
    QByteArray data;

    check_byte = m_serial->read(1);
    check = check_byte[0];

    if( check >= 128)
    {
        first_pack.append(check);
        check_byte = m_serial->read(1);
        check = check_byte[0];
        second_pack.append(check);
    }
    else
    {
        second_pack.append(check);
        if (temp != 0)
        {
            first_pack.append(temp);
        }


    }

    //qDebug() << "Bytes available before readall" << m_serial->bytesAvailable();
    data = m_serial->readAll();
    //qDebug() << "Bytes available after readall" << m_serial->bytesAvailable();
    if( data.size() % 2)
    {
        temp = data[data.size() -1];

        for (int var = 0; var < data.size() - 1; ++var)
        {
            first_pack.append(data[var]);
            ++var;
            second_pack.append(data[var]);

        }
        for (int var = 0; var < first_pack.size(); ++var)
        {
          counter++;
          ADCVal.append(128 * (first_pack[var] - 128) + second_pack[var]);
          data_counter_vector.append(counter);
        }

    }
    else
    {
        for (int var = 0; var < data.size(); ++var)
        {
            first_pack.append(data[var]);
            ++var;
            second_pack.append(data[var]);

        }

        for (int var = 0; var < first_pack.size(); ++var)
        {
          counter++;
          ADCVal.append(128 * (first_pack[var] - 128) + second_pack[var]);
          data_counter_vector.append(counter);
        }
    }


    //qDebug() << "Final value "<< (ADCVal);


    return ADCVal;


}

QVector <double> MainWindow::ApplyFilt_bandpass_1(QVector <double> data_to_filter,QVector <double> data_counter_vector)
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

QVector <double> MainWindow::ApplyFilt_bandpass_1_all(QVector <double> data_to_filter,QVector <double> data_counter_vector)
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

QVector <double> MainWindow::ApplyFilt_bandpass_filtfilt(QVector <double> data_to_filter,QVector <double> data_counter_vector)
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
    qDebug() << data_to_filter;
    for (int var = 0; var < filtered_signal.size(); ++var)
    {
        filtered_signal_reversed[var] = filtered_signal[filtered_signal.size() - var - 1];
    }

    filtered_signal = ApplyFilt_bandpass_1_all(filtered_signal_reversed,data_counter_vector);

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

QVector <double> MainWindow::Take_Square(QVector <double> data_to_square)
{
    int size_of_signal = data_to_square.size();
    QVector <double> squared_signal(size_of_signal);

    for (int var = 0; var < size_of_signal; ++var)
    {
        squared_signal[var] = data_to_square[var] * data_to_square[var];

    }

    return squared_signal;
}

QVector <double> MainWindow::ApplyFilt_Moving_Average(QVector <double> data_to_moving,QVector <double> data_counter_vector)
{
    int window_width = (Fs/9);
    double coeff = (1.0/window_width);
    int length_of_signal = data_to_moving.size();

    QVector <double> filtered_signal(length_of_signal);
    QVector <double> filtered_signal_last(data_counter_vector.size());


    int temp;
    for (int var = 0; var < window_width; ++var)
    {
        temp = 1 + var;

        for (int dump = temp; dump < length_of_signal + 1; ++dump)
        {
            filtered_signal[dump - 1] += (coeff * data_to_moving[(dump - var) - 1]);

        }
    }

    for (int var = 0; var < data_counter_vector.size()  ; ++var)
    {
       filtered_signal_last[var] =  filtered_signal[filtered_signal.size() + var - data_counter_vector.size()] * coeff;
    }

    return filtered_signal_last;
}

QVector <double> MainWindow::ApplyFilt_bandpass_2(QVector <double> data_to_filter, QVector <double> data_counter_vector)
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

QVector <double> MainWindow::ApplyFilt_bandpass_40(QVector<double> data_to_filter, QVector<double> data_counter_vector)
{
    int length_of_signal = data_to_filter.size();
    int naxpy;
    int filtered_signal_tmp;
    QVector <double> filtered_signal(length_of_signal);
    QVector <double> filtered_signal_last(data_counter_vector.size());

    static const double dv0[7] = { 0.028311406105033091, -0.0,
      -0.084934218315099258, -0.0, 0.084934218315099258, -0.0,
      -0.028311406105033091 };

    double as;

    static const double dv1[7] = { 1.0, -4.5012338735568731, 8.4670715663573777,
      -8.6010205491081084, 5.0194996412232413, -1.5979691045724465,
      0.21365288874600138 };

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

QVector <double> MainWindow::ApplyFilt_bandpass_40_fir1(QVector <double> data_to_filter, QVector <double> data_counter_vector)
{
    int length_of_signal = data_to_filter.size();
    int naxpy;
    int filtered_signal_tmp;
    QVector <double> filtered_signal(length_of_signal);
    QVector <double> filtered_signal_last(data_counter_vector.size());


    static const double dv0[7] = { 0.027566735688078031, -0.0,
      -0.082700207064234088, -0.0, 0.082700207064234088, -0.0,
      -0.027566735688078031 };

    double as;
    static const double dv1[7] = { 1.0, -4.5062414344080466, 8.4940315633696439,
      -8.6528929083839987, 5.0662540425823206, -1.6185757415562798,
      0.21742611472301365 };

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

int MainWindow::Give_me_my_peaks(QVector <double> data_to_peaks, int counter_for_length)
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

void MainWindow::Average_Shape(int peak_location)
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

void MainWindow::Get_Parameters(QVector <double> signal)
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

        if(ui->checkBox_QRS->isChecked())
        {
            Write_QRS(QRS_duration);
        }

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

        if(ui->checkBox_QT->isChecked())
        {
            Write_QT(QT_duration);
        }

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

        if(ui->checkBox_PR->isChecked())
        {
            Write_PR(PR_duration);
        }

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

void MainWindow::Give_bpm (QVector <double> R_peaks_location, int &bpm)
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

int MainWindow::Check_Peaks(QVector <double> data_to_peaks, QVector <double> &R_peaks_location, int &bpm, int counter_for_length)
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

void MainWindow::Motion_Artifact_Check(QVector <double> signal,int bpm)
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
        ui->bpm->setPalette(Qt::red);
    }
    else
    {
        ui->bpm->setPalette(Qt::white);
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

    for (int var = artifact_counter; var < counter; ++var)
    {
        motion_artifact_point.append(Motion_Artifact_Point);
        motion_artifact_keys.append(var);
    }

    artifact_counter = counter;

    ui->customplot_analysis->graph(1)->addData(motion_artifact_keys,motion_artifact_point);

    ui->lcdNumber_Artifact->display(Motion_Artifact_Point);

}

double MainWindow::standard_deviation_calculation(QVector <double> signal)
{
    int length_of_signal = signal.size();

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

void MainWindow::Write_Data(QVector <double> data_to_write)
{

QFile data(File_name + "/Raw_data.csv");


if (data.open(QFile::WriteOnly | QIODevice::Append))
{
 QTextStream stream(&data);

 for (int var = 0; var < data_to_write.size(); ++var)
 {
      stream << data_to_write[var] << endl;
 }

}
data.close();

}

void MainWindow::Write_QRS(int data_to_write)
{

QFile QRS(File_name + "/QRS_duration.csv");


if (QRS.open(QFile::WriteOnly | QIODevice::Append))
{
 QTextStream stream(&QRS);

 stream << data_to_write << endl;

}
QRS.close();

}

void MainWindow::Write_QT(int data_to_write)
{

QFile QT(File_name + "/QT_duration.csv");


if (QT.open(QFile::WriteOnly | QIODevice::Append))
{
 QTextStream stream(&QT);

 stream << data_to_write << endl;

}
QT.close();

}

void MainWindow::Write_PR(int data_to_write)
{

QFile PR(File_name + "/PR_duration.csv");


if (PR.open(QFile::WriteOnly | QIODevice::Append))
{
 QTextStream stream(&PR);

 stream << data_to_write << endl;

}
PR.close();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Saving_location_clicked()
{
    QString file_name = QFileDialog::getExistingDirectory(this);
    File_name = file_name;
}
