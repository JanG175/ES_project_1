g++ frame_changer.cpp BRAM-uio-driver/src/bram_uio.cpp `pkg-config opencv4 --cflags --libs` -o frame_changer
g++ claude_bram.cpp -o claude_bram
g++ camera_saver.cpp BRAM-uio-driver/src/bram_uio.cpp `pkg-config opencv4 --cflags --libs` -o camera_saver