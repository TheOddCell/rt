/* rt.c — minimal; checks wheel membership, optionally prompts for PAM password,
 * setuid(0), then execs.
 *
 * Exit codes:
 *   0 on successful exec (never returns on success)
 *   1 on failure or if not in wheel
 *   2 on exec failure (prints perror)
 */

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static struct pam_conv conv = { misc_conv, NULL };

int main(int argc, char *argv[]) {
    /* --- Wheel check --- */
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

    /* --- Optional PAM password check --- */
    if (access("/etc/rtnopasswd", F_OK) != 0) { /* only prompt if file doesn't exist */
        pam_handle_t *pamh = NULL;
        int retval = pam_start("sudo", pw->pw_name, &conv, &pamh);
        if (retval != PAM_SUCCESS) {
            fprintf(stderr, "PAM start failed\n");
            return 1;
        }

        retval = pam_authenticate(pamh, 0);
        if (retval != PAM_SUCCESS) {
            fprintf(stderr, "Authentication failed\n");
            pam_end(pamh, retval);
            return 1;
        }

        pam_end(pamh, PAM_SUCCESS);
    }

    /* --- Escalate --- */
    if (setuid(0) < 0) {
        perror("setuid");
        return 1;
    }

    (void) setenv("HOME", "/root", 1);

    /* --- Exec --- */
    if (argc == 1) {
        struct passwd *root_pw = getpwnam("root");
        const char *shell = "/bin/sh";
        char *const sh_argv[] = { (char *)shell, NULL };

        if (root_pw && root_pw->pw_shell && root_pw->pw_shell[0])
            shell = root_pw->pw_shell;

        execv(shell, sh_argv);
        perror("execv");
        return 2;
    } else {
        execvp(argv[1], &argv[1]);
        perror("execvp");
        return 2;
    }
}

