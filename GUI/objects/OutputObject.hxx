#ifndef OUTPUTOBJECT_H
#define OUTPUTOBJECT_H

class OutputObject
{
public:
    OutputObject();
    OutputObject(char *output_buffer);
    ~OutputObject();

    void print();

    char *outputBuffer;
};
#endif // OUTPUTOBJECT_H
