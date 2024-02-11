//
// Created by bergschaf on 1/31/24.
//

#include "filesystem.h"
File* entry_file;

void create_file(char *name, char *content, int64_t size) {
    File *new_file = malloc(sizeof(File));
    if (entry_file == NULL) {
        new_file->fd = 0;
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

void close_file(int fd) {
    // TODO
}

void read_file(int fd, char *buf, int64_t count) {
    File *file = get_file_from_fd(fd);
    if (file == NULL) {
        return;
    }
    if (count > file->size) {
        count = file->size;
        printf("Warning: count is bigger than file size\n");
    }
    for (int i = 0; i < count; ++i) {
        buf[i] = file->content[i];
    }
}