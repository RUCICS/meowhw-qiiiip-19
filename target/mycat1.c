#include <stdio.h>      // For printf, perror, remove
#include <fcntl.h>      // For open, O_RDWR, O_CREAT, O_TRUNC, O_RDONLY
#include <unistd.h>     // For write, read, close, lseek
#include <string.h>     // For strlen
#include <stdlib.h>     // For exit

int main(int argc, char* argv[]){
    int src_fd, dest_fd = 1;
    src_fd = open(argv[1], O_RDONLY, 0644);
    if(src_fd == -1){
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }
    char c;
    while(1){
        ssize_t bytes_read = read(src_fd, &c, 1);
        if (bytes_read == -1) {
            perror("Error reading from source file");
            break;
        }
        if (bytes_read == 0) {
            break;  // End of file
        }
        if (write(dest_fd, &c, 1) != 1) {  // Write one byte
            perror("Error writing to destination file");
            break;
        }
    }
    close(src_fd);
    close(dest_fd);
    
    return 0;
}