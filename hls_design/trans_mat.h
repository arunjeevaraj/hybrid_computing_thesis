#ifndef TRANS_MAT_H
#define TRANS_MAT_H

// Copyright (C) 2017
// Author: Arun Jeevaraj.
// All rights reserved.

#include <stdio.h>
#include <iostream>
#ifdef __QT__
  #include <QDebug>
  #include <QString>
#endif

// container class for storing the transfer matrix data.
// designed as a template class , then it can take any data type.

template <class T>class trans_mat {
public:
    trans_mat();
    trans_mat(unsigned trans_mat_id, const char *fileName);
    trans_mat(unsigned trans_mat_id, unsigned value);
    void  print_file(size_t row, size_t col);
    T* getDataBuffer();
    size_t getDateSize();
    void transpose_mat(T *data_buffer);
private:
    unsigned ID; // = 1011; invalid fileread.
    size_t sizeOfTransMat;
    T* data_buffer;
    char *file_name;
    void Readbinaryfile(const char* fileName, T** source, size_t* sourceSize);
};

// transpose the matrix
// pass the data buffer of matrix that needs to be trasposed.
template <class T>
void trans_mat <T> :: transpose_mat(T *data_buffer)
{
	for (unsigned i = 0; i < 6; i++) {
		for (unsigned j = 0; j < 6 ; j++) {
			this->data_buffer[i*6 + j] = data_buffer[6*j + i];
		}
	}
}

// returns the size of the data buffer.
template <class T>
T* trans_mat <T> :: getDataBuffer()
{
    return data_buffer;
}

// constructor to load the transfer matrix and assign an id.
template <class T>
trans_mat <T> ::trans_mat(unsigned trans_mat_id, const char *fileName)
{
    ID = trans_mat_id;
    Readbinaryfile(fileName, &data_buffer,&sizeOfTransMat);
}
// an empty constructor.
template <class T>
trans_mat <T> ::trans_mat()
{

}
// constructor creates a trans_matrix with all the elements with the value assigned.
// usage is for testing purpose.
template <class T>
trans_mat <T> ::trans_mat(unsigned trans_mat_id, unsigned value)
{
    ID = trans_mat_id;
    //Readbinaryfile(fileName, &data_buffer,&sizeOfTransMat);
    data_buffer = new T[36];
    for (unsigned i = 0; i < 36 ; i++) {
        data_buffer[i] = value;
    }
    sizeOfTransMat = sizeof(T)*36;
}
// returns the size of the data buffer.
template <class T>
size_t trans_mat<T>::getDateSize()
{
    return sizeOfTransMat;
}
// used to read file give the file name, pointer to the source and the size.
template <class T>
void trans_mat <T> :: Readbinaryfile(const char* fileName, T** source, size_t* sourceSize)
{

    FILE* fp = fopen(fileName, "rb");
    if (fp == NULL) {
        printf("Error: Couldn't find program source file '%s'.\n", fileName);
        this->ID = 1011;
    }
    else {
        fseek(fp, 0, SEEK_END);
        *sourceSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        *source = new T[*sourceSize];
        if (*source == NULL) {
            printf("Error: Couldn't allocate %d bytes for program source from file '%s'.\n", *sourceSize, fileName);
            this->ID = 1011;
        } else {
            fread(*source, 1, *sourceSize, fp);
        }
        fclose(fp);
    }
}

#ifndef __QT__
// used to print the transfer matrix.
template <class T>
void trans_mat <T> :: print_file(size_t row, size_t col)
{
    // prints transfer matrix row wise.
    for (unsigned i = 0; i < row; i++) {
    	for (unsigned j= 0; j < col ; j++) {
    		std :: cout << data_buffer[i*col + j] << " ";
    	}
    	std :: cout << std::endl;
    }
}

#else
template <class T>
void trans_mat <T> :: print_file(size_t row, size_t col)
    {
    QString toPrint;
        //std::cout << "File: " << filename << std::endl;
        for (unsigned i = 0; i < row; i++) {
            toPrint = QString("%1 %2 %3 %4 %5 %6").arg(data_buffer[0*col + i]).arg(data_buffer[1*col + i])
                    .arg(data_buffer[2*col + i]).arg(data_buffer[3*col + i]).arg(data_buffer[4*col + i])
                    .arg(data_buffer[5*col + i]);
            //for (unsigned j = 0; j< col ; j++) {
              // toPrint << data_buffer[j*col + i] <<" ";
            //}
            qDebug() << toPrint;
        }

}
#endif  // __QT__


#endif // TRANS_MAT_H
