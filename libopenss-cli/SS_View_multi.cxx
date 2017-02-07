/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
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
//#define DEBUG_CLI_print 1

template <class T>
struct sort_ascending_CommandResult : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return (*x.second)[0]->LT((*y.second)[0]);
    }
};
template <class T>
struct sort_descending_CommandResult : public std::binary_function<T,T,bool> {
    bool operator()(const T& x, const T& y) const {
        return (*x.second)[0]->GT((*y.second)[0]);
    }
};

static void Dump_items (std::vector<std::pair<CommandResult *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items) {

  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  std::cerr << "\nDump items.  Number of items is " << c_items.size() << "\n";

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, copy the desired sort value into the VMulti_sort_temp field.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    int64_t i;
    for (i = 0; i < (*cp.second).size(); i++ ) {
      CommandResult *p = (*cp.second)[i];
      std::cerr << "  ";
      if (p != NULL) {
        p->Print(std::cerr); std::cerr << std::endl;
      } else {
        std::cerr << "NULL\n";
      }
    }

  }

}


static void Calculate_Totals (
      CommandObject *cmd,
      ThreadGroup& tgrp,
      std::vector<Collector>& CV,
      std::vector<std::string>& MV,
      std::vector<ViewInstruction *>& IV,
      std::vector<std::pair<CommandResult *,
                            SmartPtr<std::vector<CommandResult *> > > >& c_items,
      std::vector<CommandResult *>& Total_Values) {

#if DEBUG_CLI
  printf("in Calculate_Totals, c_items.size()=%d, IV.size()=%d\n", c_items.size(), IV.size());
  printf("Total_Values:\n");
  for (int64_t i=0; i < Total_Values.size(); i++) {
    printf("\tTotal_Values[%d] = ",i);
    if (Total_Values[i] != NULL) {
      printf("%s\n",Total_Values[i]->Form().c_str());
    } else {
      printf("NULL\n");
    }
  }
#endif

  for (int64_t i = 0; i < IV.size(); i++) {

    ViewInstruction *vp = IV[i];

#if DEBUG_CLI
      printf("in Calculate_Totals, i=%d, vp->OpCode()=%d\n", i, vp->OpCode());
#endif

    CommandResult *TotalValue = NULL;
    bool Gen_Total_Percent = false;

    if (vp->OpCode() == VIEWINST_Define_Total_Metric) {
     // We calculate Total by adding all the values that were recorded for the thread group.

#if DEBUG_CLI
      printf("in Calculate_Totals, vp->OpCode() == VIEWINST_Define_Total_Metric, c_items.size()=%d\n", 
             c_items.size());
#endif

      int64_t metricIndex = vp->TMP1(); // this is a CV/MV index, not a column number!

#if DEBUG_CLI
      printf("in Calculate_Totals, vp->OpCode() == VIEWINST_Define_Total_Metric, metricIndex=%d\n", 
             metricIndex);
#endif

      TotalValue = Get_Total_Metric ( cmd, tgrp, CV[metricIndex], MV[metricIndex] );

#if DEBUG_CLI
      printf("in Calculate_Totals, VIEWINST_Define_Total_Metric, after TotalValue = Get_Total_Metric call, begin print TotalValue=\n" );
      TotalValue->Print(std::cerr);
      printf("\nin Calculate_Totals, VIEWINST_Define_Total_Metric, end print TotalValue=\n" );
#endif

      Gen_Total_Percent = true;

    } else if ( (vp->OpCode() == VIEWINST_Define_Total_Tmp) ||
                ( (vp->OpCode() == VIEWINST_Expression) &&
                  vp->AccumulateExpr() ) ) {
     // Sum the specified temp.
      int64_t tmpIndex = vp->TMP1();

#if DEBUG_CLI
      printf("in Calculate_Totals, VIEWINST_Define_Total_Tmp, tmpIndex=%d\n", tmpIndex );
#endif

      std::vector<std::pair<CommandResult *, SmartPtr<std::vector<CommandResult *> > > >::iterator ci;
      ci = c_items.begin();
      if (ci == c_items.end()) {

#if DEBUG_CLI
        printf("in Calculate_Totals, ci == c_items.end(), VIEWINST_Define_Total_Tmp, tmpIndex=%d\n", tmpIndex );
#endif

       // Clearly, we can not add up a sequence if there is none.
        Gen_Total_Percent = false;
      } else if (tmpIndex >= (*ci).second->size()) {
       // Clearly, this is an error.
#if DEBUG_CLI
        printf("in Calculate_Totals, tmpIndex >= (*ci).second->size(), VIEWINST_Define_Total_Tmp, tmpIndex=%d\n", tmpIndex );
#endif
        Gen_Total_Percent = false;
      } else {
        if (ci != c_items.end()) {
          Gen_Total_Percent = true;
#if DEBUG_CLI
        printf("in Calculate_Totals, ci != c_items.end(), VIEWINST_Define_Total_Tmp, tmpIndex=%d\n", tmpIndex );
#endif
          TotalValue = (*(*ci).second)[tmpIndex]->Copy();
#if DEBUG_CLI_print
          printf("START printing TotalValue in Copy section using  (*ci).second[tmpIndex=%d]->Print()\n", tmpIndex);
          (*(*ci).second)[tmpIndex]->Print(std::cerr);
          printf("\nEND printing TotalValue in Copy section using  (*ci).second[tmpIndex=%d]->Print()\n", tmpIndex);
#endif
        }
        ci++;
        if (Look_For_KeyWord(cmd, "ButterFly")) {
          for ( ; ci != c_items.end(); ci++) {
            TotalValue->Accumulate_Max ((*(*ci).second)[tmpIndex]);
          }
        } else {
          for ( ; ci != c_items.end(); ci++) {
            TotalValue->Accumulate_Value ((*(*ci).second)[tmpIndex]);
#if DEBUG_CLI_print
            printf("START printing TotalValue in Accumulate_Value section using  (*ci).second[tmpIndex=%d]->Print()\n", tmpIndex);
            (*(*ci).second)[tmpIndex]->Print(std::cerr);
            printf("\nEND printing TotalValue in Accumulate_Value section using  (*ci).second[tmpIndex=%d]->Print()\n", tmpIndex);
#endif
          }
        }
      }
    }

    if (Gen_Total_Percent) {
      Assert(vp->TR() >= 0);
      Total_Values[vp->TR()] = TotalValue;
#if DEBUG_CLI_print
      printf("START printing TotalValue[%d] in Gen_Total_Percent section using  TotalValue->Print()\n",vp->TR());
      TotalValue->Print(std::cerr);
      printf("\nEnd printing TotalValuei[%d]=%p in Gen_Total_Percent section using  TotalValue->Print()\n",vp->TR(),Total_Values[vp->TR()]);
#endif
    }
  }

#if DEBUG_CLI
  printf("Exit Calculate_Totals:\n");
  for (int64_t i = 0; i < Total_Values.size(); i++) {
    std::cerr << "\tTotalValue[" << i << "] = ";
    if (Total_Values[i] != NULL) {
      printf("%s\n",Total_Values[i]->Form().c_str());
    } else {
      printf("NULL\n");
    }
  }
#endif

}

