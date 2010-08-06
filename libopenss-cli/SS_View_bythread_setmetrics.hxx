// Expressions to capture by-thread values in proper type.
#define set_ExtraMetric_values(value_array, ExtraValues, index)                                      \
              if (num_temps > tmean_temp) {                                                          \
                if (ExtraValues[ViewReduction_mean]->find(index)                                     \
                                                      != ExtraValues[ViewReduction_mean]->end()) {   \
if (ByThread_use_intervals == 1) { \
                  CommandResult *P = ExtraValues[ViewReduction_mean]->find(index)->second;           \
                  value_array[tmean_temp] = new CommandResult_Interval ( (CommandResult_Float *)P ); \
} else { \
                  value_array[tmean_temp]                                                            \
                                   = ExtraValues[ViewReduction_mean]->find(index)->second->Copy();   \
} \
                } else {                                                                             \
                  value_array[tmean_temp] = CRPTR ((double)0.0);                                     \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > tmin_temp) {                                                           \
                if (ExtraValues[ViewReduction_min]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_min]->end()) {    \
if (ByThread_use_intervals == 1) { \
                  CommandResult *P = ExtraValues[ViewReduction_min]->find(index)->second;            \
                  value_array[tmin_temp] = new CommandResult_Interval ( (CommandResult_Float *)P );  \
} else { \
                  value_array[tmin_temp]                                                             \
                                   = ExtraValues[ViewReduction_min]->find(index)->second->Copy();    \
} \
                } else {                                                                             \
                  value_array[tmin_temp] = CRPTR ((double)0.0);                                      \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > timin_temp) {                                                          \
                if (ExtraValues[ViewReduction_imin]->find(index)                                     \
                                                      != ExtraValues[ViewReduction_imin]->end()) {   \
                  value_array[timin_temp]                                                            \
                                   = ExtraValues[ViewReduction_imin]->find(index)->second->Copy();   \
                } else {                                                                             \
                  value_array[timin_temp] = CRPTR ((int64_t)0);                                      \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > tmax_temp) {                                                           \
                if (ExtraValues[ViewReduction_max]->find(index)                                      \
                                                      != ExtraValues[ViewReduction_max]->end()) {    \
if (ByThread_use_intervals == 1) { \
                  CommandResult *P = ExtraValues[ViewReduction_max]->find(index)->second;            \
                  value_array[tmax_temp] = new CommandResult_Interval ( (CommandResult_Float *)P );  \
} else { \
                  value_array[tmax_temp]                                                             \
                                   = ExtraValues[ViewReduction_max]->find(index)->second->Copy();    \
} \
                } else {                                                                             \
                  value_array[tmax_temp] = CRPTR ((double)0.0);                                      \
                }                                                                                    \
              }                                                                                      \
              if (num_temps > timax_temp) {                                                          \
                if (ExtraValues[ViewReduction_imax]->find(index)                                     \
                                                      != ExtraValues[ViewReduction_imax]->end()) {   \
                  value_array[timax_temp]                                                            \
                                   = ExtraValues[ViewReduction_imax]->find(index)->second->Copy();   \
                } else {                                                                             \
                  value_array[timax_temp] = CRPTR ((int64_t)0);                                      \
                }                                                                                    \
              }
