#include "deviceinfo.h"
#include "ui_deviceinfo.h"

#include <QDebug>


deviceInfo::deviceInfo(const QString &name, const cl_device_id device_id,
                       QWidget *parent) :
    QWidget(parent),
    ui(new Ui::deviceInfo)
{
    ui->setupUi(this);
    setName(name);
    this->device_id = device_id;
    //deviceState = new DeviceStateMachine(device_id);
    connect(ui->deviceSelect, &QCheckBox::toggled,
            this, &deviceInfo::checked);
    this->device_select = 0;
}

deviceInfo::~deviceInfo()
{
    delete ui;
}

void deviceInfo::setName(const QString &name)
{
    ui->deviceSelect->setText(name);
}

void deviceInfo::checked(bool checked)
{
    //deviceState->set_device_select(checked);
    device_select = checked;
}

bool deviceInfo::IsSelected()
{
    return device_select;
}

cl_device_id deviceInfo::getDeviceId()
{
    return this->device_id;
}
