//////////////////////////////// -*- C++ -*- //////////////////////////////
//
// AUTHOR
//    Peter Shirley, Cornell University, shirley@graphics.cornell.edu 
//
// COPYRIGHT
//    Copyright (c) 1995  Peter Shirley. All rights reserved.
//
//    Permission to use, copy, modify, and distribute this software for any
//    purpose without fee is hereby granted, provided that this entire
//    notice is included in all copies of any software which is or includes
//    a copy or modification of this software and in all copies of the
//    a copy or modification of this software and in all copies of the
//    supporting documentation for such software.
//
// DISCLAIMER
//    Neither the Peter Shirley nor Cornell nor any of their
//    employees, makes any warranty, express or implied, or assumes any
//    liability or responsibility for the accuracy, completeness, or
//    usefulness of any information, apparatus, product, or process
//    disclosed, or represents that its use would not infringe
//    privately-owned rights.  
//
///////////////////////////////////////////////////////////////////////////



#ifndef GGSTRING_H
#define GGSTRING_H


#include <string.h>
#include <iostream.h>
#include <stddef.h>
#include <ggBoolean.h>



class ggString {

    friend ostream &operator<<(ostream &, const ggString &);
    friend istream &operator>>(istream &, ggString &);
    friend ggString operator+(const ggString &, const ggString &);
    friend ggBoolean operator==(const ggString &, const ggString &);
    friend ggBoolean operator==(const ggString &, const char *);
    friend ggBoolean operator==(const char *, const ggString &);
    friend ggBoolean operator!=(const ggString &, const ggString &);
    friend ggBoolean operator!=(const ggString &, const char *);
    friend ggBoolean operator!=(const char *, const ggString &);
    friend ggBoolean operator>(const ggString &, const ggString &);
    friend ggBoolean operator>(const ggString &, const char *);
    friend ggBoolean operator>(const char *, const ggString &);
    friend ggBoolean operator>=(const ggString &, const ggString &);
    friend ggBoolean operator>=(const ggString &, const char *);
    friend ggBoolean operator>=(const char *, const ggString &);
    friend ggBoolean operator<(const ggString &, const ggString &);
    friend ggBoolean operator<(const ggString &, const char *);
    friend ggBoolean operator<(const char *, const ggString &);
    friend ggBoolean operator<=(const ggString &, const ggString &);
    friend ggBoolean operator<=(const ggString &, const char *);
    friend ggBoolean operator<=(const char *, const ggString &);

public:

    ggString(const ggString &);
    ggString(int len = 0);
    ggString(const char *);
    ~ggString();

    operator const char *() const { return rep->str; }
  
  int length() const { return rep->len; }
  

    ggString &operator=(char *s) { return *this = (const char *) s; }
    ggString &operator=(const char *);

//sam philp ---------------------------------------------------
    ggString &operator=(const ggString&);
//sam philp ---------------------------------------------------


    const char &operator[](int i) { return rep->str[i]; }

private:

    struct StringRep {
        StringRep(int len);
        ~StringRep();
        void *operator new(size_t, unsigned long len);
        void *operator new(size_t s) { return new char[s]; }
//        void operator delete(void *p) { delete p; }
        int len;
        int refs;
        char str[1];
    };

    StringRep *rep;
};



#endif
