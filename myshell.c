/**
 * myshell.c
 * 
 * Author:  Yuxuan Luo
 * Data:    2/10/2021
 */

#include "myshell.h"
#include "myshell_parser.c"

int main(int argc, char *argv[]) {
    char prompt[11] = "myShell $ ";

    while (1) {
        if (!(argc > 1 && (strcmp(argv[1], "-n") == 0))) {
            fwrite(prompt, sizeof(char), 11, stdout);
            fflush(stdout);
        }

        char line[MAX_LINE_LENGTH];
        memset(line, 0, MAX_LINE_LENGTH);

        if (fgets(line, MAX_LINE_LENGTH, stdin) == NULL && feof(stdin)) {
            exit(EXIT_SUCCESS);
        }

        struct pipeline *ppl = (struct pipeline*) malloc (sizeof(struct pipeline));
        memset(ppl, 0, sizeof(struct pipeline));
        ppl = pipeline_build(line);
        if (ppl == NULL) {
            printf("Invalid input.\n");
            continue;
        }

        int fd0 = STDIN_FILENO, fd1 = STDOUT_FILENO;
        pid_t pid;
        int status;
        struct pipeline_command *currComm = ppl->commands;

        while (currComm != NULL) {
            int pipefd[2];
            if (currComm->next != NULL) {
                if (pipe(pipefd) == -1) {
                    perror("Create pipe fail");
                    exit(EXIT_FAILURE);
                }
                fd1 = pipefd[1];
            }
            pid = fork();
            if (pid > 0) {
                do {
                    waitpid(pid, &status, ((ppl->is_background) ? WNOHANG : 0));
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));

                if (fd0 != STDIN_FILENO) {close(pipefd[0]);}
                if (fd1 != STDOUT_FILENO) {close(pipefd[1]);}
            } else {
                if (currComm->redirect_in_path == NULL && fd0 != STDIN_FILENO) {
                    if (dup2(pipefd[0], STDIN_FILENO) == -1) {
                        perror("Pipe in failed");
                        exit(EXIT_FAILURE);
                    }
                }
                if (currComm->redirect_out_path == NULL && fd1 != STDOUT_FILENO && (currComm->next != NULL)) {
                    if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                        perror("Pipe out failed");
                        exit(EXIT_FAILURE);
                    }
                }

                if (currComm->redirect_in_path != NULL) {
                    if (dup2(open(currComm->redirect_in_path, O_RDONLY), STDIN_FILENO) == -1) {
                        perror("​ERROR:");
                        exit(EXIT_FAILURE);
                    }
                }
                if (currComm->redirect_out_path != NULL) {
                    if (dup2(open(currComm->redirect_out_path, O_WRONLY|O_CREAT, 0644), STDOUT_FILENO) == -1) {
                        perror("​ERROR:");
                        exit(EXIT_FAILURE);
                    }
                }
                
                if (execvp(currComm->command_args[0], currComm->command_args) < 0) {
                    perror("failed.");
                    exit(EXIT_FAILURE);
                };
            }
            fd0 = pipefd[0];
            currComm = currComm->next;
        }
        pipeline_free(ppl);
        fd0 = STDIN_FILENO;
        fd1 = STDOUT_FILENO;
    }
    return 0;
}