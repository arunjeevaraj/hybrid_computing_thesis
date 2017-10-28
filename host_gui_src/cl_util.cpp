#include "cl_util.h"
#include <QDebug>

using namespace std;
#ifndef Q_OS_WIN
char* xcl_get_xclbin_name( const char *xclbin_name, char *device_name)
{
	char *xcl_bindir = getenv("XCL_BINDIR");
	char *xcl_mode = getenv("XCL_EMULATION_MODE");
	char *xcl_target = getenv("XCL_TARGET");
	if(xcl_mode == NULL) {
		strcpy(xcl_mode, "hw");
	} else {
	/* if xcl_mode is set then check if it's equal to true*/
	if(strcmp(xcl_mode,"true") == 0) {
		/* if it's true, then check if xcl_target is set */
		if(xcl_target == NULL) {
			/* default if emulation but not specified is software emulation */
			strcpy(xcl_mode, "sw_emu");
		} else {
			/* otherwise, it's what ever is specified in XCL_TARGET */
			strcpy(xcl_mode, xcl_target);
		}
	}
	}

	/* TODO: Remove once 2016.4 is released */
	 setenv("XCL_EMULATION_MODE", "true", 1);

	// typical locations of directory containing xclbin files
	const char *dirs[] = { xcl_bindir, // $XCL_BINDIR-specified
		"xclbin",   // command line build
		"..",       // gui build + run
		".",        // gui build, run in build directory
		NULL };
	const char **search_dirs = dirs;
	if (xcl_bindir == NULL) {
		search_dirs++;
	}
	char *device_name_versionless = strdup(device_name);
		if (device_name_versionless == NULL) {
			printf("Error: Out of Memory\n");
			exit(EXIT_FAILURE);
	}
	for (char *c = device_name; *c != 0; c++) {
		if (*c == ':' || *c == '.') {
			*c = '_';
		}
	}

	unsigned short colons = 0;
	for (char *c = device_name_versionless; *c != 0; c++) {
		if (*c == ':') {
			colons++;
			*c = '_';
		}

		if (colons == 3) {
			*c = '\0';
		}
	}
	const char *file_patterns[] = {
		"%1$s/%2$s.%3$s.%4$s.xclbin",     // <kernel>.<target>.<device>.xclbin
		"%1$s/%2$s.%3$s.%5$s.xclbin",     // <kernel>.<target>.<device_versionless>.xclbin
		"%1$s/binary_container_1.xclbin", // default for gui projects
		"%1$s/%2$s.xclbin",               // <kernel>.xclbin
		NULL
		};
	char *xclbin_file_name = (char*) malloc(sizeof(char)*PATH_MAX);
	memset(xclbin_file_name, 0, PATH_MAX);
	ino_t ino = 0; // used to avoid errors if an xclbin found via multiple/repeated paths
	for (const char **dir = search_dirs; *dir != NULL; dir++) {
		struct stat sb;
		if (stat(*dir, &sb) == 0 && S_ISDIR(sb.st_mode)) {
			for (const char **pattern = file_patterns; *pattern != NULL; pattern++) {
				char file_name[PATH_MAX];
				memset(file_name, 0, PATH_MAX);
				snprintf(file_name, PATH_MAX, *pattern, *dir,
						xclbin_name, xcl_mode, device_name, device_name_versionless);
				if (stat(file_name, &sb) == 0 && S_ISREG(sb.st_mode)) {
					if (*xclbin_file_name && sb.st_ino != ino) {
						printf("Error: multiple xclbin files discovered:\n %s\n %s\n", file_name, xclbin_file_name);
						exit(EXIT_FAILURE);
					}
					ino = sb.st_ino;
					strncpy(xclbin_file_name, file_name, PATH_MAX);
				}
			}
		}
	}
	if (*xclbin_file_name == '\0') {
		snprintf(xclbin_file_name, PATH_MAX, file_patterns[0], *search_dirs,
				xclbin_name, xcl_mode, device_name);
	}
	free(device_name);
	return xclbin_file_name;
}

#endif

