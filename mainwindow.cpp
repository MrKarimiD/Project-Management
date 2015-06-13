#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    numberOfRows = 1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_add_button_clicked()
{
    ui->activity_table->insertRow(numberOfRows);
    numberOfRows++;
}
