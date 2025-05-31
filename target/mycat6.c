#include <stdio.h>      // For printf, perror, remove
#include <fcntl.h>      // For open, O_RDWR, O_CREAT, O_TRUNC, O_RDONLY
#include <unistd.h>     // For write, read, close, lseek
#include <string.h>     // For strlen
#include <stdlib.h>     // For exit
#include <stdint.h>     // uintptr_t
#include <sys/stat.h>   // For fstat
#define _POSIX_C_SOURCE 200809L // for posix_fadvise

#define OPTIMAL_MULTIPLIER_A 128

size_t io_blocksize(size_t page_size, size_t fsblock_size){
    return 4096*128;
}

char* align_alloc(size_t size){
    int alignment = getpagesize();
    void* ptr = malloc(size + alignment + sizeof(void*));
    if (!ptr) {
        perror("malloc");
        return NULL;
    }
    void *ptr_aligned = ptr + sizeof(void*);
    ptr_aligned = (void*)(((uintptr_t)ptr_aligned+alignment-1)&(~(alignment-1)));
    ((void**)ptr_aligned)[-1] = ptr;
    return ptr_aligned;
}

void align_free(void* ptr){
    void* old_ptr = ((void**)ptr)[-1];
    free(old_ptr);
}

int main(int argc, char* argv[]){
    int src_fd;
    src_fd = open(argv[1], O_RDONLY, 0644);
    if(src_fd == -1){
        perror("Error opening file for reading");
        close(src_fd);
        exit(EXIT_FAILURE);
    }
    int fadvise_ret = posix_fadvise(src_fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    if (fadvise_ret != 0) {
        perror("fadvice");
    }
    struct stat sb;
    if (fstat(src_fd, &sb) == -1) {
        perror("fstat");
        close(src_fd);
        exit(EXIT_FAILURE);
    }
    int buf_size = io_blocksize((size_t)getpagesize(),(size_t)sb.st_blksize);
    char* buf = align_alloc(buf_size);
    if (!buf) {
        perror("align_alloc");
        close(src_fd);
        exit(EXIT_FAILURE);
    }
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buf, buf_size)) > 0) {
        if (write(STDOUT_FILENO, buf, bytes_read) != bytes_read) {
            close(src_fd);
            perror("Write error");
            break;
        }
    }
    close(src_fd);
    align_free(buf);
    
    return 0;
}