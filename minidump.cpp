#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "client/linux/handler/exception_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>

extern char *program_invocation_name;
extern char *program_invocation_short_name;

static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
void* context, bool succeeded);


static google_breakpad::ExceptionHandler eh(google_breakpad::MinidumpDescriptor(0), NULL, dumpCallback, NULL, true, -1);


static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
void* context, bool succeeded) {
  return succeeded;
}

void crash() { volatile int* a = (int*)(NULL); *a = 1; }

void minidump_register_mem(void *ptr, uint32_t sz_bytes) {
  //printf("Registering address: %p\n", ptr);
  eh.RegisterAppMemory(ptr, sz_bytes);
}

void minidump_unregister_mem(void *ptr) {
  //printf("Unregistering address: %p\n", ptr);
  eh.UnregisterAppMemory(ptr);
}

int minidump_dump(const char *output_path, int signal) {

  // core-${core_executable}.${core_pid}.sig-${core_signum}-XXXXXXXXXXXXXXXX.dmp
  char file_path[256] = {0};
  snprintf(file_path, sizeof(file_path), "%s/core-%s.%d.sig-%d-%lx.dmp", output_path, (char *)program_invocation_short_name, getpid(), signal, (unsigned long)time(NULL));
  
  int fd = open(file_path, O_WRONLY | O_CREAT | O_EXCL, 0600);
  if(fd < 0) {
    printf("Error %d while openning minidump file at: %s\n", errno, file_path);
    return 1;
  }

  eh.set_minidump_descriptor(google_breakpad::MinidumpDescriptor(fd));
  printf("Dump to %s\n", file_path);

  if (0 < signal) {
    eh.SimulateSignalDelivery(signal);
  }
  else {
    eh.WriteMinidump();
  }

  close(fd);
  return 0;
}

#ifdef __cplusplus
}
#endif
