#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "BRAM-uio-driver/src/bram_uio.h"

#define WRITER_BRAM_UIO_NUMBER 0
#define READER_BRAM_UIO_NUMBER 1
#define BRAM_SIZE 256


int main(int argc, char* argv[])
{
    BRAM writer(WRITER_BRAM_UIO_NUMBER, BRAM_SIZE);
    BRAM reader(READER_BRAM_UIO_NUMBER, BRAM_SIZE);

    // get frame from camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error opening camera" << std::endl;
        return -1;
    }
    cv::Mat orig_frame;
    cv::Mat frame;

    while (1)
    {
        cap.read(orig_frame);
        if (orig_frame.empty())
        {
            std::cerr << "Error reading frame" << std::endl;
            return -1;
        }

        cv::resize(orig_frame, frame,cv::Size(32, 32), cv::INTER_LINEAR);

        // std::cout << "Frame size:\n" << frame.size() << std::endl;

        // convert the RGB image to grayscale
        cv::Mat grayImage;
        cv::cvtColor(frame, grayImage, cv::COLOR_BGR2GRAY);

        uint8_t pixel_array[4];
        int pixel_array_index = 0;
        int bram_index = 0;
        uint32_t debug_array[BRAM_SIZE];
        int debug_index = 0;

        // write image to BRAM writer
        for (int i = 0; i < grayImage.rows; i++)
        {
            for (int j = 0; j < grayImage.cols; j++)
            {
                pixel_array[pixel_array_index] = grayImage.at<uint8_t>(i, j);
                pixel_array_index++;

                if (pixel_array_index >= 4)
                {
                    uint32_t bram_memory = pixel_array[0] << 24 | pixel_array[1] << 16 |
                                            pixel_array[2] << 8 | pixel_array[3];
                    pixel_array_index = 0;

                    // std::cout << "Memory " << bram_memory << std::endl;
                    writer[bram_index] = bram_memory;
                    bram_index++;

                    debug_array[debug_index] = bram_memory;
                    debug_index++;

                    if (bram_index >= BRAM_SIZE)
                    {
                        bram_index = 0;
                        debug_index = 0;
                    }
                }
            }
        }

        cv::waitKey(10);

        uint8_t received_frame[BRAM_SIZE * 4];
        int received_frame_index = 0;

        // read BRAM
        for (int i = 0; i < BRAM_SIZE; i++)
        {
            uint32_t mem = reader[i];

            if (mem != debug_array[i])
                std::cout << "Reader [" << i << "]: " << mem << " | Debug: " << debug_array[i] << std::endl;

            // std::cout << "Memory " << mem << std::endl;
            received_frame[received_frame_index] = static_cast<uint8_t>(mem >> 24 & 0xFF);
            received_frame[received_frame_index+1] = static_cast<uint8_t>(mem >> 16 & 0xFF);
            received_frame[received_frame_index+2] = static_cast<uint8_t>(mem >> 8 & 0xFF);
            received_frame[received_frame_index+3] = static_cast<uint8_t>(mem & 0xFF);
            received_frame_index += 4;
        }

        // std::cout << received_frame << std::endl;

        bool same_frame = true;

        for (int i = 0; i < grayImage.rows; i++)
        {
            for (int j = 0; j < grayImage.cols; j++)
            {
                uint8_t pixel = grayImage.at<uint8_t>(i, j);

                if (pixel != received_frame[i*32+j])
                {
                    same_frame = false;
                    std:: cout << "Pixel [" << i << "][" << j << "]:" << std::endl;  
                    std::cout << static_cast<int>(pixel) << " | " << static_cast<int>(received_frame[i*32+j]) << std::endl;
                    break;
                }
            }

            if (same_frame == false)
                break;
        }

        std::cout << "Is the image the same: " << same_frame << std::endl<< std::endl;

        cv::waitKey(20);
    }

    cap.release();
    return 0;
}

