/*******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2014 Krell Institute  All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Definition of the CommandResult_(objects) class.
 *
 */

#include <cmath>

// The CommandResult_(objects)

// types of results that can be returned in a CommandObject
enum cmd_result_type_enum {
/* 0 */  CMD_RESULT_NULL,
/* 1 */  CMD_RESULT_ADDRESS,
/* 2 */  CMD_RESULT_UINT,
/* 3 */  CMD_RESULT_INT,
/* 4 */  CMD_RESULT_FLOAT,
/* 5 */  CMD_RESULT_STRING,
/* 6 */  CMD_RESULT_RAWSTRING,
/* 7 */  CMD_RESULT_FUNCTION,
/* 8 */  CMD_RESULT_STATEMENT,
/* 9 */  CMD_RESULT_LINKEDOBJECT,
/* 10 */ CMD_RESULT_LOOP,
/* 11 */ CMD_RESULT_CALLTRACE,
/* 12 */ CMD_RESULT_TIME,
/* 13 */ CMD_RESULT_DURATION,
/* 14 */ CMD_RESULT_INTERVAL,
/* 15 */ CMD_RESULT_TITLE,
/* 16 */ CMD_RESULT_COLUMN_HEADER,
/* 17 */ CMD_RESULT_COLUMN_VALUES,
/* 18 */ CMD_RESULT_COLUMN_ENDER,
/* 19 */ CMD_RESULT_EXTENSION,
};

class PrintControl {

 public:
  int64_t num_columns;
  int64_t *column_widths;
  int64_t field_size;
  std::string eoc;
  std::string eol;
  std::string eov;
  int64_t precision;
  int64_t date_time_precision;
  bool field_size_dynamic;
  bool full_path;
  bool entire_string;
  bool defining_location;
  bool mangled_name;
  bool blank_in_place_of_zero;
  bool left_justify_all;

  PrintControl () {
    column_widths = NULL;
    num_columns = 0;
    field_size = OPENSS_VIEW_FIELD_SIZE;
    eoc = OPENSS_VIEW_EOC;
    eol = OPENSS_VIEW_EOL;
    eov = OPENSS_VIEW_EOV;
    precision = OPENSS_VIEW_PRECISION;
    date_time_precision = OPENSS_VIEW_DATE_TIME_PRECISION;
    field_size_dynamic = OPENSS_VIEW_FIELD_SIZE_IS_DYNAMIC;
    full_path = OPENSS_VIEW_FULLPATH;
    entire_string = OPENSS_VIEW_ENTIRE_STRING;
    defining_location = OPENSS_VIEW_DEFINING_LOCATION;
    mangled_name = OPENSS_VIEW_MANGLED_NAME;
    blank_in_place_of_zero = OPENSS_VIEW_USE_BLANK_IN_PLACE_OF_ZERO;
    left_justify_all = false;
  }

  ~PrintControl () { column_widths = NULL; }

  void Set_PrintControl_column_widths(int64_t n, int64_t *w) { num_columns = n; column_widths = w; }
  void Set_PrintControl_field_size(int64_t l) { field_size = l; }
  void Set_PrintControl_eoc(std::string c) { eoc = c; }
  void Set_PrintControl_eol(std::string c) { eol = c; }
  void Set_PrintControl_eov(std::string c) { eov = c; }
  void Set_PrintControl_precision(int64_t p) { precision = p; }
  void Set_PrintControl_date_time_precision(int64_t p) { date_time_precision = p; }
  void Set_PrintControl_dynamic_size(bool b) { field_size_dynamic = b; }
  void Set_PrintControl_full_path(bool b) { full_path = b; }
  void Set_PrintControl_entire_string(bool b) { entire_string = b; }
  void Set_PrintControl_defining_location(bool b) { defining_location = b; }
  void Set_PrintControl_mangled_name(bool b) { mangled_name = b; }
  void Set_PrintControl_blank_in_place_of_zero(bool b) { blank_in_place_of_zero = b; }
  void Set_PrintControl_left_justify_all(bool b) { left_justify_all = b; }
};

// Adjust size of string when the field is to be placed left justified into a fixed size field.
// If there is unused space in the field, pad with blanks.
inline std::string Left_Justify( int64_t fieldsize, std::string S ) {
 // Find the first non-blank character.
  int64_t stringsize = S.length();
  int64_t first_non_blank = 0;
  for ( ; first_non_blank < stringsize; first_non_blank++) {
    if (S.substr(first_non_blank, 1) != (" ")) {
      break;
    }
  }

  std::string R = S.substr(first_non_blank, (stringsize - first_non_blank));

  if (R.length() < fieldsize) {
    for (int64_t j=R.length(); j < fieldsize; j++) {
      R += " ";
    }
  } else if (R.length() > fieldsize) {
    return R.substr( 0, fieldsize );
  }
  return R;
}

class CommandResult {
  cmd_result_type_enum Result_Type;
  bool ValueIsID;  // ID's can not be combined or blanked.

 private:
  CommandResult () {
    Result_Type = CMD_RESULT_NULL; }

 public:
  CommandResult (cmd_result_type_enum T) {
    Result_Type = T;
    ValueIsID = false; }
  cmd_result_type_enum Type () {
    return Result_Type;
  }
  bool IsValueID () { return ValueIsID; }
  void SetValueIsID () { ValueIsID = true; }
  void ClearValueIsID () { ValueIsID = false; }
  virtual ~CommandResult () { }

  virtual CommandResult *Init () { std::cerr << "Init operation not implimented\n"; return NULL; }
  virtual CommandResult *Copy () { std::cerr << "Copy operation not implimented\n"; return NULL; }
  virtual bool LT (CommandResult *A) { std::cerr << "< operation not implimented\n"; return false; }
  virtual bool GT (CommandResult *A) { std::cerr << "> operation not implimented\n"; return false; }
  virtual void Accumulate_Value (CommandResult *A) { std::cerr << "Sum operation not implimented\n"; }
  virtual void AbsDiff_value (CommandResult *A) { std::cerr << "Difference operation not implimented\n"; }
  virtual void Accumulate_Min (CommandResult *A) { std::cerr << "Min operation not implimented\n"; }
  virtual void Accumulate_Max (CommandResult *A) { std::cerr << "Max operation not implimented\n"; }
  virtual bool ValueIsNull () { return false; }

  void Value (char *&C) {
    C = NULL;
  }

  virtual std::string Form (int64_t fieldsize) {
    return std::string("");
  }
  virtual std::string Form () {
    return Form (OPENSS_VIEW_FIELD_SIZE);
  }
  virtual PyObject * pyValue () {
    return Py_BuildValue("");
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column=0) {
    int64_t fieldsize = strlen("(none)");
    if ( (pc.column_widths != NULL) &&
         (column >= 0) &&
         (pc.column_widths[column] != 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    to << (leftjustified ? std::setiosflags(std::ios::left) : std::setiosflags(std::ios::right))
       << std::setw(fieldsize) << "(none)";
  }
  virtual void Print (std::ostream& to, int64_t fieldsize=20, bool leftjustified=false) {
    PrintControl default_format;
    default_format.Set_PrintControl_field_size( fieldsize );
    default_format.Set_PrintControl_left_justify_all( leftjustified );
    Print ( to, default_format, 0 );  // Invoke format controlled print.
  }
};

// Reclaim CommandResult objects within other structures.
inline void Reclaim_CR_Space ( CommandResult *R) {
  if (R != NULL) delete R;
}

template <typename TL>
void Reclaim_CR_Space ( std::list<TL>& L) {
  typename std::list<TL>::iterator li;
  for ( li = L.begin(); li != L.end(); li++) {
    Reclaim_CR_Space (*li);
  }
}

template <typename TE>
void Reclaim_CR_Space ( std::map<TE, CommandResult *>& V) {
  typename std::map<TE, CommandResult *>::iterator vi;
  for ( vi = V.begin(); vi != V.end(); vi++) {
    Reclaim_CR_Space ((*vi).second);
  }
}

template <typename TV>
void Reclaim_CR_Space ( std::vector<TV>& V) {
  typename std::vector<TV>::iterator vi;
  for ( vi = V.begin(); vi != V.end(); vi++) {
    Reclaim_CR_Space (*vi);
  }
}

template  <typename TS>
void Reclaim_CR_Space ( std::pair<Function, TS>& P ) {
  Reclaim_CR_Space (P.second);
}

template  <typename TS>
void Reclaim_CR_Space ( std::pair<Statement, TS>& P ) {
  Reclaim_CR_Space (P.second);
}

template  <typename TS>
void Reclaim_CR_Space ( std::pair<LinkedObject, TS>& P ) {
  Reclaim_CR_Space (P.second);
}

template  <typename TF, typename TS>
void Reclaim_CR_Space ( std::pair<TF, TS>& P ) {
  Reclaim_CR_Space (P.first);
  Reclaim_CR_Space (P.second);
}

template  <typename TE>
void Reclaim_CR_Space ( SmartPtr<TE>& P) {
  Reclaim_CR_Space (*P);
}

template <typename TV>
void Reclaim_CR_Space (
              int64_t starting_with,
              std::vector<TV>& V) {
  typename std::vector<TV>::iterator vi;
  for (vi = (V.begin() + starting_with); vi != V.end(); vi++) {
    Reclaim_CR_Space (*vi);
  }
}

class CommandResult_Address :
     public CommandResult {
  uint64_t uint_value;

 public:
  CommandResult_Address (uint64_t U = 0) : CommandResult(CMD_RESULT_ADDRESS) {
    uint_value = U;
  }
  CommandResult_Address (CommandResult_Address *C) :
       CommandResult(CMD_RESULT_ADDRESS) {
    uint_value = C->uint_value;
    if (C->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_Address () { }

  virtual CommandResult *Init () { return new CommandResult_Address (); }
  virtual CommandResult *Copy () { return new CommandResult_Address (this); }
  virtual bool LT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Address));
    return uint_value < ((CommandResult_Address *)A)->uint_value; }
  virtual bool GT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Address));
    return uint_value > ((CommandResult_Address *)A)->uint_value; }
  virtual void Accumulate_Value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Address));
    if (!IsValueID()) uint_value += ((CommandResult_Address *)A)->uint_value; }
  virtual void AbsDiff_value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Address));
    if (uint_value >= ((CommandResult_Address *)A)->uint_value) {
      uint_value -= ((CommandResult_Address *)A)->uint_value;
    } else {
      uint_value = ((CommandResult_Address *)A)->uint_value - uint_value;
    }
  }
  virtual void Accumulate_Min (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Address));
    uint_value = std::min (uint_value, ((CommandResult_Address *)A)->uint_value); }
  virtual void Accumulate_Max (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Address));
    uint_value = std::max (uint_value, ((CommandResult_Address *)A)->uint_value); }
  virtual bool ValueIsNull () { return (uint_value == (uint64_t)0 && !IsValueID()); }

  void Value (uint64_t& U) {
    U = uint_value;
  }

  virtual std::string Form (int64_t fieldsize) {
    char s[OPENSS_VIEW_FIELD_SIZE];
    sprintf ( s, "0x%llx", static_cast<long long unsigned int>(uint_value));
    return std::string (s);
  }
  virtual PyObject * pyValue () {
    return Py_BuildValue("l", uint_value);
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string S = Form(fieldsize);
    if (fieldsize == 0) fieldsize = S.length();
    to << (leftjustified ? std::setiosflags(std::ios::left) : std::setiosflags(std::ios::right))
       << std::setw(fieldsize) << S;
  }
};

