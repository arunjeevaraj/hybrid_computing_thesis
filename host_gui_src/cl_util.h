#pragma once
#include <QtGlobal>
#include "ocl_h.h"
//#include <iostream>
#include <cstring>
#include <stdio.h>
#include <fstream>
#include <limits.h>
#include <sys/stat.h>
#include <stdio.h>


#ifdef Q_OS_WIN
void read_src_file(const char *file_name, char** source, size_t* src_size);
const char* TranslateOpenCLError(cl_int errorCode);
//char* xcl_get_xclbin_name(const char *xclbin_name, char *device_name);

void read_src_file(const char *file_name, char **source, size_t *src_size);
//static int loadFile2Memory(const char *filename, char **result);
void event_cb(cl_event event, void *data);
#elif defined(Q_OS_MAC)
void read_src_file(const char *file_name, char** source, size_t* src_size);
const char* TranslateOpenCLError(cl_int errorCode);
char* xcl_get_xclbin_name(const char *xclbin_name, char *device_name);

void read_src_file(const char *file_name, char **source, size_t *src_size);
//static int loadFile2Memory(const char *filename, char **result);
void event_cb(cl_event event, void *data);
#elif defined(Q_OS_LINUX)
void read_src_file(const char *file_name, char** source, size_t* src_size);
const char* TranslateOpenCLError(cl_int errorCode);
char* xcl_get_xclbin_name(const char *xclbin_name, char *device_name);

void read_src_file(const char *file_name, char **source, size_t *src_size);
//static int loadFile2Memory(const char *filename, char **result);
void event_cb(cl_event event, void *data);
#endif
