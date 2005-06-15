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



#include <iostream.h>
#include <ggString.h>



/******************************************************************************
* ggString
*/

/*
 * Constructors/destructors
 */

ggString::ggString(const ggString &s) {
    rep = s.rep;
    rep->refs++;
}
ggString::ggString(int l) {
    rep = new (l) StringRep(l);
}
ggString::ggString(const char *s) {
    int l = strlen(s);
    rep = new (l) StringRep(l);
    strcpy(rep->str, s);
}
ggString::~ggString() {
    if (--rep->refs == 0)
        delete rep;
}

/*
 * Operators
 */

ggString &
ggString::operator=(const char *s) {
    if (--rep->refs == 0)
        delete rep;
    int l = strlen(s);
    rep = new (l) StringRep(l);
    strcpy(rep->str, s);
    return *this;
}


// sam philp -----------------------------------------------------
ggString &
ggString::operator=(const ggString& s) {
    if (this == &s) return *this;
    if (--rep->refs == 0)
        delete rep;
    const char * tmp = s;
    int l = strlen(tmp);
    rep = new (l) StringRep(l);
    strcpy(rep->str, tmp);
    return *this;
}
// sam philp -----------------------------------------------------



ggString operator+(const ggString &s1, const ggString &s2) {
    ggString s(s1.rep->len + s2.rep->len);
    strcpy(s.rep->str, s1.rep->str);
    strcat(s.rep->str, s2.rep->str);
    return s;
}

/*
 * Friends
 */

ggBoolean operator==(const ggString &s1, const ggString &s2) {
    return strcmp(s1.rep->str, s2.rep->str) == 0; }
ggBoolean operator==(const ggString &s1, const char *s2) {
    return strcmp(s1.rep->str, s2) == 0; }
ggBoolean operator==(const char *s1, const ggString &s2) {
    return strcmp(s1, s2.rep->str) == 0; }
ggBoolean operator!=(const ggString &s1, const ggString &s2) {
    return strcmp(s1.rep->str, s2.rep->str) != 0; }
ggBoolean operator!=(const ggString &s1, const char *s2) {
    return strcmp(s1.rep->str, s2) != 0; }
ggBoolean operator!=(const char *s1, const ggString &s2) {
    return strcmp(s1, s2.rep->str) != 0; }
ggBoolean operator>(const ggString &s1, const ggString &s2) {
    return strcmp(s1.rep->str, s2.rep->str) > 0; }
ggBoolean operator>(const ggString &s1, const char *s2) {
    return strcmp(s1.rep->str, s2) > 0; }
ggBoolean operator>(const char *s1, const ggString &s2) {
    return strcmp(s1, s2.rep->str) > 0; }
ggBoolean operator>=(const ggString &s1, const ggString &s2) {
    return strcmp(s1.rep->str, s2.rep->str) >= 0; }
ggBoolean operator>=(const ggString &s1, const char *s2) {
    return strcmp(s1.rep->str, s2) >= 0; }
ggBoolean operator>=(const char *s1, const ggString &s2) {
    return strcmp(s1, s2.rep->str) >= 0; }
ggBoolean operator<(const ggString &s1, const ggString &s2) {
    return strcmp(s1.rep->str, s2.rep->str) < 0; }
ggBoolean operator<(const ggString &s1, const char *s2) {
    return strcmp(s1.rep->str, s2) < 0; }
ggBoolean operator<(const char *s1, const ggString &s2) {
    return strcmp(s1, s2.rep->str) < 0; }
ggBoolean operator<=(const ggString &s1, const ggString &s2) {
    return strcmp(s1.rep->str, s2.rep->str) <= 0; }
ggBoolean operator<=(const ggString &s1, const char *s2) {
    return strcmp(s1.rep->str, s2) <= 0; }
ggBoolean operator<=(const char *s1, const ggString &s2) {
    return strcmp(s1, s2.rep->str) <= 0; }

ostream &operator<<(ostream &os, const ggString &s) {
    return os << s.rep->str;
}


// added by PS.  Hope it works!
istream &operator>>(istream &is, ggString &s) {
    char temp[256];
    is >> temp;
// sam philp  -------------------------	if (is) s = ggString(temp);
    if (is) s = temp;
    return is;
}



/******************************************************************************
* StringRep
*/

ggString::StringRep::StringRep(int l) {
    len = l;
    refs = 1;
    str[0] = '\0';
}

ggString::StringRep::~StringRep() {
    if (refs != 0) {
        cerr << "StringRep::~StringRep() StringRep with references destroyed."
             << endl;
    }
}

void *
ggString::StringRep::operator new(size_t s, unsigned long l) {
    return new char[s + l];
}



/* vi:set ts=4 sw=4: */
