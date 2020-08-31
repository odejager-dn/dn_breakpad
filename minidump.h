#ifndef MINIDUMP_H_
#define MINIDUMP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void minidump_set_path(const char *output_path);
extern void minidump_register_mem(void *ptr, uint32_t sz_bytes);
extern int create_minidump(const char *output_path, int signal);

#ifdef __cplusplus
}
#endif

#endif // MINIDUMP_H_