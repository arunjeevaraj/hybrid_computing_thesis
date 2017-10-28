#include "device_list.h"
#include "ui_device_list.h"
#include <QDebug>

device_list::device_list(const QString &name,
                         const cl_platform_id platform_id,
                         QWidget *parent) :
    QWidget(parent),
    ui(new Ui::device_list),
    mDevice()
{

    //initialize
    this->platform_id = platform_id;
    ui->setupUi(this);
    setName(name);

    // connect the slots.
    connect(ui->platformSelect, &QCheckBox::toggled,
            this, &device_list::check_select);
    //OpenCL part.
    cl_device_id* devices_id_list;

    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
    devices_id_list = new cl_device_id[num_devices];
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, num_devices, devices_id_list, NULL);

    QString deviceName;
    char queryStr[50];
    deviceInfo* d_info;

      qDebug() << QString("num of devices found %1 ").arg(num_devices);
    for (unsigned i = 0 ; i < num_devices ; i++) {

            clGetDeviceInfo(devices_id_list[i], CL_DEVICE_NAME, sizeof(queryStr),
             &queryStr, NULL);
            deviceName = QString::fromUtf8(queryStr);
            d_info = new deviceInfo(deviceName, devices_id_list[i]);
            mDevice.append(d_info);
            ui->deviceListLay->addWidget(d_info);
    }

    current_platform_state = platform_state::state_idle;
}

device_list::~device_list()
{
    delete ui;
}

void device_list::setName(const QString &name)
{
    ui->platformSelect->setText(name);
    this->platformName = name;
}

QString device_list::name() const
{
    return ui->platformSelect->text();
}

bool device_list::isSelected() const
{
    return this->platformSelected;
   // return ui->platformSelect->isChecked();
}

bool device_list::run_platform()
{
    qDebug() << "running platform";
    if(current_platform_state==platform_state::state_done){

        qDebug() << "platform state done";
        return true;
    } else if(current_platform_state== platform_state::state_run){
        current_platform_state = platform_state::state_done;
        total_devices_done = 0;
        qDebug() << "platform state run";
        return false;
    } else { //for state idle.
        current_platform_state= platform_state::state_run;
        qDebug() << "platform state idle";

        return false;
    }
}

void device_list::init_platform()
{
    current_platform_state= platform_state::state_idle;
}

cl_uint device_list::getNumberDevices()
{
    return num_devices;
}

cl_platform_id device_list::getPlatformId()
{
    return platform_id;
}

cl_uint device_list::getSelectedDevices()
{
    cl_uint selectedDevices = 0;
    for(unsigned i=0; i < num_devices ; i++)
    {
      selectedDevices +=  mDevice[i]->IsSelected();
    }
    selectedDeviceId = new cl_device_id [selectedDevices];

    unsigned j = 0;
    for(unsigned i = 0; i < num_devices ; i++)
    {
      if( mDevice[i]->IsSelected()) {
        selectedDeviceId[j] = mDevice[i]->getDeviceId();
        j++;
      }
    }
    return selectedDevices;
}

cl_device_id* device_list::getSelectedDevicesIds()
{
    return selectedDeviceId;
}

void device_list::check_select(bool checked)
{
    qDebug() << "platform checkbox was toggled." << checked;
    platformSelected = checked;
}
