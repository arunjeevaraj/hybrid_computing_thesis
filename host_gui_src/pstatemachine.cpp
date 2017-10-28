#include "pstatemachine.h"
#include <QDebug>


void pStateMachine::setCurrenState(psmState nextState)
{
    QMutex mutex;
    mutex.lock();
        this->currentState = nextState;
    mutex.unlock();
}

psmState pStateMachine :: getCurrentState()
{
    return this->currentState;
}

pStateMachine::pStateMachine()
{
    qDebug() << "constructor was called";
    this->stop = 0;
    currentState = psmState::st_idle;
}

void pStateMachine::run()
{
    count = 1;
    qDebug() << "Initialising stateMachine Now";
    for (unsigned j = 0; j < num_platforms; j++) {
        this->DeviceInfo[j].getsw_info(0)->writeToDevice(); // has a cl_finish at writeToDevice()
        qDebug() << "writing to device done " << j ;
        this->DeviceInfo[j].getsw_info(0)->kernelExecute(this->DeviceInfo[j].getKernel(0), particle_cnt);
        qDebug() << "executing NDrange";
     //   this->DeviceInfo[j].getsw_info(0)->readFromDevice();
     //   qDebug() << "reading back from device done";
    }

    // runnning for the rest of the cells.
    for (unsigned i = 0; i < count; ) {
        QMutex mutex;
        mutex.lock();
        if(this->currentState == psmState::st_stop) {
            break;
        }
        mutex.unlock();
        //qDebug() << "count " << i;
        // send to device memory.



        for (unsigned j = 0; j < num_platforms; j++) {
            this->DeviceInfo[j].getsw_info(0)->kernelRunsim(i);
        }


        i++;
        // execute device
        //
        qDebug() << "iteration i:" << i;
    }
    for (unsigned j = 0; j < num_platforms; j++) {
        this->DeviceInfo[j].getsw_info(0)->readFromDevice(count);
        qDebug() << "reading back from device done";
    }
    this->currentState = psmState::st_check;
  //  emit finished_work();
}

// populates all the other device info such as kernel and program.
void pStateMachine::populateDeviceInfo(cl_hw_info* taskSettings, cl_uint num_platforms)
{
    qDebug() << "populating Device info " << num_platforms;
    this->num_platforms = num_platforms;
    this->DeviceInfo = new cl_hw_info[num_platforms];
    // copying the device info set from GUI to the thread.
    for (unsigned j = 0; j < num_platforms; j++) {
        this->DeviceInfo[j] = taskSettings[j];
    }
    qDebug() << "number of devices " << this->DeviceInfo[0].getNumberDevices();

    for (unsigned j = 0; j < num_platforms; j++) {
       // for (unsigned i = 0 ; i < this->DeviceInfo[j].getNumberDevices(); i++) {
            DeviceInfo[j].updateDeviceType();
            DeviceInfo[j].buildOclKernel();
        //}
    }
}

void pStateMachine::setParticleCount(ulong particle_cnt)
{
    this->particle_cnt = particle_cnt;
}

void pStateMachine::create_test_bed(unsigned config)
{
   // particle_cnt = 4;
   // if(config == 0) {
    //     particle_cnt = 8*1024*1024;
   // }
    qDebug() << "creating the testbed for simulation." ;
    current_data_state = new particle_s<float>(particle_cnt);
    next_data_state = new particle_s<float>(particle_cnt);
    exp_data_state = new particle_s<float>(particle_cnt);
    intermed_state = new particle_s<float>(particle_cnt);
    // creating the test data input.
    current_data_state->test_create_input();
    // creating the transfer matrix cells for the test.
    trans_mat_cell = new trans_mat<float>[3];
    for(unsigned i = 0 ; i < 3 ; i++) {
        qDebug() << "trans Mat " << i;
        trans_mat_cell[i] =  trans_mat<float>(i, i + 1);
       // trans_mat_cell[i].print_file(6,6);
    }
    //current_data_state->print_particle(0);
    qDebug() << "Create the expected data out. ";
    exp_data_state->test_create_output(trans_mat_cell[1].getDataBuffer(),
        current_data_state->get_data_buffer());
    intermed_state->test_create_output(trans_mat_cell[1].getDataBuffer(),
            exp_data_state->get_data_buffer());
    //intermed_state->copy_data_to(exp_data_state->get_data_buffer());
    //intermed_state->print_particle(0);
   // exp_data_state->test_create_output(trans_mat_cell[1].getDataBuffer(),
     //       intermed_state->get_data_buffer());


    //exp_data_state->print_particle(0);
    //intermed_state->print_particle(0);
    //exp_data_state->print_particle(0);
}

void pStateMachine::mapDataToOcl()
{

    qDebug() << "Mapping to Data to Ocl ";

    // partition the data here later.

    for (unsigned i = 0 ; i<num_platforms ; i++) {

        qDebug() << "Setting Up Ocl platform " << i;
        //trans_mat_cell[1].print_file(6,6);
        this->DeviceInfo[i].setup_ocl(current_data_state->get_data_buffer(), trans_mat_cell[1].getDataBuffer(),
                next_data_state->get_data_buffer(), current_data_state->get_data_size(),
                trans_mat_cell[0].getDateSize());
        }
}

//void pStateMachine::finished_work()
//{}

void pStateMachine::checkDataOut()
{
    qDebug() << "checking data to be done...";
    float* data_out = next_data_state->get_data_buffer();
   // float* data_exp = exp_data_state->get_data_buffer();
    float* data_exp = intermed_state->get_data_buffer();
    int matches = 0;
    qDebug() << "particle_cnt " << particle_cnt;
    for(ulong i = 0; i < particle_cnt*6 ; i++) {
        if(data_out[i] == data_exp[i]) {
           // qDebug() << "data out" << data_out[i] << " expected data" << data_exp[i];
            matches++;
        }   else {
            qDebug() << "data out" << data_out[i] << " expected data" << data_exp[i];
            matches--;
        }
    }

    if (matches == particle_cnt*6) {
        qDebug() << "the first test passed";
    } else {
        qDebug() << "the test failed ";
        qDebug() << "mismatches " << - matches;
    }
    this->currentState = psmState::st_done;
}
