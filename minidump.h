#ifndef MINIDUMP_H_
#define MINIDUMP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void minidump_register_mem(void *ptr, uint32_t sz_bytes);
extern void minidump_unregister_mem(void *ptr);
extern int minidump_dump(const char *output_path, int signal);

#ifdef __cplusplus
}
#endif

#endif // MINIDUMP_H_
