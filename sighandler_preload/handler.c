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
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define sighandler_t __sighandler_t

extern int create_minidump(char *output_path, int signum);

static sighandler_t sighandler_cbs[NSIG/*MAX SIGNUM including RT*/] = {0};

static int in_sig_handler = 0;
static void sighdlr(int signum);

static int save_userhandler(int signum, sighandler_t userhandler)
{
    int ret = 0;
    if (signum>0 && signum < NSIG)
    {
	//printf("Saving usercb [%p] for signal [%d]\n", userhandler, signum);
        if (sighandler_cbs[signum])
	{
	    //printf("Already found saved [%p]\n", sighandler_cbs[signum]);
	    ret = 1;
	}
	sighandler_cbs[signum] = userhandler;
    }
    return ret;
}

sighandler_t signal(int signum, sighandler_t usrhandler)
{
    static sighandler_t (*orig_func)(int, sighandler_t) = NULL;
    //printf("In [%s %d %p]\n", __func__, signum, usrhandler);

    /* get reference to original (libc provided) function */
    if (!orig_func)
    {
        orig_func = (sighandler_t(*)(int, sighandler_t)) dlsym(RTLD_NEXT, "signal");
    }
    if (in_sig_handler ||
	(signum != SIGQUIT && signum != SIGILL && signum != SIGABRT && signum != SIGFPE && signum != SIGSEGV) ||
	//usrhandler == SIG_DFL || usrhandler == SIG_ERR || usrhandler == SIG_IGN ||
	save_userhandler(signum, usrhandler))
    {
        //printf("Calling original(usrhandler) SIGNAL function...\n");
        return orig_func(signum, usrhandler);
    }

    //printf("Calling original(sighdlr) SIGNAL function...\n");
    return orig_func(signum, sighdlr);
}

int sigaction(int signum, const struct sigaction *act,
                     struct sigaction *oldact)
{
    struct sigaction sigact, oact;
    static int (*orig_func)(int, const struct sigaction *, struct sigaction *) = NULL;
    //printf("In [%s %d %p]\n", __func__, signum, act ? act->sa_handler : NULL);

    /* get reference to original (libc provided) function */
    if (!orig_func)
    {
        orig_func = (int(*)(int, const struct sigaction *, struct sigaction *)) dlsym(RTLD_NEXT, "sigaction");
    }
    if (in_sig_handler || (!act) || 
	(signum != SIGQUIT && signum != SIGILL && signum != SIGABRT && signum != SIGFPE && signum != SIGSEGV) ||
	//act->sa_handler == SIG_DFL || act->sa_handler == SIG_ERR || act->sa_handler == SIG_IGN ||
	save_userhandler(signum, act->sa_handler))
    {
        //printf("Calling original(usrhandler) SIGACTION function...\n");
        return orig_func(signum, act, oldact);
    }
    memset(&sigact, 0, sizeof(sigact));
    sigemptyset(&sigact.sa_mask); /* all signals unblocked */
    sigact.sa_handler = sighdlr;
    sigact.sa_flags = 0;
    sigaction(signum, &sigact, NULL);
    
    save_userhandler(signum, act->sa_handler);

    //printf("Calling original(sighdlr) SIGACTION function...\n");
    return orig_func(signum, &sigact, &oact);
}

static void _mkdir(const char *dir)
{
        char tmp[PATH_MAX] = {0};
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp),"%s",dir);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        mkdir(tmp, S_IRWXU);
                        *p = '/';
                }
        mkdir(tmp, S_IRWXU);
}

static void get_target_dir(char *output_dir, int size)
{
    if (output_dir)
    {
        char hostname[256] = {0};
	struct stat sb;
        gethostname(hostname, 256);
	snprintf(output_dir, size, "/core/minidump/%s", hostname);
	_mkdir(output_dir);
        if (mkdir(output_dir, S_IRWXU) == -1)
        {
	    if (errno != EEXIST)
	    {
	        // fallback to /core dir in case of error
                snprintf(output_dir, size, "/core");
	    }
        }
    }
}


static void sighdlr(int signum)
{
    char output_path[1024] = {0};
    in_sig_handler = 1;
    //printf("In: [%s %d]\n", __func__, signum);
    
    sighandler_t usercb = sighandler_cbs[signum];
    //printf("Got usercb=[%p]\n", usercb);
    if (getppid() != 1) // don't collect minidump for orphand processes
    {
        get_target_dir(output_path, sizeof(output_path));
        create_minidump(output_path, signum);
    }
    if (usercb && usercb != SIG_DFL && usercb != SIG_ERR && usercb != SIG_IGN)
    {
	//printf("Calling user ORIG[%p] cb...\n", usercb);
        usercb(signum);
    }
    else
    {
        signal(signum, usercb ? usercb : SIG_DFL);
	raise(signum);
    }

    in_sig_handler = 0;
}

void _init(void)
{
    //struct sigaction sigact;
    //printf("[%s] Registring default handlers [%s/%d]\n", __func__, (char*)program_invocation_short_name, getpid());
    in_sig_handler = 1;
    signal(SIGQUIT, sighdlr);
    signal(SIGILL, sighdlr);
    signal(SIGABRT, sighdlr);
    signal(SIGFPE, sighdlr);
    signal(SIGSEGV, sighdlr);
    in_sig_handler = 0;
}

