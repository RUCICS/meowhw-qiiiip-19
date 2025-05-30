#include <stdio.h>      // For printf, perror, remove
#include <fcntl.h>      // For open, O_RDWR, O_CREAT, O_TRUNC, O_RDONLY
#include <unistd.h>     // For write, read, close, lseek
#include <string.h>     // For strlen
#include <stdlib.h>     // For exit

int io_blocksize(){
    return getpagesize();
}

int main(int argc, char* argv[]){
    int src_fd;
    src_fd = open(argv[1], O_RDONLY, 0644);
    if(src_fd == -1){
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }
    int page_size = io_blocksize();
    char* buf = malloc(page_size);
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buf, page_size)) > 0) {
        if (write(STDOUT_FILENO, buf, bytes_read) != bytes_read) {
            perror("Write error");
            break;
        }
    }
    close(src_fd);
    free(buf);
    
    return 0;
}