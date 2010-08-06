// Recognize and generate pseudo instructions to calculate and display By Thread metrics for
// ThreadMax, ThreadMaxIndex, ThreadMin, ThreadMinIndex, ThreadAverage, and loadbalance.
//
// Must have a declaration for 'std::string ByThread_Header;'
// and '#define's for 'ByThread_Rank' and 'ByThread_use_intervals'.
// Such as the following:
//
// std::string ByThread_Header = Find_Metadata ( CV[0], MV[1] ).getShortName();
// #define ByThread_Rank true       // headers will say "Rank", otherwise "ThreadID"
// #define ByThread_use_intervals 1 // "1" => times reported in milliseconds,
//                                  // "2" => times reported in seconds,
//                                  //  otherwise don't add anything.
        else if (!strcasecmp(M_Name.c_str(), "ThreadMean") ||
                 !strcasecmp(M_Name.c_str(), "ThreadAverage")) {
         // Do a By-Thread average.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_mean));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmean_temp));
            HV.push_back(std::string("Average ") + ByThread_Header
               + " Across " + ((ByThread_Rank == 1)?"Ranks":"ThreadIDs")
               + ((ByThread_use_intervals == 1)?"(ms)":((ByThread_use_intervals == 2)?"(s)":"")));
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMin")) {
         // Find the By-Thread Min.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_min));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmin_temp));
            HV.push_back(std::string("Min ") + ByThread_Header
               + " Across " + ((ByThread_Rank == 1)?"Ranks":"ThreadIDs")
               + ((ByThread_use_intervals == 1)?"(ms)":((ByThread_use_intervals == 2)?"(s)":"")));
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMinIndex")) {
         // Find the Rank of the By-Thread Min.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_imin));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timin_temp));
            HV.push_back(std::string((ByThread_Rank == 1)?"Rank":"ThreadID") + " of Min");
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMax")) {
         // Find the By-Thread Max.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_max));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmax_temp));
            HV.push_back(std::string("Max ") + ByThread_Header
               + " Across " + ((ByThread_Rank == 1)?"Ranks":"ThreadIDs")
               + ((ByThread_use_intervals == 1)?"(ms)":((ByThread_use_intervals == 2)?"(s)":"")));
          }
        } else if (!strcasecmp(M_Name.c_str(), "ThreadMaxIndex")) {
         // Find the Rank of the By-Thread Max.
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_imax));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timax_temp));
            HV.push_back(std::string((ByThread_Rank == 1)?"Rank":"ThreadID") + " of Max");
          }
        } else if (!strcasecmp(M_Name.c_str(), "loadbalance")) {
          if ((vfc == VFC_CallStack) && (!Generate_ButterFly)) {
            Mark_Cmd_With_Soft_Error(cmd,"Warning: Unsupported combination, '-m " + M_Name + "' with call traces.");
          } else {
         // Find the By-Thread Max.
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_max));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmax_temp));
            HV.push_back(std::string("Max ") + ByThread_Header
               + " Across " + ((ByThread_Rank == 1)?"Ranks":"Threads")
               + ((ByThread_use_intervals == 1)?"(ms)":((ByThread_use_intervals == 2)?"(s)":"")));

         // Report ThreadId of Max.
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_imax));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timax_temp));
            HV.push_back(std::string((ByThread_Rank == 1)?"Rank":"ThreadID") + " of Max");

         // Find the By-Thread Min.
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_min));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmin_temp));
            HV.push_back(std::string("Min ") + ByThread_Header
               + " Across " + ((ByThread_Rank == 1)?"Ranks":"Threads")
               + ((ByThread_use_intervals == 1)?"(ms)":((ByThread_use_intervals == 2)?"(s)":"")));

         // Report ThreadId of Min.
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_imin));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, timin_temp));
            HV.push_back(std::string((ByThread_Rank == 1)?"Rank":"ThreadID") + " of Min");

         // Do a By-Thread average.
            IV.push_back(new ViewInstruction (VIEWINST_Define_ByThread_Metric, -1, 1, ViewReduction_mean));
            IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, tmean_temp));
            HV.push_back(std::string("Average ") + ByThread_Header
               + " Across " + ((ByThread_Rank == 1)?"Ranks":"Threads")
               + ((ByThread_use_intervals == 1)?"(ms)":((ByThread_use_intervals == 2)?"(s)":"")));
          }
        }