static void Calculate_Temporary_Values (
      CommandObject *cmd,
      ThreadGroup& tgrp,
      std::vector<Collector>& CV,
      std::vector<std::string>& MV,
      std::vector<ViewInstruction *>& IV,
      std::vector<std::pair<CommandResult *,
                            SmartPtr<std::vector<CommandResult *> > > >& c_items,
      std::vector<CommandResult *>& Total_Values) {

#if DEBUG_CLI
  printf("in Calculate_Temporary_Values, c_items.size()=%d, IV.size()=%d\n", c_items.size(), IV.size());
#endif

 // Set up quick access to expression data combining instructions.
  int64_t num_temps_used = std::max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
  std::vector<bool> temp_is_accumulated(num_temps_used);
  for (int64_t i = 0; i < num_temps_used; i++) temp_is_accumulated[i] = false;
  std::vector<ViewInstruction *> ExprInst(num_temps_used);
  int64_t last_evaluated_temp = num_temps_used;
  int64_t num_ExprInst = 0;
  for (int64_t i = 0; i < num_temps_used; i++) ExprInst[i] = NULL;

 // Find the instructions we need to process.
  for (int64_t i = 0; i < IV.size(); i++) {
    ViewInstruction *vp = IV[i];
    if (vp->OpCode() == VIEWINST_Expression) {
      if (vp->TR() < last_evaluated_temp) last_evaluated_temp = vp->TR();
      if (vp->OpCode() == VIEWINST_Expression) {
        ExprInst[num_ExprInst++] = vp;
      }
    }
    else if ((vp->OpCode() == VIEWINST_SetConstString) ||
             (vp->OpCode() == VIEWINST_SetConstInt) ||
             (vp->OpCode() == VIEWINST_SetConstFloat)) {
        ExprInst[num_ExprInst++] = vp;
    }
  }

 // Decide which temps are metrics values and which are calculated from expressions.
  if ( (num_ExprInst > 0) &&
       (c_items.begin() != c_items.end()) ) {

    for (int i=0; i<num_ExprInst; i++) {
     // Evaluate one instruction across every sample.
      ViewInstruction *vp = ExprInst[i];
      std::vector<std::pair<CommandResult *, SmartPtr<std::vector<CommandResult *> > > >::iterator ci;

      if ((vp->OpCode() == VIEWINST_SetConstString) ||
           (vp->OpCode() == VIEWINST_SetConstInt) ||
           (vp->OpCode() == VIEWINST_SetConstFloat)) {
       // Copy the constant into temporary.
        int64_t result_temp = vp->TR();
        CommandResult *constvalue = vp->ConstValue();
        for (ci = c_items.begin(); ci != c_items.end(); ci++) {
          (*(*ci).second)[result_temp] = constvalue->Copy();
        }
        Total_Values[result_temp] = constvalue->Copy();
        last_evaluated_temp = result_temp;
        continue;
      }

     // Calculate this expression for each sample.
      CommandResult *TotalValue_temp = NULL;
      Assert( (vp->TMP1() <= last_evaluated_temp) &&
              (vp->TMP2() <= last_evaluated_temp) &&
              (vp->TMP3() <= last_evaluated_temp) );

     // Scan each data sample for each instruction.
     // (Assume the ExprInst table is sorted by the result of each instruction.)
      for (ci = c_items.begin(); ci != c_items.end(); ci++) {
        CommandResult *ExpressionValue_temp = NULL;
        if (Can_Accumulate(vp->ExprOpCode())) {
          ExpressionValue_temp = (*(*ci).second)[vp->TMP1()];
          temp_is_accumulated[vp->TR()] = true;
        } else {
          CommandResult *opr1 = NULL;
          if (vp->TMP1() >= 0) {
            opr1 = (temp_is_accumulated[vp->TMP1()])
                                ? Total_Values[vp->TMP1()] : (*(*ci).second)[vp->TMP1()];
          }
          CommandResult *opr2 = NULL;
          if (vp->TMP2() >= 0) {
            opr2 = (temp_is_accumulated[vp->TMP2()])
                                ? Total_Values[vp->TMP2()] : (*(*ci).second)[vp->TMP2()];
          }
          CommandResult *opr3 = NULL;
          if (vp->TMP3() >= 0) {
            opr3 = (temp_is_accumulated[vp->TMP3()])
                                ? Total_Values[vp->TMP3()] : (*(*ci).second)[vp->TMP3()];
          }
          ExpressionValue_temp = Calculate_Expression ( vp->ExprOpCode(),
                                                        opr1, // (*(*ci).second)[vp->TMP1()],
                                                        opr2, // (*(*ci).second)[vp->TMP2()],
                                                        opr3); // (*(*ci).second)[vp->TMP3()]);
          (*(*ci).second)[vp->TR()] = ExpressionValue_temp;
        }
        if (Can_Accumulate(vp->ExprOpCode())) {
          TotalValue_temp = Calculate_Expression ( vp->ExprOpCode(),
                                                   TotalValue_temp,
                                                   ExpressionValue_temp,
                                                 (*(*ci).second)[vp->TMP3()]);
        }
      }

      if (TotalValue_temp != NULL) {
        Total_Values[vp->TR()] = TotalValue_temp;
      }
      last_evaluated_temp = vp->TR();
    }

  }

}

static void Pack_Vector_Elements (
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
#if DEBUG_CLI
  printf("in Pack_Vector_Elements\n");
#endif
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Find the next NULL element in the vector.
    if ((*vpi).first == NULL) {
      std::vector<std::pair<CommandResult *,
                            SmartPtr<std::vector<CommandResult *> > > >::iterator nvpi = vpi+1;

      for ( ; nvpi != c_items.end(); nvpi++) {
        if ((*nvpi).first != NULL) {
         // Fill the NULL element with the first non-NULL one.
          *vpi = *nvpi;
          (*nvpi).first = NULL;
          break;
        }
      }

      if (nvpi == c_items.end()) {
       // Found no non-NULL elements in rest of vector.
        break;
      }

    }
  }

  if (vpi != c_items.end()) {
   // There are NULL elements at the end of the vector.
   // Get rid of them!
    c_items.erase (vpi, c_items.end());
  }

#if DEBUG_CLI
// Integrity check:
// Be sure there are no NULL elements left  in the vector.
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
    Assert ((*vpi).first != NULL);
  }
#endif

}

static void Suppress_Unused_Elements (
       std::vector<std::pair<CommandResult *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  bool NullItemsRemoved = false;
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach entry, eliminate the entries that have a NULL sort value.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    CommandResult *Sort_Value = (*cp.second)[VMulti_sort_temp];
    if (Sort_Value->ValueIsNull()) {
     // Because a "c_items.erase(vpi);" operation takes a lot of time
     // when vectors get long, just fill with a null pointer and compress
     // the vector after all unnecessary entries have been identified.
#if DEBUG_CLI
      std::cerr << "Suppress Element: ";
      int64_t i;
      for (i = 0; i < (*cp.second).size(); i++ ) {
        CommandResult *p = (*cp.second)[i];
        if (p != NULL) {
          std::cerr << p->Form();
        } else {
          std::cerr << "(NULL)";
        }
        std::cerr << ", ";
      }
      std::cerr << (*cp.first).Form() << std::endl;
#endif
      (*vpi).first = NULL;
      NullItemsRemoved = true;
    }
  }
  if (NullItemsRemoved) {
    Pack_Vector_Elements (c_items);
  }
}

static void Setup_Sort( 
       int64_t temp_index,
       std::vector<std::pair<CommandResult *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items,
       std::vector<CommandResult *>& Total_Values) {
#if DEBUG_CLI_print
  printf("in Setup_Sort, temp_index=%d, c_items.size-%d\n", temp_index,c_items.size());
#endif
  if (temp_index == VMulti_sort_temp) return;
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, copy the desired sort value into the VMulti_sort_temp field.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    CommandResult *Old = (*cp.second)[VMulti_sort_temp];
#if DEBUG_CLI_print
    if (Old != NULL) {
      printf("Setup_Sort, temp_index, START printing CommandResult (Old)\n");
      std::cerr << "  ";
      if (Old != NULL) {
        Old->Print(std::cerr); std::cerr << "\n";
      } else {
        std::cerr << "NULL\n";
      }
      printf("Setup_Sort, temp_index, END printing CommandResult Old\n");
    }
#endif
    if (Old != NULL) delete Old;
    CommandResult *V1 = (*cp.second)[temp_index];
#if DEBUG_CLI_print
    if (V1 != NULL) {
      printf("Setup_Sort, temp_index, START printing CommandResult (V1)\n");
      std::cerr << "  ";
      if (V1 != NULL) {
        V1->Print(std::cerr); std::cerr << "\n";
      } else {
        std::cerr << "NULL\n";
      }
      printf("Setup_Sort, temp_index, END printing CommandResult V1\n");
    }
#endif
    CommandResult *New = V1->Copy();
    Assert (New != NULL);
    (*cp.second)[VMulti_sort_temp] = New;
  }

  if (OPENSS_VIEW_SUPPRESS_UNUSED_ELEMENTS) {
    Suppress_Unused_Elements (c_items);
  }
}

