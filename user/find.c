#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(const char *path, const char *filename, const char *curr_dir)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot %s\n", path);
        return;
    }
    
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    
    switch (st.type) {
        case T_FILE:
            if (curr_dir && 0 == strcmp(curr_dir, filename))
                printf("%s\n", path);
        break;
        
        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            
            while (read(fd, &de, sizeof(de)) == sizeof(de)){
                if (de.inum == 0)
                    continue;
                
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                find(buf, filename, p);
            }
        break;
    }
    close(fd);
}

void
findfile(const char *path, const char *filename)
{
    find(path, filename, (void*)0);
}

int 
main(int argc, char const *argv[])
{
    if (argc != 3) {
        printf("Usage: find <dir> <filename>");
        exit();
    }
    findfile(argv[1], argv[2]);
    exit();
}