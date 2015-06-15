#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    numberOfRows = 1;
    QTableWidgetItem *newItem = new QTableWidgetItem("Activity");
    ui->activity_table->setItem(0, 0, newItem);

    QTableWidgetItem *newItem2 = new QTableWidgetItem("Duration");
    ui->activity_table->setItem(0, 1, newItem2);

    QTableWidgetItem *newItem3 = new QTableWidgetItem("Resource");
    ui->activity_table->setItem(0, 2, newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("Precedence");
    ui->activity_table->setItem(0, 3, newItem4);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_add_button_clicked()
{
    if( numberOfRows > 1 )
    {
        QString activity_name = ui->activity_table->item(numberOfRows-1,0)->text();
        int duration = ui->activity_table->item(numberOfRows-1,1)->text().toInt();
        int resources = ui->activity_table->item(numberOfRows-1,2)->text().toInt();
        QString precedence = ui->activity_table->item(numberOfRows-1,3)->text();
        qDebug()<<activity_name<<","<<duration<<","<<resources<<","<<precedence;
        Activity tmp(activity_name,duration,resources,precedence);
        activity_list.append(tmp);
    }
    ui->activity_table->insertRow(numberOfRows);
    numberOfRows++;
}

void MainWindow::on_calculate_table_clicked()
{
    QString activity_name = ui->activity_table->item(numberOfRows-1,0)->text();
    int duration = ui->activity_table->item(numberOfRows-1,1)->text().toInt();
    int resources = ui->activity_table->item(numberOfRows-1,2)->text().toInt();
    QString precedence = ui->activity_table->item(numberOfRows-1,3)->text();
    Activity tmp(activity_name,duration,resources,precedence);
    activity_list.append(tmp);

    QList<int> linked2Start = without_precedence();

    for(int i=0;i<linked2Start.size();i++)
    {
        Activity tmp = activity_list.takeAt(linked2Start.at(i));
        tmp.set_ES(0);
        activity_list.insert(linked2Start.at(i),tmp);
    }

    while ( !all_of_them_checked_forward() )
    {
        for(int i=0;i<activity_list.size();i++)
        {
            Activity tmp = activity_list.at(i);

            if( !tmp.forward_checked_value() && check_all_precedences(tmp.get_name()) )
            {
                tmp.set_ES(find_max_EF(tmp.get_name()));
                activity_list.replace(i,tmp);
            }
        }
    }

    for(int i=0;i<activity_list.size();i++)
    {
        Activity tmp = activity_list.at(i);
        qDebug()<<tmp.get_information();
    }
}

bool MainWindow::all_of_them_checked_forward()
{
    for(int i=0;i<activity_list.size();i++)
    {
        Activity tmp = activity_list.at(i);
        if( !tmp.forward_checked_value() )
            return false;
    }

    return true;
}

QList<int> MainWindow::without_precedence()
{
    QList<int> output;

    for(int i=0;i<activity_list.size();i++)
    {
        Activity tmp = activity_list.at(i);
        if( tmp.get_precedence() == "-" )
            output.append(i);
    }

    return output;
}

bool MainWindow::check_all_precedences(QString name)
{
    QList<int> precedence_ids = find_precedence(name);

    bool output = true;

    for(int i=0;i<precedence_ids.size();i++)
    {
        Activity tmp = activity_list.at(precedence_ids.at(i));
        if( !tmp.forward_checked_value() )
        {
            output = false;
            break;
        }
    }

    return output;
}

int MainWindow::find_max_EF(QString name)
{
    QList<int> precedence_ids = find_precedence(name);

    int output = -1;

    for(int i=0;i<precedence_ids.size();i++)
    {
        Activity tmp = activity_list.at(precedence_ids.at(i));
        if( tmp.get_EF() > output )
        {
            output = tmp.get_EF();
        }
    }

    return output;
}

QList<int> MainWindow::find_precedence(QString activity_name)
{
    QList<int> output;

    int mainIndex = find_by_name(activity_name);

    Activity tmp = activity_list.at(mainIndex);
    QString precedences = tmp.get_precedence();
    QStringList string_list = precedences.split(",");

    for(int i=0;i<string_list.size();i++)
    {
        int index = find_by_name(string_list.at(i));
        output.append(index);
    }

    return output;

}

int MainWindow::find_by_name(QString name)
{
    int mainIndex = -1;

    for(int i=0;i<activity_list.size();i++)
    {
        Activity tmp = activity_list.at(i);
        if( tmp.get_name() == name )
        {
            mainIndex = i;
            break;
        }
    }

    return mainIndex;
}

