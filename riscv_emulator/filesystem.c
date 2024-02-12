//
// Created by bergschaf on 1/31/24.
//

#include "filesystem.h"
#include "util.h"

File *entry_file;

void create_file(char *name, char *content, int64_t size) {
    File *new_file = malloc(sizeof(File));
    if (entry_file == NULL) {
        new_file->fd = 2;
    } else {
        new_file->fd = entry_file->fd + 1;
    }
    new_file->name = name;
    new_file->content = content;
    new_file->size = size;
    new_file->next = entry_file;
    entry_file = new_file;
}

void initialize_filesystem() {
    //entry_file = NULL;
    create_file("/proc/sys/vm/overcommit_memory", "0\0", 2);
    create_file("test.txt", "4242\0", 5);
}

File *get_file_from_fd(int fd) {
    File *current = entry_file;
    while (current != NULL) {
        if (current->fd == fd) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int open_file(char *pathname, int flags, int mode) { // TODO flags and mode ignored
    File *current = entry_file;
    while (current != NULL) {
        if (strcmp(current->name, pathname) == 0) {
            // file found
            return current->fd;
        }
        current = current->next;
    }
    return -1;
}


int64_t close_file(int fd) {
    File *current = entry_file;
    File *prev = NULL;
    while (current != NULL) {
        if (current->fd == fd) {
            if (prev == NULL) {
                entry_file = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return 0;
        }
        prev = current;
        current = current->next;
    }
    return -1;
}

int64_t read_file(int fd, char *buf, int64_t count) {
    File *file = get_file_from_fd(fd);
    if (file == NULL) {
        printf("Error: file not found\n");
        return -1;
    }

    if (count > file->size) {
        count = file->size;
        printf("Warning: count is bigger than file size\n");
    }
    for (int i = 0; i < count; ++i) {
        buf[i] = file->content[i];
    }
    file->offset += count;
    printf("Read %ld bytes\n", count);
    printf("Content: %s\n", buf);
    return count;
}

int64_t lseek_file(int fd, int64_t offset, int whence) {
    File *file = get_file_from_fd(fd);
    if (file == NULL) {
        return -1;
    }
    if (check_flag(whence, SEEK_SET)) {
        file->offset = offset;
    } else if (check_flag(whence, SEEK_CUR)) {
        file->offset += offset;
    } else if (check_flag(whence, SEEK_END)) {
        file->offset = file->size + offset;
    }
    return file->offset;
}
