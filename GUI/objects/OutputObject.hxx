#ifndef OUTPUTOBJECT_H
#define OUTPUTOBJECT_H

//! This is the return object from a cli command.  -prototype
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
