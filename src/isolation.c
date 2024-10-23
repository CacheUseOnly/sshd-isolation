#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#include "isolation.h"

int __iso_setup_socket()
{
    int sock_fd, err;
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SUN_PATH
    };

    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        return -1;
    }

    err = connect(sock_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un));
    if (err == -1) {
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

lzma_bool caged_lzma_check_is_supported(lzma_check type) {
    int sock_fd;
    struct payload *buffer;
    size_t payload_size;
    ssize_t read_size;
    lzma_bool res;

    sock_fd = __iso_setup_socket();
    if (sock_fd == -1) {
        perror("__iso_setup_socket()");
        return -1;
    }

    payload_size = payload_header + sizeof(type);
    buffer = (struct payload*)malloc(payload_size);
    buffer->id = CAGED_LZMA_CHECK_IS_SUPPORTED;
    buffer->payload_size = payload_size;
    memcpy(buffer->data, &type, sizeof(type));

    write(sock_fd, buffer, buffer->payload_size);

    read_size = read(sock_fd, buffer, sizeof(lzma_bool));
    if (read_size == -1) {
        perror("read()");
        goto err;
    }
    res = *(lzma_bool*)buffer;

    close(sock_fd);
    free(buffer);
    return res;

err:
    close(sock_fd);
    free(buffer);
    return -1;
}

int iso_add(int a, int b)
{
    int sock_fd, res;
    struct payload *buffer;
    size_t payload_size;
    ssize_t read_size;
    struct add_req data = {
        .a = a,
        .b = b,
    };

    sock_fd = __iso_setup_socket();
    if (sock_fd == -1) {
        perror("__iso_setup_socket()");
        return -1;
    }

    payload_size = payload_header + sizeof(data);
    buffer = (struct payload*)malloc(payload_size);
    buffer->id = ISO_ADD;
    buffer->payload_size = payload_size;
    memcpy(buffer->data, &data, sizeof(data));

    write(sock_fd, buffer, buffer->payload_size);

    read_size = read(sock_fd, buffer, sizeof(add_res));
    if (read_size == -1) {
        perror("read()");
        goto err;
    }
    res = *(add_res*)buffer;

    close(sock_fd);
    free(buffer);
    return res;

err:
    close(sock_fd);
    free(buffer);
    return -1;
}

char* iso_cat(char *file)
{
    int sock_fd;
    struct payload *buffer;
    ssize_t read_size;
    size_t payload_size, str_len;
    cat_res res;
    cat_req *req;

    sock_fd = __iso_setup_socket();
    if (sock_fd == -1) {
        perror("__iso_setup_socket()");
        return "";
    }

    str_len = strlen(file)+1;
    payload_size = payload_header + sizeof(req->len) + str_len;

    req = (cat_req *)malloc(sizeof(req->len) + str_len);
    buffer = (struct payload*)malloc(payload_size);

    buffer->id = ISO_CAT;
    buffer->payload_size = payload_size;
    req->len = str_len;
    memcpy(req->file_path, file, str_len);
    memcpy(buffer->data, req, sizeof(req->len) + str_len);

    write(sock_fd, buffer, buffer->payload_size);

    memset(buffer, 0, buffer->payload_size);
    read_size = read(sock_fd, buffer, 8);
    if (read_size == -1) {
        perror("read()");
        goto err;
    }
    res = (cat_res)buffer;

    close(sock_fd);
    return res;

err:
    close(sock_fd);
    free(buffer);
    return "";
}