#ifndef MESSAGEOBJECT_H
#define MESSAGEOBJECT_H

#include <qstring.h>

class MessageObject
{
public:
    MessageObject();
    MessageObject(QString  msg_type);
    ~MessageObject();

    void print();

    QString msgType;
};
#endif // MESSAGEOBJECT_H
