#ifndef PANDORA_CAMERA_
#define PANDORA_CAMERA_

#include <cuda_runtime_api.h>
#include "nvJPEG_helper.hpp"
#include <iostream>
#include <string>
#include <vector>
#define random(x) (rand()%x)



typedef std::vector<std::string> FileNames;
typedef std::vector<std::vector<char> > FileData;

int dev_malloc(void **p, size_t s);
int dev_free(void *p);


class PandoraCamera{
private:
    nvjpegJpegState_t nvjpeg_state;
    nvjpegHandle_t nvjpeg_handle;
    nvjpegDevAllocator_t dev_allocator;
    nvjpegOutputFormat_t fmt;
    cudaStream_t stream;
    nvjpegImage_t iout;
    nvjpegImage_t isz;
    std::vector<unsigned char> *vchanRGB;
public:
    int decompressJpeg(std::vector<char>& jpgBuffer, 
                       const uint32_t jpgSize,
                       std::vector<unsigned char> **bmp);
    PandoraCamera();
    ~PandoraCamera();
};

#endif