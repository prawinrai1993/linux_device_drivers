#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_LEN 1024

int main() {
    int fd = open("/dev/simple_char_device", O_RDONLY);
    if (fd == -1) {
        perror("Error opening the device");
        return 1;
    }

    char buffer[BUF_LEN];
    ssize_t bytes_read = read(fd, buffer, BUF_LEN);
    if (bytes_read == -1) {
        perror("Error reading from the device");
        close(fd);
        return 1;
    }

    printf("Read from the device: %.*s\n", (int)bytes_read, buffer);
    close(fd);
    return 0;
}