class CommandResult_Uint :
     public CommandResult {
  uint64_t uint_value;

 public:
  CommandResult_Uint (uint64_t U = 0) : CommandResult(CMD_RESULT_UINT) {
    uint_value = U;
  }
  CommandResult_Uint (CommandResult_Uint *C) :
       CommandResult(CMD_RESULT_UINT) {
    uint_value = C->uint_value;
    if (C->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_Uint () { }

  virtual CommandResult *Init () { return new CommandResult_Uint (); }
  virtual CommandResult *Copy () { return new CommandResult_Uint (this); }
  virtual bool LT (CommandResult *A); //  defined in SS_CommandResult.cxx
  virtual bool GT (CommandResult *A); //  defined in SS_CommandResult.cxx
  virtual void Accumulate_Value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Uint));
    if (!IsValueID()) uint_value += ((CommandResult_Uint *)A)->uint_value; }
  virtual void AbsDiff_value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Uint));
    if (uint_value >= ((CommandResult_Uint *)A)->uint_value) {
      uint_value -= ((CommandResult_Uint *)A)->uint_value;
    } else {
      uint_value = ((CommandResult_Uint *)A)->uint_value - uint_value;
    }
  }
  virtual void Accumulate_Min (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Uint));
    uint_value = std::min (uint_value, ((CommandResult_Uint *)A)->uint_value); }
  virtual void Accumulate_Max (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Uint));
    uint_value = std::max (uint_value, ((CommandResult_Uint *)A)->uint_value); }
  virtual bool ValueIsNull () { return (uint_value == (uint64_t)0 && !IsValueID()); }

  void Value (uint64_t& U) {
    U = uint_value;
  }

  virtual std::string Form (int64_t fieldsize) {
    char s[OPENSS_VIEW_FIELD_SIZE];
    sprintf ( s, "%lld", static_cast<long long int>(uint_value));
    return std::string (s);
  }
  virtual PyObject * pyValue () {
    return Py_BuildValue("l", uint_value);
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string S = Form(fieldsize);
    if (fieldsize == 0) fieldsize = S.length();
    to << (leftjustified ? std::setiosflags(std::ios::left) : std::setiosflags(std::ios::right))
       << std::setw(fieldsize) << S;
  }
};

class CommandResult_Int :
     public CommandResult {
  int64_t int_value;

 public:
  CommandResult_Int (CommandResult_Uint *U) : CommandResult(CMD_RESULT_INT) {
    uint64_t Uvalue;
    U->Value(Uvalue);
    int_value = Uvalue;
  }
  CommandResult_Int (int64_t I = 0) : CommandResult(CMD_RESULT_INT) {
    int_value = I;
  }
  CommandResult_Int (CommandResult_Int *C) :
       CommandResult(CMD_RESULT_INT) {
    int_value = C->int_value;
    if (C->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_Int () { }

  virtual CommandResult *Init () { return new CommandResult_Int (); }
  virtual CommandResult *Copy () { return new CommandResult_Int (this); }
  virtual bool LT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Int));
    return int_value < ((CommandResult_Int *)A)->int_value; }
  virtual bool GT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Int));
    return int_value > ((CommandResult_Int *)A)->int_value; }
  virtual void Accumulate_Value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Int));
    if (!IsValueID()) int_value += ((CommandResult_Int *)A)->int_value; }
  virtual void AbsDiff_value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Int));
    if (int_value >= ((CommandResult_Int *)A)->int_value) {
      int_value -= ((CommandResult_Int *)A)->int_value;
    } else {
      int_value = ((CommandResult_Int *)A)->int_value - int_value;
    }
  }
  virtual void Accumulate_Min (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Int));
    int_value = std::min (int_value, ((CommandResult_Int *)A)->int_value); }
  virtual void Accumulate_Max (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Int));
    int_value = std::max (int_value, ((CommandResult_Int *)A)->int_value); }
  virtual bool ValueIsNull () { return (int_value == (int64_t)0 && !IsValueID()); }

  void Value (int64_t& I) {
    I = int_value;
  }

  virtual std::string Form (int64_t fieldsize) {
    char s[OPENSS_VIEW_FIELD_SIZE];
    sprintf ( s, "%lld", static_cast<long long int>(int_value));
    return std::string (s);
  }
  virtual PyObject * pyValue () {
    return Py_BuildValue("l", int_value);
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string S = Form(fieldsize);
    if (fieldsize == 0) fieldsize = S.length();
    to << (leftjustified ? std::setiosflags(std::ios::left) : std::setiosflags(std::ios::right))
       << std::setw(fieldsize) << S;
  }
};

class CommandResult_Float :
     public CommandResult {
  double float_value;

 public:
  CommandResult_Float (double f = 0.0) : CommandResult(CMD_RESULT_FLOAT) {
    float_value = f;
  }
  CommandResult_Float (CommandResult_Uint *U) : CommandResult(CMD_RESULT_FLOAT) {
    uint64_t Uvalue;
    U->Value(Uvalue);
    float_value = Uvalue;
  }
  CommandResult_Float (CommandResult_Int *I) : CommandResult(CMD_RESULT_FLOAT) {
    int64_t Ivalue;
    I->Value(Ivalue);
    float_value = Ivalue;
  }
  CommandResult_Float (CommandResult_Float *C) :
       CommandResult(CMD_RESULT_FLOAT) {
    float_value = C->float_value;
    if (C->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_Float () { }

  virtual CommandResult *Init () { return new CommandResult_Float (); }
  virtual CommandResult *Copy () { return new CommandResult_Float (this); }
  virtual bool LT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Float));
    return float_value < ((CommandResult_Float *)A)->float_value; }
  virtual bool GT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Float));
    return float_value > ((CommandResult_Float *)A)->float_value; }
  virtual void Accumulate_Value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Float));
    if (!IsValueID()) float_value += ((CommandResult_Float *)A)->float_value; }
  virtual void AbsDiff_value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Float));
    if (float_value >= ((CommandResult_Float *)A)->float_value) {
      float_value -= ((CommandResult_Float *)A)->float_value;
    } else {
      float_value = ((CommandResult_Float *)A)->float_value - float_value;
    }
  }
  virtual void Accumulate_Min (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Float));
    float_value = std::min (float_value, ((CommandResult_Float *)A)->float_value); }
  virtual void Accumulate_Max (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Float));
    float_value = std::max (float_value, ((CommandResult_Float *)A)->float_value); }
  virtual bool ValueIsNull () { return (float_value == (double)0.0 && !IsValueID()); }

  void Value (double& F) {
    F = float_value;
  }

  virtual std::string Form (int64_t fieldsize) {
    if (std::isnan(float_value)) return std::string ("nan");
    char F[20];
    F[0] = *("%"); // left justify in field
    sprintf(&F[1], "%lld.%lldf\0", static_cast<long long int>(fieldsize),
            static_cast<long long int>(OPENSS_VIEW_PRECISION));

    char s[OPENSS_VIEW_FIELD_SIZE];
    sprintf ( s, &F[0], float_value);
    return std::string (s);
  }
  virtual PyObject * pyValue () {
    return Py_BuildValue("d", float_value);
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string S = Form(fieldsize);
    if (fieldsize == 0) fieldsize = S.length();
    if (leftjustified) {
      S = Left_Justify( fieldsize, S );
    }
    to << (leftjustified ? std::setiosflags(std::ios::left) : std::setiosflags(std::ios::right))
       << std::setw(fieldsize) << S;
  }
};

