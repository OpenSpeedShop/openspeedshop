/* A program that never finishes */
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void fred_read ()
{
  int i;
  int d, d1, d2;
  char buf[256];
  char tmpname_buf[256];

  pid_t pid = getpid();
  sprintf(tmpname_buf, "/tmp/forever_fred_read%dXXXXXX", pid);
  char *tmpname = tmpname_buf;

  d = open(tmpname, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG );

  if( d == -1 )
  {
    fprintf(stderr, "Unable to open %s\n", tmpname );
    return;
  }

  for( i = 0; i < 300; i++ )
  {
    sprintf(buf, "Put this line to a tmpfile %d\n", i );
    int bytes_written = write(d, buf, strlen(tmpname) );
  }
  close( d );
  d1 = open (tmpname, O_RDONLY);
  d2 = open (tmpname, O_RDONLY);
  lseek (d1, 256, SEEK_SET);
  int bytes_read = read (d2, buf, 16);

  close( d1 );
  close( d2 );

  unlink( tmpname );
} 

void frank()
{
  int i, j;
  int t = 0;

  for(i=0; i<1000; i++)
  {
    for(j=0; j<1000; j++)
    {
      t += i*j;
    }
  }
}

void foo()
{
  int i, j;
  int t = 0;

  for(i=0; i<1000; i++)
  {
    for(j=0; j<1000; j++)
    {
      t += i*j;
    }
  }
}

void bill()
{
  int i, j, k;
  int t = 0;

  for(i=0; i<100; i++)
  {
    for(j=0; j<100; j++)
    {
      for(k=0; k<1000; k++)
      {
        t += i*j*k;
      }
    }
  }
}

main(int argc, char **argv)
{
  int i;
  int number = 100;
  char input[1024];

   int size = 0;

   if(argc == 2)
   {
     size = atoi(argv[1]);
     number = size;
   }

  
  if( size )
  {
    printf("%d\n", getpid() );
  } else
  {
    printf("Started a program that never finishes.... pid=%d\n", getpid() );
  }

  while( number > 0 )
  {
    for(i=0;i<number;i++)
    {
      foo();
      frank();
      bill();
//      sleep(1);
      printf(".");
      fflush(stdout);
    }
    input[0] = '\0';
    if( size )
    {
      break;
    }
    printf("\nGive me a number: ");
    scanf("%s", input);
    number = atoi(input);
    printf("Input was %d\n", number);
  }

  fred_read();

  if( !size )
  {
    printf("End of life for the program....\n");
  }
}
