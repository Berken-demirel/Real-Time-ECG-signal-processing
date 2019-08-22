#ifndef OPTIONS_FIRST_H
#define OPTIONS_FIRST_H

#include <QDialog>
#include "mainwindow.h"
#include "open_file.h"

namespace Ui {
class options_first;
}

class options_first : public QDialog
{
    Q_OBJECT

public:
    explicit options_first(QWidget *parent = nullptr);
    ~options_first();

private slots:
    void on_Real_time_clicked();

    void on_Read_from_file_clicked();

private:
    Ui::options_first *ui;

    MainWindow *sWindow;
    Open_file *tWindow;
};

#endif // OPTIONS_FIRST_H
