void foo(int *var)
{
  int i=0;
  int j=0;
  int tmp = (*var) * 1000;
  int in=0;

  for(i=0;i<tmp;i++)
  {
    in = tmp*i/(i+1);
    for(j=0;j<tmp;j++)
    {
      in = (tmp*i/(i+1))*j;
    }
  }

  i = in+i;

  fred(var);
}
