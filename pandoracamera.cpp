#include "pandoracamera.hpp" 


int device_id = 1;
int WIDTH = 480;
int HEIGHT = 640;
int CHANNELS = 1;
int MUL = 3;

int AW = MUL * WIDTH;
int AH = HEIGHT;
int SZ = AW * AH;

int dev_malloc(void **p, size_t s) { 
    return (int)cudaMalloc(p, s); 
}
int dev_free(void *p) { 
    return (int)cudaFree(p); 
}

PandoraCamera::PandoraCamera(){
   
    fmt = NVJPEG_OUTPUT_RGBI;
    
    dev_allocator = {&dev_malloc, &dev_free};

    checkCudaErrors(nvjpegCreate(NVJPEG_BACKEND_DEFAULT, &dev_allocator, &nvjpeg_handle));
    checkCudaErrors(
        nvjpegJpegStateCreate(nvjpeg_handle, &nvjpeg_state));
    checkCudaErrors(
        nvjpegDecodeBatchedInitialize(nvjpeg_handle, nvjpeg_state, 1, 1, fmt));

    int devID = gpuDeviceInit(device_id);
    if (devID < 0)
    {
        printf("exiting...\n");
        exit(EXIT_FAILURE);
    }
    
    iout.pitch[0] = AW;
    isz.pitch[0] = SZ;
    checkCudaErrors(cudaMalloc(&iout.channel[0], SZ));
    vchanRGB = new std::vector<unsigned char>(  HEIGHT * WIDTH * 3);
    checkCudaErrors(
        cudaStreamCreateWithFlags(&stream, cudaStreamNonBlocking));
}

PandoraCamera::~PandoraCamera(){
    checkCudaErrors(cudaStreamDestroy(stream));
    checkCudaErrors(cudaFree(iout.channel[0]));
    checkCudaErrors(nvjpegJpegStateDestroy(nvjpeg_state));
    checkCudaErrors(nvjpegDestroy(nvjpeg_handle));
    delete vchanRGB;
}

int PandoraCamera::decompressJpeg(std::vector<char> &jpgBuffer, const uint32_t jpgSize,
                                  std::vector<unsigned char> **bmp){

    checkCudaErrors(cudaStreamSynchronize(stream));
    checkCudaErrors(nvjpegDecode(nvjpeg_handle, nvjpeg_state,
                                     (const unsigned char *)jpgBuffer.data(),
                                     jpgSize, fmt, &iout,
                                     stream));
    checkCudaErrors(cudaStreamSynchronize(stream));
                          
    unsigned char* chanRGB = (*vchanRGB).data();
    checkCudaErrors(cudaMemcpy2D(chanRGB, (size_t)WIDTH * 3, iout.channel[0], (size_t)iout.pitch[0],
                               WIDTH * 3, HEIGHT, cudaMemcpyDeviceToHost));
    *bmp = vchanRGB;

    char filename[100];
    sprintf(filename, "/home/adas/NVIDIA_CUDA-10.1_Samples/7_CUDALibraries/nvJPEG_v3/images/img%d.bmp", random(10));
    std::cout << filename << std::endl;
    writeBMPi_test(filename, *bmp, (size_t)iout.pitch[0], WIDTH, HEIGHT);

    return EXIT_SUCCESS;
}