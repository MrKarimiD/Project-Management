#include "activity.h"

Activity::Activity(QString name, int duration, int resources1, int resources2, QString precedence)
{
    this->forward_checked = false;
    this->backward_checked = false;
    this->rp_done = false;
    this->name = name;
    this->duration = duration;
    this->resources1 = resources1;
    this->resources2 = resources2;
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

void Activity::append_successor(QString input)
{
    if( this->successors.isEmpty() )
        this->successors = input;
    else
    {
        this->successors.append(",");
        this->successors.append(input);
    }
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

QString Activity::get_successors()
{
    return this->successors;
}

QString Activity::get_name()
{
    return this->name;
}

int Activity::get_EF()
{
    return this->EF;
}

int Activity::get_LS()
{
    return this->LS;
}

int Activity::get_LF()
{
    return this->LF;
}

int Activity::get_duration()
{
    return this->duration;
}

int Activity::get_rp_finish()
{
    return this->rp_finish;
}

int Activity::get_resource1()
{
    return this->resources1;
}

int Activity::get_resource2()
{
    return this->resources2;
}

QString Activity::get_information()
{
    return name+"=>  Early:("+QString::number(ES)+","+QString::number(EF)+")  Late:("+
            QString::number(LS)+","+QString::number(LF)
            +")  Resource:("+QString::number(rp_start)+","+QString::number(rp_finish)+")";
}

void Activity::set_rpStart(int start)
{
    this->rp_start = start;
    this->rp_finish = start + duration;
    resource_checked();
}

void Activity::resource_checked()
{
    this->rp_done = true;
}

bool Activity::resource_checked_value()
{
    return this->rp_done;
}

void Activity::check_forward()
{
    this->forward_checked = true;
}

void Activity::check_backward()
{
    this->backward_checked = true;
}
