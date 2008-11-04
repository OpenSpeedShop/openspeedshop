/******************************************************************************
** Copyright (c) 2006 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006, 2007, 2008 Krell Institute  All Rights Reserved.
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

#include "SS_Input_Manager.hxx"


// Defined here so they can reference CommandResult_CallStackEntry.
  bool CommandResult_Uint::LT (CommandResult *A) {
    if (typeid(*A) == typeid(CommandResult_CallStackEntry)) {
      std::vector<CommandResult *> *rs = ((CommandResult_CallStackEntry *)A)->Value ();
      if(rs->size() == 1) {
        return this < (*rs)[0];
      }
    }
    Assert (typeid(*this) == typeid(CommandResult_Uint));
    return uint_value < ((CommandResult_Uint *)A)->uint_value; }
  bool CommandResult_Uint::GT (CommandResult *A) {
    if (typeid(*A) == typeid(CommandResult_CallStackEntry)) {
      std::vector<CommandResult *> *rs = ((CommandResult_CallStackEntry *)A)->Value ();
      if(rs->size() == 1) {
        return this > (*rs)[0];
      }
    }
    Assert (typeid(*this) == typeid(CommandResult_Uint));
    return uint_value > ((CommandResult_Uint *)A)->uint_value; }

// Some utilities to perform various computations.

CommandResult *Calculate_Average (CommandResult *A, CommandResult *B) {


#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Average, A=%x, B=%x\n", A, B);
#endif

  if (B == NULL) {
    return NULL;
  }
  if (A == NULL) {
    return new CommandResult_Float (0.0);
  }

  double Avalue;
  double Bvalue;
  int64_t Ivalue;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Average, A->Type()=%d, B->Type()=%d\n", A->Type(), B->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    uint64_t Uvalue;
    ((CommandResult_Uint *)A)->Value(Uvalue);
    Avalue = Uvalue;
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(Ivalue);
    Avalue = Ivalue;
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(Avalue);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(Ivalue);
    Avalue = Ivalue;
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(Avalue);
    break;
  }


  switch (B->Type()) {
   case CMD_RESULT_UINT:
    uint64_t Uvalue;
    ((CommandResult_Uint *)B)->Value(Uvalue);
    Bvalue = Uvalue;
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(Ivalue);
    Bvalue = Ivalue;
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(Bvalue);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(Ivalue);
    Bvalue = Ivalue;
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(Bvalue);
    break;
  }

  double average = Avalue / Bvalue;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Average, Avalue=%f, Bvalue()=%f, average=%f\n", 
         Avalue, Bvalue, average);
#endif

  return new CommandResult_Float (average);
}

CommandResult *Calculate_StdDev  (CommandResult *A, CommandResult *B, CommandResult *C) {

  if ((A == NULL) ||
      (B == NULL) ||
      (C == NULL)) {
    return NULL;
  }

#if DEBUG_CLI
  printf("In CommandResult *Calculate_StdDev, A=%x, B=%x, C=%x\n", A, B, C);
#endif

  double Avalue;
  double Bvalue;
  double Cvalue;
  int64_t Ivalue;

  
#if DEBUG_CLI
  printf("In CommandResult *Calculate_StdDev, A->Type()=%d, B->Type()=%d, C->Type()=%d\n", 
         A->Type(), B->Type(), C->Type());
#endif

  switch (A->Type()) {
   case CMD_RESULT_UINT:
    uint64_t Uvalue;
    ((CommandResult_Uint *)A)->Value(Uvalue);
    Avalue = Uvalue;
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(Ivalue);
    Avalue = Ivalue;
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(Avalue);
    break;
  case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(Ivalue);
    Avalue = Ivalue;
    break;
  case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(Avalue);
    break;
  }
  switch (B->Type()) {
   case CMD_RESULT_UINT:
    uint64_t Uvalue;
    ((CommandResult_Uint *)B)->Value(Uvalue);
    Bvalue = Uvalue;
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(Ivalue);
    Bvalue = Ivalue;
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(Bvalue);
    break;
  case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(Ivalue);
    Bvalue = Ivalue;
    break;
  case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(Bvalue);
    break;
  }
  switch (C->Type()) {
   case CMD_RESULT_UINT:
    uint64_t Uvalue;
    ((CommandResult_Uint *)C)->Value(Uvalue);
    Cvalue = Uvalue;
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)C)->Value(Ivalue);
    Cvalue = Ivalue;
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)C)->Value(Cvalue);
    break;
  case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)C)->Value(Ivalue);
    Cvalue = Ivalue;
    break;
  case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)C)->Value(Cvalue);
    break;
  }
  if (Cvalue <= 1.0) {
    return NULL;
  }

  double result = sqrt (((Cvalue * Bvalue) - (Avalue * Avalue)) / (Cvalue * (Cvalue -1.0)));
  return isnan(result) ? NULL : new CommandResult_Float (result);
}

CommandResult *Calculate_Percent (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Percent, A=%x, B=%x\n", A, B);
#endif

  if (B == NULL) {
    return NULL;
  }
  if (A == NULL) {
    return new CommandResult_Float (0.0);
  }

  double Avalue;
  double Bvalue;
  int64_t Ivalue;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Percent, A->Type()=%d, B->Type()=%d\n", 
         A->Type(), B->Type());
#endif

  switch (A->Type()) {
   case CMD_RESULT_UINT:
    uint64_t Uvalue;
    ((CommandResult_Uint *)A)->Value(Uvalue);
    Avalue = Uvalue;
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(Ivalue);
    Avalue = Ivalue;
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(Avalue);
    break;
  case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(Ivalue);
    Avalue = Ivalue;
    break;
  case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(Avalue);
    break;
  }
  switch (B->Type()) {
   case CMD_RESULT_UINT:
    uint64_t Uvalue;
    ((CommandResult_Uint *)B)->Value(Uvalue);
    Bvalue = Uvalue;
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(Ivalue);
    Bvalue = Ivalue;
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(Bvalue);
    break;
  case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(Ivalue);
    Bvalue = Ivalue;
    break;
  case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(Bvalue);
    break;
  }

  double percent = 0.0;
  if (Bvalue > 0.0) {
    percent = (Avalue *100) / Bvalue;
    // if (percent > 100.0) percent = 100.0;
    if (percent < 0.0) percent = 0.0;
  }
  return new CommandResult_Float (percent);
}
