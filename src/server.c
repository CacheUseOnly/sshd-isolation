#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "isolation.h"

int iso_dispatcher(void *payload, int fd);

int proc_add(void *data, int fd);
int proc_cat(void *data, int fd);

struct __func_table {
    enum func_id id;
    int(*handler)(void *payload, int fd);
};

const struct __func_table func_table[] = {
    {ISO_ADD,   proc_add},
    {ISO_CAT,   proc_cat},
    {ISO_NONE,  NULL},
};

int iso_dispatcher(void *payload, int fd)
{
    struct __func_table const *iter = func_table;
    struct payload *extracted;

    extracted = (struct payload*)payload;

    while (iter->id != ISO_NONE) {
        if (extracted->id == iter->id) {
            break;
        }
        iter++;
    }

    if (iter->handler == NULL) {
        return -ENOKEY;
    }

    return iter->handler(extracted->data, fd);
}

int main(void)
{
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SUN_PATH
    };
    int sock_fd, accepted_fd, err;
    char buffer[BUFFER_SIZE];
    ssize_t read_size;

    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket()");
        return -1;
    }

    unlink(SUN_PATH);

    err = bind(sock_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
    if (err != 0) {
        perror("bind()");
        goto err;
    }

    err = listen(sock_fd, 256);

    while (1) {
        accepted_fd = accept(sock_fd, NULL, NULL);
        if (accepted_fd == -1) {
            perror("accept()");
            goto err;
        }

        read_size = read(accepted_fd, buffer, BUFFER_SIZE);
        if (read_size == -1) {
            perror("read()");
            goto err;
        }

        err = iso_dispatcher(buffer, accepted_fd);
        if (err < 0) {
            perror("iso_dispatcher()");
        }

        close(accepted_fd);
    }

    return 0;

err:
    close(sock_fd);
    return -1;
}

int proc_add(void *data, int fd)
{
    add_res *res;
    struct add_req *extracted;
    int a, b;

    res = (int*)malloc(sizeof(int));
    if (!res) {
        return -ENOMEM;
    }
    extracted = (struct add_req *)data;

    a = extracted->a;
    b = extracted->b;
    printf("a: %d, b: %d\n", a, b);
    *res = a + b;

    write(fd, res, sizeof(*res));

    return 0;
}

int proc_cat(void *data, int fd)
{
    char file_path[BUFFER_SIZE];
    cat_res res;
    int file_fd;
    ssize_t read_size;

    res = (cat_res)malloc(BUFFER_SIZE);
    if (!res) {
        return -ENOMEM;
    }

    snprintf(file_path, ((cat_req *)data)->len, "%s", ((cat_req *)data)->file_path);
    printf("proc_cat: %s\n", file_path);
    fflush(stdout);

    file_fd = open(file_path, O_RDONLY);
    if (file_fd == -1) {
        return -EACCES;
    }

    read_size = read(file_fd, res, 8);
    if (read_size == -1) {
        close(file_fd);
        return -EACCES;
    }
    printf("cat: %s\n", res);
    fflush(stdout);

    write(fd, res, read_size);

    close(file_fd);
    return 0;
}
