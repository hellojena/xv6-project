#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"


char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // find first character after last '/'
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if(strlen(p) >= DIRSIZ)
    return p;

  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

void run_exec(int cmd_argc, char **cmd_argv, char *file){
	char *argv[32];
	int i;

	for (i=0; i< cmd_argc; i++){
		//printf("%s\n", cmd_argv[i]);
		argv[i] = cmd_argv[i];
	}
	argv[cmd_argc] = file;
	argv[cmd_argc +1] = 0;

	if (fork() == 0){
		exec(cmd_argv[0], argv);
		fprintf(2, "exec %s failed\n", cmd_argv[0]);
		exit(1);
	}
	wait(0);
}

void find(char *path, char *name, int exec_flag, int cmd_argc, char **cmd_argv){
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;


	if ((fd = open(path, 0)) < 0){
		printf("find: cannot open %s\n", path);
		return;
	}

	if (stat(path, &st) < 0){
		printf("find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch (st.type){
		case T_FILE:{
			if (strcmp(fmtname(path), name) == 0){
				if (exec_flag)
					run_exec(cmd_argc, cmd_argv, path);
				else
					printf("%s\n",path);
			}
			break;
		}
		case T_DIR:{
			if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
				fprintf(2, "find: path too long\n");
				break;
			}
			strcpy(buf, path);
			p = buf + strlen(buf);
			*p++ = '/';

			while(read(fd, &de, sizeof(de)) == sizeof(de)){
				if (de.inum == 0)
					continue;
				if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
					continue;

				memmove(p, de.name, DIRSIZ);
				p[DIRSIZ] = 0;

				find(buf, name, exec_flag,cmd_argc, cmd_argv);

			}
			break;

		}
	}
	close(fd);

}

int
main(int argc, char *argv[]){
	int exec_flag = 0;
	char **cmd_argv = 0;
	int cmd_argc = 0;

	if (argc < 3) {
		fprintf(2,"usage: find path filename [-exec cmd]\n ");
		exit(1);
	}

	if (argc >= 4){
		if (strcmp(argv[3], "-exec") != 0 || argc < 5){
			fprintf(2, "usage:find path filename [-exec cmd]\n");
			exit(1);
		}
		exec_flag = 1;
		cmd_argv = &argv[4];
		cmd_argc = argc -4;
	}


	find(argv[1], argv[2], exec_flag, cmd_argc, cmd_argv);
	exit(0);
}
