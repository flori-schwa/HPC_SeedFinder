#include "include/GPUSlimeChunkFinder.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


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

    unsigned long long max_memory = this->device->getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();

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

void GPUSlimeChunkFinder::look_for_slime_chunks(const jlong seed, const jint start_cx, const jint start_cz, Grid2D<SlimeFlag>* result) {
    using namespace std;
    using namespace cl;
    if (!this->init_ok) {
        return;
    }

    cl_mem_flags flags = CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY;

    Buffer b = Buffer(this->context, flags, result->size);

    this->kernel.setArg<jlong>(0, seed);
    this->kernel.setArg<jint>(1, start_cx);
    this->kernel.setArg<jint>(2, start_cz);
    this->kernel.setArg<jlong>(3, result->width);
    this->kernel.setArg<Buffer>(4, b);

    Event runEvent;

    this->queue.enqueueNDRangeKernel(this->kernel, NullRange, NDRange(result->width, result->height), cl::NullRange, nullptr, &runEvent);

    const vector<Event> event_wait = { runEvent };

    this->queue.enqueueReadBuffer(b, true, 0, result->size, result->data, &event_wait);

    this->queue.finish(); // waits until all commands in queue are done


}
