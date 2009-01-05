float work(float size)
{
    extern float f1(float);
    float i, j, t = 0.0;

    printf("In work, calling f1 with size=%d\n", size);
    i = f1(size);
    printf("In work, after calling f1 returns i=%d\n", i);
}
