// Define errno before any includes so it gets into .bss
int errno;

int *__errno(void) {
    return &errno;
}

int *__errno_location(void) {
    return &errno;
}

// Allocate memory for cpu_features struct
const char _dl_x86_cpu_features[80];
