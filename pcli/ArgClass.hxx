#ifndef ARGCLASS_H
#define ARGCLASS_H
class ArgStruct
{
  public:
    ArgStruct(int ac, char **av)
    {
      argc = ac;
      argv = (char **)calloc( argc, sizeof(char *) * argc );
      for(int i=0;i<argc;i++)
      {
        argv[i] = strdup(av[i]);
      }
    }

    void addArg(char *a)
    {
      char **old_argv = argv;

      argv = (char **)calloc( argc, sizeof(char *) * (argc+1) );
      int i = 0;
      for(i=0;i<argc;i++)
      {
        argv[i] = strdup(old_argv[i]);
      }
      argv[i] = strdup(a);
      argc++;
    }

    int argc;
    char **argv;
};
#endif // ARGCLASS_H
