/******************************************************************************
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

#include "SS_Input_Manager.hxx"

//#define DEBUG_CLI 1

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

// Create an initial CommandResult* with the same data type
// as the argument but with a NULL initial value.
// Some items are for accounting and do not represent data.
CommandResult *CR_Init_of_CR_type( CommandResult *A )
{
#if DEBUG_CLI
  std::cerr << "Enter CommandResultCR_Init_of_CR_type A->Type()=" <<  A->Type() 
            << " CMD_RESULT_STRING=" << CMD_RESULT_STRING << " CMD_RESULT_FLOAT=" <<  CMD_RESULT_FLOAT << std::endl;
#endif
  switch (A->Type()) {
    case CMD_RESULT_ADDRESS: return new CommandResult_Address();
    case CMD_RESULT_UINT: return new CommandResult_Uint();
    case CMD_RESULT_INT: return new CommandResult_Int();
    case CMD_RESULT_FLOAT: return new CommandResult_Float();
    case CMD_RESULT_STRING: return new CommandResult_String();
    case CMD_RESULT_RAWSTRING: return new CommandResult_RawString();
//    case CMD_RESULT_FUNCTION: return new CommandResult_Function();
//    case CMD_RESULT_STATEMENT: return new CommandResult_Statement();
//    case CMD_RESULT_LINKEDOBJECT: return new CommandResult_LinkedObject();
//    case CMD_RESULT_CALLTRACE: return new CommandResult_CallStackEntry();
    case CMD_RESULT_TIME: return new CommandResult_Time();
    case CMD_RESULT_DURATION: return new CommandResult_Duration();
    case CMD_RESULT_INTERVAL: return new CommandResult_Interval();
//    case CMD_RESULT_TITLE: return new CommandResult_Tile();
//    case CMD_RESULT_COLUMN_HEADER: return new CommandResult_Header();
//    case CMD_RESULT_COLUMN_VALUES: return new CommandResult_Values();
//    case CMD_RESULT_COLUMN_ENDER: return new CommandResult_Ender();
//    case CMD_RESULT_EXTENSION: return new CommandResult_Extension();
    default: return NULL;  // Should we Assert?
  }
}

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
  uint64_t Uvalue;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Average, A->Type()=%d, B->Type()=%d\n", A->Type(), B->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
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

  if (Bvalue == 0.0) return NULL;
  double average = Avalue / Bvalue;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Average, Avalue=%f, Bvalue()=%f, average=%f\n", 
         Avalue, Bvalue, average);
#endif

  switch (A->Type()) {
   case CMD_RESULT_DURATION:
    return std::isnan(average) ? NULL : new CommandResult_Duration (average);
   case CMD_RESULT_INTERVAL:
    return std::isnan(average) ? NULL : new CommandResult_Interval (average);
  }
  return std::isnan(average) ? NULL : new CommandResult_Float (average);
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

  double Avalue = 0.0;
  double Bvalue = 0.0;
  double Cvalue = 0.0;
  int64_t Ivalue = 0;
  uint64_t Uvalue;
  
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
  switch (A->Type()) {
   case CMD_RESULT_DURATION:
    return std::isnan(result) ? NULL : new CommandResult_Duration (result);
   case CMD_RESULT_INTERVAL:
    return std::isnan(result) ? NULL : new CommandResult_Interval (result);
  }
  return std::isnan(result) ? NULL : new CommandResult_Float (result);
}


CommandResult *Calculate_Flops  (CommandResult *A, CommandResult *B) {

  if ((A == NULL) ||
      (B == NULL)) {
    return NULL;
  }

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Flops, A=%x, B=%x\n", A, B);
#endif

  double Avalue = 0.0;
  double Bvalue = 1.0;
  int64_t Ivalue = 0;
  uint64_t Uvalue;
  
#if DEBUG_CLI
  printf("In CommandResult *Calculate_Flops, A->Type()=%d, B->Type()=%d\n", 
         A->Type(), B->Type());
#endif

  switch (A->Type()) {
   case CMD_RESULT_UINT:
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

  if (Bvalue == 0.0) return NULL;
//  double result =  ( (Avalue / ((Bvalue / 1000000.0) * 1000000.0))) ;
  double result =  ( (Avalue / ((Bvalue / 1000000.0) * 1000000.0)) / 1000000.0) ;

  switch (A->Type()) {
   case CMD_RESULT_DURATION:
    return std::isnan(result) ? NULL : new CommandResult_Duration (result);
   case CMD_RESULT_INTERVAL:
    return std::isnan(result) ? NULL : new CommandResult_Interval (result);
  }
  return std::isnan(result) ? NULL : new CommandResult_Float (result);
}


CommandResult *Calculate_Ratio  (CommandResult *A, CommandResult *B) {

  if ((A == NULL) ||
      (B == NULL)) {
    return NULL;
  }

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Ratio, A=%x, B=%x\n", A, B);
#endif

  double Avalue = 0.0;
  double Bvalue = 1.0;
  int64_t Ivalue = 0;
  uint64_t Uvalue;
  
#if DEBUG_CLI
  printf("In CommandResult *Calculate_Ratio, A->Type()=%d, B->Type()=%d\n", 
         A->Type(), B->Type());
#endif

  switch (A->Type()) {
   case CMD_RESULT_UINT:
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


  if (Bvalue == 0.0) return NULL;
  double result =  (Avalue / Bvalue ) ;

  switch (A->Type()) {
   case CMD_RESULT_DURATION:
    return std::isnan(result) ? NULL : new CommandResult_Duration (result);
   case CMD_RESULT_INTERVAL:
    return std::isnan(result) ? NULL : new CommandResult_Interval (result);
  }
  return std::isnan(result) ? NULL : new CommandResult_Float (result);
}


CommandResult *Calculate_Inverse_Ratio_Percent  (CommandResult *A, CommandResult *B) {

  if ((A == NULL) ||
      (B == NULL)) {
    return NULL;
  }

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Ratio, A=%x, B=%x\n", A, B);
#endif

  double Avalue = 0.0;
  double Bvalue = 1.0;
  int64_t Ivalue = 0;
  uint64_t Uvalue;
  
#if DEBUG_CLI
  printf("In CommandResult *Calculate_Ratio, A->Type()=%d, B->Type()=%d\n", 
         A->Type(), B->Type());
#endif

  switch (A->Type()) {
   case CMD_RESULT_UINT:
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


  double result;
  if (Bvalue == 0.0) return NULL;
#if 1
  result = 100.0 - ((Avalue *100) / Bvalue);
  if (result < 0.0) result = 0.0;
#else
  result =  (Avalue / Bvalue ) ;
#endif

  switch (A->Type()) {
   case CMD_RESULT_DURATION:
    return std::isnan(result) ? NULL : new CommandResult_Duration (result);
   case CMD_RESULT_INTERVAL:
    return std::isnan(result) ? NULL : new CommandResult_Interval (result);
  }
  return std::isnan(result) ? NULL : new CommandResult_Float (result);
}


CommandResult *Calculate_Ratio_Percent  (CommandResult *A, CommandResult *B) {

  if ((A == NULL) ||
      (B == NULL)) {
    return NULL;
  }

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Ratio_Percent, A=%x, B=%x\n", A, B);
#endif

  double Avalue = 0.0;
  double Bvalue = 1.0;
  int64_t Ivalue = 0;
  uint64_t Uvalue;
  
#if DEBUG_CLI
  printf("In CommandResult *Calculate_Ratio_Percent, A->Type()=%d, B->Type()=%d\n", 
         A->Type(), B->Type());
#endif

  switch (A->Type()) {
   case CMD_RESULT_UINT:
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


  double result;
  if (Bvalue == 0.0) return NULL;
#if 1
  result = (Avalue *100) / Bvalue;
  if (result < 0.0) result = 0.0;
#else
  result =  (Avalue / Bvalue ) ;
#endif

  switch (A->Type()) {
   case CMD_RESULT_DURATION:
    return std::isnan(result) ? NULL : new CommandResult_Duration (result);
   case CMD_RESULT_INTERVAL:
    return std::isnan(result) ? NULL : new CommandResult_Interval (result);
  }
  return std::isnan(result) ? NULL : new CommandResult_Float (result);
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
  uint64_t Uvalue;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Percent, A->Type()=%d, B->Type()=%d\n", 
         A->Type(), B->Type());
#endif

  switch (A->Type()) {
   case CMD_RESULT_UINT:
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
  return std::isnan(percent) ? NULL : new CommandResult_Float (percent);
}


static CommandResult *Calculate_Add (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Add, A->Type()=%d, B->Type()=%d\n", A->Type(), B->Type());
  std::cerr << "\tA = " << ((A != NULL) ? A->Form() : "(nil)")
            << "\tB = " << ((B != NULL) ? B->Form() : "(nil)")
            << std::endl;
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;
  double BDvalue= 0.0;
  int64_t BIvalue= 0;
  uint64_t BUvalue= 0;

  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);

    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    break;
  }


  switch (B->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)B)->Value(BUvalue);
    if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue + BUvalue);
    } else if ( (A->Type() == CMD_RESULT_INT) ||
                (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue + BUvalue);
    } else {
      return CRPTR (ADvalue + BUvalue);
    }
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue + BIvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue + BIvalue);
    } else {
      return CRPTR (ADvalue + BIvalue);
    }
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR (ADvalue + BDvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue + BDvalue);
    } else {
      return CRPTR (AIvalue + BDvalue);
    }
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue + BIvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue + BIvalue);
    } else {
      return CRPTR (ADvalue + BIvalue);
    }
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR (ADvalue + BDvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue + BDvalue);
    } else {
      return CRPTR (AIvalue + BDvalue);
    }
    break;
  }

  return NULL;
}

static CommandResult *Calculate_Sub (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Sub, A=%x, B=%x\n", A, B);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;
  double BDvalue= 0.0;
  int64_t BIvalue= 0;
  uint64_t BUvalue= 0;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Sub, A->Type()=%d, B->Type()=%d\n", A->Type(), B->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    break;
  }


  switch (B->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)B)->Value(BUvalue);
    if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue - BUvalue);
    } else if ( (A->Type() == CMD_RESULT_INT) ||
                (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue - BUvalue);
    } else {
      return CRPTR (ADvalue - BUvalue);
    }
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue - BIvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue - BIvalue);
    } else {
      return CRPTR (ADvalue - BIvalue);
    }
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR (ADvalue - BDvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue - BDvalue);
    } else {
      return CRPTR (AIvalue - BDvalue);
    }
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue - BIvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue - BIvalue);
    } else {
      return CRPTR (ADvalue - BIvalue);
    }
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR (ADvalue - BDvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue - BDvalue);
    } else {
      return CRPTR (AIvalue - BDvalue);
    }
    break;
  }

  return NULL;
}

static CommandResult *Calculate_Mult (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Mult, A=%x, B=%x\n", A, B);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;
  double BDvalue= 0.0;
  int64_t BIvalue= 0;
  uint64_t BUvalue= 0;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Mult, A->Type()=%d, B->Type()=%d\n", A->Type(), B->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    break;
  }


  switch (B->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)B)->Value(BUvalue);
    if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue * BUvalue);
    } else if ( (A->Type() == CMD_RESULT_INT) ||
                (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue * BUvalue);
    } else {
      return CRPTR (ADvalue * BUvalue);
    }
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue * BIvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue * BIvalue);
    } else {
      return CRPTR (ADvalue * BIvalue);
    }
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR (ADvalue * BDvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue * BDvalue);
    } else {
      return CRPTR (AIvalue * BDvalue);
    }
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue * BIvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue * BIvalue);
    } else {
      return CRPTR (ADvalue * BIvalue);
    }
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR (ADvalue * BDvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue * BDvalue);
    } else {
      return CRPTR (AIvalue * BDvalue);
    }
    break;
  }

  return NULL;
}

static CommandResult *Calculate_Div (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Div, A=%x, B=%x\n", A, B);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }
  if (B->ValueIsNull()) return NULL;

  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;
  double BDvalue= 0.0;
  int64_t BIvalue= 0;
  uint64_t BUvalue= 0;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Div, A->Type()=%d, B->Type()=%d\n", A->Type(), B->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    break;
  }


  switch (B->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)B)->Value(BUvalue);
    if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue / BUvalue);
    } else if ( (A->Type() == CMD_RESULT_INT) ||
                (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue / BUvalue);
    } else {
      return CRPTR (ADvalue / BUvalue);
    }
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue / BIvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue / BIvalue);
    } else {
      return CRPTR (ADvalue / BIvalue);
    }
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR (ADvalue / BDvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue / BDvalue);
    } else {
      return CRPTR (AIvalue / BDvalue);
    }
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR (AIvalue / BIvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue / BIvalue);
    } else {
      return CRPTR (ADvalue / BIvalue);
    }
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR (ADvalue / BDvalue);
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue / BDvalue);
    } else {
      return CRPTR (AIvalue / BDvalue);
    }
    break;
  }

  return NULL;
}

static CommandResult *Calculate_Mod (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Mod, A=%x, B=%x\n", A, B);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;
  double BDvalue= 0.0;
  int64_t BIvalue= 0;
  uint64_t BUvalue= 0;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Mod, A->Type()=%d, B->Type()=%d\n", A->Type(), B->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    AIvalue = ADvalue;
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    AIvalue = ADvalue;
    break;
   default: return NULL;
  }


  switch (B->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)B)->Value(BUvalue);
    if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR (AUvalue % BUvalue);
    }
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(BIvalue);
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(BDvalue);
    BIvalue = BDvalue;
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(BIvalue);
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(BDvalue);
    BIvalue = BDvalue;
    break;
   default: return NULL;
  }

  if (BIvalue == 0) return  NULL;
  if (A->Type() == CMD_RESULT_UINT) {
    return CRPTR (AUvalue % BIvalue);
  }
  return CRPTR (AIvalue % BIvalue);
}

static CommandResult *Calculate_Min (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Min, A=%x, B=%x\n", A, B);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;
  double BDvalue= 0.0;
  int64_t BIvalue= 0;
  uint64_t BUvalue= 0;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Min, A->Type()=%d, B->Type()=%d\n", A->Type(), B->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    break;
  }


  switch (B->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)B)->Value(BUvalue);
    if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (AUvalue <= BUvalue) ? AUvalue : BUvalue );
    } else if ( (A->Type() == CMD_RESULT_INT) ||
                (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR ( (AIvalue <= BUvalue) ? AIvalue : BUvalue );
    } else {
      return CRPTR ( (ADvalue <= BUvalue) ? ADvalue : BUvalue );
    }
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR ( (AIvalue <= BIvalue) ? AIvalue : BIvalue );
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (AUvalue <= BIvalue) ? AUvalue : BIvalue );
    } else {
      return CRPTR ( (ADvalue <= BIvalue) ? ADvalue : BIvalue );
    }
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR ( (ADvalue <= BDvalue) ? ADvalue : BDvalue );
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (AUvalue <= BDvalue) ? AUvalue : BDvalue );
    } else {
      return CRPTR ( (AIvalue <= BDvalue) ? AIvalue : BDvalue );
    }
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR ( (AIvalue <= BIvalue) ? AIvalue : BIvalue );
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (AUvalue <= BIvalue) ? AUvalue : BIvalue );
    } else {
      return CRPTR ( (ADvalue <= BIvalue) ? ADvalue : BIvalue );
    }
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR ( (ADvalue <= BDvalue) ? ADvalue : BDvalue );
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (AUvalue <= BDvalue) ? AUvalue : BDvalue );
    } else {
      return CRPTR ( (AIvalue <= BDvalue) ? AIvalue : BDvalue );
    }
    break;
  }

  return NULL;
}

static CommandResult *Calculate_Max (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Max, A=%x, B=%x\n", A, B);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;
  double BDvalue= 0.0;
  int64_t BIvalue= 0;
  uint64_t BUvalue= 0;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Max, A->Type()=%d, B->Type()=%d\n", A->Type(), B->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    break;
  }


  switch (B->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)B)->Value(BUvalue);
    if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (AUvalue > BUvalue) ? AUvalue : BUvalue );
    } else if ( (A->Type() == CMD_RESULT_INT) ||
                (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR ( (AIvalue > BUvalue) ? AIvalue : BUvalue );
    } else {
      return CRPTR ( (ADvalue > BUvalue) ? ADvalue : BUvalue );
    }
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR ( (AIvalue > BIvalue) ? AIvalue : BIvalue );
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (AUvalue > BIvalue) ? AUvalue : BIvalue );
    } else {
      return CRPTR ( (ADvalue > BIvalue) ? ADvalue : BIvalue );
    }
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR ( (ADvalue > BDvalue) ? ADvalue : BDvalue );
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (AUvalue > BDvalue) ? AUvalue : BDvalue );
    } else {
      return CRPTR ( (AIvalue > BDvalue) ? AIvalue : BDvalue );
    }
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(BIvalue);
    if ( (A->Type() == CMD_RESULT_INT) ||
         (A->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR ( (AIvalue > BIvalue) ? AIvalue : BIvalue );
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (AUvalue > BIvalue) ? AUvalue : BIvalue );
    } else {
      return CRPTR ( (ADvalue > BIvalue) ? ADvalue : BIvalue );
    }
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(BDvalue);
    if ( (A->Type() == CMD_RESULT_FLOAT) ||
         (A->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR ( (ADvalue > BDvalue) ? ADvalue : BDvalue );
    } else if (A->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (AUvalue > BDvalue) ? AUvalue : BDvalue );
    } else {
      return CRPTR ( (AIvalue > BDvalue) ? AIvalue : BDvalue );
    }
    break;
  }

  return NULL;
}

static CommandResult *Calculate_Condexp (CommandResult *A, CommandResult *B, CommandResult *C) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Condexp, A=%x, B=%x C=%x\n", A, B, C);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  bool condition = FALSE;
  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;
  double BDvalue= 0.0;
  int64_t BIvalue= 0;
  uint64_t BUvalue= 0;
  double CDvalue= 0.0;
  int64_t CIvalue= 0;
  uint64_t CUvalue= 0;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Condexp, A->Type()=%d, B->Type()=%d\n", A->Type(), B->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    condition = (AUvalue != 0);
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    condition = (AIvalue != 0);
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    condition = (ADvalue != 0);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    condition = (AIvalue != 0);
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    condition = (ADvalue != 0);
    break;
  }

  switch (B->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)B)->Value(BUvalue);
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)B)->Value(BIvalue);
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)B)->Value(BDvalue);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)B)->Value(BIvalue);
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)B)->Value(BDvalue);
    break;
  }


  switch (C->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)C)->Value(CUvalue);
    if (B->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (condition) ? BUvalue : CUvalue );
    } else if ( (B->Type() == CMD_RESULT_INT) ||
                (B->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR ( (condition) ? BIvalue : CUvalue );
    } else {
      return CRPTR ( (condition) ? BDvalue : CUvalue );
    }
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)C)->Value(CIvalue);
    if ( (B->Type() == CMD_RESULT_INT) ||
         (B->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR ( (condition) ? BIvalue : CIvalue );
    } else if (B->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (condition) ? BUvalue : CIvalue );
    } else {
      return CRPTR ( (condition) ? BDvalue : CIvalue );
    }
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)C)->Value(CDvalue);
    if ( (B->Type() == CMD_RESULT_FLOAT) ||
         (B->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR ( (condition) ? BDvalue : CDvalue );
    } else if (B->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (condition) ? BUvalue : CDvalue );
    } else {
      return CRPTR ( (condition) ? BIvalue : CDvalue );
    }
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)C)->Value(CIvalue);
    if ( (B->Type() == CMD_RESULT_INT) ||
         (B->Type() == CMD_RESULT_DURATION) ) {
      return CRPTR ( (condition) ? BIvalue : CIvalue );
    } else if (B->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (condition) ? BUvalue : CIvalue );
    } else {
      return CRPTR ( (condition) ? BDvalue : CIvalue );
    }
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)C)->Value(CDvalue);
    if ( (B->Type() == CMD_RESULT_FLOAT) ||
         (B->Type() == CMD_RESULT_INTERVAL) ) {
      return CRPTR ( (condition) ? BDvalue : CDvalue );
    } else if (B->Type() == CMD_RESULT_UINT) {
      return CRPTR ( (condition) ? BUvalue : CDvalue );
    } else {
      return CRPTR ( (condition) ? BIvalue : CDvalue );
    }
    break;
  }

  return NULL;
}

static CommandResult *Calculate_Uminus (CommandResult *A) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Unimus, A=%x\n", A);
#endif

  if (A == NULL) {
    return NULL;
  }

  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Uminus, A->Type()=%d\n", A->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    return CRPTR ( -AUvalue );
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    return CRPTR ( -AIvalue );
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    return CRPTR ( -ADvalue );
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    return CRPTR ( -AIvalue );
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    return CRPTR ( -ADvalue );
    break;
  }

  return NULL;
}

static CommandResult *Calculate_Abs (CommandResult *A) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Abs, A=%x\n", A);
#endif

  if (A == NULL) {
    return NULL;
  }

  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Abs, A->Type()=%d\n", A->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    return CRPTR ( (AUvalue >= 0) ? AUvalue : -AUvalue );
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    return CRPTR ( (AIvalue >= 0) ? AIvalue : -AIvalue );
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    return CRPTR ( (ADvalue >= 0) ? ADvalue : -ADvalue );
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    return CRPTR ( (AIvalue >= 0) ? AIvalue : -AIvalue );
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    return CRPTR ( (ADvalue >= 0) ? ADvalue : -ADvalue );
    break;
  }

  return NULL;
}

static CommandResult *Calculate_Sqrt (CommandResult *A) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Sqrt, A=%x\n", A);
#endif

  if (A == NULL) {
    return NULL;
  }

  double ADvalue = 0.0;
  int64_t AIvalue= 0;
  uint64_t AUvalue= 0;

#if DEBUG_CLI
  printf("In CommandResult *Calculate_Sqrt, A->Type()=%d\n", A->Type());
#endif


  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    ADvalue = AUvalue;
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    ADvalue = AIvalue;
    break;
   case CMD_RESULT_FLOAT:
    ((CommandResult_Float *)A)->Value(ADvalue);
    break;
   case CMD_RESULT_DURATION:
    ((CommandResult_Duration *)A)->Value(AIvalue);
    ADvalue = AIvalue;
    break;
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Interval *)A)->Value(ADvalue);
    break;
  }

  double result = sqrt (ADvalue);
  switch (A->Type()) {
   case CMD_RESULT_DURATION:
    return std::isnan(result) ? NULL : new CommandResult_Duration (result);
   case CMD_RESULT_INTERVAL:
    return std::isnan(result) ? NULL : new CommandResult_Interval (result);
   default:
    return std::isnan(result) ? NULL : new CommandResult_Float (result);
  }
}

// The Accumulate (or Array) operations must appear to accumulate values
// into the first operand.  If a new result operand is created, the
// space for the first operand must be reclaimed by the system.
static CommandResult *Calculate_A_Add (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_A_Add, A=%x, B=%x\n", A, B);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  if (A->Type() == B->Type()) {
    A->Accumulate_Value(B);
    return A;
  } else {
    CommandResult *R = Calculate_Add (A, B);
    delete A;
    return R;
  }
}

static CommandResult *Calculate_A_Mult (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_A_Mult, A=%x, B=%x\n", A, B);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  CommandResult *R = Calculate_Add (A, B);
  delete A;
  return R;
}

static CommandResult *Calculate_A_Min (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_A_Min, A=%x, B=%x\n", A, B);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  if (A->Type() == B->Type()) {
    A->Accumulate_Min(B);
    return A;
  } else {
    CommandResult *R = Calculate_Min (A, B);
    delete A;
    return R;
  }
}

static CommandResult *Calculate_A_Max (CommandResult *A, CommandResult *B) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_A_Max, A=%x, B=%x\n", A, B);
#endif

  if (A == NULL) {
    return (B == NULL) ? NULL : B->Copy();
  } else if (B == NULL) {
    return A->Copy();
  }

  if (A->Type() == B->Type()) {
    A->Accumulate_Max(B);
    return A;
  } else {
    CommandResult *R = Calculate_Max (A, B);
    delete A;
    return R;
  }
}

static CommandResult *ConvertToUint (CommandResult *A)
{
  uint64_t AUvalue = 0;

#if DEBUG_CLI
  printf("Enter CommandResult *ConvertToUint, A=%n",A);
#endif

  if (A == NULL) {
    return NULL;
  }

  switch (A->Type()) {
   case CMD_RESULT_UINT:
    ((CommandResult_Uint *)A)->Value(AUvalue);
    break;
   case CMD_RESULT_INT:
    { 
      int64_t AIvalue = 0;
      ((CommandResult_Int *)A)->Value(AIvalue);
      AUvalue = AIvalue;
    }
    break;
   case CMD_RESULT_FLOAT:
   case CMD_RESULT_DURATION:
   case CMD_RESULT_INTERVAL:
    {
      double ADvalue = 0.0;
      ((CommandResult_Float *)A)->Value(ADvalue);
      AUvalue = ADvalue;
    }
    break;
  }

  return CRPTR ( AUvalue );
}

static CommandResult *ConvertToInt (CommandResult *A)
{
  int64_t AIvalue = 0;

#if DEBUG_CLI
  printf("Enter CommandResult *ConvertToInt, A=%n",A);
#endif

  if (A == NULL) {
    return NULL;
  }

  switch (A->Type()) {
   case CMD_RESULT_UINT:
    {
      uint64_t AUvalue = 0;
      ((CommandResult_Uint *)A)->Value(AUvalue);
      AIvalue = AUvalue;
    }
    break;
   case CMD_RESULT_INT:
    ((CommandResult_Int *)A)->Value(AIvalue);
    break;
   case CMD_RESULT_FLOAT:
   case CMD_RESULT_DURATION:
   case CMD_RESULT_INTERVAL:
    {
      double ADvalue = 0.0;
      ((CommandResult_Float *)A)->Value(ADvalue);
      AIvalue = ADvalue;
    }
    break;
  }

  return CRPTR ( AIvalue );
}

static CommandResult *ConvertToFloat (CommandResult *A)
{
  double ADvalue = 0.0;

#if DEBUG_CLI
  printf("Enter CommandResult *ConvertToUint, A=%n",A);
#endif

  if (A == NULL) {
    return NULL;
  }

  switch (A->Type()) {
   case CMD_RESULT_UINT:
    {
      uint64_t AUvalue = 0;
      ((CommandResult_Uint *)A)->Value(AUvalue);
      ADvalue = AUvalue;
    }
    break;
   case CMD_RESULT_INT:
    {
      int64_t AIvalue = 0;
      ((CommandResult_Int *)A)->Value(AIvalue);
      ADvalue = AIvalue;
    }
    break;
   case CMD_RESULT_FLOAT:
   case CMD_RESULT_DURATION:
   case CMD_RESULT_INTERVAL:
    ((CommandResult_Float *)A)->Value(ADvalue);
    break;
  }

  return CRPTR ( ADvalue );
}

CommandResult *Calculate_Expression(expression_operation_t op,
                                    CommandResult *A,
                                    CommandResult *B,
                                    CommandResult *C) {

#if DEBUG_CLI
  printf("Enter CommandResult *Calculate_Expression %s, A=%x, B=%x C=%x\n",
          ExprOperator(op).c_str(), A, B, C);
#endif

  switch (op) {
   case EXPRESSION_OP_ERROR: return NULL;
//   case EXPRESSION_OP_NAME:
//   case EXPRESSION_OP_CONST:
   case EXPRESSION_OP_UMINUS: return Calculate_Uminus (A);
   case EXPRESSION_OP_ABS: return Calculate_Abs (A);
   case EXPRESSION_OP_ADD: return Calculate_Add (A, B);
   case EXPRESSION_OP_SUB: return Calculate_Sub (A, B);
   case EXPRESSION_OP_MULT: return Calculate_Mult (A, B);
   case EXPRESSION_OP_DIV: return Calculate_Div (A, B);
   case EXPRESSION_OP_MOD: return Calculate_Mod (A, B);
   case EXPRESSION_OP_MIN: return Calculate_Min (A, B);
   case EXPRESSION_OP_MAX: return Calculate_Max (A, B);
   case EXPRESSION_OP_SQRT: return Calculate_Sqrt (A);
   case EXPRESSION_OP_STDEV: return Calculate_StdDev (A, B, C);
   case EXPRESSION_OP_PERCENT: return Calculate_Percent (A, B);
   case EXPRESSION_OP_CONDEXP: return Calculate_Condexp (A, B, C);
   case EXPRESSION_OP_A_ADD: return Calculate_A_Add (A, B);
   case EXPRESSION_OP_A_MULT: return Calculate_A_Mult (A, B);
   case EXPRESSION_OP_A_MIN: return Calculate_A_Min (A, B);
   case EXPRESSION_OP_A_MAX: return Calculate_A_Max (A, B);
   case EXPRESSION_OP_UINT: return ConvertToUint (A);
   case EXPRESSION_OP_INT: return ConvertToInt (A);
   case EXPRESSION_OP_FLOAT: return ConvertToFloat (A);
   case EXPRESSION_OP_RATIO: return Calculate_Ratio (A, B);
   default:
    return NULL;
  }
  return new CommandResult_Int (1);
}
