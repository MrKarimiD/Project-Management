#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "QString"

class Activity
{
public:
    Activity(QString name ,int duration, int resources1, int resources2, QString precedence);
    int free_float();
    int total_float();

    void set_ES(int ES);
    void set_LF(int LF);

    void append_successor(QString input);

    bool forward_checked_value();
    bool backward_checked_value();
    QString get_precedence();
    QString get_successors();
    QString get_name();
    int get_EF();
    int get_LS();
    int get_LF();
    int get_duration();
    int get_rp_start();
    int get_rp_finish();
    int get_resource1();
    int get_resource2();

    QString get_information();

    void set_rpStart(int start);
    void resource_checked();
    bool resource_checked_value();

    void check_forward();
    void check_backward();
private:
    QString name;
    int ES,EF,LS,LF;
    int duration;
    int resources1;
    int resources2;
    QString precedence;
    QString successors;
    bool forward_checked;
    bool backward_checked;

    bool rp_done;
    int rp_start;
    int rp_finish;
};

#endif // ACTIVITY_H
