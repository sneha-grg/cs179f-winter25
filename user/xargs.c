#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: xargs <command> [args...]\n");
        exit();
    }

    char buf[512];
    char *args[MAXARG];
    int n = 0;

    for (int i = 1; i < argc; i++) {
        args[n++] = argv[i];
    }
    
    while(1) {
        int m = 0;
        while(1) {
            char c;
            int bytes_read = read(0, &c, 1);
            if(bytes_read <= 0) {
                if (m==0) 
                    exit();
                break;
            }
            if (c == '\n')
                break;

            buf[m++] = c;
        }

        buf[m] = 0;
        args[n] = buf;
        args[n + 1] = 0;

        if(fork() == 0) {
            exec(args[0], args);
            fprintf(2, "xargs: exec failed\n");
            exit();
        }
        else {
            wait();
        }
    }
}