const char* TranslateOpenCLError(cl_int errorCode)
{
	switch (errorCode)
	{
	case CL_SUCCESS:                            return "CL_SUCCESS";
	case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:       return "CL_MISALIGNED_SUB_BUFFER_OFFSET";                          //-13
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";   //-14
	case CL_COMPILE_PROGRAM_FAILURE:            return "CL_COMPILE_PROGRAM_FAILURE";                               //-15
	case CL_LINKER_NOT_AVAILABLE:               return "CL_LINKER_NOT_AVAILABLE";                                  //-16
	case CL_LINK_PROGRAM_FAILURE:               return "CL_LINK_PROGRAM_FAILURE";                                  //-17
	case CL_DEVICE_PARTITION_FAILED:            return "CL_DEVICE_PARTITION_FAILED";                               //-18
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";                         //-19
	case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";                           //-63
	case CL_INVALID_PROPERTY:                   return "CL_INVALID_PROPERTY";                                   //-64
	case CL_INVALID_IMAGE_DESCRIPTOR:           return "CL_INVALID_IMAGE_DESCRIPTOR";                           //-65
	case CL_INVALID_COMPILER_OPTIONS:           return "CL_INVALID_COMPILER_OPTIONS";                           //-66
	case CL_INVALID_LINKER_OPTIONS:             return "CL_INVALID_LINKER_OPTIONS";                             //-67
	case CL_INVALID_DEVICE_PARTITION_COUNT:     return "CL_INVALID_DEVICE_PARTITION_COUNT";                     //-68
   // case CL_INVALID_PIPE_SIZE:                  return "CL_INVALID_PIPE_SIZE";                                  //-69
   // case CL_INVALID_DEVICE_QUEUE:               return "CL_INVALID_DEVICE_QUEUE";                               //-70

	default:
		return "UNKNOWN ERROR CODE";
	}
}

void read_src_file(const char *file_name, char **source, size_t *src_size)
{
	FILE* fp = fopen(file_name, "rb");
	if (!fp) {
        qDebug() << "Reading file : " << file_name << " failed";
        exit(EXIT_FAILURE);
	}
	else {
		fseek(fp, 0, SEEK_END);
		*src_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		*source = new char[*src_size];
		if (*source == NULL) {
            qDebug() << "Not enough memory to store the file data." ;
            exit(EXIT_FAILURE);
		}
		else {
			fread(*source, 1, *src_size, fp);
            qDebug() << "reading done: " << file_name << "file size :" << *src_size <<
                " Bytes";
		}
	}
}

/*
static int loadFile2Memory(const char *filename, char **result)
{
    int size = 0;

    std::ifstream stream(filename, std::ifstream::binary);
    if (!stream) {
        return -1;
    }

    stream.seekg(0, stream.end);
    size = stream.tellg();
    stream.seekg(0, stream.beg);

    *result = new char[size + 1];
    stream.read(*result, size);
    if (!stream) {
        return -2;
    }
    stream.close();
    (*result)[size] = 0;
    return size;
}
*/

void event_cb(cl_event event, void *data)
{
  cl_command_type command;
  clGetEventInfo(event, CL_EVENT_COMMAND_TYPE, sizeof(cl_command_type),
                 &command, NULL);
  cl_int status;
  clGetEventInfo(event, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int),
                 &status, NULL);
  const char *command_str;
  const char *status_str;
  switch (command) {
  case CL_COMMAND_READ_BUFFER:
    command_str = "buffer read";
    break;
  case CL_COMMAND_WRITE_BUFFER:
    command_str = "buffer write";
    break;
  case CL_COMMAND_NDRANGE_KERNEL:
    command_str = "kernel";
    break;
  case CL_COMMAND_MAP_BUFFER:
    command_str = "kernel";
    break;
  case CL_COMMAND_COPY_BUFFER:
    command_str = "kernel";
    break;
  case CL_COMMAND_MIGRATE_MEM_OBJECTS:
        command_str = "buffer migrate";
      break;
  default:
    command_str = "unknown";
  }
  switch (status) {
  case CL_QUEUED:
    status_str = "Queued";
    break;
  case CL_SUBMITTED:
    status_str = "Submitted";
    break;
  case CL_RUNNING:
    status_str = "Executing";
    break;
  case CL_COMPLETE:
    status_str = "Completed";
    break;
  }
  printf("[%s]: %s %s\n", reinterpret_cast<char *>(data), status_str,
         command_str);
  fflush(stdout);
}
