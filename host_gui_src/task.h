#ifndef TASK_H
#define TASK_H

#include <QWidget>
#include <QThread>


namespace Ui {
class task;
}

class task : public QWidget,
     QThread
{
    Q_OBJECT

public:
    explicit task(QWidget *parent = 0);
    ~task();
    void run(); //override from Qthread class.
    bool stop;
private:
    Ui::task *ui;
};

#endif // TASK_H
