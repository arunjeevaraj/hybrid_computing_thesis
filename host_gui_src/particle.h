#ifndef PARTICLE_H
#define PARTICLE_H
//
//  particle.hpp
//  learn
//
//  Created by Arun Jeevaraj on 30/06/17.
//
//

#include <stdlib.h>
#include <QDebug>

using namespace std;

//typedef unsigned long long ulong;

template <class T>
class particle_s
{
public:
    particle_s(ulong num_of_particles);
    ~particle_s();
    void test_create_input();
   // void test_load_input(char * filename);
    void test_create_output();
    void test_create_output(T* trans_mat_ptr, T* data_in);
    void print_particle(int setting);
    void copy_data_to(T* data_in);
    T* get_data_buffer();
    ulong get_data_size();
    ulong get_particle_cnt();
private:
    ulong particle_cnt;
    ulong i;
    T *data_buffer;
    ulong size_of_buffer;
};


template <class T>
void particle_s <T> :: copy_data_to(T *data_in)
{
    for(i = 0; i < particle_cnt ; i++) {
        data_buffer[i] = data_in[i];
    }
}

template <class T>
particle_s <T> :: particle_s(ulong num_of_particles)
{
    particle_cnt = num_of_particles;
    size_of_buffer = num_of_particles * sizeof(T) * 6;
    data_buffer = new T[num_of_particles*6];
    if (data_buffer == NULL) {
        qDebug() << "Not enough memory to store particle information." << endl;
    }
    else {
        qDebug() << "Allocated memory : " << size_of_buffer << " Bytes" << endl;
    }
}

template <class T>
particle_s <T> ::~particle_s()
{
    free(data_buffer);
}

template <class T>
inline void particle_s <T> ::test_create_input()
{
    qDebug() << "initiliazing the input data to some numbers." << endl;
    for ( i = 0; i < particle_cnt; i++) {
        for (unsigned j = 0; j < 6; j++) {
            data_buffer[6*i + j] = i;
            //	cout << data_buffer[6*i + j] << endl;
        }
    }
}

template <class T>
inline void particle_s <T> ::test_create_output()
{
    for ( i = 0; i < particle_cnt; i++) {
        for (unsigned j = 0; j < 6; j++) {
            data_buffer[6*i + j] = i*6;
        }
    }
}

template <class T>
inline void particle_s <T> ::test_create_output(T *trans_mat_ptr, T* data_in)
{
    // initializing all the variables to zero.
    for( i= 0; i < particle_cnt; i ++){
        for(unsigned k = 0;k < 6; k++){
            data_buffer[i*6+k]= 0;
        }
    }

    // performing matrix multiplication on the input vector to get the output vector.
    for( i= 0; i < particle_cnt; i ++)
    {
        for(unsigned k = 0;k < 6; k++){
            for(unsigned j = 0;j < 6; j++) {
                data_buffer[i*6+j] += trans_mat_ptr[6*k + j] * data_in[6*i+k];
            }
        }

    }
}

template <class T>
inline void particle_s <T> ::print_particle(int setting)
{
    QString toPrint;
    unsigned col = 6;
    if (setting == 0) {
        for ( i = 0; i < particle_cnt; i++) {

            toPrint = QString("%1 %2 %3 %4 %5 %6").arg(data_buffer[i*col + 0]).arg(data_buffer[i*col + 1])
                    .arg(data_buffer[i*col + 2]).arg(data_buffer[i*col + 3]).arg(data_buffer[i*col + 4])
                    .arg(data_buffer[i*col + 5]);
            qDebug() << toPrint;
        }
    }
    else { // print the details of that particle, instead.

        for (unsigned j = 0; j < 6; j++)
            qDebug() << data_buffer[6*setting + j] << ' ';
        qDebug() << endl;
    }
}

template<class T>
inline T * particle_s<T>::get_data_buffer()
{
    return data_buffer;
}

template<class T>
inline ulong particle_s<T>::get_data_size()
{
    return size_of_buffer;
}

template <class T>
ulong particle_s <T> ::get_particle_cnt()
{
    return particle_cnt;
}

/*
template <class T>
void particle_s <T> ::test_load_input(char * filename)
{

}
*/
#endif // PARTICLE_H