static void Setup_Sort( 
       ViewInstruction *vinst,
       std::vector<std::pair<CommandResult *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items,
       std::vector<CommandResult *>& Total_Values) {
#if DEBUG_CLI_print
  printf("in Setup_Sort\n");
  if (vinst == NULL) {
    printf("\tbad instruction\n");
  } else {
    printf("\tprocess instruction: ");
    vinst->Print (std::cout);
  }
#endif

  if ((vinst->OpCode() == VIEWINST_Display_Metric) ||
      (vinst->OpCode() == VIEWINST_Display_Percent_Column)) {
    return;
  }

  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  int64_t temp_index = vinst->TMP1();
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, set the desired sort value into the VMulti_sort_temp field.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    CommandResult *Old = (*cp.second)[VMulti_sort_temp];

#if DEBUG_CLI_print
    if (Old != NULL) {
      printf("Setup_Sort, START printing CommandResult (Old)\n");
      std::cerr << "  ";
      if (Old != NULL) {
        Old->Print(std::cerr); std::cerr << "\n";
      } else {
        std::cerr << "NULL\n";
      }
      printf("Setup_Sort, END printing CommandResult Old\n");
    }
#endif

    if (Old != NULL) delete Old;
    CommandResult *New = NULL;
    CommandResult *V1 = (*cp.second)[temp_index];

#if DEBUG_CLI_print
    if (V1 != NULL) {
      printf("Setup_Sort, START printing CommandResult (V1)\n");
      std::cerr << "  ";
      if (V1 != NULL) {
        V1->Print(std::cerr); std::cerr << "\n";
      } else {
        std::cerr << "NULL\n";
      }
      printf("Setup_Sort, END printing CommandResult V1\n");
    }
#endif


    if (vinst->OpCode() == VIEWINST_Display_Tmp) {

#if DEBUG_CLI_print
      printf("in Setup_Sort, VIEWINST_Display_Tmp %d\n",vinst->TMP1());
#endif

      if (V1 == NULL) {
        V1 = Total_Values[temp_index];
      }
      if (V1 != NULL) New = V1->Copy();

    } else if (vinst->OpCode() == VIEWINST_Display_Percent_Tmp) {

#if DEBUG_CLI_print
      printf("in Setup_Sort, VIEWINST_Display_Percent_Tmp\n");
#endif

     // Use value without calculating percent - order will be the same.
      New = V1->Copy();

    } else if (vinst->OpCode() == VIEWINST_Display_Average_Tmp) {

#if DEBUG_CLI_print
      printf("in Setup_Sort, VIEWINST_Display_Average_Tmp\n");
#endif

      if (!V1->ValueIsNull ()) {
        New = Calculate_Average (V1, (*cp.second)[vinst->TMP2()]);
      }
    } else if (vinst->OpCode() ==VIEWINST_Display_StdDeviation_Tmp) {

#if DEBUG_CLI_print
      printf("in Setup_Sort, VIEWINST_Display_StdDeviation_Tmp\n");
#endif

      CommandResult *V1 = (*cp.second)[vinst->TMP1()];
      CommandResult *V2 = (*cp.second)[vinst->TMP2()];
      CommandResult *V3 = (*cp.second)[vinst->TMP3()];
      New = Calculate_StdDev (V1, V2, V3);

    } else if (vinst->OpCode() == VIEWINST_Display_Flops_Tmp) {

#if DEBUG_CLI_print
      printf("in Setup_Sort, VIEWINST_Display_Flops_Tmp\n");
#endif

      CommandResult *V1 = (*cp.second)[vinst->TMP1()];
      CommandResult *V2 = (*cp.second)[vinst->TMP2()];
      New = Calculate_Flops (V1, V2);

    } else if (vinst->OpCode() == VIEWINST_Display_Ratio_Tmp) {

#if DEBUG_CLI_print
      printf("in Setup_Sort, VIEWINST_Display_Ratio_Tmp\n");
#endif

      CommandResult *V1 = (*cp.second)[vinst->TMP1()];
      CommandResult *V2 = (*cp.second)[vinst->TMP2()];

    } else if (vinst->OpCode() == VIEWINST_Display_Ratio_Percent_Tmp) {

#if DEBUG_CLI_print
      printf("in Setup_Sort, VIEWINST_Display_Ratio_Percent_Tmp\n");
#endif

      CommandResult *V1 = (*cp.second)[vinst->TMP1()];
      CommandResult *V2 = (*cp.second)[vinst->TMP2()];

    } else if (vinst->OpCode() == VIEWINST_Display_Inverse_Ratio_Percent_Tmp) {

#if DEBUG_CLI_print
      printf("in Setup_Sort, VIEWINST_Display_Inverse_Ratio_Percent_Tmp\n");
#endif

      CommandResult *V1 = (*cp.second)[vinst->TMP1()];
      CommandResult *V2 = (*cp.second)[vinst->TMP2()];

    } else if (vinst->OpCode() == VIEWINST_Expression) {

#if DEBUG_CLI_print
      printf("in Setup_Sort, VIEWINST_Expression\n");
#endif

      CommandResult *V1 = (*cp.second)[vinst->TMP1()];
      CommandResult *V2 = (*cp.second)[vinst->TMP2()];
      CommandResult *V3 = (*cp.second)[vinst->TMP3()];
      New = Calculate_Expression (vinst->ExprOpCode(), V1, V2, V3);

    }

    Assert (New != NULL);
    (*cp.second)[VMulti_sort_temp] = New;
  }

  if (OPENSS_VIEW_SUPPRESS_UNUSED_ELEMENTS) {
    Suppress_Unused_Elements (c_items);
  }
}

static void Dump_Intermediate_CallStack (std::ostream &tostream,
       std::vector<std::pair<CommandResult *,
                             SmartPtr<std::vector<CommandResult *> > > >& c_items) {
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
#if DEBUG_CLI
  printf("in Dump_Intermediate_CallStack\n");
#endif
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, dump the corresponding value and the last call stack function name.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    tostream << "    ";
    for (int64_t i = 0; i < (*cp.second).size(); i++) {
      (*cp.second)[i]->Print(tostream); tostream << "  ";
    }
    ((CommandResult *)(cp.first))->Print(tostream); tostream << std::endl;
  }
}

static std::vector<CommandResult *> *
       Dup_Call_Stack (int64_t len,
                       std::vector<CommandResult *> *cs) {

#if DEBUG_CLI
  printf("in Dup_Call_Stack, len=%d\n", len);
#endif
  std::vector<CommandResult *> *call_stack;
  if (len == 0) return call_stack;
  Assert (len <= cs->size());
  call_stack = new std::vector<CommandResult *>();
  for (int64_t i = 0; i < len; i++) {
    CommandResult *CE = (*cs)[i];
    CommandResult *NCE;
    NCE = CE->Copy();
    call_stack->push_back(NCE);
  }
  return call_stack;
}

