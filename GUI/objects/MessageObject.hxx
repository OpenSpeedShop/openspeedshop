#ifndef MESSAGEOBJECT_H
#define MESSAGEOBJECT_H

#include <qstring.h>

//! The base message class that contains a simple QString type of the message.
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