class CommandResult_String :
     public CommandResult {
  std::string string_value;

 public:
  CommandResult_String (const char *S = "") : CommandResult(CMD_RESULT_STRING) {
    string_value = std::string(S);
  }
  CommandResult_String (std::string S) : CommandResult(CMD_RESULT_STRING) {
    string_value = S;
  }
  CommandResult_String (CommandResult_String *C) :
       CommandResult(CMD_RESULT_STRING) {
    string_value = C->string_value;
    if (C->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_String () { }

  virtual CommandResult *Init () { return new CommandResult_String (""); }
  virtual CommandResult *Copy () { return new CommandResult_String (this); }
  virtual bool LT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_String));
    return string_value < ((CommandResult_String *)A)->string_value; }
  virtual bool GT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_String));
    return string_value > ((CommandResult_String *)A)->string_value; }
  virtual void Accumulate_Value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_String));
    if (!IsValueID()) string_value += ((CommandResult_String *)A)->string_value; }
  virtual bool ValueIsNull () { return string_value.length() == 0; }

  void Accumulate_String (CommandResult_String *B) {
    string_value += B->string_value;
  }
  void Value (std::string& S) {
    S = string_value;
  }

  virtual std::string Form (int64_t fieldsize) {
    return string_value;
  }
  virtual PyObject * pyValue () {
    return Py_BuildValue("s",string_value.c_str());
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    if (fieldsize == 0) fieldsize = string_value.length();
    if (leftjustified) {
     // Left justification is only done on the last column of a report.
     // Don't truncate the string if it is bigger than the field size.
     // This is done to make sure everything gets printed.

      to << std::setiosflags(std::ios::left) << string_value;

     // If there is unused space in the field, pad with blanks.
      if ((string_value.length() < fieldsize) &&
          (string_value[string_value.length()-1] != *("\n"))) {
        for (int64_t i = string_value.length(); i < fieldsize; i++) to << " ";
      }

    } else if (string_value.length() > fieldsize) {
     // The string is too big for the field.
     // Decide how, or if, to truncate the string.
      if (pc.entire_string) {
       // Ignore the field size and print the full string.
        to << std::setiosflags(std::ios::left) << string_value;
      } else {
       // Don't let it exceed the size of the field.
       // Print leading indicator to signal incomplete value.
        to << std::setiosflags(std::ios::left) << "???"
           << string_value.substr((string_value.length() - fieldsize + 3), string_value.length());
      }
    } else {
     // Right justify the string in the field.
     // Don't let it exceed the size of the field.
     // Also, limit the size based on our internal buffer size.
      to << std::setiosflags(std::ios::right) << std::setw(fieldsize)
         << ((string_value.length() <= fieldsize) ? string_value : string_value.substr(0, fieldsize));
    }
  }
};

class CommandResult_RawString :
     public CommandResult {
  std::string string_value;

 public:
  CommandResult_RawString (const char *S = "") : CommandResult(CMD_RESULT_RAWSTRING) {
    string_value = std::string(S);
  }
  CommandResult_RawString (std::string S) : CommandResult(CMD_RESULT_RAWSTRING) {
    string_value = S;
  }
  CommandResult_RawString (CommandResult_RawString *C) :
       CommandResult(CMD_RESULT_RAWSTRING) {
    string_value = C->string_value;
    if (C->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_RawString () { }

  virtual CommandResult *Init () { return new CommandResult_RawString (""); }
  virtual CommandResult *Copy () { return new CommandResult_RawString (this); }
  virtual bool LT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_RawString));
    return string_value < ((CommandResult_RawString *)A)->string_value; }
  virtual bool GT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_RawString));
    return string_value > ((CommandResult_RawString *)A)->string_value; }
  virtual void Accumulate_Value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_RawString));
    if (!IsValueID()) string_value += ((CommandResult_RawString *)A)->string_value; }
  virtual bool ValueIsNull () { return string_value.length() == 0; }

  void Accumulate_RawString (CommandResult_RawString *B) {
    string_value += B->string_value;
  }
  void Value (std::string& S) {
    S = string_value;
  }

  virtual std::string Form (int64_t fieldsize) {
    return string_value;
  }
  virtual PyObject * pyValue () {
    return Py_BuildValue("s",string_value.c_str());
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    // Ignore fieldsize and leftjustified specifications and just dump the
    // the raw string to the output stream.
    to << string_value;
  }
};