static std::vector<CommandResult *> *
       Copy_Call_Stack_Entry (int64_t idx,
                              int64_t bias,
                              std::vector<CommandResult *> *cs) {
#if DEBUG_CLI
  printf("in Copy_Call_Stack_Entry, idx=%d, bias=%d\n", idx, bias);
#endif
  std::vector<CommandResult *> *call_stack;
  call_stack = new std::vector<CommandResult *>();
  if (bias < 0) {
    call_stack->push_back (CRPTR(""));
    call_stack->push_back ((*cs)[idx-1]->Copy() );
  }
  if (bias == 0) {
    call_stack->push_back ((*cs)[idx]->Copy() );
  }
  if (bias > 0) {
    call_stack->push_back (CRPTR(""));
    call_stack->push_back ((*cs)[idx+1]->Copy() );
  }

  return call_stack;
}

static int64_t Match_Call_Stack (std::vector<CommandResult *> *cs,
                                 std::vector<CommandResult *> *ncs) {
  int64_t csz = cs->size();
  int64_t ncsz = ncs->size();
#if DEBUG_CLI
//std::cerr << "Match_Call_Stack cs.size:" << csz << " ncs.size:" << ncsz << std::endl;
#endif
  int64_t minsz = std::min(csz, ncsz);
  for (int64_t i = 0; i < minsz; i++) {
    CommandResult *cse = (*cs)[i];
    CommandResult *ncse = (*ncs)[i];
    cmd_result_type_enum ty = cse->Type();
    if (ty != ncse->Type()) {
	//std::cerr << "Match_Call_Stack cse.type:" << ty << " !=  ncse.type:" << ncse->Type() << std::endl;
	return (i - 1);
    }

    if (ty == CMD_RESULT_FUNCTION) {
     // Compare functions and Statements.
      if (*((CommandResult_Function *)cse) != *((CommandResult_Function *)ncse)) {
	//std::cerr << "Match_Call_Stack cse.CommandResult_Function !=  ncse.CommandResult_Function" << std::endl;
	return (i - 1);
      }

     // To match, the calls must be on the same line.
      int64_t leftLine = ((CommandResult_Function *)cse)->getLine();
      int64_t rightLine = ((CommandResult_Function *)ncse)->getLine();
      if (leftLine != rightLine) {
       // Line numbers, and therefore, the callstacks are different.
	//std::cerr << "Match_Call_Stack leftLine != rightLine" << std::endl;
        return (i - 1);
      }
     // To match, the calls must be at the same address.
      int64_t leftColumn = ((CommandResult_Function *)cse)->getColumn();
      int64_t rightColumn = ((CommandResult_Function *)ncse)->getColumn();
      if ( (leftColumn != 0) &&
           (rightColumn != 0) &&
           (leftColumn != rightColumn) ) {
       // Column numbers, and therefore, the callstacks are different.
	//std::cerr << "Match_Call_Stack columns do not match" << std::endl;
        return (i - 1);
      }
    } else if (ty == CMD_RESULT_LINKEDOBJECT) {
     // Compare LinkedObjects and offsets.
      uint64_t V;
      uint64_t NV;
      ((CommandResult_LinkedObject *)cse)->Value(V);
      ((CommandResult_LinkedObject *)ncse)->Value(NV);
      if (V != NV) {
	//std::cerr << "Match_Call_Stack LO V:" << V << " !=  NV:" << NV << std::endl;
	return (i - 1);
      }
      if (*((CommandResult_LinkedObject *)cse) != *((CommandResult_LinkedObject *)ncse)) {
	//std::cerr << "Match_Call_Stack cse.CommandResult_LO !=  ncse.CommandResult_LO" << std::endl;
	return (i - 1);
      }
    } else if (ty == CMD_RESULT_ADDRESS) {
     // Compare absolute addresses.
      uint64_t V;
      uint64_t NV;
      ((CommandResult_Uint *)cse)->Value(V);
      ((CommandResult_Uint *)ncse)->Value(NV);
      if (V != NV) {
	//std::cerr << "Match_Call_Stack UINT V:" << V << " !=  NV:" << NV << std::endl;
	return (i - 1);
      }
    } else if (ty == CMD_RESULT_UINT) {
     // Compare absolute addresses.
      uint64_t V;
      uint64_t NV;
      ((CommandResult_Uint *)cse)->Value(V);
      ((CommandResult_Uint *)ncse)->Value(NV);
      if (V != NV) {
	//std::cerr << "Match_Call_Stack UINT V:" << V << " !=  NV:" << NV << std::endl;
	return (i - 1);
      }
    } else if (ty == CMD_RESULT_STRING) {
     // Compare characters.
      std::string V;
      std::string NV;
      ((CommandResult_String *)cse)->Value(V);
      ((CommandResult_String *)ncse)->Value(NV);
      if (V != NV) {
	//std::cerr << "Match_Call_Stack STRING V:" << V << " !=  NV:" << NV << std::endl;
	return (i - 1);
      }
    } else {
	//std::cerr << "Match_Call_Stack DEFAULT" << std::endl;
	return (i - 1);
    }

  }
  return minsz;
}

// I don't think this function is needed anymore.
static int64_t Match_Short_Stack (SmartPtr<std::vector<CommandResult *> >& cs,
                                  SmartPtr<std::vector<CommandResult *> >& ncs) {
  int64_t csz = cs->size();
  int64_t ncsz = ncs->size();
#if DEBUG_CLI
  printf("in Match_Short_Stack, csz=%d, ncsz=%d\n", csz, ncsz);
#endif
  if ((csz <= 0) || (ncsz <= 0)) return -1;
  int64_t minsz = std::min(csz, ncsz);
  int64_t i = 0;
  // for (int64_t i = 0; i < minsz; i++) {
    CommandResult *cse = (*cs)[csz-1];
    CommandResult *ncse = (*ncs)[ncsz-1];
    cmd_result_type_enum ty = cse->Type();
    if (ty != ncse->Type()) return (i - 1);

    if (ty == CMD_RESULT_FUNCTION) {
     // Compare functions only.
      if (*((CommandResult_Function *)cse) != *((CommandResult_Function *)ncse)) return (i - 1);
    } else if (ty == CMD_RESULT_LINKEDOBJECT) {
     // Compare LinkedObjects and offsets.
      uint64_t V;
      uint64_t NV;
      ((CommandResult_LinkedObject *)cse)->Value(V);
      ((CommandResult_LinkedObject *)ncse)->Value(NV);
      if (V != NV) return (i - 1);
      if (*((CommandResult_LinkedObject *)cse) != *((CommandResult_LinkedObject *)ncse)) return (i - 1);
    } else if (ty == CMD_RESULT_UINT) {
     // Compare absolute addresses.
      uint64_t V;
      uint64_t NV;
      ((CommandResult_Uint *)cse)->Value(V);
      ((CommandResult_Uint *)ncse)->Value(NV);
      if (V != NV) return (i - 1);
    } else {
      return (i - 1);
    }

  // }
  return minsz;
}

static bool Match_Field_Requirements (std::vector<ViewInstruction *>& IV,
                                      std::vector<CommandResult *>& A,
                                      std::vector<CommandResult *>& B) {
  int64_t len =IV.size();
  for (int64_t i = 0; i < len; i++) {
    ViewInstruction *vp = IV[i];
    if (vp->OpCode() == VIEWINST_Require_Field_Equal) {
      int64_t field = vp->TMP1();
      if ((field < 0) ||
          (A.size() <= field) ||
          (B.size() <= field)) continue;
      CommandResult *X = A[field];
      CommandResult *Y = B[field];
      if ((X == NULL) ||
          (Y == NULL)) continue;
      if ((X < Y) ||
          (Y < X)) return false;
    }
  }
  return true;
}

static inline void Accumulate_PreDefined_Temps (std::vector<ViewInstruction *>& IV,
                                                std::vector<CommandResult *>& A,
                                                std::vector<CommandResult *>& B) {
  int64_t len = std::min(IV.size(),A.size());
#if DEBUG_CLI
  printf("in Accumulate_PreDefined_Temps, len=%d\n", len);
#endif
  for (int64_t i = 0; i < len; i++) {
    if ((A[i] == NULL) &&
        (B[i] == NULL)) continue;
    ViewInstruction *vp = IV[i];
    if (vp != NULL) {
      ViewOpCode Vop = vp->OpCode();
      if (Vop == VIEWINST_Add) {
        A[i]->Accumulate_Value (B[i]);
      } else if (Vop == VIEWINST_Min) {
        A[i]->Accumulate_Min (B[i]);
      } else if (Vop == VIEWINST_Max) {
        A[i]->Accumulate_Max (B[i]);
      } else if (Vop == VIEWINST_Expression) {
        A[i]->Accumulate_Max (B[i]);
      }
    }
  }
}

