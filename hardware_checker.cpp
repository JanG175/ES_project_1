#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "BRAM-uio-driver/src/bram_uio.h"
#include <unistd.h>

#define WRITER_BRAM_UIO_NUMBER 0
#define READER_BRAM_UIO_NUMBER 1
#define BRAM_SIZE 256


int main(int argc, char* argv[])
{
    // // get frame from camera
    // cv::VideoCapture cap(0);
    // if (!cap.isOpened())
    // {
    //     std::cerr << "Error opening camera" << std::endl;
    //     return -1;
    // }

    // cv::Mat frame;

    // while (1)
    // {
    //     cap.read(frame);
    //     if (frame.empty())
    //     {
    //         std::cerr << "Error reading frame" << std::endl;
    //         return -1;
    //     }

    //     std::cout << "Frame size:\n" << frame.size() << std::endl;

    //     // save image
    //     cv::imwrite("frame.jpg", frame);

    //     // show image in the terminal
    //     system("ascii-image-converter -C frame.jpg");

    //     // cv::imshow("Frame", frame);
    //     cv::waitKey(20);
    // }

    BRAM writer(WRITER_BRAM_UIO_NUMBER, BRAM_SIZE);
    BRAM reader(READER_BRAM_UIO_NUMBER, BRAM_SIZE);

    for (int i = 0; i < BRAM_SIZE; i++)
    {
        writer[i] = 0;
        reader[i] = 0;
    }

    // write to BRAM writer
    writer[100] = 420;

    // read BRAM writer
    for (int i = 0; i < BRAM_SIZE; i++)
    {
        uint32_t mem = writer[i];
        std::cout << "Writer memory[" << i << "] = " << mem << std::endl;
    }

    std::cout << std::endl << "====================" << std::endl << std::endl;

    sleep(1);

    // read BRAM reader
    for (int i = 0; i < BRAM_SIZE; i++)
    {
        uint32_t mem = reader[i];
        std::cout << "Reader memory[" << i << "] = " << mem << std::endl;
    }

    // cap.release();

    return 0;
}