class CommandResult_Function :
     public CommandResult,
     public Function {
  std::set<Statement> ST;
  int64_t Line;    // Line number of first statement in set.
  int64_t Column;  // Column number of first statement in set.

 public:

  CommandResult_Function (Function F) :
       Framework::Function(F),
       CommandResult(CMD_RESULT_FUNCTION) {
    Line = 0;
    Column = 0;
  }
  CommandResult_Function  (Function F, std::set<Statement>& st) :
       Framework::Function(F),
       CommandResult(CMD_RESULT_FUNCTION) {
    ST = st;
    Line = 0;
    Column = 0;
    if (ST.begin() != ST.end()) {
      std::set<Statement>::const_iterator STi = ST.begin();
      Line = (int64_t)((*STi).getLine());
      Column = (int64_t)((*STi).getColumn());
    }
  }
  CommandResult_Function  (CommandResult_Function *C) :
       Framework::Function(*C),
       CommandResult(CMD_RESULT_FUNCTION) {
    ST = C->ST;
    Line = C->Line;
    Column = C->Column;
    if (C->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_Function () { }

  virtual CommandResult *Copy () { return new CommandResult_Function (this); }
  virtual bool LT (CommandResult *A) {
    if (A->Type() != CMD_RESULT_FUNCTION) {
      Assert (A->Type() == CMD_RESULT_CALLTRACE);
      return A->GT(this);
    }
    return OpenSpeedShop::Queries::CompareFunctions()(*this,
                                                      *((CommandResult_Function *)A),
                                                      OPENSS_LESS_RESTRICTIVE_COMPARISONS); }
  virtual bool GT (CommandResult *A) {
    if (A->Type() != CMD_RESULT_FUNCTION) {
      Assert (A->Type() == CMD_RESULT_CALLTRACE);
      return A->LT(this);
    }
    if (OpenSpeedShop::Queries::CompareFunctions()(*((CommandResult_Function *)A),
                                                   *this,
                                                   OPENSS_LESS_RESTRICTIVE_COMPARISONS)) {
      return true;
    }
    if (OpenSpeedShop::Queries::CompareFunctions()(*this,
                                                   *((CommandResult_Function *)A),
                                                   OPENSS_LESS_RESTRICTIVE_COMPARISONS)) {
      return false;
    }
    int64_t Ls = Line;
    int64_t Rs = (int64_t)(((CommandResult_Function *)A)->getLine());
    return (Ls > Rs);
  }

  void Value (std::string& F) {
    F = OPENSS_VIEW_MANGLED_NAME ? getMangledName() : getDemangledName();
  }
  void Value (std::set<Statement>& st) {
    st = ST;
  }
  int64_t getLine () {
    return Line;
  }
  int64_t getColumn () {
    return Column;
  }

  virtual std::string Form (int64_t fieldsize) {
    std::string S = OPENSS_VIEW_MANGLED_NAME ? getMangledName() : getDemangledName();
    LinkedObject L = getLinkedObject();
    std::set<Statement> T = getDefinitions();

    if (OPENSS_VIEW_DEFINING_LOCATION) {
      S = S + " (" + ((OPENSS_VIEW_FULLPATH)
                        ? L.getPath()
                        : L.getPath().getBaseName());

      if (T.size() > 0) {
        std::set<Statement>::const_iterator ti;
        for (ti = T.begin(); ti != T.end(); ti++) {
          if (ti != T.begin()) {
            S += "  &...";
            break;
          }
          Statement s = *ti;
          int64_t gotLine = (int64_t)s.getLine();
          int64_t gotColumn = (int64_t)s.getColumn();
          char l[50];
          if (gotColumn > 0) {
            sprintf( &l[0], "%lld:%lld", static_cast<long long int>(gotLine),
                     static_cast<long long int>(gotColumn));
          } else {
            sprintf( &l[0], "%lld", static_cast<long long int>(gotLine));
          }
          S = S + ": "
                + ((OPENSS_VIEW_FULLPATH)
                        ? s.getPath()
                        : s.getPath().getBaseName())
                + "," + &l[0];
        }
      }
      S += ")";
    }

    return S;
  }
  virtual PyObject * pyValue () {
    std::string F = Form (OPENSS_VIEW_FIELD_SIZE);
    return Py_BuildValue("s",F.c_str());
  }

  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string string_value = Form (fieldsize);
    if (fieldsize == 0) fieldsize = string_value.length();
    if (leftjustified) {
     // Left justification is only done on the last column of a report.
     // Don't truncate the string if it is bigger than the field size.
     // This is done to make sure everything gets printed.

      to << std::setiosflags(std::ios::left) << string_value;

     // If there is unused space in the field, pad with blanks.
      if ((string_value.length() < fieldsize) &&
          (string_value[string_value.length()-1] != *("\n"))) {
        for (int64_t i = string_value.length(); i < fieldsize; i++) to << " ";
      }

    } else {
     // Right justify the string in the field.
     // Don't let it exceed the size of the field.
     // Also, limit the size based on our internal buffer size.
      to << std::setiosflags(std::ios::right) << std::setw(fieldsize)
         << ((string_value.length() <= fieldsize) ? string_value : string_value.substr(0, fieldsize));
    }
  }
};

class CommandResult_Loop :
     public CommandResult,
     public Loop {
  std::set<Statement> ST;
  int64_t Line;    // Line number of first statement in set.
  int64_t Column;  // Column number of first statement in set.

 public:

  CommandResult_Loop (Loop L) :
       Framework::Loop(L),
       CommandResult(CMD_RESULT_LOOP) {
    Line = 0;
    Column = 0;
  }

  CommandResult_Loop  (Loop L, std::set<Statement>& st) :
       Framework::Loop(L),
       CommandResult(CMD_RESULT_LOOP) {
    ST = st;
    Line = 0;
    Column = 0;
    if (ST.begin() != ST.end()) {
      std::set<Statement>::const_iterator STi = ST.begin();
      Line = (int64_t)((*STi).getLine());
      Column = (int64_t)((*STi).getColumn());
    }
  }
  CommandResult_Loop  (CommandResult_Loop *L) :
       Framework::Loop(*L),
       CommandResult(CMD_RESULT_LOOP) {
    ST = L->ST;
    Line = L->Line;
    Column = L->Column;
    if (L->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_Loop () { }

  virtual CommandResult *Copy () { return new CommandResult_Loop (this); }
  virtual bool LT (CommandResult *A) {
    if (A->Type() != CMD_RESULT_LOOP) {
      Assert (A->Type() == CMD_RESULT_CALLTRACE);
      return A->GT(this);
    }
    return OpenSpeedShop::Queries::CompareLoops()(*this,
                                                  *((CommandResult_Loop *)A),
                                                   OPENSS_LESS_RESTRICTIVE_COMPARISONS); }
  virtual bool GT (CommandResult *A) {
    if (A->Type() != CMD_RESULT_LOOP) {
      Assert (A->Type() == CMD_RESULT_CALLTRACE);
      return A->LT(this);
    }
    if (OpenSpeedShop::Queries::CompareLoops()(*((CommandResult_Loop *)A),
                                               *this,
                                               OPENSS_LESS_RESTRICTIVE_COMPARISONS)) {
      return true;
    }
    if (OpenSpeedShop::Queries::CompareLoops()(*this,
                                               *((CommandResult_Loop *)A),
                                               OPENSS_LESS_RESTRICTIVE_COMPARISONS)) {
      return false;
    }
    int64_t Ls = Line;
    int64_t Rs = (int64_t)(((CommandResult_Loop *)A)->getLine());
    return (Ls > Rs);
  }

  void Value (std::set<Statement>& st) {
    st = ST;
  }
  int64_t getLine () {
    return Line;
  }
  int64_t getColumn () {
    return Column;
  }

  virtual std::string Form (int64_t fieldsize) {
    std::string S = "";
    std::set<Statement> DEF = getDefinitions();
    std::set<Statement>::const_iterator s = DEF.begin();
    if ( ! DEF.empty()) {
      int64_t gotLine = (int64_t)s->getLine();
      int64_t gotColumn = (int64_t)s->getColumn();
      char l[50];
      if (gotColumn > 0) {
        sprintf( &l[0], "%lld:%lld", static_cast<long long int>(gotLine),
                 static_cast<long long int>(gotColumn));
      } else {
        sprintf( &l[0], "%lld", static_cast<long long int>(gotLine));
      }
      S = (OPENSS_VIEW_FULLPATH) ? s->getPath() : s->getPath().getBaseName();
      S = S + "(" + std::string(l) + ")";
    } else {
      // There are no statements associated with the loop, at least in the symbol information.
      // See if we can put out the function name and if not then the linked object name with
      // the starting address of the loop.

      std::set<Function> thisFunction = getFunctions();
      std::set<Function>::const_iterator f = thisFunction.begin();
      std::string Fname = "";
      if ( ! thisFunction.empty() ) {
        Fname = f->getName();
#if DEBUG_CLI
        if ( Fname.size() ) {
           std::cerr << "SS_CommandResult_Loop, Form, Function was found, Fname=" << Fname << std::endl;
        } 
#endif
      } 

      //std::cerr << "Function for null loop is not defined?" << std::endl;
      LinkedObject thisLinkedObject = getLinkedObject();
      std::string linkedOname = ((OPENSS_VIEW_FULLPATH) ? thisLinkedObject.getPath() : thisLinkedObject.getPath().getBaseName());

#if DEBUG_CLI
      std::cerr << "SS_CommandResult_Loop, Form, We have a linked object=" << linkedOname << std::endl;
#endif

      // Get the thread that the loop is in then use that to get 
      // the address range and print out the beginning address
      // for the loop within the linked object file.

      std::set<Thread> loop_threads = getThreads();
      std::set<Thread>:: iterator ti = loop_threads.begin();
      Thread t = *ti;
      AddressRange ar = getExtentIn(t).getBounds().getAddressRange();
      std::stringstream ssa;
      ssa << ar.getBegin();
      S += ssa.str();
      S += " (";
      if ( Fname.size() ) {
        S += Fname;
        S += " : ";
      }
      S += linkedOname;
      S += ")";

    }
    return S;
  }


  virtual PyObject * pyValue () {
    std::string S = Form (OPENSS_VIEW_FIELD_SIZE);
    return Py_BuildValue("s", S.c_str());
  }

  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {

    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string string_value = Form (fieldsize);
    if (fieldsize == 0) fieldsize = string_value.length();
    if (leftjustified) {
     // Left justification is only done on the last column of a report.
     // Don't truncate the string if it is bigger than the field size.
     // This is done to make sure everything gets printed.

      to << std::setiosflags(std::ios::left) << string_value;

     // If there is unused space in the field, pad with blanks.
      if ((string_value.length() < fieldsize) &&
          (string_value[string_value.length()-1] != *("\n"))) {
        for (int64_t i = string_value.length(); i < fieldsize; i++) to << " ";
      }

    } else {
     // Right justify the string in the field.
     // Don't let it exceed the size of the field.
     // Also, limit the size based on our internal buffer size.
      to << std::setiosflags(std::ios::right) << std::setw(fieldsize)
         << ((string_value.length() <= fieldsize) ? string_value : string_value.substr(0, fieldsize));
    }
  }
};

class CommandResult_Statement :
     public CommandResult,
     public Statement {

 public:
  CommandResult_Statement (Statement S) :
       Framework::Statement(S),
       CommandResult(CMD_RESULT_STATEMENT) {
  }
  CommandResult_Statement (CommandResult_Statement *C) :
       Framework::Statement(*C),
       CommandResult(CMD_RESULT_STATEMENT) {
    if (C->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_Statement () { }

  virtual CommandResult *Copy () { return new CommandResult_Statement (this); }
  virtual bool LT (CommandResult *A) {
    if (A->Type() != CMD_RESULT_STATEMENT) {
      Assert (A->Type() == CMD_RESULT_CALLTRACE);
      return A->GT(this);
    }
    return OpenSpeedShop::Queries::CompareStatements()(*this,
                                                       *((CommandResult_Statement *)A),
                                                       OPENSS_LESS_RESTRICTIVE_COMPARISONS); }
  virtual bool GT (CommandResult *A) {
    if (A->Type() != CMD_RESULT_STATEMENT) {
      Assert (A->Type() == CMD_RESULT_CALLTRACE);
      return A->LT(this);
    }
    return OpenSpeedShop::Queries::CompareStatements()(*((CommandResult_Statement *)A),
                                                       *this,
                                                       OPENSS_LESS_RESTRICTIVE_COMPARISONS); }

  void Value (int64_t& L) {
    L = (int64_t)getLine();
  }

  virtual std::string Form (int64_t fieldsize) {
    int64_t gotLine = (int64_t)getLine();
    int64_t gotColumn = (int64_t)getColumn();
    char l[50];

    if (gotColumn > 0) {
      sprintf( &l[0], "%lld:%lld", static_cast<long long int>(gotLine),
               static_cast<long long int>(gotColumn));
    } else {
      sprintf( &l[0], "%lld", static_cast<long long int>(gotLine));
    }
    std::string S = (OPENSS_VIEW_FULLPATH)
                        ? getPath()
                        : getPath().getBaseName();
    S = S + "(" + std::string(l) + ")";
    return S;
  }

  virtual PyObject * pyValue () {
    std::string S = Form (OPENSS_VIEW_FIELD_SIZE);
    return Py_BuildValue("s", S.c_str());
  }

  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string string_value = Form (fieldsize);
    if (fieldsize == 0) fieldsize = string_value.length();
    if (leftjustified) {
     // Left justification is only done on the last column of a report.
     // Don't truncate the string if it is bigger than the field size.
     // This is done to make sure everything gets printed.

      to << std::setiosflags(std::ios::left) << string_value;

     // If there is unused space in the field, pad with blanks.
      if ((string_value.length() < fieldsize) &&
          (string_value[string_value.length()-1] != *("\n"))) {
        for (int64_t i = string_value.length(); i < fieldsize; i++) to << " ";
      }

    } else {
     // Right justify the string in the field.
     // Don't let it exceed the size of the field.
     // Also, limit the size based on our internal buffer size.
      to << std::setiosflags(std::ios::right) << std::setw(fieldsize)
         << ((string_value.length() <= fieldsize) ? string_value : string_value.substr(0, fieldsize));
    }
  }
};

class CommandResult_LinkedObject :
     public CommandResult,
     public LinkedObject {
  uint64_t Offset;

 public:

  CommandResult_LinkedObject (LinkedObject F) :
       Framework::LinkedObject(F),
       CommandResult(CMD_RESULT_LINKEDOBJECT) {
  }
  CommandResult_LinkedObject (LinkedObject F, uint64_t off) :
       Framework::LinkedObject(F),
       CommandResult(CMD_RESULT_LINKEDOBJECT) {
    Offset = off;
  }
  CommandResult_LinkedObject  (CommandResult_LinkedObject *C) :
       Framework::LinkedObject(*C),
       CommandResult(CMD_RESULT_LINKEDOBJECT) {
    Offset = C->Offset;
    if (C->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_LinkedObject () { }

  virtual CommandResult *Copy () { return new CommandResult_LinkedObject (this); }
  virtual bool LT (CommandResult *A) {
    if (A->Type() != CMD_RESULT_LINKEDOBJECT) {
      Assert (A->Type() == CMD_RESULT_CALLTRACE);
      return A->GT(this);
    }
    return OpenSpeedShop::Queries::CompareLinkedObjects()(*this,
                                                          *((CommandResult_LinkedObject *)A)); }
  virtual bool GT (CommandResult *A) {
    if (A->Type() != CMD_RESULT_LINKEDOBJECT) {
      Assert (A->Type() == CMD_RESULT_CALLTRACE);
      return A->LT(this);
    }
    return OpenSpeedShop::Queries::CompareLinkedObjects()(*((CommandResult_LinkedObject *)A),
                                                          *this); }

  void Value (std::string& L) {
    L = (OPENSS_VIEW_FULLPATH)
                      ? getPath()
                      : getPath().getBaseName();
  }
  void Value (uint64_t& off) {
    off = Offset;
  }

  virtual std::string Form (int64_t fieldsize) {
    std::string S = (OPENSS_VIEW_FULLPATH)
                      ? getPath()
                      : getPath().getBaseName();
    return S;
  }
  virtual PyObject * pyValue () {
    std::string F = Form (OPENSS_VIEW_FIELD_SIZE);
    return Py_BuildValue("s",F.c_str());
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string string_value = Form (fieldsize);
    if (fieldsize == 0) fieldsize = string_value.length();
    if (leftjustified) {
     // Left justification is only done on the last column of a report.
     // Don't truncate the string if it is bigger than the field size.
     // This is done to make sure everything gets printed.

      to << std::setiosflags(std::ios::left) << string_value;

     // If there is unused space in the field, pad with blanks.
      if ((string_value.length() < fieldsize) &&
          (string_value[string_value.length()-1] != *("\n"))) {
        for (int64_t i = string_value.length(); i < fieldsize; i++) to << " ";
      }

    } else {
     // Right justify the string in the field.
     // Don't let it exceed the size of the field.
     // Also, limit the size based on our internal buffer size.
      to << std::setiosflags(std::ios::right) << std::setw(fieldsize)
         << ((string_value.length() <= fieldsize) ? string_value : string_value.substr(0, fieldsize));
    }
  }
};

class CommandResult_CallStackEntry : public CommandResult {
 private:

  class CallStackEntry {
   public:
    std::vector<CommandResult *> *CSV;
    CallStackEntry () {
    }
    CallStackEntry (std::vector<CommandResult *> *call_vector) {
      CSV = call_vector;
    }
    ~CallStackEntry () {
      Reclaim_CR_Space (*CSV);
    }
  };

  bool Bottom_up;
  SmartPtr<CallStackEntry> CallStack;

  CommandResult_CallStackEntry () : CommandResult(CMD_RESULT_CALLTRACE) {
    Bottom_up = false;
  }

 public:
  CommandResult_CallStackEntry (std::vector<CommandResult *> *call_vector,
                                bool Reverse=false)
      : CommandResult(CMD_RESULT_CALLTRACE) {
    Bottom_up = Reverse;
    if (CallStack.isNull()) {
      CallStack = SmartPtr<CallStackEntry>(new CallStackEntry(call_vector));
    } else {
      CallStack->CSV = call_vector;
    }
  }
  CommandResult_CallStackEntry (CommandResult_CallStackEntry *CSE)
      : CommandResult(CMD_RESULT_CALLTRACE) {
    int64_t len = CSE->CallStack->CSV->size();
    Bottom_up = CSE->Bottom_up;
    CallStack = CSE->CallStack;
    if (CSE->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_CallStackEntry () {
   // Only delete std::vector<CommandResult *> when last SmartPtr is released.
  }

  virtual CommandResult *Copy () { return new CommandResult_CallStackEntry (this); }
  virtual bool LT (CommandResult *A) {
    std::vector<CommandResult *> *ls = CallStack->CSV;
    int64_t sz = ls->size();
    Assert (sz > 0);
    if (A->Type() != CMD_RESULT_CALLTRACE) {
      CommandResult *CE = (*ls)[sz - 1];
      return CE->LT(A);
    }
    std::vector<CommandResult *> *rs = ((CommandResult_CallStackEntry *)A)->CallStack->CSV;
    int64_t ll = ls->size();
    int64_t rl = rs->size();
    int64_t lm = std::min(ll,rl);
    for (int64_t i = 0; i < lm; i++) {
      if ((*ls)[i]->LT((*rs)[i])) {
        return true;
      }
    }
    return (ll < rl); }
  virtual bool GT (CommandResult *A) {
    std::vector<CommandResult *> *ls = CallStack->CSV;
    int64_t sz = ls->size();
    Assert (sz > 0);
    if (A->Type() != CMD_RESULT_CALLTRACE) {
      CommandResult *CE = (*ls)[sz - 1];
      return CE->GT(A);
    }
    std::vector<CommandResult *> *rs = ((CommandResult_CallStackEntry *)A)->CallStack->CSV;
    int64_t ll = ls->size();
    int64_t rl = rs->size();
    int64_t lm = std::min(ll,rl);
    for (int64_t i = 0; i < lm; i++) {
      if ((*ls)[i]->GT((*rs)[i])) {
        return true;
      }
    }
    return (ll < rl); }

  std::vector<CommandResult *> *Value () {
    return (CallStack->CSV);
  };

  virtual std::string Form (int64_t fieldsize) {
    int64_t sz = CallStack->CSV->size();
    if (sz <= 0) return std::string("");
    std::vector<CommandResult *> *C1 = CallStack->CSV;
    CommandResult *CE = (*C1)[sz - 1];
    std::string Name;
   // Add indentation.
    for (int64_t i = 1; i < sz; i++) {
      Name += ((Bottom_up) ? "<" : ">");
    }
   // Add line number.
    if (sz > 1) {
      if (CE->Type() == CMD_RESULT_FUNCTION) {
        std::set<Statement> T;
        ((CommandResult_Function *)CE)->Value(T);
        if (T.begin() != T.end()) {
          std::set<Statement>::const_iterator sti = T.begin();;
          Statement S = *sti;
          int64_t gotLine = (int64_t)S.getLine();
          int64_t gotColumn = (int64_t)S.getColumn();
          char l[50];
          if (gotColumn > 0) {
            sprintf( &l[0], "%lld:%lld", static_cast<long long int>(gotLine),
                     static_cast<long long int>(gotColumn));
          } else {
            sprintf( &l[0], "%lld", static_cast<long long int>(gotLine));
          }
          Name = Name + " @ " + l + " in ";
        }
      } else if (CE->Type() == CMD_RESULT_LINKEDOBJECT) {
        uint64_t V;
        ((CommandResult_LinkedObject *)CE)->Value(V);
        char l[50];
        sprintf( &l[0], "+0x%llx", static_cast<long long unsigned int>(V));
        std::string l_name;
        ((CommandResult_LinkedObject *)CE)->Value(l_name);
        Name = Name + " @ " + l_name + l + " in ";
      } else if (CE->Type() == CMD_RESULT_UINT) {
        Name += " @ ";
      }
    }
   // Add function name and location information.
    Name += CE->Form();
    return Name;
  }
  virtual std::string Form () {
    return Form(OPENSS_VIEW_FIELD_SIZE);
  }
  virtual PyObject * pyValue () {
    std::string F = Form (OPENSS_VIEW_FIELD_SIZE);
    return Py_BuildValue("s",F.c_str());
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string string_value = Form (fieldsize);
    if (fieldsize == 0) fieldsize = string_value.length();
    if (leftjustified) {
     // Left justification is only done on the last column of a report.
     // Don't truncate the string if it is bigger than the field size.
     // This is done to make sure everything gets printed.

      to << std::setiosflags(std::ios::left) << string_value;

     // If there is unused space in the field, pad with blanks.
      if ((string_value.length() < fieldsize) &&
          (string_value[string_value.length()-1] != *("\n"))) {
        for (int64_t i = string_value.length(); i < fieldsize; i++) to << " ";      }

    } else {
     // Right justify the string in the field.
     // Don't let it exceed the size of the field.
     // Also, limit the size based on our internal buffer size.
      to << std::setiosflags(std::ios::right) << std::setw(fieldsize)
         << ((string_value.length() <= fieldsize) ? string_value : string_value.substr(0, fieldsize));
    }
  }
};

// A Time is nano-second time that is displayed in date-time format.
class CommandResult_Time : public CommandResult {
 private:
  Time time_value;

 public:
  CommandResult_Time () : CommandResult(CMD_RESULT_TIME) {
    time_value = (Time)0;
  }
  CommandResult_Time (Time t)
      : CommandResult(CMD_RESULT_TIME) {
    time_value = t;
  }
  CommandResult_Time (CommandResult_Time *T)
      : CommandResult(CMD_RESULT_TIME) {
    time_value = T->time_value;
    if (T->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_Time () {
  }

  virtual CommandResult *Init () { return new CommandResult_Time (); }
  virtual CommandResult *Copy () { return new CommandResult_Time (this); }
  virtual bool LT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Time));
    return time_value < ((CommandResult_Time *)A)->time_value; }
  virtual bool GT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Time));
    return time_value > ((CommandResult_Time *)A)->time_value; }
  virtual void Accumulate_Min (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Time));
    time_value = std::min (time_value, ((CommandResult_Time *)A)->time_value); }
  virtual void Accumulate_Max (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Time));
    time_value = std::max (time_value, ((CommandResult_Time *)A)->time_value); }

  Time& Value () {
    return time_value;
  };
  void Value (Time& t) {
    t = time_value;
  };

  virtual std::string Form (int64_t fieldsize) {
    std::ostringstream form(std::ios::out);
    form << time_value;
    if (OPENSS_VIEW_DATE_TIME_PRECISION > 0) {
      int64_t remaining = time_value.getValue() % 1000000000;  // Fractions of seconds.
      remaining += 1000000000;  // Add dummy padding to force leading zero fill.
      std::ostringstream s(std::ios::ate);
      s << remaining;
      form << "." << s.str().substr(1,OPENSS_VIEW_DATE_TIME_PRECISION); // Remove padding.
    }
    //return form.ostringstream::str();
    return form.str();
  }
  virtual PyObject * pyValue () {
    std::string F = Form (OPENSS_VIEW_FIELD_SIZE);
    return Py_BuildValue("s",F.c_str());
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string string_value = Form (fieldsize);
    if (fieldsize == 0) fieldsize = string_value.length();
    if (leftjustified) {
     // Left justification is only done on the last column of a report.
     // Don't truncate the string if it is bigger than the field size.
     // This is done to make sure everything gets printed.

      to << std::setiosflags(std::ios::left) << string_value;

     // If there is unused space in the field, pad with blanks.
      if ((string_value.length() < fieldsize) &&
          (string_value[string_value.length()-1] != *("\n"))) {
        for (int64_t i = string_value.length(); i < fieldsize; i++) to << " ";      }

    } else {
     // Right justify the string in the field.
     // Don't let it exceed the size of the field.
     // Also, limit the size based on our internal buffer size.
      to << std::setiosflags(std::ios::right) << std::setw(fieldsize)
         << ((string_value.length() <= fieldsize) ? string_value : string_value.substr(0, fieldsize));
    }
  }
};

