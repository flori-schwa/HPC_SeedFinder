#ifdef USE_OPENCL

#include "include/GPUSlimeChunkFinder.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "include/perfmeasure.hpp"

/*
 * Not yet fully functional, still produces erroneous Slime Chunk Data
 */

GPUSlimeChunkFinder::GPUSlimeChunkFinder() {
    // region Device setup
    using namespace std;
    using namespace cl;

    vector<Platform> platforms;
    if (Platform::get(&platforms) != CL_SUCCESS) {
        cout << "Error while getting platforms!" << endl;
        return;
    }

    if (platforms.empty()) {
        cout << "No Platform found!" << endl;
        return;
    }

    if (platforms.size() > 1) {
        cout << "Too many platforms found, using Platform 1!" << endl;
    }

    Platform plat = platforms[0];

    vector<Device> devices;
    if (plat.getDevices(CL_DEVICE_TYPE_GPU, &devices) != CL_SUCCESS) {
        cout << "Error while getting devices!" << endl;
        return;
    }

    if (devices.empty()) {
        cout << "No Devices found!" << endl;
        return;
    }

    if (devices.size() > 1) {
        cout << "Too many Devices found, using Devices 1!" << endl;
    }

    this->device = new Device(devices[0]);

    vector<unsigned long long> max_wi_sizes = this->device->getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();

    cout << "Device Max work item dimensions: " << max_wi_sizes[0] << "x" << max_wi_sizes[1] << "x" << max_wi_sizes[2] << endl;

    unsigned long long max_wg_sizes = this->device->getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

    cout << "Device Max work items per work group: " << max_wg_sizes << endl;

    this->max_memory = this->device->getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();

    cout << "Max memory on device: " << max_memory << endl;

    this->context = Context(*this->device);

    this->queue = CommandQueue(this->context, *this->device);

    std::ifstream in;
    in.open("src/GPUSlimeChunkFinder.cl");
    std::stringstream ss;

    if (in.is_open()) {
        std::string line;

        while (std::getline(in, line)) {
            ss << line << std::endl;
        }
    } else {
        cerr << "Failed to open cl File";
        return;
    }

    this->program = Program(this->context, ss.str(), false);

    try {
        this->program.build();
    } catch (cl::Error &e) {
        cout << "Build status: " << this->program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(*this->device) << endl;
        cout << "Log:\n" << this->program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(*this->device) << endl;
    }

    this->kernel = Kernel(this->program, "look_for_slime_chunks");

    cout << "Kernel Max Work Group size" << this->kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(*this->device) << endl;

    cout <<"InitOK: " << (this->init_ok = true) << endl;
    // endregion
}

GPUSlimeChunkFinder::~GPUSlimeChunkFinder() {
    delete this->device;
}

void GPUSlimeChunkFinder::get_slime_chunks_1d(const jlong seed, const jint offset_x, const jint offset_z,
                                              SlimeGrid *result) {

    cl_mem_flags flags = CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY;

    cl::Buffer b = cl::Buffer(this->context, flags, result->size);

    kernel_guard.lock();

    this->kernel.setArg<jlong>(0, seed);
    this->kernel.setArg<jint>(1, offset_x);
    this->kernel.setArg<jint>(2, offset_z);
    this->kernel.setArg<jlong>(3, result->width);
    this->kernel.setArg<int>(4, 1);
    this->kernel.setArg<cl::Buffer>(5, b);

    cl::Event runEvent;
    this->queue.enqueueNDRangeKernel(this->kernel, cl::NullRange, cl::NDRange(result->width), cl::NullRange, nullptr, &runEvent);

    kernel_guard.unlock();

    try {
        const std::vector<cl::Event> event_wait = { runEvent };
        this->queue.enqueueReadBuffer(b, true, 0, result->size, result->data, &event_wait);
    } catch (cl::Error& e) {
        std::cout << e.err() << ": " << e.what() << std::endl;
        exit(-1);
    }
}

void GPUSlimeChunkFinder::look_for_slime_chunks(const jlong seed, const jint start_cx, const jint start_cz, SlimeGrid* result) {
    using namespace std;
    using namespace cl;
    if (!this->init_ok) {
        return;
    }

    if (result->height == 1) {
        this->get_slime_chunks_1d(seed, start_cx, start_cz, result);
        return;
    }

    cl_mem_flags flags = CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY;

    kernel_guard.lock();

    Buffer b = Buffer(this->context, flags, result->size);

    this->kernel.setArg<jlong>(0, seed);
    this->kernel.setArg<jint>(1, start_cx);
    this->kernel.setArg<jint>(2, start_cz);
    this->kernel.setArg<jlong>(3, result->width);
    this->kernel.setArg<int>(4, 2);
    this->kernel.setArg<Buffer>(5, b);

    Event runEvent;

    this->queue.enqueueNDRangeKernel(this->kernel, NullRange, NDRange(result->width, result->height), cl::NullRange, nullptr, &runEvent);

    try {
        const vector<Event> event_wait = { runEvent };
        this->queue.enqueueReadBuffer(b, true, 0, result->size, result->data, &event_wait);
    } catch (cl::Error& e) {
        std::cout << e.err() << ": " << e.what() << std::endl;
        exit(-1);
    }

    this->queue.finish(); // waits until all commands in queue are done
    kernel_guard.unlock();
}

#endif