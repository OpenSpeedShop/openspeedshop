int f2(int x, int y)
{
    int i, t = 0;

    for(i = 0; i < 10000; i++)
        t += x * y;
    return t;
}