// A Duration is nano-second time that is displayed as days:hours:minutes:seconds.
class CommandResult_Duration : public CommandResult {
 private:
  int64_t duration_value;

 public:
//  CommandResult_Duration () : CommandResult(CMD_RESULT_DURATION) {
//    duration_value = 0;
//  }
  CommandResult_Duration (int64_t d = 0)
      : CommandResult(CMD_RESULT_DURATION) {
    duration_value = d;
  }
  CommandResult_Duration (double d)
      : CommandResult(CMD_RESULT_DURATION) {
    duration_value = d;
  }
  CommandResult_Duration (CommandResult_Duration *D)
      : CommandResult(CMD_RESULT_DURATION) {
    duration_value = D->duration_value;
    if (D->IsValueID()) SetValueIsID();
  }
  virtual ~CommandResult_Duration () {
  }

  virtual CommandResult *Init () { return new CommandResult_Duration (); }
  virtual CommandResult *Copy () { return new CommandResult_Duration (this); }
  virtual bool LT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Duration));
    return duration_value < ((CommandResult_Duration *)A)->duration_value; }
  virtual bool GT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Duration));
    return duration_value > ((CommandResult_Duration *)A)->duration_value; }
  virtual void Accumulate_Value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Duration));
    if (!IsValueID()) duration_value += ((CommandResult_Duration *)A)->duration_value; }
  virtual void AbsDiff_value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Duration));
    if (duration_value >= ((CommandResult_Duration *)A)->duration_value) {
      duration_value -= ((CommandResult_Duration *)A)->duration_value;
    } else {
      duration_value = ((CommandResult_Duration *)A)->duration_value - duration_value;
    }
  }
  virtual void Accumulate_Min (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Duration));
    duration_value = std::min (duration_value, ((CommandResult_Duration *)A)->duration_value); }
  virtual void Accumulate_Max (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Duration));
    duration_value = std::max (duration_value, ((CommandResult_Duration *)A)->duration_value); }
  virtual bool ValueIsNull () { return duration_value == (int64_t)0; }

  int64_t& Value () {
    return duration_value;
  };
  void Value (int64_t& d) {
    d = duration_value;
  };

  virtual std::string Form (int64_t fieldsize) {
    std::ostringstream form(std::ios::ate);
    int64_t time_scale = 1000000000;
    int64_t remaining = duration_value;
    int64_t next_threshold = (time_scale * 60 * 60 * 24);
    bool output_started = false;
    if (remaining >= next_threshold) {
     // Output days
      form << (remaining / next_threshold) << ":";
      remaining = (remaining % next_threshold);
      output_started = true;
    }
    next_threshold = (time_scale * 60 * 60);
    if (remaining >= next_threshold) {
     // Output hours
      form << (remaining / next_threshold) << ":";
      remaining = (remaining % next_threshold);
      output_started = true;
    }
    next_threshold = (time_scale * 60);
    if (output_started ||
        (remaining >= next_threshold)) {
     // output minutes
      form << (remaining / next_threshold) << ":";
      remaining = (remaining % next_threshold);
      output_started = true;
    }
    if (output_started ||
        (remaining >= time_scale)) {
     // output seconds
      form << (remaining / time_scale) << ".";
      remaining = (remaining % time_scale);
      output_started = true;
    }
    if (!output_started) {
      form << "0.";
    }
    if (remaining == 0) {
      form << "0";
    } else if (!output_started &&
               (remaining < 1000000)) {
     // number is less than 1 milli-second
     // ouput to nano-second precision
      form << remaining;
    } else {
     // output normal precision
      std::ostringstream s(std::ios::ate);
      s << remaining;
      if (s.str().size() < OPENSS_VIEW_PRECISION) {
        form << s.str();
      } else {
        form << s.str().substr(0,OPENSS_VIEW_PRECISION);
      }
    }
    return form.str();
  }
  virtual PyObject * pyValue () {
    std::string F = Form (OPENSS_VIEW_FIELD_SIZE);
    return Py_BuildValue("s",F.c_str());
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string string_value = Form (fieldsize);
    if (fieldsize == 0) fieldsize = string_value.length();
    if (leftjustified) {
     // Left justification is only done on the last column of a report.
     // Don't truncate the string if it is bigger than the field size.
     // This is done to make sure everything gets printed.

      to << std::setiosflags(std::ios::left) << string_value;

     // If there is unused space in the field, pad with blanks.
      if ((string_value.length() < fieldsize) &&
          (string_value[string_value.length()-1] != *("\n"))) {
        for (int64_t i = string_value.length(); i < fieldsize; i++) to << " ";      }

    } else {
     // Right justify the string in the field.
     // Don't let it exceed the size of the field.
     // Also, limit the size based on our internal buffer size.
      to << std::setiosflags(std::ios::right) << std::setw(fieldsize)
         << ((string_value.length() <= fieldsize) ? string_value : string_value.substr(0, fieldsize));
    }
  }
};

