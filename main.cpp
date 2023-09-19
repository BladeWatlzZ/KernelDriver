#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

#define DEVICE_PATH "/dev/regctl"

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        cerr << strerror(errno) << endl;
        return -1;
    }

    ioctl(fd, 1, 0);
    close(fd);

    return 0;
}