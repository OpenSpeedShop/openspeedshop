int ted(int in)
{
  int i=0;
  int j=0;
  int tmp = in * 100;

  for(i=0;i < tmp;i++)
  {
    in = tmp*i/(i+1);
    for(j=0;j < tmp;j++)
    {
      in = (tmp*i/(i+1))*j;
    }
  }

  return in;
}
