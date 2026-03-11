#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int is_delim(char c){
	char *d = " -\r\t\n./,^";
	for (int i = 0; d[i]; i++){
		if (c == d[i])
			return 1;
	}
	return 0;
}

void process(int fd){
	int n = 0;
	char buf[512];
	char prev = ' ';
	int innum = 0;
	int num = 0;


	while ((n = read(fd, buf, sizeof(buf))) > 0){

		for (int i = 0; i < n ; i ++ ){
			char c = buf[i];
			if (c >= '0' && c <= '9'){
				if(!innum){
					if (is_delim(prev))
						innum = 1;
					else
						innum = -1;
					num = 0;
				}

				if (innum == 1)
					num = num * 10 + (c- '0');
			}
			else{
				if (innum == 1){
					if (is_delim(c)){
						if (num % 5 == 0 || num % 6 == 0)
							printf("%d\n", num);
					}
				}
				num = 0;
				innum = 0;
			}

			prev = c;

		}
	}
	if (innum == 1)
		printf("%d\n", num);
}

int
main(int argc,char *argv[]){
	int fd;	

	if (argc < 2) {
		fprintf(2, "Please input the file name");
		exit(1);
	}
	else{

		for (int i  = 1; i < argc; i++){
			if ((fd = open(argv[i], 0)) < 0){
				fprintf(2, "file %s doesn't exist", argv[i]);
				continue;

			}

			process(fd);
			close(fd);
		}
		exit(0);	
	}



}
