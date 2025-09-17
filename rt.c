/* rt.c — minimal; checks wheel membership, setuid(0), then execs.
 * If no argv given, execs root's shell (from /etc/passwd) or /bin/sh.
 * If argv given, execvp(argv[1], &argv[1]).
 *
 * Exit codes:
 *   0 on successful exec (never returns on success)
 *   1 on failure or if not in wheel
 *   2 on exec failure (prints perror)
 */

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    struct passwd *pw = getpwuid(getuid());
    struct group  *gr = getgrnam("wheel");

    if (!pw || !gr) return 1;

    int in_wheel = 0;
    if (pw->pw_gid == gr->gr_gid) in_wheel = 1;
    else {
        for (char **m = gr->gr_mem; *m; ++m) {
            if (strcmp(*m, pw->pw_name) == 0) { in_wheel = 1; break; }
        }
    }
    if (!in_wheel) return 1; /* not allowed */

    if (setuid(0) < 0) {
        perror("setuid");
        return 1;
    }

    /* small convenience: make HOME reflect root (optional) */
    if (setenv("HOME", "/root", 1) != 0) {
        /* non-fatal: continue */
    }

    if (argc == 1) {
        struct passwd *root_pw = getpwnam("root");
        const char *shell = "/bin/sh";
        char *const sh_argv[] = { (char *)shell, NULL };

        if (root_pw && root_pw->pw_shell && root_pw->pw_shell[0])
            shell = root_pw->pw_shell;

        execv(shell, sh_argv);
        perror("execv"); /* if execv returns, it's an error */
        return 2;
    } else {
        /* exec the provided command and args (argv[1] .. argv[argc-1]) */
        execvp(argv[1], &argv[1]);
        perror("execvp"); /* exec failed */
        return 2;
    }
}