// An Interval is a double precision value of seconds that is displayed in milli-seconds.
class CommandResult_Interval : public CommandResult {
 private:
  double interval_value;

 public:

//  CommandResult_Interval () : CommandResult(CMD_RESULT_INTERVAL) {
//    interval_value = 0.0;
//  }

  CommandResult_Interval (int64_t d = 0.0)
      : CommandResult(CMD_RESULT_INTERVAL) {
    interval_value = d;
  }
  CommandResult_Interval (double d)
      : CommandResult(CMD_RESULT_INTERVAL) {
    interval_value = d;
  }

  CommandResult_Interval (CommandResult_Float *D)
      : CommandResult(CMD_RESULT_INTERVAL) {
    double F;
    D->Value(F);
    interval_value = F;
    if (D->IsValueID()) SetValueIsID();
  }

  CommandResult_Interval (CommandResult_Interval *D)
      : CommandResult(CMD_RESULT_INTERVAL) {
    interval_value = D->interval_value;
    if (D->IsValueID()) SetValueIsID();
  }

  virtual ~CommandResult_Interval () { }

  virtual CommandResult *Init () { return new CommandResult_Interval (); }
  virtual CommandResult *Copy () { return new CommandResult_Interval (this); }

  virtual bool LT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Interval));
    return interval_value < ((CommandResult_Interval *)A)->interval_value; 
  }

  virtual bool GT (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Interval));
    return interval_value > ((CommandResult_Interval *)A)->interval_value; 
  }

  virtual void Accumulate_Value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Interval));
    if (!IsValueID()) interval_value += ((CommandResult_Interval *)A)->interval_value; 
  }

  virtual void AbsDiff_value (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Interval));
    if (interval_value >= ((CommandResult_Interval *)A)->interval_value) {
      interval_value -= ((CommandResult_Interval *)A)->interval_value;
    } else {
      interval_value = ((CommandResult_Interval *)A)->interval_value - interval_value;
    }
  }

  virtual void Accumulate_Min (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Interval));
    interval_value = std::min (interval_value, ((CommandResult_Interval *)A)->interval_value); 
  }

  virtual void Accumulate_Max (CommandResult *A) {
    Assert (typeid(*this) == typeid(CommandResult_Interval));
    interval_value = std::max (interval_value, ((CommandResult_Interval *)A)->interval_value); 
  }
  virtual bool ValueIsNull () { return (interval_value == (double)0.0 && !IsValueID()); }

  double& Value () {
    return interval_value;
  };

  void Value (double& d) {
    d = interval_value;
  };

  virtual std::string Form (int64_t fieldsize) {
    if (std::isnan(interval_value)) return std::string ("nan");
    std::ostringstream form(std::ios::ate);
    double float_value = interval_value * 1000; // Convert to milli-seconds
    if (std::isnan(float_value)) return std::string ("nan");

    char F[20];
    F[0] = *("%"); // left justify in field
    sprintf(&F[1], "%lld.%lldf\0", static_cast<long long int>(fieldsize),
            static_cast<long long int>(OPENSS_VIEW_PRECISION));

    char s[OPENSS_VIEW_FIELD_SIZE];
    sprintf ( s, &F[0], float_value);
    return std::string (s);
  }
  virtual PyObject * pyValue () {
    std::string F = Form (OPENSS_VIEW_FIELD_SIZE);
    return Py_BuildValue("s",F.c_str());
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    int64_t fieldsize = pc.field_size;
    if ( (pc.column_widths != NULL) &&
         (column >= 0) ) fieldsize = pc.column_widths[column];
    bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
    std::string string_value = Form (fieldsize);
    if (fieldsize == 0) fieldsize = string_value.length();
    if (leftjustified) {
     // Left justification is only done on the last column of a report.
     // Don't truncate the string if it is bigger than the field size.
     // This is done to make sure everything gets printed.

      to << std::setiosflags(std::ios::left) << string_value;

     // If there is unused space in the field, pad with blanks.
      if ((string_value.length() < fieldsize) &&
          (string_value[string_value.length()-1] != *("\n"))) {
        for (int64_t i = string_value.length(); i < fieldsize; i++) to << " ";      }

    } else {
     // Right justify the string in the field.
     // Don't let it exceed the size of the field.
     // Also, limit the size based on our internal buffer size.
      to << std::setiosflags(std::ios::right) << std::setw(fieldsize)
         << ((string_value.length() <= fieldsize) ? string_value : string_value.substr(0, fieldsize));
    }
  }
};

