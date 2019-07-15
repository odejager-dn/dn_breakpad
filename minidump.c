#include <stdio.h>
#include <string>
#include "client/linux/handler/exception_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>

extern char *program_invocation_name;
extern char *program_invocation_short_name;

extern void set_dump_filename(char *filename);

static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
void* context, bool succeeded) {
  printf("Dump path: %s\n", descriptor.path());
  return succeeded;
}

void crash() { volatile int* a = (int*)(NULL); *a = 1; }

int create_minidump(char *output_path, int signal) {
// core-${core_executable}.${core_pid}.sig-${core_signum}
  char filename[256] = {0};
  snprintf(filename, sizeof(filename), "core-%s.%d.sig-%d", (char *)program_invocation_short_name, getpid(), signal);
  //set_dump_filename((char *)program_invocation_short_name);
  set_dump_filename(filename);
  google_breakpad::MinidumpDescriptor descriptor(output_path);
  google_breakpad::ExceptionHandler eh(descriptor, NULL, dumpCallback, NULL, true, -1);
  if (0 < signal)
  {
    eh.SimulateSignalDelivery(signal);
  }
  else
  {
    eh.WriteMinidump();
  }
  //crash();
  return 0;
}

#ifdef __cplusplus
}
#endif