static void Combine_Duplicate_CallStacks (
              std::vector<ViewInstruction *>& IV,
              std::vector<ViewInstruction *>& FieldRequirements,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
#if DEBUG_CLI
  printf("in Combine_Duplicate_CallStacks\n");
#endif
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates.
    std::vector<std::pair<CommandResult *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator nvpi = vpi+1;
    if (nvpi == c_items.end()) break;
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    if (cp.first == NULL) {
      continue;
    }
    std::vector<CommandResult *> *cs = ((CommandResult_CallStackEntry *)cp.first)->Value();
    int64_t cs_size = cs->size();
   // Compare the current entry to all following ones.

    for ( ; nvpi != c_items.end(); ) {
      std::pair<CommandResult *,
                SmartPtr<std::vector<CommandResult *> > > ncp = *nvpi;
      if (ncp.first == NULL) {
        nvpi++;
        continue;
      }
      std::vector<CommandResult *> *ncs = ((CommandResult_CallStackEntry *)ncp.first)->Value();
      if (cs_size > ncs->size()) {
        break;
      }
      if (cs_size != ncs->size()) {
       // We can do this because the original call stacks are expanded in place.
        nvpi++;
        continue;
      }
      int64_t matchcount = Match_Call_Stack (cs, ncs);
      if ((matchcount >= 0) &&
          (matchcount == cs->size()) &&
          (matchcount == ncs->size()) &&
          Match_Field_Requirements(FieldRequirements, (*cp.second), (*ncp.second))) {
       // Call stacks are identical - combine values.
        Accumulate_PreDefined_Temps (IV, (*cp.second), (*ncp.second));
        delete ncp.first;
        if ((*ncp.second).begin() != (*ncp.second).end()) {
          for (int64_t i = 0; i < (*ncp.second).size(); i++) {
            delete (*ncp.second)[i];
            (*ncp.second)[i] = NULL;
          }
        }
       // Because a "c_items.erase(nvpi);" operation takes a lot of time
       // when vectors get long, just fill with a null pointer and compress
       // the vector after all unnecessary entries have been identified.
        (*nvpi).first = NULL;
        nvpi++;
        continue;
      }
     // Match failed.  Continue looking for other stacks that match.
      nvpi++;
    }

  }

  Pack_Vector_Elements (c_items);
}

static void Combine_Short_Stacks (
              std::vector<ViewInstruction *>& IV,
              std::vector<ViewInstruction *>& FieldRequirements,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
#if DEBUG_CLI
  printf("in Combine_Short_Stacks\n");
#endif
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for duplicates.
    std::vector<std::pair<CommandResult *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator nvpi = vpi+1;
    if (nvpi == c_items.end()) break;
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    if (cp.first == NULL) {
      continue;
    }
    std::vector<CommandResult *> *cs = ((CommandResult_CallStackEntry *)cp.first)->Value();
    int64_t cs_size = cs->size();
   // Compare the current entry to all following ones.

    for ( ; nvpi != c_items.end(); ) {
      std::pair<CommandResult *,
                SmartPtr<std::vector<CommandResult *> > > ncp = *nvpi;
      if (ncp.first == NULL) {
        nvpi++;
        continue;
      }
      std::vector<CommandResult *> *ncs = ((CommandResult_CallStackEntry *)ncp.first)->Value();
      if (cs_size > ncs->size()) {
        break;
      }
      if (cs_size != ncs->size()) {
       // We can do this because the original call stacks are expanded in place.
        nvpi++;
        continue;
      }
      int64_t matchcount = Match_Call_Stack (cs, ncs);
      if ((matchcount >= 0) &&
          (matchcount == cs->size()) &&
          (matchcount == ncs->size()) &&
          Match_Field_Requirements(FieldRequirements, (*cp.second), (*ncp.second))) {
       // Call stacks are identical - combine values.
        Accumulate_PreDefined_Temps (IV, (*cp.second), (*ncp.second));
        delete ncp.first;
        if ((*ncp.second).begin() != (*ncp.second).end()) {
          for (int64_t i = 0; i < (*ncp.second).size(); i++) {
            delete (*ncp.second)[i];
          }
        }
       // Because a "c_items.erase(nvpi);" operation takes a lot of time
       // when vectors get long, just fill with a null pointer and compress
       // the vector after all unnecessary entries have been identified.
        (*nvpi).first = NULL;
        nvpi++;
        continue;
      }
     // Match failed.  Keep looking.
      nvpi++;
    }

  }

  Pack_Vector_Elements (c_items);
}

static SmartPtr<std::vector<CommandResult *> >
  Dup_CRVector (std::vector<ViewInstruction *>& IV,
                SmartPtr<std::vector<CommandResult *> >& crv) {
     // Insert intermediate, dummy entry to fill a gap in the trace.
      SmartPtr<std::vector<CommandResult *> > vcs
              = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
     // Determine which data fields to duplicate
      std::set<int64_t> xset;
    // std::vector<ViewInstruction *> ViewInst(IV.size());
      for (int64_t i=0; i < IV.size(); i++) {
      //  ViewInstruction *vinst = Find_Column_Def (IV, i);
        if (IV[i]->OpCode() == VIEWINST_StackExpand) {
         // Exit if we didn't find a definition
          xset.insert(IV[i]->TMP1());
        }
      }   
     // Generate initial value for each column.
      for (int64_t j = 0; j < crv->size(); j++) {
        CommandResult *next;
        if (xset.find(j) != xset.end()) {
         // Propagate value up the calling tree.
          next = (*crv)[j]->Copy();
// printf("Propagate value up the calling tree location %lld: ",j);
// next->Print(std::cerr,20,true);printf("\n");
        } else {
         // Create an empty initial value.
          if ((*crv)[j] != NULL) {
            next = (*crv)[j]->Init();
          } else {
            next = NULL;
          }
        }
        vcs->push_back ( next );
      }

  return vcs;
}

static SmartPtr<std::vector<CommandResult *> >
  Copy_CRVector (SmartPtr<std::vector<CommandResult *> >& crv) {
     // Insert intermediate, dummy entry to fill a gap in the trace.
      SmartPtr<std::vector<CommandResult *> > vcs
              = Framework::SmartPtr<std::vector<CommandResult *> >(
                           new std::vector<CommandResult *>()
                           );
     // Generate initial value for each column.
      for (int64_t j = 0; j < crv->size(); j++) {
        if ((*crv)[j] != NULL) {
          vcs->push_back ( (*crv)[j]->Copy() );
        } else {
          vcs->push_back ( NULL );
        }
      }
  return vcs;
}

