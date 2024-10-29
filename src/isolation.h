#ifndef ISOLATION_H
#define ISOLATION_H

#include <string.h>
#include <lzma.h>

#define SUN_PATH "/tmp/isolation.sock"
#define BUFFER_SIZE 1024

extern int iso_add(int a, int b);
extern char* iso_cat(char *file);
extern lzma_bool lzma_check_is_supported(lzma_check type);

enum func_id {
    ISO_ADD = 0,
    ISO_CAT,
    CAGED_LZMA_CHECK_IS_SUPPORTED,

    ISO_NONE /* terminator */
};

struct payload {
    enum func_id id;
    size_t payload_size;
    char data[];
};
/* size of enum is actually 8 */
const size_t payload_header = sizeof(long) + sizeof(size_t);

struct add_req {
    int a;
    int b;
};
typedef int add_res;

typedef struct __cat_req {
    size_t len;
    char file_path[];
} cat_req;
typedef char *cat_res;

#endif