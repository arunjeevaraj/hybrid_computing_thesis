#ifndef OCL_H_H
#define OCL_H_H

#include <QtGlobal>

#ifdef Q_OS_WIN
    #include "CL/cl.h"
#elif defined(Q_OS_MAC)
     #include "OpenCL/cl.h"
#elif defined(Q_OS_LINUX)
    #include "CL/cl.h"
#endif
#endif // OCL_H_H