static void Extract_Pivot_Items (
              CommandObject * cmd,
              ExperimentObject *exp,
              std::vector<ViewInstruction *>& IV,
              std::vector<ViewInstruction *>& FieldRequirements,
              bool TraceBack_Order,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items,
              Function& func,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& result) {
  bool pivot_added = false;
#if 0
  printf("in Extract_Pivot_Items\n");
#endif
  std::pair<CommandResult *,
            SmartPtr<std::vector<CommandResult *> > > pivot;
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > pred;
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > succ;

  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, look for the matching function name.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    std::vector<CommandResult *> *cs = ((CommandResult_CallStackEntry *)cp.first)->Value();

    for (int64_t i = 0; i < cs->size(); i++) {
      CommandResult *cof = (*cs)[i];
      if ((cof->Type() == CMD_RESULT_FUNCTION) &&
          ((*(CommandResult_Function *)cof) == func)) {
       // Insert intermediate, dummy entry to fill a gap in the trace.
        if (!pivot_added) {
          SmartPtr<std::vector<CommandResult *> > vcs = Copy_CRVector (cp.second);
          std::vector<CommandResult *> *ncs = Copy_Call_Stack_Entry (i, 0, cs);
          CommandResult *CSE = new CommandResult_CallStackEntry (ncs, TraceBack_Order);
          pivot = std::make_pair(CSE, vcs);
          pivot_added = true;
        } else {
          Accumulate_PreDefined_Temps (IV, (*pivot.second), (*cp.second));
        }
        if (i != 0) {
          SmartPtr<std::vector<CommandResult *> > vcs = Copy_CRVector (cp.second);
          std::vector<CommandResult *> *ncs = Copy_Call_Stack_Entry (i, -1, cs);
          CommandResult *CSE = new CommandResult_CallStackEntry (ncs, !TraceBack_Order);
          pred.push_back (std::make_pair(CSE, vcs));
        }
        if ((i+1) < cs->size()) {
          SmartPtr<std::vector<CommandResult *> > vcs = Copy_CRVector (cp.second);
          std::vector<CommandResult *> *ncs = Copy_Call_Stack_Entry (i, +1, cs);
          CommandResult *CSE = new CommandResult_CallStackEntry (ncs, TraceBack_Order);
          succ.push_back (std::make_pair(CSE, vcs));
        }
        break;
      }
    }
  }
  if (pivot_added) {
    if (!pred.empty()) {
      Combine_Short_Stacks (IV, FieldRequirements, pred);
      result.insert(result.end(), pred.begin(), pred.end());
    }
    result.push_back (pivot);
    if (!succ.empty()) {
      Combine_Short_Stacks (IV, FieldRequirements, succ);
      result.insert(result.end(), succ.begin(), succ.end());
    }
  }
}

static void Expand_CallStack (
              bool TraceBack_Order,
              std::vector<ViewInstruction *>& IV,
              std::vector<std::pair<CommandResult *,
                                    SmartPtr<std::vector<CommandResult *> > > >& c_items) {
#if DEBUG_CLI
     printf("in EXPAND_CALLSTACK TraceBack_Order=%d\n",TraceBack_Order);
 // Rewrite base report by expanding the call stack.
//  std::cerr << "\nEXPAND_CALLSTACK Dump items.  Number of items is " << c_items.size() << "\n";
     printf("in EXPAND_CALLSTACK c_items.size()=%d\n",c_items.size());
//  Dump_items(std::vector<std::pair<CommandResult *, SmartPtr<std::vector<CommandResult *> > > >);
#endif
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > > result;
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;

  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {

   // Foreach CallStack entry, look for duplicates and missing intermediates.
    std::vector<std::pair<CommandResult *,
                          SmartPtr<std::vector<CommandResult *> > > >::iterator nvpi = vpi+1;
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    std::vector<CommandResult *> *cs = ((CommandResult_CallStackEntry *)cp.first)->Value();

#if DEBUG_CLI
    printf("in EXPAND_CALLSTACK  ((CommandResult_CallStackEntry *)cp.first)->Value()=%f\n", 
            ((CommandResult_CallStackEntry *)cp.first)->Value());

    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > xcp = *vpi;
    int64_t i;
   for (i = 0; i < (*xcp.second).size(); i++ ) {
      CommandResult *xp = (*xcp.second)[i];
      std::cerr << " EXPAND_CALLSTACK, CommandResult, second ";
      if (xp != NULL) {
        xp->Print(std::cerr); std::cerr << "\n";
      } else {
        std::cerr << "NULL\n";
      }
    }
#endif

    // for (int64_t i = cs->size()-1; i > 0; i--) 
    for (int64_t i = 1; i < cs->size(); i++) {
     // Insert intermediate, dummy entry to fill a gap in the trace.
      SmartPtr<std::vector<CommandResult *> > vcs = Dup_CRVector (IV, cp.second);
      std::vector<CommandResult *> *ncs = Dup_Call_Stack (i, cs);
      CommandResult *CSE = new CommandResult_CallStackEntry (ncs, TraceBack_Order);
      result.push_back (std::make_pair(CSE, vcs));
    }
    result.push_back (cp);
  }
  c_items = result;
}

