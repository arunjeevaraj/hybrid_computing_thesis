#ifndef PARTICLE_H
#define PARTICLE_H

// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <stdio.h>
//#include <QDebug>

using namespace std;


// container class for storing the frame information of the particle.
// designed as a template class as it can take any data type.

template <class T>
class particle_s
{
public:
    particle_s(uint64_t num_of_particles);
    ~particle_s();
    void test_create_input_ramp();
    void test_load_file(const char *file_name);
    void test_create_output_ramp();
    void test_create_output(T* trans_mat_ptr, T* data_in);
    void print_particle(int setting);
    void copy_data_to(T* data_in);
    T* get_data_buffer();
    uint64_t get_data_size();
    uint64_t get_particle_cnt();
private:
    uint64_t particle_cnt;
    uint64_t i;
    T *data_buffer;
    uint64_t size_of_buffer;
};

// copy function to copy the frame with same size to this frame.
template <class T>
void particle_s <T> :: copy_data_to(T *data_in)
{
    for(i = 0; i < particle_cnt ; i++) {
        data_buffer[i] = data_in[i];
    }
}

// constructor, that initialize the parameters related to particle count.
template <class T>
particle_s <T> :: particle_s(uint64_t num_of_particles)
{
	i = 0;
    particle_cnt = num_of_particles;
    size_of_buffer = num_of_particles * sizeof(T) * 6;
    data_buffer = new T[num_of_particles*6];
    if (data_buffer == NULL) {
        std :: cout << "Not enough memory to store particle information." << std :: endl;
    }
    else {
        std :: cout << "Allocated memory : " << size_of_buffer << " Bytes" << std :: endl;
    }
}

// destructor.
template <class T>
particle_s <T> ::~particle_s()
{
    free(data_buffer);
}

// create a ramp function into the frame. Just for test purpose.
template <class T>
inline void particle_s <T> ::test_create_input_ramp()
{
    std :: cout << "initiliazing the input data to some numbers." << std :: endl;
    for ( i = 0; i < particle_cnt; i++) {
        for (unsigned j = 0; j < 6; j++) {
            data_buffer[6*i + j] = i;
            //	cout << data_buffer[6*i + j] << std :: endl;
        }
    }
}

// creates the next frame, if the input was a ramp.
template <class T>
inline void particle_s <T> ::test_create_output_ramp()
{
    for ( i = 0; i < particle_cnt; i++) {
        for (unsigned j = 0; j < 6; j++) {
            data_buffer[6*i + j] = i*6;
        }
    }
}

// creates the next frame, given the prev frame buffer, and the trans_mat buffer.
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
// prints the particle information of the frame.
// setting -0 prints all the particle.
// setting -n prints the nth particle in the frame.
template <class T>
inline void particle_s <T> ::print_particle(int setting)
{
  //  QString toPrint;
    unsigned col = 6;
    if (setting == 0) {
        for ( i = 0; i < particle_cnt; i++) {

    //        toPrint = QString("%1 %2 %3 %4 %5 %6").arg(data_buffer[i*col + 0]).arg(data_buffer[i*col + 1])
    //                .arg(data_buffer[i*col + 2]).arg(data_buffer[i*col + 3]).arg(data_buffer[i*col + 4])
    //                .arg(data_buffer[i*col + 5]);
        	std :: cout << data_buffer[i*col + 0] << " "
        				<< data_buffer[i*col + 1] << " "
						<< data_buffer[i*col + 2] << " "
						<< data_buffer[i*col + 3] << " "
						<< data_buffer[i*col + 4] << " "
						<< data_buffer[i*col + 5] << std :: endl;
        }
    }
    else { // print the details of that particle, instead.

        for (unsigned j = 0; j < 6; j++)
            std :: cout << data_buffer[6*setting + j] << ' ';
        std :: cout <<  std :: endl;
    }
}
// returns the pointer to the data buffer.
template<class T>
inline T * particle_s<T>::get_data_buffer()
{
    return data_buffer;
}
// returns the size of the buffer in bytes.
template<class T>
inline uint64_t particle_s<T>::get_data_size()
{
    return size_of_buffer;
}
// returns the number of particles present in the frame.
template <class T>
uint64_t particle_s <T> ::get_particle_cnt()
{
    return particle_cnt;
}

// loads the frame info from a file, given the filename/ path to file is provided.
// also checks if the particle count in the data countainer matches with the file.
template <class T>
void particle_s <T> ::test_load_file(const char *file_name)
{
	FILE* fp = fopen(file_name, "rb");
	fseek(fp, 0, SEEK_END);
	uint64_t len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (this->size_of_buffer != len) {
		std :: cout << "particle count mismatch between file and"
				<< "data countainer." << std :: endl;
	} else {
		fread((void*)this->data_buffer, 1, len, fp);
		std :: cout << "reading file :" << file_name << "done "
				<< std :: endl;
	}
	fclose(fp);
}

#endif // PARTICLE_H
