#ifndef COLLECTORINFO_H
#define COLLECTORINFO_H

#include "MetricInfo.hxx"

//! Experiment Information Class
/*! Experiment Information Class is the class contain information about
    performance experiments.   Time, Data, Expreriment Type, and any sampling
    data, or pointers to sampling data.
 */

#include <qvaluelist.h>  // For Qt iterators.... but make this non-qt iterator.
typedef QValueList<MetricInfo *> MetricInfoList;  // This should be some structure or class type...

class CollectorInfo
{
public:
    /*! ExperInfo() - A default constructor the the Panel Class.
     */
    CollectorInfo();

    CollectorInfo(char *experiment_name);

    //! ~CollectorInfo() - The default destructor.
    /*! Destroys the object and frees any allocated resources.
     */
    ~CollectorInfo();

    void init();
    void fudgeValues();

    char *targetProgram;
    char *experimentName;
    char *marchingOrders;
    char *experimentNotes;
    MetricInfoList metricInfoList;
};
#endif // COLLECTORINFO_H
