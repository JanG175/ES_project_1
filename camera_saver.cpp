#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "BRAM-uio-driver/src/bram_uio.h"

#define WRITER_BRAM_UIO_NUMBER 0
#define READER_BRAM_UIO_NUMBER 1
#define BRAM_SIZE 1024


int main(int argc, char* argv[])
{
    BRAM writer(WRITER_BRAM_UIO_NUMBER, BRAM_SIZE);
    BRAM reader(READER_BRAM_UIO_NUMBER, BRAM_SIZE);

    // // read BRAM
    // for (int i = 0; i < BRAM_SIZE / 32; i++){
    //     uint32_t mem = writer[i];
    //     std::cout << mem << std::endl;
    // }
    // std::cout << "------------------------" << std::endl;

    // // write BRAM
    // writer[7] = 69;
    // cv::waitKey(100);

    // // read BRAM 0
    // for (int i = 0; i < BRAM_SIZE / 32; i++) {
    //     uint32_t mem = writer[i];
    //     std::cout << "BRAM0 " << i << ": " << mem << std::endl;
    // }

    // std::cout << "------------------------" << std::endl;
    
    // // read BRAM 1
    // for (int i = 0; i < BRAM_SIZE / 32; i++) {
    //     uint32_t mem = reader[i];
    //     std::cout << "BRAM1 " << i << ": " << mem << std::endl;
    // }

    // cv::waitKey(10000);

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

        cv::resize(orig_frame, frame,cv::Size(64, 64), cv::INTER_LINEAR);

        // std::cout << "Frame size:\n" << frame.size() << std::endl;

        cv::Size frameSize = frame.size();

        cv::Mat grayscale;

        // convert the RGB image to grayscale
        cv::Mat grayImage;
        cv::cvtColor(frame, grayImage, cv::COLOR_BGR2GRAY);

        uint8_t pixel_array[4];
        int pixel_array_index = 0;
        int bram_index = 0;
        uint32_t debug_array[BRAM_SIZE];
        int debug_index = 0;

        for (int i = 0; i < grayImage.rows; i++)
        {
            for (int j = 0; j < grayImage.cols; j++)
            {
                cv::Vec3b bgrPixel = grayImage.at<cv::Vec3b>(i, j);
                // std::cout << static_cast<int>(bgrPixel.val[0]) << ' ';
                pixel_array[pixel_array_index] = static_cast<uint8_t>(bgrPixel.val[0]);
                pixel_array_index++;

                if (j % 4 == 0)
                {
                    uint32_t bram_memory = pixel_array[0] << 24 | pixel_array[1] << 16 | pixel_array[2] << 8 | pixel_array[3];
                    pixel_array_index = 0;

                    // std::cout << "Memory " << bram_memory << std::endl;
                    writer[bram_index] = bram_memory;

                    debug_array[debug_index] = bram_memory;
                    debug_index ++;

                    bram_index++;
                    if (bram_index >= BRAM_SIZE)
                        bram_index = 0;
                }
            }
        }

        cv::waitKey(100);

        uint8_t received_frame[64*64];
        int received_frame_index = 0;

        // read BRAM
        for (int i = 0; i < BRAM_SIZE; i++)
        {
            uint32_t mem = reader[i];

            // if (mem != debug_array[i])
            //     std::cout << "Value: " << mem << " Debug: " << debug_array[i] << std::endl;

            // std::cout << "Memory " << mem << std::endl;
            received_frame[received_frame_index] = static_cast<uint8_t>(mem >> 24 & 0xff);
            received_frame[received_frame_index+1] = static_cast<uint8_t>(mem >> 16 & 0xff);
            received_frame[received_frame_index+2] = static_cast<uint8_t>(mem >> 8 & 0xff);
            received_frame[received_frame_index+3] = static_cast<uint8_t>(mem & 0xff);
            received_frame_index += 4;
        }

        // std::cout << received_frame << std::endl;

        bool same_frame = 1;

        for (int i = 0; i < grayImage.rows; i++)
        {
            for (int j = 0; j < grayImage.cols; j++)
            {
                cv::Vec3b bgrPixel = grayImage.at<cv::Vec3b>(i, j);
                // std::cout << static_cast<int>(bgrPixel.val[0]) << ' ';
                if (static_cast<uint8_t>(bgrPixel.val[0]) != received_frame[i*64+j])
                {
                    same_frame = 0;
                    std::cout << static_cast<uint8_t>(bgrPixel.val[0]) << " " << received_frame[i*64+j] << std::endl;
                    break;
                }
            }

            if (same_frame == 0)
                break;
        }

        std::cout << same_frame << std::endl;
        cv::waitKey(20);
    }

    cap.release();
    return 0;
}

