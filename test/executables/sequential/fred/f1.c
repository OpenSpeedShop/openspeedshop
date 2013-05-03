int f1(int x, int y)
{
    int i, t = 0;

    for(i = 0; i < 5000; i++)
        t += x * y;
    return t;
}
