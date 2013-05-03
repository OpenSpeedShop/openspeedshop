#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


int main(int argc, char* argv[])
{
    
    char testbuf[1024];
    char *mypwd = getenv("PWD");
    pid_t mypid = getpid();
    sprintf(testbuf,"%s/%s%d",mypwd,"test",mypid);

    int retval = 0;
    int fd1 = 0;
    extern int errno;

    printf("\nTESTING creat.\n");
    errno = 0;
    fd1 = creat(testbuf,S_IRWXU);
    if (fd1 < 0 ) {
        perror("creat");
    } else {
	printf("created %s, fd1 = %d\n",testbuf,fd1);
    }

    printf("\nTESTING write.\n");
    errno = 0;
    retval = write(fd1,"HELLO WORLD\n",sizeof("HELLO WORLD\n"));
    if (retval < 0 ) {
        perror("write");
    } else {
	printf("write %s, fd1 = %d, count %d\n",
		"HELLO WORLD",fd1,sizeof("HELLO WORLD\n"));
	printf("write returns %d\n",retval);
    }

    printf("\nTESTING open.\n");
    errno = 0;
    int openFD = open(testbuf,O_RDONLY,S_IRWXU);
    if (openFD < 0 ) {
	perror("open");
    } else {
        printf("opened %s, fd = %d\n",testbuf,openFD);
	printf("open returns %d\n",openFD);
    }

    printf("\nTESTING read.\n");
    errno = 0;
    char readbuf[1024] = {0};
    printf("attempting to read 9 bytes from fd %d\n",openFD);
    retval = read(openFD,readbuf,9);
    if (retval < 0 ) {
        perror("read");
    } else {
        printf("read %s from fd %d\n",readbuf, openFD);
    }

    printf("\nTESTING lseek.\n");
    errno = 0;
    off_t myoffset = lseek(fd1,0,SEEK_SET);
    if (myoffset < 0 ) {
        perror("lseek");
    } else {
	printf("lseek(%d,0,%d) success\n",fd1,SEEK_SET);
    }

    printf("\nTESTING pread.\n");
    errno = 0;
    char preadbuf[1024] = {0};
    printf("attempting to read 9 bytes from fd %d at offset 1\n",openFD);
    retval = pread(openFD,preadbuf,9,1);
    if (retval < 0 ) {
        perror("pread");
    } else {
        printf("pread %s from fd %d\n",preadbuf, openFD);
    }

    printf("\nTESTING dup2.\n");
    errno = 0;
    int fd2 = fd1;
    retval = dup2(openFD,fd2);
    if (retval < 0 ) {
        perror("dup2");
    } else {
	printf("dup2 fd %d\n",fd2);
    }

    printf("\nTESTING dup.\n");
    errno = 0;
    int fd3 = -1;
    retval = dup(fd1);
    if (retval < 0 ) {
        perror("dup");
    } else {
	fd3 = retval;
	printf("dup fd %d\n",fd3);
    }

    printf("\nTESTING pipe.\n");
    errno = 0;
    int pdes[2];
    retval = pipe(pdes);
    if (retval < 0 ) {
        perror("pipe");
    } else {
	printf("pipe pdes[0]=%d, pdes[1]=%d\n",pdes[0],pdes[1]);
    }

    printf("\nTESTING close.\n");
    errno = 0;
    retval = close(fd1);
    if (retval < 0 ) {
        perror("close");
    } else {
	printf("closed fd %d\n",fd1);
    }

    return 0;
}