bool Generic_Multi_View (
           CommandObject *cmd,
           ExperimentObject *exp,
           int64_t topn,
           ThreadGroup& tgrp,
           std::vector<Collector>& CV,
           std::vector<std::string>& MV,
           std::vector<ViewInstruction *>& IV,
           std::vector<std::string>& HV,
           View_Form_Category vfc,
           std::vector<std::pair<CommandResult *,
                                 SmartPtr<std::vector<CommandResult *> > > >& c_items,
           std::list<CommandResult *>& view_output) {
  bool success = false;
#if DEBUG_CLI
  std::cerr << "Enter Generic_Multi_View, in SS_View_multi.cxx, begin calling Print_View_Params" << std::endl;
  Print_View_Params (std::cerr, CV,MV,IV);
  std::cerr << "\nEnter Generic_Multi_View, in SS_View_multi.cxx, c_items.size()= " << c_items.size() << "\n";
  std::cerr << "\nDump items.  Number of items is " << c_items.size() << "\n";
 // Dump the input callstack entries.
  std::vector<std::pair<CommandResult *, SmartPtr<std::vector<CommandResult *> > > >::iterator jegvpi;

  for (jegvpi = c_items.begin(); jegvpi != c_items.end(); jegvpi++) {
   // Foreach CallStack entry, copy the desired sort value into the VMulti_sort_temp field.
    std::pair<CommandResult *, SmartPtr<std::vector<CommandResult *> > > jegcp = *jegvpi;
/* TEST
    int64_t jegi;
    for (jegi = 0; jegi < (*jegcp.second).size(); jegi++ ) {
      CommandResult *jegp = (*jegcp.second)[jegi];
      printf("Generic_Multi_View, START printing CommandResult (jegp), indexed by jegi=%d, jegp=%x\n", jegi, jegp);
      std::cerr << "  ";
      if (jegp != NULL) {
        jegp->Print(std::cerr); std::cerr << "\n";
      } else {
        std::cerr << "NULL\n";
      }
      printf("Enter Generic_Multi_View, END printing CommandResult jegp\n");
    }
TEST */

  }
  std::cerr << "Enter Generic_Multi_View, in SS_View_multi.cxx, after calling Print_View_Params" << std::endl;
  fflush(stderr);
#endif

  Assert (vfc != VFC_Unknown);

  if (Look_For_KeyWord(cmd, "ButterFly") &&
      !Filter_Uses_F (cmd)) {
    Mark_Cmd_With_Soft_Error(cmd, "(The required function list ('-f ...') for ButterFly views is missing.)");
    return false;   // There is no collector, return.
  }

  int64_t i;
  std::vector<CommandResult *> Total_Value(Find_Max_Temp(IV)+1); // Values needed for % computations.
  for (i = 0; i < Total_Value.size(); i++) Total_Value[i] = NULL;
  if (topn == 0) topn = LONG_MAX;

  try {
   // Set up quick access to instructions for columns.
    int64_t num_columns = 0;
    std::vector<ViewInstruction *> ViewInst(IV.size());
    for ( i=0; i < IV.size(); i++) {
      ViewInstruction *vinst = Find_Column_Def (IV, i);
      if (vinst == NULL) {
       // Exit if we didn't find a definition
        break;
      }
      num_columns++;
      ViewInst[i] = vinst;
    }   
    if (num_columns == 0) {
      Mark_Cmd_With_Soft_Error(cmd, "(There is no display requested.)");
      return false;   // There is no column[0] defined, return.
    }

   // Set up quick access to instructions for data combining.
    int64_t num_temps_used = std::max ((int64_t)VMulti_time_temp, Find_Max_Temp(IV)) + 1;
    std::vector<ViewInstruction *> AccumulateInst(num_temps_used);
    std::vector<ViewInstruction *> FieldRequirements;
    ViewInstruction *sortInst = NULL;
    for ( i = 0; i < num_temps_used; i++) AccumulateInst[i] = NULL;
    for ( i = 0; i < IV.size(); i++) {
      ViewInstruction *vp = IV[i];
      if ((vp->OpCode() == VIEWINST_Add) ||
          (vp->OpCode() == VIEWINST_Min) ||
          (vp->OpCode() == VIEWINST_Max)) {
        if (vp->TMP1() < num_temps_used) {
          AccumulateInst[vp->TMP1()] = vp;
        }
      } else if ( (vp->OpCode() == VIEWINST_Expression) &&
                  ( (vp->ExprOpCode() == EXPRESSION_OP_ADD) ||
                    (vp->ExprOpCode() == EXPRESSION_OP_MIN) ||
                    (vp->ExprOpCode() == EXPRESSION_OP_MAX) ||
                    (vp->ExprOpCode() == EXPRESSION_OP_PERCENT) ||
                    (vp->ExprOpCode() == EXPRESSION_OP_A_ADD) ||
                    (vp->ExprOpCode() == EXPRESSION_OP_A_MIN) ||
                    (vp->ExprOpCode() == EXPRESSION_OP_A_MAX) )  ) {
        AccumulateInst[vp->TR()] = vp;
      } else if (vp->OpCode() == VIEWINST_Sort_Ascending) {
       sortInst = vp;
      } else if (vp->OpCode() == VIEWINST_Require_Field_Equal) {
       FieldRequirements.push_back(vp);
      }
    }

   // Acquire base set of metric values.
    int64_t Column0index = (ViewInst[0]->OpCode() == VIEWINST_Display_Metric) ? ViewInst[0]->TMP1() : 0;

   // Calculate any temporaries that are needed.
    Calculate_Temporary_Values (cmd, tgrp, CV, MV, IV, c_items, Total_Value);

   // Calculate any Totals that are needed to do percentages.
    bool Gen_Total_Percent = false;
    int64_t max_percent_index = -1;
    for (i = 0; i < IV.size(); i++) {
      ViewInstruction *vp = IV[i];
      if (vp->OpCode() == VIEWINST_Display_Percent_Tmp) {
        max_percent_index = std::max(max_percent_index,vp->TMP2());
      } else if (vp->OpCode() == VIEWINST_Display_Percent_Column) {
        max_percent_index = std::max(max_percent_index,vp->TMP2());
      } else if (vp->OpCode() == VIEWINST_Define_Total_Tmp) {
        max_percent_index = std::max(max_percent_index,vp->TR());
      } else if ( (vp->OpCode() == VIEWINST_Expression) &&
                  (vp->ExprOpCode() == EXPRESSION_OP_PERCENT) ) {
        max_percent_index = std::max(max_percent_index,vp->TMP2());
      }
    }
    if (max_percent_index >= 0) {
      Gen_Total_Percent = true;
      Total_Value.reserve(Find_Max_Temp(IV)+1);
      for (i = 0; i < (max_percent_index+1); i++) Total_Value[i] = NULL;
      if (!Look_For_KeyWord(cmd, "ButterFly")) {
       // Totals must be determined before we get rid of unneeded records
       // when we eliminate all but the "topN" items.
        Calculate_Totals (cmd, tgrp, CV, MV, IV, c_items, Total_Value);
      }
    }

   // Determine call stack ordering
    bool TraceBack_Order = Determine_TraceBack_Ordering (cmd);

   // What granularity has been requested?
    std::string EO_Title;

    if (vfc == VFC_Trace) {
      EO_Title = "Call Stack Function (defining location)";

#if DEBUG_CLI
      printf("CallTree - processin, topn=%d, (int64_t)c_items.size()=%d\n", topn, (int64_t)c_items.size());
#endif
      if ((topn < (int64_t)c_items.size()) &&
          !Look_For_KeyWord(cmd, "ButterFly")) {
       // Determine the topn items based on the time spent in each call.
#if DEBUG_CLI
        printf("in Generic_Multi_View, calling Setup_Sort, VFC_Trace, Sort VMulti_time_temp, by the value displayed in the left most column. \n");
#endif
        Setup_Sort (VMulti_time_temp, c_items, Total_Value);


#if DEBUG_CLI
  std::cerr << "\nDump items.  in VFC_Trace section, Number of items is " << c_items.size() << "\n";
  std::vector<std::pair<CommandResult *,
                        SmartPtr<std::vector<CommandResult *> > > >::iterator vpi;
  for (vpi = c_items.begin(); vpi != c_items.end(); vpi++) {
   // Foreach CallStack entry, copy the desired sort value into the VMulti_sort_temp field.
    std::pair<CommandResult *,
              SmartPtr<std::vector<CommandResult *> > > cp = *vpi;
    int64_t i;
    for (i = 0; i < (*cp.second).size(); i++ ) {
      CommandResult *p = (*cp.second)[i];
      std::cerr << "  ";
      if (p != NULL) {
        p->Print(std::cerr); std::cerr << "\n";
      } else {
        std::cerr << "NULL\n";
      }
    }

  }
  fflush(stderr);
#endif

        std::sort(c_items.begin(), c_items.end(),
                  sort_descending_CommandResult<std::pair<CommandResult *,
                                                          SmartPtr<std::vector<CommandResult *> > > >());
        Reclaim_CR_Space (topn, c_items);
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }

     // Sort by the value displayed in the left most column.

#if DEBUG_CLI
      printf("in Generic_Multi_View, calling Setup_Sort, VFC_Trace, Sort ViewInst[0], by the value displayed in the left most column. \n");
#endif

      Setup_Sort (ViewInst[0], c_items, Total_Value);

      if ((sortInst == NULL) ||
          (sortInst->TMP1() == 0)) {

#if DEBUG_CLI
      printf("in Generic_Multi_View, sort, VFC_Trace, sortInst == NULL, by the value displayed in the left most column. \n");
#endif

        std::sort(c_items.begin(), c_items.end(),
                  sort_descending_CommandResult<std::pair<CommandResult *,
                                                          SmartPtr<std::vector<CommandResult *> > > >());
      } else {

#if DEBUG_CLI
      printf("in Generic_Multi_View, sort, VFC_Trace, NOT sortInst == NULL, by the value displayed in the left most column. \n");
#endif

        std::sort(c_items.begin(), c_items.end(),
                  sort_ascending_CommandResult<std::pair<CommandResult *,
                                                          SmartPtr<std::vector<CommandResult *> > > >());
      }

     // Should we expand the call stack entries in the report?
      if (Look_For_KeyWord(cmd, "CallTree") ||
          Look_For_KeyWord(cmd, "CallTrees") ||
          Look_For_KeyWord(cmd, "TraceBack") ||
          Look_For_KeyWord(cmd, "TraceBacks") ||
          Look_For_KeyWord(cmd, "FullStack") ||
          Look_For_KeyWord(cmd, "FullStacks")) {
        if (!Look_For_KeyWord(cmd, "ButterFly")) {
          if (!Look_For_KeyWord(cmd, "DontExpand")) {
            Expand_CallStack (TraceBack_Order, IV, c_items);
          }

/* We could compress the output, but this routine goes too far
   by combining the traces.
         // Should we eliminate redundant entries in the report?
          if (!Look_For_KeyWord(cmd, "FullStack") &&
              !Look_For_KeyWord(cmd, "FullStacks")) {
            Combine_Duplicate_CallStacks (AccumulateInst, FieldRequirements, c_items);
          }
*/
        }
      }
    } else if (vfc == VFC_CallStack) {
     // Straight Report will break down report by call stack.
      EO_Title = "Call Stack Function (defining location)";
      bool add_stmts = (!Look_For_KeyWord(cmd, "ButterFly") ||
                        Look_For_KeyWord(cmd, "FullStack") ||
                        Look_For_KeyWord(cmd, "FullStacks"));

      Combine_Duplicate_CallStacks (AccumulateInst, FieldRequirements, c_items);

     // Sort by the value displayed in the left most column.
#if DEBUG_CLI
      printf("in Generic_Multi_View, calling Setup_Sort VFC_CallStack, Sort by the value displayed in the left most column. \n");
#endif
      Setup_Sort (ViewInst[0], c_items, Total_Value);
      std::sort(c_items.begin(), c_items.end(),
                sort_descending_CommandResult<std::pair<CommandResult *,
                                                        SmartPtr<std::vector<CommandResult *> > > >());
      if ((topn < (int64_t)c_items.size()) &&
          !Look_For_KeyWord(cmd, "ButterFly")) {
       // Retain the topn items.
        Reclaim_CR_Space (topn, c_items);
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
/* The preferred output seems to be by time spent, not call tree order.
     // Sort report in calling tree order.
      std::sort(c_items.begin(), c_items.end(),
                sort_descending_CallStacks<std::pair<CommandResult *,
                                                     SmartPtr<std::vector<CommandResult *> > > >());
*/

     // Should we expand the call stack entries in the report?
      if (!Look_For_KeyWord(cmd, "ButterFly")) {
        if (!Look_For_KeyWord(cmd, "DontExpand")) {
          Expand_CallStack (TraceBack_Order, IV, c_items);
        }

       // Should we eliminate redundant entries in the report?
        if (!Look_For_KeyWord(cmd, "FullStack") &&
            !Look_For_KeyWord(cmd, "FullStacks")) {
          Combine_Duplicate_CallStacks (AccumulateInst, FieldRequirements, c_items);
        }
      }
    } else {

     if (vfc == VFC_Function) {
       EO_Title = "Function (defining location)";
     } else if (vfc == VFC_Loop) {
       EO_Title = "Loop Start Location (Line Number)";
     } else if (vfc == VFC_Statement) {
       EO_Title = "Statement Location (Line Number)";
     } else if (vfc == VFC_LinkedObject) {
       EO_Title = "LinkedObject";
     } else {
       Mark_Cmd_With_Soft_Error(cmd, "(Unrecognized report type for multi-data view.)");
       return false;
     }

     // Sort by the value displayed in the left most column.
#if DEBUG_CLI
      printf("in Generic_Multi_View, calling Setup_Sort, else clause, Sort by the value displayed in the left most column. \n");
#endif
      Setup_Sort (ViewInst[0], c_items, Total_Value);
      std::sort(c_items.begin(), c_items.end(),
                sort_descending_CommandResult<std::pair<CommandResult *,
                                                        SmartPtr<std::vector<CommandResult *> > > >());
      if (topn < (int64_t)c_items.size()) {
        Reclaim_CR_Space (topn, c_items);
        c_items.erase ( (c_items.begin() + topn), c_items.end());
      }
    }

/* Don't issue this message - just go ahead an print the headers and an empty report.
   Consider turning this message into a Annotation.
    if (c_items.empty()) {
      std::string s("(There are no data samples for " + MV[Column0index] + " available.)");
      Mark_Cmd_With_Soft_Error(cmd,s);
      return false;   // There is no data, return.
    }
*/
#if DEBUG_CLI
    if (c_items.empty()) {
      printf("in Generic_Multi_View, there are no data samples available.)\n");
    }
#endif

   // jeg 12-28-06 added this to prevent the headers from being put out
   // when there is no data.  A "no data samples message goes out instead
   if (!c_items.empty()) {
     // Add Header for each column in the table.
      CommandResult_Headers *H = new CommandResult_Headers ();
      for ( i=0; i < num_columns; i++) {
        ViewInstruction *vinst = ViewInst[i];
        std::string column_header;
        if ((HV.size() > i) &&
            (HV[i].length() > 0)) {
          column_header = HV[i];
        } else {
          column_header = "?";
        }
        H->CommandResult_Headers::Add_Header ( CRPTR ( column_header ) );
      }
     // Add Entry Object name
      H->CommandResult_Headers::Add_Header ( CRPTR ( EO_Title ) );
      view_output.push_back(H);
   } // if there are items to output

   // Now format the view.
    if (Look_For_KeyWord(cmd, "ButterFly")) {
     // Foreach function name, build a ButterFly view.
     // Note: we have already verified that there is a '-f' list.

      OpenSpeedShop::cli::ParseResult *p_result = cmd->P_Result();
      std::vector<OpenSpeedShop::cli::ParseTarget> *p_tlist = p_result->getTargetList();

      Assert (p_tlist->begin() != p_tlist->end());
      OpenSpeedShop::cli::ParseTarget pt = *p_tlist->begin(); // There can only be one!

      std::vector<OpenSpeedShop::cli::ParseRange> *f_list = pt.getFileList();
      Assert ((f_list != NULL) && !(f_list->empty()));

      bool MoreThanOne = false;
      std::vector<OpenSpeedShop::cli::ParseRange>::iterator pr_iter;

      for (pr_iter=f_list->begin(); pr_iter != f_list->end(); pr_iter++) {
        OpenSpeedShop::cli::parse_range_t R = *pr_iter->getRange();
        OpenSpeedShop::cli::parse_val_t pval1 = R.start_range;
        Assert (pval1.tag == OpenSpeedShop::cli::VAL_STRING);
        std::set<Function> FS = exp->FW()->getFunctionsByNamePattern (pval1.name);

        std::set<Function>::iterator fsi;
        for (fsi = FS.begin(); fsi != FS.end(); fsi++) {
          std::vector<std::pair<CommandResult *,
                                SmartPtr<std::vector<CommandResult *> > > > result;
          Function func = *fsi;
          Extract_Pivot_Items (cmd, exp, AccumulateInst, FieldRequirements, TraceBack_Order, c_items, func, result);

          if (!result.empty()) {
            std::list<CommandResult *> view_unit;
           // Calculate any temporaries that are needed.
            if (Gen_Total_Percent) {
             // Needed to calculate percents for ButterFly views.
              Calculate_Totals (cmd, tgrp, CV, MV, IV, result, Total_Value);
            }
            Construct_View_Output (cmd, exp, tgrp, CV, MV, IV, Total_Value, result, view_unit);
            if (!view_unit.empty()) {
              if (MoreThanOne) {
               // Delimiter between items is a null string.
                view_output.push_back (new CommandResult_RawString(""));
               }
              view_output.splice (view_output.end(), view_unit);
              MoreThanOne = true;
            }
            Reclaim_CR_Space (result);

           // Reclaim Total_Value results.
            for ( i = 0; i < Total_Value.size(); i++) {
              delete Total_Value[i];
              Total_Value[i] = NULL;
            }
          }
        }
      }

    } else {
#if DEBUG_CLI
     printf("in Generic_Multi_View, calling Construct_View_Output, c_items.size()=%d\n", c_items.size());
#endif
      Construct_View_Output (cmd, exp, tgrp, CV, MV, IV, Total_Value, c_items, view_output);
    }

    success = true;
  }
  catch (std::bad_alloc) {
    Mark_Cmd_With_Soft_Error (cmd, "ERROR: unable to allocate enough memory to generate the View.");
  }
  catch(const Exception& error) {
    Mark_Cmd_With_Std_Error (cmd, error);
  }

 // Release space for no longer needed items.
  Reclaim_CR_Space (c_items);

 // Reclaim Total_Value results.
  for ( i = 0; i < Total_Value.size(); i++) {
    if (Total_Value[i] != NULL) {
      delete Total_Value[i];
    }
  }

#if DEBUG_CLI
  std::cerr << "\nExit Generic_Multi_View, success= " << success << "\n";
#endif

  return success;
}
