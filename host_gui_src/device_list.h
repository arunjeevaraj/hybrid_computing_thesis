#ifndef DEVICE_LIST_H
#define DEVICE_LIST_H

#include <QWidget>
#include <QString>
#include <QVector>

#include "ocl_h.h"

#include "deviceinfo.h"
#include "device_list.h"

namespace Ui {
class device_list;
}

enum class platform_state {state_idle, state_run, state_done};

class device_list : public QWidget
{
    Q_OBJECT

public:
    explicit device_list(const QString& name,
                         const cl_platform_id platform_id,
                         QWidget *parent = 0);
    ~device_list();
void setName(const QString& name);
QString name() const;
bool isSelected() const;

bool run_platform();
void init_platform();
cl_uint getNumberDevices();
cl_platform_id getPlatformId();
cl_uint getSelectedDevices();
cl_device_id* getSelectedDevicesIds();
void check_select(bool checked);
private:
   // std::vector <cl_hw_info> cl_device_config;
    Ui::device_list *ui;
    QVector<deviceInfo*> mDevice;
    platform_state current_platform_state;
    cl_uint num_devices;
    cl_uint total_devices_done;
    bool platformSelected;
    cl_platform_id platform_id;
    QString platformName;
    cl_device_id* selectedDeviceId;
};

#endif // DEVICE_LIST_H
