#ifndef COLLECTORINFO_H
#define COLLECTORINFO_H

#include "MetricInfo.hxx"

#include <qvaluelist.h>  // For Qt iterators.... but make this non-qt iterator.

//! Experiment Information Class
/*! Experiment Information Class is the class contain information about
    performance experiments.   Time, Data, Expreriment Type, and any sampling
    data, or pointers to sampling data.
 */

enum MetricHeaderType { INT_T, FLOAT_T, CHAR_T };

class MetricHeaderInfo
{
  public:

    MetricHeaderInfo() {};
    MetricHeaderInfo(QString l, MetricHeaderType t)
    {
      label = l;
      type = t;
    };

    QString label;
    MetricHeaderType type;
};


typedef QValueList<MetricInfo *> MetricInfoList;  // This should be some structure or class type...
class MetricHeaderInfo;
typedef QValueList<MetricHeaderInfo *> MetricHeaderInfoList;

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

    MetricHeaderInfoList metricHeaderInfoList;
    int metricHeaderCount() { return metricHeaderInfoList.count(); };

    void init();
    void fudgeValues();

    char *targetProgram;
    char *experimentName;
    char *marchingOrders;
    char *experimentNotes;
    MetricInfoList metricInfoList;
};
#endif // COLLECTORINFO_H
