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

    QTableWidgetItem *newItem3 = new QTableWidgetItem("Resource1");
    ui->activity_table->setItem(0, 2, newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("Resource2");
    ui->activity_table->setItem(0, 3, newItem4);

    QTableWidgetItem *newItem5 = new QTableWidgetItem("Precedence");
    ui->activity_table->setItem(0, 4, newItem5);

    ui->start_dateEdit->setDate(QDate::currentDate());
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

    // Rsource Planning
    resource_planing();

    for(int i=0;i<activity_list.size();i++)
    {
        Activity tmp = activity_list.at(i);
        qDebug()<<tmp.get_information();
    }

    //Draw Gantt Chart
    draw_gannt_chart();


    QPixmap pix("gantt.png");
    QRect rect(0, 0, pix.width(), pix.height()-10);
    QPixmap cropped = pix.copy(rect);

    QPainter paint(&cropped);
    paint.setPen(QColor(0,0,0));
    paint.drawRect(0,0,cropped.width(), cropped.height());
    ui->gantt_label->setPixmap(cropped);

}

void MainWindow::resource_planing()
{
    int T=0;

    int finish_id = find_by_name("finish");
    if( finish_id == -1 )
    {
        qDebug()<<"There is a problem in resource planning!";
        return ;
    }

    Activity last_one = activity_list.at(finish_id);

    int max_num = last_one.get_LF()*2;
    int resource1_per_day[max_num];
    int resource2_per_day[max_num];

    for(int i=0;i<max_num;i++)
    {
        resource1_per_day[i] = ui->maxRes1_lineedit->text().toInt();
        resource2_per_day[i] = ui->maxRes2_lineedit->text().toInt();
    }

    while ( !all_of_them_checked_resource() )
    {
        QList<int> EAS = ready_for_resource_planing(T);
        QList<int> OSS = sort_activity_based_on_LS(EAS);

        for(int i=0;i<OSS.size();i++)
        {
            Activity tmp = activity_list.at(OSS.at(i));
            if( (tmp.get_resource1() <= resource1_per_day[T]) &&
                    (tmp.get_resource2() <= resource2_per_day[T]) )
            {
                tmp.set_rpStart(T);
                activity_list.replace(OSS.at(i), tmp);
                for( int j=0;j<tmp.get_duration();j++)
                {
                    resource1_per_day[T+j] = resource1_per_day[T+j] - tmp.get_resource1();
                    resource2_per_day[T+j] = resource2_per_day[T+j] - tmp.get_resource2();
                }
            }
        }

        T++;
    }
}

QList<int> MainWindow::ready_for_resource_planing(int today)
{
    QList<int> output;

    for(int i=0;i<activity_list.size();i++)
    {
        Activity tmp = activity_list.at(i);
        if( !tmp.resource_checked_value() && check_all_precedences_resource(tmp.get_name(),today) )
            output.append(i);
    }

    return output;
}

QList<int> MainWindow::sort_activity_based_on_LS(QList<int> input)
{
    QList<int> output = input;

    for(int i=0;i<output.size();i++)
    {
        for(int j=i+1;j<output.size();j++)
        {
            Activity first = activity_list.at(output.at(i));
            Activity second = activity_list.at(output.at(j));
            if( first.get_LS() > second.get_LS() )
            {
                int tmp = output.at(i);
                output.replace(i,output.at(j));
                output.replace(j,tmp);
            }
            else if( first.get_LS() == second.get_LS() )
            {
                if( first.get_duration() > second.get_duration() )
                {
                    int tmp = output.at(i);
                    output.replace(i,output.at(j));
                    output.replace(j,tmp);
                }
            }
        }
    }

    return output;
}

