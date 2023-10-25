#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int get_fd(const char* file_name, int open_style);
int fd_allocate(int fd, const char* filename, off_t start_value, off_t len_value);
int snapshot(int myfd);

int main() {
    const char* file_name = "Data/hello.txt";
    int open_style = O_CREAT | O_RDWR;
    int fd = get_fd(file_name, open_style);
    if (fd == -1) {
        return 1;
    }
    fd_allocate(fd, file_name, 0, 125);
    snapshot(fd);
    return 0;
}

int get_fd(const char* file_name, int open_style) {
    int fd = open(file_name, open_style, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Failed to open the file");
    }
    return fd;
}

int fd_allocate(int fd, const char* filename, off_t start_value, off_t len_value) {
    if (posix_fallocate(fd, start_value, len_value) != 0) {
        perror("Error allocating file space");
        close(fd);
        return -1;
    }

    printf("File '%s' created and allocated with %lld bytes of space.\n", filename, (long long)len_value);
    return fd;
}

int snapshot(int myfd) {
    printf("Enter snapshot\n");

    struct stat file_info;
    if (fstat(myfd, &file_info) == -1) {
        perror("Error getting file attributes");
        close(myfd);
        return 1;
    }

    printf("File Size: %lld bytes \n", (long long)file_info.st_size);
    printf("File Permissions: %o \n", file_info.st_mode & ~S_IFMT);
    printf("File Owner UID: %d \n", file_info.st_uid);
    printf("File Group GID: %d \n", file_info.st_gid);

    off_t cur_offset = lseek(myfd, 0, SEEK_CUR);
    if (cur_offset == -1) {
        perror("Error getting current offset");
    }
    printf("Current offset: %lld \n", (long long)cur_offset);

    if (close(myfd) == -1) {
        perror("Error closing file");
        return 1;
    }

    printf("Leave snapshot\n");
    return 0;
}
