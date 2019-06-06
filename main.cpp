#include <iostream>
#include <string>
#include "pandoracamera.hpp"
#include "nvJPEG_helper.hpp"

static int batch_size = 3;
int W = 480;
int H = 640;

int read_next_batch(FileNames &image_names, int batch_size,
                    FileNames::iterator &cur_iter, FileData &raw_data,
                    std::vector<size_t> &raw_len, FileNames &current_names) {
  int counter = 0;

  while (counter < batch_size) {
    if (cur_iter == image_names.end()) {
      std::cerr << "Image list is too short to fill the batch, adding files "
                   "from the beginning of the image list"
                << std::endl;
      cur_iter = image_names.begin();
    }

    if (image_names.size() == 0) {
      std::cerr << "No valid images left in the input list, exit" << std::endl;
      return EXIT_FAILURE;
    }

    std::ifstream input(cur_iter->c_str(),
                        std::ios::in | std::ios::binary | std::ios::ate);
    if (!(input.is_open())) {
      std::cerr << "Cannot open image: " << *cur_iter
                << ", removing it from image list" << std::endl;
      image_names.erase(cur_iter);
      continue;
    }

    // Get the size
    std::streamsize file_size = input.tellg();
    input.seekg(0, std::ios::beg);
    // resize if buffer is too small
    if (raw_data[counter].size() < file_size) {
      raw_data[counter].resize(file_size);
    }
    if (!input.read(raw_data[counter].data(), file_size)) {
      std::cerr << "Cannot read from file: " << *cur_iter
                << ", removing it from image list" << std::endl;
      image_names.erase(cur_iter);
      continue;
    }
    raw_len[counter] = file_size;

    current_names[counter] = *cur_iter;

    counter++;
    cur_iter++;
  }
  return EXIT_SUCCESS;
}

int main(int argc, const char *argv[]){
    FileNames image_names;
    std::string inputDir = "/home/adas/NVIDIA_CUDA-10.1_Samples/7_CUDALibraries/nvJPEG_v3/images/";
    readInput(inputDir, image_names);

    FileData file_data(batch_size);
    std::vector<size_t> file_len(batch_size);
    FileNames current_names(batch_size);
    FileNames::iterator file_iter = image_names.begin();

    if (read_next_batch(image_names, batch_size, file_iter, file_data, file_len, current_names))
            return EXIT_FAILURE;
    
    std::vector<unsigned char> *bmp;
    uint32_t bmpSize;
    PandoraCamera jpegDecom;

    int i = 0;
    while(i < batch_size){
        jpegDecom.decompressJpeg(file_data[i], 
                                 file_len[i],
                                 &bmp 
                                 );
        i++;
        //char filename[100];
        //sprintf(filename, "/home/adas/NVIDIA_CUDA-10.1_Samples/7_CUDALibraries/nvJPEG_v3/img%d.bmp", random(10));
        //std::cout << filename << std::endl;
        //writeBMPi_test(filename, bmp, (size_t)jpegDecom.iout.pitch[0],W, H);
    }
    
    return EXIT_SUCCESS;
}