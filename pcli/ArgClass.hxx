#ifndef ARGCLASS_H
#define ARGCLASS_H
class ArgStruct
{
  public:
    ArgStruct(int ac, char **av)
    {
      argc = ac;
      argv = (char **)calloc( argc, sizeof(char *)*argc );
      for(int i=0;i<argc;i++)
      {
        argv[i] = strdup(av[i]);
      }
    }

    int argc;
    char **argv;
};
#endif // ARGCLASS_H
