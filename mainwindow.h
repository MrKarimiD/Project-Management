#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "activity.h"
#include "QDebug"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_add_button_clicked();

    void on_calculate_table_clicked();

private:
    Ui::MainWindow *ui;
    int numberOfRows;
    QList<Activity> activity_list;

    void resource_planing();
    QList<int> ready_for_resource_planing(int today);
    QList<int> sort_activity_based_on_LS(QList<int> input);

    bool all_of_them_checked_forward();
    bool all_of_them_checked_backward();
    bool all_of_them_checked_resource();
    QList<int> without_precedence();
    QList<int> linked_to_finish();
    QList<int> find_precedence(QString activity_name);
    QList<int> find_successors(QString activity_name);
    void add_to_successors(QString activity,QString succesor);
    int find_by_name(QString name);
    bool check_all_precedences(QString name);
    bool check_all_precedences_resource(QString name,int today);
    bool check_all_successors(QString name);
    int find_max_EF(QString name);
    int find_min_LS(QString name);
    void get_one_row_from_table();
};

#endif // MAINWINDOW_H
