#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <immintrin.h>

#define PG_SZ 4096
#define SIZE (1 << 28)

void flush_cache(volatile void *ptr, size_t size) {
    volatile char *current = (volatile char *)ptr;
    volatile char *end = current + size;

    while (current < end) {
        _mm_clflush((void *)current);
        current += 64;  // Assuming a cache line size of 64 bytes
    }
}

void flush(void* p) {
    asm volatile ("clflush 0(%0)\n"
      :
      : "c" (p)
      : "rax");
}

void read_dev_dram(u_int64_t base, int64_t offset, int size, int first) {
    int rd_fd;
    char *rd_data;

    if ((rd_fd = open("/dev/mem", O_RDWR)) < 0) {
        perror("Error: could not open RDIMM character device");
        exit(EXIT_FAILURE);
    }

    // Ensure page alignment
    if (offset % PG_SZ != 0) {
        fprintf(stderr, "Error: Offset is not page-aligned.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate aligned memory
    char *pg_al_loc;
    if (posix_memalign((void **)&pg_al_loc, PG_SZ, SIZE) != 0) {
        perror("Error: could not allocate aligned memory");
        exit(EXIT_FAILURE);
    }

    pg_al_loc = (char *)mmap(pg_al_loc, SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, rd_fd, base);

    if (pg_al_loc == MAP_FAILED) {
        perror("could not mmap");
        exit(EXIT_FAILURE);
    }

    // Flush cache lines before reading
    flush((void *)(pg_al_loc + offset));

    // Read data from the mapped region to rd_data
    rd_data = (char *)malloc(size);
    asm volatile ("" : : : "memory");
    
    memcpy(rd_data, pg_al_loc + offset, size);

    // Memory fence
    asm volatile ("" : : : "memory");

    printf("\n");

    munmap((void *)pg_al_loc, SIZE);
    close(rd_fd);
    free(rd_data);
    
}

void write_dev_dram(u_int64_t base, u_int64_t offset, char *data, int size) {
    int rd_fd;

    if ((rd_fd = open("/dev/mem", O_RDWR)) < 0) {
        perror("Error: could not open RDIMM character device");
        exit(EXIT_FAILURE);
    }

    u_int64_t physical_address = base + offset;
    printf("Physical Address: 0x%llx\n", physical_address);

    volatile char *pg_al_loc = (char *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, rd_fd, base);

    if (pg_al_loc == MAP_FAILED) {
        perror("could not mmap");
        exit(EXIT_FAILURE);
    }

    // Flush cache lines before writing
    // flush_cache(pg_al_loc + offset, size);
     flush((void *)(pg_al_loc + offset));

    // Memory barrier if needed
    _mm_mfence();

    // Write data to memory
    memcpy((void *)(pg_al_loc + offset), (void *)data, size);

    // Memory fence if needed after the write
    _mm_mfence();

    // Flush cache lines after writing
    flush_cache(pg_al_loc + offset, size);

    flush_cache(pg_al_loc, SIZE);

    munmap((void *)pg_al_loc, SIZE);
    close(rd_fd);

    printf("\n");
}

int main(int argc, char **argv) {
    u_int64_t base = 0x200000000;
    int64_t offset = 0x100;     // Set your offset
    char *wr_data = NULL;
    int size = 64;

    // Parse command line arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [read/write] [offset] [size]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // initialize the modes
    int do_read = 0;
    int do_write = 0;

    // set the offset
    offset = strtoull(argv[2], NULL, 0);

    if (argc >= 4) {
        size = atoi(argv[3]);
        if (size <= 0) {
            fprintf(stderr, "Invalid size. Please specify a positive size.\n");
            exit(EXIT_FAILURE);
        }
    }

    wr_data = (char *)malloc(size + 1); // +1 for null terminator
    char *pattern = "A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5"; // Default pattern
    int pattern_len = strlen(pattern);
    for (int i = 0; i < size; i++) {
        wr_data[i] = pattern[i % pattern_len];
    }
    wr_data[size] = '\0'; // Null-terminate the string

    if (strcmp(argv[1], "read") == 0) {
        do_read = 1;
    } else if (strcmp(argv[1], "write") == 0) {
        do_write = 1;
    } else {
        fprintf(stderr, "Invalid mode. Use 'read' or 'write'.\n");
        exit(EXIT_FAILURE);
    }

    if (do_write) {
        write_dev_dram(base, offset, wr_data, size);
    }

    if (do_read) {
        // read_dev_dram(base, offset, size, 1);
        read_dev_dram(base, offset, size, 0);
    }

    free(wr_data);

    return 0;
}
