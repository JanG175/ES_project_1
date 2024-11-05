#include <iostream>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

class BRAMController {
private:
    void* mapped_base;
    volatile uint32_t* bram_ptr;
    int fd;
    size_t mapped_size;
    off_t base_addr;

public:
    BRAMController(off_t base_address, size_t size) : 
        mapped_base(nullptr), 
        bram_ptr(nullptr),
        fd(-1),
        mapped_size(size),
        base_addr(base_address) {}

    bool init() {
        fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd == -1) {
            std::cerr << "Error opening /dev/mem" << std::endl;
            return false;
        }

        mapped_base = mmap(nullptr, mapped_size, 
                          PROT_READ | PROT_WRITE, 
                          MAP_SHARED, 
                          fd, 
                          base_addr);

        if (mapped_base == MAP_FAILED) {
            std::cerr << "Error mapping memory" << std::endl;
            close(fd);
            return false;
        }

        bram_ptr = reinterpret_cast<volatile uint32_t*>(mapped_base);
        return true;
    }

    void cleanup() {
        if (mapped_base) {
            munmap(mapped_base, mapped_size);
        }
        if (fd != -1) {
            close(fd);
        }
    }

    bool write32(size_t offset_bytes, uint32_t value) {
        if (offset_bytes >= mapped_size) {
            std::cerr << "Write offset out of range" << std::endl;
            return false;
        }
        bram_ptr[offset_bytes/4] = value;
        return true;
    }

    bool read32(size_t offset_bytes, uint32_t& value) {
        if (offset_bytes >= mapped_size) {
            std::cerr << "Read offset out of range" << std::endl;
            return false;
        }
        value = bram_ptr[offset_bytes/4];
        return true;
    }
};

int main() {
    // Create controllers for both BRAM instances
    BRAMController bram1(0xA0002000, 0x2000);
    BRAMController bram2(0xA0000000, 0x2000);

    // Initialize both controllers
    if (!bram1.init() || !bram2.init()) {
        std::cerr << "Failed to initialize BRAM controllers" << std::endl;
        return 1;
    }

    // Test values
    uint32_t test_patterns[] = {
        0x12345678,
        0xDEADBEEF,
        0xAAAA5555,
        0x00FF00FF
    };

    std::cout << "Testing cross-BRAM access patterns:" << std::endl;
    std::cout << "--------------------------------" << std::endl;

    // Try different offsets
    const size_t offsets[] = {0x0, 0x100, 0x500, 0x1000};

    for (size_t offset : offsets) {
        std::cout << "\nTesting at offset 0x" << std::hex << offset << ":" << std::endl;
        
        for (uint32_t pattern : test_patterns) {
            // Write to BRAM1
            std::cout << "Writing to BRAM1: 0x" << std::hex << pattern << std::endl;
            bram1.write32(offset, pattern);
            
            // Try to read the same offset from BRAM2
            uint32_t read_value;
            bram2.read32(offset, read_value);
            std::cout << "Reading from BRAM2: 0x" << std::hex << read_value << std::endl;
            
            // Check if values match
            if (pattern == read_value) {
                std::cout << "WARNING: Values match! BRAMs might be connected" << std::endl;
            } else {
                std::cout << "Values differ - BRAMs are independent" << std::endl;
            }
            
            // Small delay to ensure all operations complete
            usleep(1000);
        }
    }

    // Cleanup
    bram1.cleanup();
    bram2.cleanup();

    return 0;
}