int f3(int x, int y)
{
    int i, t = 0;

    for(i = 0; i < 15000; i++)
        t += x * y;
    return t;
}
