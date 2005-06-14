
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void mysleep() {
	printf("I am sleeping\n");
}
int main(int argc, char* argv[]) {
    int i, n;
    if(argc != 2) {
	printf("Usage: %s <cound>\n", argv[0]);
	exit(1); }
    n = atoi(argv[1]);
    for(i = 0; i < n; i++) {
	mysleep(); }		
    exit(7);
}