class CommandResult_Title :
     public CommandResult {
  std::string string_value;

 public:
  CommandResult_Title (std::string S = "") : CommandResult(CMD_RESULT_TITLE) {
    string_value = S;
  }
  virtual ~CommandResult_Title () { }
  virtual bool ValueIsNull () { return string_value.length() == 0; }

  void Value (std::string& S) {
    S = string_value;
  }

  virtual std::string Form (int64_t fieldsize) {
    return string_value;
  }
  virtual PyObject * pyValue () {
    return Py_BuildValue("s",string_value.c_str());
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    to << string_value;
  }
};

class CommandResult_Headers :
     public CommandResult {

 int64_t number_of_columns;
 std::list<CommandResult *> Headers;
 int64_t *column_widths;

 public:
  CommandResult_Headers () : CommandResult(CMD_RESULT_COLUMN_HEADER) {
    number_of_columns = 0;
    column_widths = NULL;
  }
  virtual ~CommandResult_Headers () {
    Reclaim_CR_Space (Headers);
  }
  virtual bool ValueIsNull () { return number_of_columns == 0; }

  void Add_Header (CommandResult *R) {
    number_of_columns++;
    Headers.push_back(R);
  }
  void Add_Column_Widths (int64_t *C) {
    column_widths = C;
  }
  void Value (std::list<CommandResult *>& R) {
    R = Headers;
  }
  void Value (int64_t& NC) {
    NC = number_of_columns;
  }

  virtual std::string Form (int64_t fieldsize) {
    std::string S;
    S = "";
    std::list<CommandResult *>::iterator cri = Headers.begin();
    int64_t num_results = 0;
    for (cri = Headers.begin(); cri != Headers.end(); cri++) {
      if (num_results++ != 0) S += "  "; // 2 spaces between strings
      std::string R = (*cri)->Form ();
      if (num_results < number_of_columns) {
       // Except for the last column ...
        if (R.size () > fieldsize) {
         // Shorten the original string.
          R.resize (fieldsize);
        } else if (R.size () < fieldsize) {
         // Lengthen the original string.
          std::string T;
          T.resize ((fieldsize - R.size ()), *" ");
          R = T + R;
        }
      }
      S += R;
    }
    return S;
  }
  virtual PyObject * pyValue () {
    PyObject *p_object = PyList_New(0);
    std::list<CommandResult *>::iterator cri = Headers.begin();
    for (cri = Headers.begin(); cri != Headers.end(); cri++) {
      PyObject *p_item = (*cri)->pyValue ();
      PyList_Append(p_object,p_item);
    }
    return p_object;
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    std::list<CommandResult *>::iterator coi;
    int64_t num_results = 0;
    for (coi = Headers.begin(); coi != Headers.end(); coi++) {
     // column_widths allows support of dynamic field size.
      if (num_results != 0) to << pc.eoc;
      (*coi)->Print (to, pc, num_results);
      num_results++;
    }

  }
};

