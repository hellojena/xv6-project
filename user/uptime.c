#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
	int ticks;
	ticks = uptime();
	fprintf(1, "Uptime: %d ticks\n", ticks);
	exit(0);
}
