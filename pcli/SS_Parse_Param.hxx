/** @file
 *
 * Parser param description.
 *
 */

#ifndef __OpenSpeedShop_Parse_Param_HXX__
#define __OpenSpeedShop_Parse_Param_HXX__

namespace OpenSpeedShop { namespace cli {

/**
 * Parser result class.
 *
 *  Describes the results from parsing one OSS command.
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class ParseParam {

    public:

//	/** Constructor. */
	ParseParam(char * exp_type, char * parm_type, int val);
	ParseParam(char * exp_type, char * parm_type, char * name);

//	/** Destructor. */
//	~ParseRange();

    	char *getParmExpType() {return dm_exptype;}
    	char *getParmParamType() {return dm_param_type;}
	bool isValString() {return dm_val_is_string;}
	char *getStingVal() {return dm_param_val_string;}
	int getnumVal() {return dm_param_val_num







;}

    private:
    	/** range struct to fill */
    	bool dm_has_exptype;
    	char * dm_exptype;
    	char * dm_param_type;

    	bool dm_val_is_string;
    	int dm_param_val_num;
    	char * dm_param_val_string;

};

} }

#endif // __OpenSpeedShop_Parse_Param_HXX__
