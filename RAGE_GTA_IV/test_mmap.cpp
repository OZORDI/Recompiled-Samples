#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    printf("Starting mmap test...\n");
    fflush(stdout);
    
    size_t size = 0x100000000ULL;  // 4GB
    printf("Attempting to allocate %zu bytes (4GB)...\n", size);
    fflush(stdout);
    
    void* mem = mmap(nullptr, size, PROT_READ | PROT_WRITE, 
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (mem == MAP_FAILED) {
        printf("mmap FAILED\n");
        return 1;
    }
    
    printf("mmap SUCCESS: %p\n", mem);
    munmap(mem, size);
    printf("Done\n");
    return 0;
}
