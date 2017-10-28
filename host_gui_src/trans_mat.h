#ifndef TRANS_MAT_H
#define TRANS_MAT_H

#include <stdio.h>
#include <QDebug>
#include <QString>

template <class T>class trans_mat {


public:
    trans_mat();
    trans_mat(unsigned trans_mat_id, char *fileName);
    trans_mat(unsigned trans_mat_id, unsigned value);
    void  print_file(size_t row, size_t col);
    T* getDataBuffer();
    size_t getDateSize();
private:
    unsigned ID; // = 1011; invalid fileread.
    size_t sizeOfTransMat;
    T* data_buffer;
    char *file_name;
    void Readbinaryfile(const char* fileName, T** source, size_t* sourceSize);
};

template <class T>
T* trans_mat <T> :: getDataBuffer()
{
    return data_buffer;
}

template <class T>
trans_mat <T> ::trans_mat(unsigned trans_mat_id, char *fileName)
{
    ID = trans_mat_id;
    Readbinaryfile(fileName, &data_buffer,&sizeOfTransMat);
}

template <class T>
trans_mat <T> ::trans_mat()
{

}

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

template <class T>
size_t trans_mat<T>::getDateSize()
{
    return sizeOfTransMat;
}

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

        *source = new float[*sourceSize];
        if (*source == NULL) {
            printf("Error: Couldn't allocate %d bytes for program source from file '%s'.\n", *sourceSize, fileName);
            this->ID = 1011;
        } else {
            fread(*source, 1, *sourceSize, fp);
        }
    }
}

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

#endif // TRANS_MAT_H
