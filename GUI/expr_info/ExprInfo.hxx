#ifndef EXPRINFO_H
#define EXPRINFO_H

#include "FuncInfo.hxx"

//! Experiment Information Class
/*! Experiment Information Class is the class contain information about
    performance experiments.   Time, Data, Expreriment Type, and any sampling
    data, or pointers to sampling data.
 */

#include <qvaluelist.h>  // For Qt iterators.... but make this non-qt iterator.
typedef QValueList<FuncInfo *> FuncInfoList;  // This should be some structure or class type...

class ExprInfo
{
public:
    /*! ExperInfo() - A default constructor the the Panel Class.
     */
    ExprInfo();

    ExprInfo(char *experiment_name);

    //! ~ExprInfo() - The default destructor.
    /*! Destroys the object and frees any allocated resources.
     */
    ~ExprInfo();

    void init();
    void fudgeValues();

    char *targetProgram;
    char *experimentName;
    char *marchingOrders;
    char *experimentNotes;
    FuncInfoList funcInfoList;
};
#endif // EXPRINFO_H