class CommandResult_Enders :
     public CommandResult {

 int64_t number_of_columns;
 std::list<CommandResult *> Enders;
 int64_t *column_widths;

 public:
  CommandResult_Enders () : CommandResult(CMD_RESULT_COLUMN_ENDER) {
    number_of_columns = 0;
  }
  virtual ~CommandResult_Enders () {
    Reclaim_CR_Space (Enders);
  }
  virtual bool ValueIsNull () { return number_of_columns == 0; }

  void Add_Ender (CommandResult *R) {
    number_of_columns++;
    Enders.push_back(R);
  }
  void Add_Column_Widths (int64_t *C) {
    column_widths = C;
  }
  void Value (std::list<CommandResult *>& R) {
    R = Enders;
  }
  void Value (int64_t& NC) {
    NC = number_of_columns;
  }
  void Enders_AbsDiff (CommandResult *A, int IDX) {
    cmd_result_type_enum T = A->Type();
    if ( (T == CMD_RESULT_ADDRESS) ||
         (T == CMD_RESULT_UINT) ||
         (T == CMD_RESULT_INT) ||
         (T == CMD_RESULT_FLOAT) ||
         (T == CMD_RESULT_DURATION) ||
         (T == CMD_RESULT_FLOAT) ) {
     // Determine which column value to use.
      CommandResult *delta = NULL;
      std::list<CommandResult *>::iterator cri = Enders.begin();
      int64_t column_num = 0;
      for (cri = Enders.begin(); cri != Enders.end(); cri++) {
        if (column_num++ == IDX) {
          delta = (*cri);
          break;
        }
      }
      if (delta != NULL) {
        cmd_result_type_enum T = delta->Type();
        if ( (T == CMD_RESULT_ADDRESS) ||
             (T == CMD_RESULT_UINT) ||
             (T == CMD_RESULT_INT) ||
             (T == CMD_RESULT_FLOAT) ||
             (T == CMD_RESULT_DURATION) ||
             (T == CMD_RESULT_FLOAT) ) {
           // Calculate differences with the first column entry.
            delta->AbsDiff_value( A );
        } else {
          Enders.insert (cri, A->Copy());
          Enders.erase (cri);
        }
      }
    }
  }

  virtual std::string Form (int64_t fieldsize) {
    std::string S;
    S = "";
    std::list<CommandResult *>::iterator cri = Enders.begin();
    int64_t num_results = 0;
    for (cri = Enders.begin(); cri != Enders.end(); cri++) {
      if (num_results++ != 0) S += "  "; // 2 spaces between strings
      std::string R = (*cri)->Form ();
      if (num_results < number_of_columns) {
       // Except for the last column ...
        if (R.size () > fieldsize) {
         // shorten the original string.
          R.resize (fieldsize);
        } else if (R.size () < fieldsize) {
         // lengthen the original string.
          std::string T;
          T.resize ((fieldsize - R.size ()), *" ");
          R = T + R;
        }
      }
      S += R;
    }
    return S;
  }
  virtual PyObject * pyValue () {
    PyObject *p_object = PyList_New(0);
    std::list<CommandResult *>::iterator cri = Enders.begin();
    for (cri = Enders.begin(); cri != Enders.end(); cri++) {
      PyObject *p_item = (*cri)->pyValue ();
      PyList_Append(p_object,p_item);
    }
    return p_object;
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    std::list<CommandResult *>::iterator coi;
    int64_t num_results = 0;
    for (coi = Enders.begin(); coi != Enders.end(); coi++) {
     // column_widths allows support of dynamic field size.
      if (num_results != 0) to << pc.eoc;
      (*coi)->Print (to, pc, num_results);
      num_results++;
    }

  }
};

class CommandResult_Columns :
     public CommandResult {

 int64_t number_of_columns;
 std::list<CommandResult *> Columns;
 int64_t *column_widths;

 public:
  CommandResult_Columns () : CommandResult(CMD_RESULT_COLUMN_VALUES) {
    number_of_columns = 0;
    column_widths = NULL;
  }
  virtual ~CommandResult_Columns () {
    Reclaim_CR_Space (Columns);
  }
  virtual bool ValueIsNull () { return number_of_columns == 0; }

  void Add_Column (CommandResult *R) {
    number_of_columns++;
    Columns.push_back(R);
  }
  void Value (std::list<CommandResult *>& R) {
    R = Columns;
  }
  void Value (int64_t& NC) {
    NC = number_of_columns;
  }
  void Add_Column_Widths (int64_t *C) {
    column_widths = C;
  }
  void Column_AbsDiff (CommandResult *A, int IDX) {
    cmd_result_type_enum T = A->Type();
    if ( (T == CMD_RESULT_ADDRESS) ||
         (T == CMD_RESULT_UINT) ||
         (T == CMD_RESULT_INT) ||
         (T == CMD_RESULT_FLOAT) ||
         (T == CMD_RESULT_DURATION) ||
         (T == CMD_RESULT_FLOAT) ) {
     // Determine which column value to use.
      CommandResult *delta = NULL;
      std::list<CommandResult *>::iterator cri = Columns.begin();
      int64_t column_num = 0;
      for (cri = Columns.begin(); cri != Columns.end(); cri++) {
        if (column_num++ == IDX) {
          delta = (*cri);
          break;
        }
      }
      if (delta != NULL) {
        cmd_result_type_enum T = delta->Type();
        if ( (T == CMD_RESULT_ADDRESS) ||
             (T == CMD_RESULT_UINT) ||
             (T == CMD_RESULT_INT) ||
             (T == CMD_RESULT_FLOAT) ||
             (T == CMD_RESULT_DURATION) ||
             (T == CMD_RESULT_FLOAT) ) {
           // Calculate differences with the first column entry.
            delta->AbsDiff_value( A );
        } else {
          Columns.insert (cri, A->Copy());
          Columns.erase (cri);
        }
      }
    }
  }

  virtual std::string Form (int64_t fieldsize) {
    std::string S;
    S = "";
    std::list<CommandResult *>::iterator cri = Columns.begin();
    int64_t num_results = 0;
    for (cri = Columns.begin(); cri != Columns.end(); cri++) {
      if (num_results++ != 0) S += "  "; // 2 spaces between strings
      std::string R = (*cri)->Form ();
      if (num_results < number_of_columns) {
       // Except for the last column ...
        if (R.size () > fieldsize) {
         // Shorten the original string.
          R.resize (fieldsize);
        } else if (R.size () < fieldsize) {
         // Lengthen the original string.
          std::string T;
          T.resize ((fieldsize - R.size ()), *" ");
          R = T + R;
        }
      }
      S += R;
    }
    return S;
  }
  virtual PyObject * pyValue () {
    PyObject *p_object = PyList_New(0);
    std::list<CommandResult *>::iterator cri = Columns.begin();
    for (cri = Columns.begin(); cri != Columns.end(); cri++) {
      PyObject *p_item = (*cri)->pyValue ();
      PyList_Append(p_object,p_item);
    }
    return p_object;
  }
  virtual void Print (std::ostream& to, PrintControl &pc, int64_t column) {
    std::list<CommandResult *>::iterator coi;
    int64_t num_results = 0;
    for (coi = Columns.begin(); coi != Columns.end(); coi++) {
     // column_widths allows support of dynamic field size.
      int64_t fieldsize = pc.field_size;
      if ( (pc.column_widths != NULL) &&
           (column >= 0) ) fieldsize = pc.column_widths[column];
      bool leftjustified = (column == (pc.num_columns-1)) ? true : pc.left_justify_all;
      if (num_results != 0) to << pc.eoc;
      if ((*coi)->ValueIsNull() &&
          !((*coi)->IsValueID()) &&
          pc.blank_in_place_of_zero) {
       // Avoid printing lots of meaningless "0" values - blank fill the field.
        to << (leftjustified ? std::setiosflags(std::ios::left) : std::setiosflags(std::ios::right))
           << std::setw(fieldsize) << " ";
      } else {
        (*coi)->Print (to, pc, num_results);
      }
      num_results++;
    }

  }
};


// Overloaded utility that will generate the right CommandResult object.
inline CommandResult *CRPTR (uint V) { return new CommandResult_Uint (V); }
inline CommandResult *CRPTR (uint64_t V) { return new CommandResult_Uint (V); }
inline CommandResult *CRPTR (int V) { return new CommandResult_Int (V); }
inline CommandResult *CRPTR (int64_t V) { return new CommandResult_Int (V); }
inline CommandResult *CRPTR (float V) { return new CommandResult_Float (V); }
inline CommandResult *CRPTR (double V) { return new CommandResult_Float (V); }
inline CommandResult *CRPTR (const char *V) { return new CommandResult_String (std::string(V)); }
inline CommandResult *CRPTR (std::string& V) { return new CommandResult_String (V); }
inline CommandResult *CRPTR (Function& V) { return new CommandResult_Function (V); }
inline CommandResult *CRPTR (Statement& V) { return new CommandResult_Statement (V); }
inline CommandResult *CRPTR (Loop& V) { return new CommandResult_Loop (V); }
inline CommandResult *CRPTR (LinkedObject& V) { return new CommandResult_LinkedObject (V); }
inline CommandResult* CRPTR (Time& V) { return new CommandResult_Time (V); }

// Create an initial CommandResult* with the same type
// as the argument but with a NULL initial value.
CommandResult *CR_Init_of_CR_type( CommandResult *A );

// Computation utilities.
CommandResult *Calculate_Average (CommandResult *A, CommandResult *B);
CommandResult *Calculate_StdDev  (CommandResult *A, CommandResult *B, CommandResult *C);
CommandResult *Calculate_Flops  (CommandResult *A, CommandResult *B);
CommandResult *Calculate_Ratio  (CommandResult *A, CommandResult *B);
CommandResult *Calculate_Inverse_Ratio_Percent  (CommandResult *A, CommandResult *B);
CommandResult *Calculate_Ratio_Percent  (CommandResult *A, CommandResult *B);
CommandResult *Calculate_Percent (CommandResult *A, CommandResult *B);
CommandResult *Calculate_Expression(expression_operation_t op, 
                                    CommandResult *A,
                                    CommandResult *B, 
                                    CommandResult *C);
