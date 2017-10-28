#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QWidget>
#include <QString>
//#include "devicestatemachine.h"
#include "ocl_h.h"

namespace Ui {
class deviceInfo;
}

class deviceInfo : public QWidget
{
    Q_OBJECT

public:
    explicit deviceInfo(const QString& name,
      const  cl_device_id device_id,
        QWidget *parent = 0);
    ~deviceInfo();
    void setName(const QString& name);
    bool IsSelected();
    cl_device_id getDeviceId();
signals:
   // void deviceSelectSignal();

private:
    Ui::deviceInfo *ui;
   // DeviceStateMachine *deviceState;
    bool device_select;
    cl_device_id device_id;
private slots:
    void checked(bool checked);
};

#endif // DEVICEINFO_H
