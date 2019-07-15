// handler.c

#define _GNU_SOURCE

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>

#include <execinfo.h>
#include <string.h>
#include <errno.h>

#if 0
void handler(int sig) {
    printf("Error: signal %d:\n", sig);
    printf("I can do whatever I want in here!\n");
    exit(1);
}
#endif

#define sighandler_t __sighandler_t

struct handler_list
{
    int signum;
    sighandler_t sighandler;
    struct handler_list *next;
};

typedef struct handler_list handler_list;

static sighandler_t sighandler_cbs[NSIG/*MAX SIGNUM including RT*/];

static void add_handler_to_list(sighandler_t userhandler)
{
}


sighandler_t signal(int signum, sighandler_t usrhandler)
{
    struct sigaction sigact;
    printf("In [%s]\n", __func__);

    memset(&sigact, 0, sizeof(sigact));
    sigemptyset(&sigact.sa_mask); /* all signals unblocked */
    sigact.sa_handler = usrhandler;
    sigact.sa_flags = 0;
    sigaction(signum, &sigact, NULL);
    return usrhandler; 
}

static void full_write(int fd, const char *buf, size_t len)
{
    while (len > 0)
    {
        ssize_t ret = write(fd, buf, len);

        if ((ret == -1) && (errno != EINTR)) break;

        buf += (size_t) ret;
        len -= (size_t) ret;
    }
}

void print_backtrace(void)
{
    static const char start[] = "BACKTRACE ------------\n";
    static const char end[] = "----------------------\n";

    void *bt[1024];
    int bt_size;
    char **bt_syms;
    int i;

    printf("In [%s]\n", __func__);

    bt_size = backtrace(bt, 1024);
    bt_syms = backtrace_symbols(bt, bt_size);
    full_write(STDERR_FILENO, start, strlen(start));
    for (i = 1; i < bt_size; i++)
    {
        size_t len = strlen(bt_syms[i]);
        full_write(STDERR_FILENO, bt_syms[i], len);
        full_write(STDERR_FILENO, "\n", 1);
    }
    full_write(STDERR_FILENO, end, strlen(end));
    free(bt_syms);
}

int sigaction(int signum, const struct sigaction *act,
                     struct sigaction *oldact)
{
    static int (*orig_func)(int, const struct sigaction *, struct sigaction *) = NULL;
    printf("In [%s]\n", __func__);

    /* get reference to original (libc provided) function */
    if (!orig_func)
    {
        orig_func = (int(*)(int, const struct sigaction *, struct sigaction *)) dlsym(RTLD_NEXT, "sigaction");
    }
    print_backtrace();

    printf("Calling original function...\n");
    return orig_func(signum, act, oldact);
}


void sighdlr(int sig)
{
    struct sigaction oact, nex;
    printf("In [%s]\n", __func__);

    print_backtrace();

    sigaction( sig, NULL, &oact);
    if ( oact.sa_handler != SIG_DFL && oact.sa_handler != SIG_ERR && oact.sa_handler != SIG_IGN &&
		oact.sa_handler != NULL ) {
        printf("sighdlr: calling ohandler %p\n", oact.sa_handler );
        (oact.sa_handler)( sig );
    }
}

//main()
void _init(void)
{
    struct sigaction sigact;
    printf("In [%s]\n", __func__);
    memset(&sigact, 0, sizeof(sigact));
    sigemptyset(&sigact.sa_mask); /* all signals unblocked */
    sigact.sa_handler = sighdlr;
    sigact.sa_flags = 0;
    sigaction(SIGSEGV, &sigact, NULL); /* set up signal handler, don't
    care about the old handler - system should save it I think */

//shlib1();   /* sets up sigaction same as main does */
//shlib2()    /* ditto */
//pause();    /* wait for signal */
}

#if 0
void _init(void)
{
    printf("Loading hack.\n");
    signal(SIGSEGV, handler);
}
#endif

