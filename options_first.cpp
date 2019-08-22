#include "options_first.h"
#include "ui_options_first.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "open_file.h"
#include "ui_open_file.h"

options_first::options_first(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::options_first)
{
    ui->setupUi(this);

    sWindow = new MainWindow();

    tWindow = new Open_file();

    QPixmap pix(":/resources/img/medit_logo.png");
    int w = ui->label_medit->width();
    int h = ui->label_medit->height();
    // set a scaled pixmap to a w x h window keeping its aspect ratio
    ui->label_medit->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
}

options_first::~options_first()
{
    delete ui;
}

void options_first::on_Real_time_clicked()
{
    sWindow->show();
    this->close();
}

void options_first::on_Read_from_file_clicked()
{
    tWindow->show();
    this->close();
}
