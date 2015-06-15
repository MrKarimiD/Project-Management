#include "activity.h"

Activity::Activity(QString name, int duration, int resources, QString precedence)
{
    this->forward_checked = false;
    this->backward_checked = false;
    this->name = name;
    this->duration = duration;
    this->resources = resources;
    this->precedence = precedence;
}

int Activity::free_float()
{
    return 0;
}

int Activity::total_float()
{
    return (LF - EF);
}

void Activity::set_ES(int ES)
{
    this->ES = ES;
    this->EF = ES + duration;
    check_forward();
}

void Activity::set_LF(int LF)
{
    this->LF = LF;
    this->LS = LF - duration;
    check_backward();
}

bool Activity::forward_checked_value()
{
    return this->forward_checked;
}

bool Activity::backward_checked_value()
{
    return this->backward_checked;
}

QString Activity::get_precedence()
{
    return this->precedence;
}

QString Activity::get_name()
{
    return this->name;
}

int Activity::get_EF()
{
    return this->EF;
}

QString Activity::get_information()
{
    return name+"=>  Early:("+QString::number(ES)+","+QString::number(EF)+")  Late:("+
            QString::number(LS)+","+QString::number(LF)+")";
}

void Activity::check_forward()
{
    this->forward_checked = true;
}

void Activity::check_backward()
{
    this->backward_checked = true;
}
