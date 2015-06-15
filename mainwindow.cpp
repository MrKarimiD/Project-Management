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
        get_one_row_from_table();
    }
    ui->activity_table->insertRow(numberOfRows);
    numberOfRows++;
}

void MainWindow::on_calculate_table_clicked()
{
    get_one_row_from_table();

    // Early Time Scheduling
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

    //Late Time Scheduling
    int finish_id = find_by_name("finish");
    if( finish_id == -1 )
    {
        qDebug()<<"There is a problem here!";
        return ;
    }

    Activity last_one = activity_list.at(finish_id);
    last_one.set_LF(last_one.get_EF());
    activity_list.replace(finish_id,last_one);

    QList<int> linked2finish = linked_to_finish();

    for(int i=0;i<linked2finish.size();i++)
    {
        Activity tmp = activity_list.at(linked2finish.at(i));
        tmp.set_LF(last_one.get_LS());
        activity_list.replace(linked2finish.at(i),tmp);
    }

    while ( !all_of_them_checked_backward() )
    {
        for(int i=0;i<activity_list.size();i++)
        {
            Activity tmp = activity_list.at(i);

            if( !tmp.backward_checked_value() && check_all_successors(tmp.get_name()) )
            {
                tmp.set_LF(find_min_LS(tmp.get_name()));
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

bool MainWindow::all_of_them_checked_backward()
{
    for(int i=0;i<activity_list.size();i++)
    {
        Activity tmp = activity_list.at(i);
        if( !tmp.backward_checked_value() )
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

QList<int> MainWindow::linked_to_finish()
{
    QList<int> output;

    for(int i=0;i<activity_list.size();i++)
    {
        Activity tmp = activity_list.at(i);
        if( tmp.get_successors() == "finish" )
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

bool MainWindow::check_all_successors(QString name)
{
    QList<int> successors_ids = find_successors(name);

    bool output = true;

    for(int i=0;i<successors_ids.size();i++)
    {
        Activity tmp = activity_list.at(successors_ids.at(i));
        if( !tmp.backward_checked_value() )
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

int MainWindow::find_min_LS(QString name)
{
    QList<int> successors_ids = find_successors(name);

    int output = 300000;

    for(int i=0;i<successors_ids.size();i++)
    {
        Activity tmp = activity_list.at(successors_ids.at(i));
        if( tmp.get_LS() < output )
        {
            output = tmp.get_LS();
        }
    }

    return output;
}

void MainWindow::get_one_row_from_table()
{
    QString activity_name = ui->activity_table->item(numberOfRows-1,0)->text();
    int duration = ui->activity_table->item(numberOfRows-1,1)->text().toInt();
    int resources = ui->activity_table->item(numberOfRows-1,2)->text().toInt();
    QString precedence = ui->activity_table->item(numberOfRows-1,3)->text();
    qDebug()<<activity_name<<","<<duration<<","<<resources<<","<<precedence;

    QStringList str_list = precedence.split(",");
    for(int i=0;i<str_list.size();i++)
    {
        add_to_successors(str_list.at(i),activity_name);
    }
    Activity tmp(activity_name,duration,resources,precedence);
    activity_list.append(tmp);
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

QList<int> MainWindow::find_successors(QString activity_name)
{
    QList<int> output;

    int mainIndex = find_by_name(activity_name);

    Activity tmp = activity_list.at(mainIndex);
    QString successors = tmp.get_successors();
    QStringList string_list = successors.split(",");

    for(int i=0;i<string_list.size();i++)
    {
        int index = find_by_name(string_list.at(i));
        output.append(index);
    }

    return output;
}

void MainWindow::add_to_successors(QString activity, QString succesor)
{
    int mainIndex = find_by_name(activity);
    if( mainIndex != -1 )
    {
        Activity tmp = activity_list.at(mainIndex);
        tmp.append_successor(succesor);
        activity_list.replace(mainIndex,tmp);
    }
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

