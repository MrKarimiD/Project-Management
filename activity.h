#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "QString"

class Activity
{
public:
    Activity(QString name ,int duration, int resources, QString precedence);
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

    QString get_information();

    void check_forward();
    void check_backward();
private:
    QString name;
    int ES,EF,LS,LF;
    int duration;
    int resources;
    QString precedence;
    QString successors;
    bool forward_checked;
    bool backward_checked;
};

#endif // ACTIVITY_H