bool MainWindow::draw_gannt_chart()
{
    // data for the gantt chart, representing the start date, end date and names for various
    // activities
    double startDate[activity_list.size()-1];
    double endDate[activity_list.size()-1];
    const char *labels[activity_list.size()-1];

    for(int i=0;i<activity_list.size()-1;i++)
    {
        Activity tmp = activity_list.at(i);
        QDate start = ui->start_dateEdit->date();
        start = start.addDays(tmp.get_rp_start());
        QDate finish = ui->start_dateEdit->date();
        finish = finish.addDays(tmp.get_rp_finish());

        char* p = new char[tmp.get_name().length() + 1];
        strcpy(p, tmp.get_name().toLatin1().constData());
        labels[i] = p ;

        startDate[i] = Chart::chartTime(start.year(), start.month(), start.day());
        endDate[i] = Chart::chartTime(finish.year(), finish.month(), finish.day());
    }

    // Create a XYChart object of size 620 x 280 pixels. Set background color to light blue
    // (ccccff), with 1 pixel 3D border effect.
    XYChart *c = new XYChart(620, 280, 0xccccff, 0x000000, 1);

    // Add a title to the chart using 15 points Times Bold Itatic font, with white (ffffff) text on
    // a deep blue (000080) background
    c->addTitle("Gantt Chart", "timesbi.ttf", 15, 0xffffff)->setBackground(0x000080);

    // Set the plotarea at (140, 55) and of size 460 x 200 pixels. Use alternative white/grey
    // background. Enable both horizontal and vertical grids by setting their colors to grey
    // (c0c0c0). Set vertical major grid (represents month boundaries) 2 pixels in width
    c->setPlotArea(140, 55, 460, 200, 0xffffff, 0xeeeeee, Chart::LineColor, 0xc0c0c0, 0xc0c0c0
                   )->setGridWidth(2, 1, 1, 1);

    // swap the x and y axes to create a horziontal box-whisker chart
    c->swapXY();

    // Set the y-axis scale to be date scale from Aug 16, 2004 to Nov 22, 2004, with ticks every 7
    // days (1 week)
    int finish_id = find_by_name("finish");
    Activity last_one = activity_list.at(finish_id);
    QDate input_date = ui->start_dateEdit->date();
    QDate end_date = input_date;
    end_date = end_date.addDays(last_one.get_rp_finish());
    c->yAxis()->setDateScale(Chart::chartTime(input_date.year(), input_date.month(), input_date.day()),
                             Chart::chartTime(end_date.year(), end_date.month(), end_date.day()),
                             86400 * 1);

    // Set multi-style axis label formatting. Month labels are in Arial Bold font in "mmm d" format.
    // Weekly labels just show the day of month and use minor tick (by using '-' as first character
    // of format string).
    c->yAxis()->setMultiFormat(Chart::StartOfMonthFilter(), "<*font=arialbd.ttf*>{value|mmm d}",
                               Chart::StartOfDayFilter(), "-{value|d}");

    // Set the y-axis to shown on the top (right + swapXY = top)
    c->setYAxisOnRight();

    // Set the labels on the x axis
    c->xAxis()->setLabels(StringArray(labels, (int)(sizeof(labels) / sizeof(labels[0]))));

    // Reverse the x-axis scale so that it points downwards.
    c->xAxis()->setReverse();

    // Set the horizontal ticks and grid lines to be between the bars
    c->xAxis()->setTickOffset(0.5);

    // Add a green (33ff33) box-whisker layer showing the box only.
    c->addBoxWhiskerLayer(DoubleArray(startDate, (int)(sizeof(startDate) / sizeof(startDate[0]))),
            DoubleArray(endDate, (int)(sizeof(endDate) / sizeof(endDate[0]))), DoubleArray(),
            DoubleArray(), DoubleArray(), 0x00cc00, Chart::SameAsMainColor, Chart::SameAsMainColor);

    // Output the chart
    c->makeChart("gantt.png");
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

bool MainWindow::all_of_them_checked_resource()
{
    for(int i=0;i<activity_list.size();i++)
    {
        Activity tmp = activity_list.at(i);
        if( !tmp.resource_checked_value() )
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

bool MainWindow::check_all_precedences_resource(QString name, int today)
{
    QList<int> precedence_ids = find_precedence(name);

    bool output = true;

    for(int i=0;i<precedence_ids.size();i++)
    {
        Activity tmp = activity_list.at(precedence_ids.at(i));
        if( !tmp.resource_checked_value() || today<tmp.get_rp_finish() )
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
    int resources1 = ui->activity_table->item(numberOfRows-1,2)->text().toInt();
    int resources2 = ui->activity_table->item(numberOfRows-1,3)->text().toInt();
    QString precedence = ui->activity_table->item(numberOfRows-1,4)->text();
    qDebug()<<activity_name<<","<<duration<<","<<resources1<<","<<resources2<<","<<precedence;

    QStringList str_list = precedence.split(",");
    for(int i=0;i<str_list.size();i++)
    {
        add_to_successors(str_list.at(i),activity_name);
    }
    Activity tmp(activity_name,duration,resources1,resources2,precedence);
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
        if( index != -1 )
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

