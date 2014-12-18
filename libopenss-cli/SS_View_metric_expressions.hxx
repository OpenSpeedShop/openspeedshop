// This is the common code for views that traps and processes metric expressions.
// The intent is that this this be pulled into each 'view.cxx' routine with a '#include'.
// For this coding approach to work, many variables must be named the same.  If adding
// this capability for a new view, it is helpful to investigate how the process is done
// within an existing vieew.

      if ((*mi).getParseType() == PARSE_EXPRESSION_VALUE) {
       // Convert Metric Expression to instructions.
        std::string header = "user expression";
        ParseRange *pr = &(*mi);
        if (pr->getOperation() == EXPRESSION_OP_HEADER) {
         // Replace the default header with the first argument and
         // replace the original expression with the second argument.
          header = pr->getExpression()->exp_operands[0]->getRange()->start_range.name;
          pr = pr->getExpression()->exp_operands[1];
        }
       // Generate the instructions for the expression.
        int64_t new_result = evaluate_parse_expression (cmd, exp, CV, MV, IV, HV, vfc,
                                      pr, last_used_temp, std::string("hwcsamp"), MetricMap);
        if (new_result < 0) {
          char s[100+OPENSS_VIEW_FIELD_SIZE];
          sprintf ( s, "Column %lld not generated because of an error in the metric expression.",
                    static_cast<long long int>(last_column) );
          Mark_Cmd_With_Soft_Error(cmd,s);
          continue; // Allow other columns to be generated - 'return false;' would abort view.
        }
        IV.push_back(new ViewInstruction (VIEWINST_Display_Tmp, last_column++, new_result));
        HV.push_back(header);
        continue;
      } // Convert Metric Expression to instructions.
