#include "include/GPUSlimeChunkFinder.hpp"


long get_numeric_dev_info(cl_device_id* device, cl_device_info param) {
    long value = -1;
    long* valueP = &value;
    cl_int ret = clGetDeviceInfo(*device, param, sizeof(long), valueP, nullptr);
    if (ret != CL_SUCCESS) {
        printf("Error %i", ret);
    }
    return value;
}


char* get_string_dev_info(cl_device_id* device, cl_device_info param, size_t str_length = 150) {
    char* value = static_cast<char*>(malloc(sizeof(char) * str_length));
    cl_int ret = clGetDeviceInfo(*device, param, sizeof(char) * str_length, value, nullptr);
    if (ret != CL_SUCCESS) {
        printf("Error %i", ret);
    }
    return value;
}


GPUSlimeChunkFinder::GPUSlimeChunkFinder() {
    // ToDo: error when count > allocated
    cl_int ret;
    cl_uint* count = static_cast<cl_uint*>(malloc(sizeof(cl_uint)));
    *count = 999;

    // region Platform

    cl_platform_id* platforms = static_cast<cl_platform_id*>(malloc(sizeof(cl_platform_id) * EXPECTED_PLATFORMS));
    ret = clGetPlatformIDs(EXPECTED_PLATFORMS, platforms, count);
    if (*count != 1) {
        printf("GetPlatform: Ret=%i, num Pf: %i\n", ret, *count);
    }

    if (ret != CL_SUCCESS) {
        return;
    }

    // endregion+

    *count = 999;

    // region Devices

    cl_device_id* devices = static_cast<cl_device_id*>(malloc(sizeof(cl_device_id) * EXPECTED_DEVICES));
    ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, EXPECTED_DEVICES, devices, count);

    if (*count != 1) {
        printf("GetPlatform: Ret=%i, num Pf: %i\n", ret, *count);
    }

    if (ret != CL_SUCCESS) {
        return;
    }

    // endregion

    printf("Found %i devices\n", *count);

    for (int i = 0; i < *count; i++) {
        printf("Device %i\n", i);
        long compute_units = get_numeric_dev_info(devices + i, CL_DEVICE_MAX_COMPUTE_UNITS);
        long frequency = get_numeric_dev_info(devices + i, CL_DEVICE_MAX_CLOCK_FREQUENCY);
        long address_size = get_numeric_dev_info(devices + i, CL_DEVICE_ADDRESS_BITS);
        char* name = get_string_dev_info(devices + i, CL_DEVICE_NAME);
        char* vendor = get_string_dev_info(devices + i, CL_DEVICE_VENDOR);
        char* version = get_string_dev_info(devices + i, CL_DEVICE_VERSION);
        char* profile = get_string_dev_info(devices + i, CL_DEVICE_PROFILE, 1000);
        char* drv_version = get_string_dev_info(devices + i, CL_DRIVER_VERSION);
        char* extensions = get_string_dev_info(devices + i, CL_DEVICE_EXTENSIONS, 1000);
        printf("\tVendor '%s' Name: '%s'\n", name, vendor);
        printf("\tDevice Version: '%s' Driver Version '%s'\n", version, drv_version);
        printf("\tCUs: %i, Freq: %i Mhz, Adrr Size: %i\n", compute_units, frequency, address_size);
        printf("\tProfile: '%s'\n", profile);
        printf("\tExtensions: '%s'\n", extensions);
        free(name);
        free(vendor);
        free(version);
        free(profile);
        free(drv_version);
        free(extensions);
    }

    // ToDo: Select device with highest CU count
    this->UseDevice(devices[0]);

    // region Free

    free(count);
    free(platforms);
    free(devices);

    // endregion
    printf("InitOK: %d", this->init_ok);
}

void GPUSlimeChunkFinder::UseDevice(cl_device_id dev) {
    this->device = dev;

    cl_int err = -1;
    this->context = clCreateContext(nullptr, 1, &this->device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        printf("Failure while creating Context: %i", err);
        return;
    }

    err = -1;
    this->queue = clCreateCommandQueue(this->context, this->device, 0, &err);
    if (err != CL_SUCCESS) {
        printf("Failure while creating Queue: %i", err);
        return;
    }
    this->init_ok = true;
}

void GPUSlimeChunkFinder::look_for_slime_chunks(const jlong seed, const jint start_cx, const jint start_cz, Grid2D<bool>* result) {
    if (!this->init_ok) {
        return;
    }
}
