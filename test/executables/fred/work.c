int work(int size)
{
    extern int f3(int, int);
    extern int f2(int, int);
    extern int f1(int, int);
    int i, j, t = 0;

    for(i = 0; i < size; i++)
        for(j = 0; j < size; j++)
            t += f3(i, j) - f2(i, j) - f1(i, j);
    return t;
}
