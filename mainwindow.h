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
    QList<Activity> activity_list,calculated_list,uncalculated_list;

    bool all_of_them_checked_forward();
    QList<int> without_precedence();
    QList<int> find_precedence(QString activity_name);
    int find_by_name(QString name);
    bool check_all_precedences(QString name);
    int find_max_EF(QString name);
};

#endif // MAINWINDOW_H
