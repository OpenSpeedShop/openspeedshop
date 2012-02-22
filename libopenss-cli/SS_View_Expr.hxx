
/**
 * function: evaluate_parse_expression
 * 
 * Convert a parse expression into view instructions that can
 * be used to generate the value of a Metric Expression that
 * is needed for the user's requested report.
 *
 */
int64_t evaluate_parse_expression (
            CommandObject *cmd,
            ExperimentObject *exp,
            std::vector<Collector>& CV,
            std::vector<std::string>& MV,
            std::vector<ViewInstruction *>& IV,
            std::vector<std::string>& HV,
            View_Form_Category vfc,
            ParseRange *pr,
            int64_t &last_used_temp,
            std::string view_for_collector,
            std::map<std::string, int64_t> &MetricMap);
