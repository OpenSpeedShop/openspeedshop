/** @file
 *
 * Parser CLI target description.
 *
 */

#ifndef __SS_Parse_Target_HXX__
#define __SS_Parse_Target_HXX__

namespace OpenSpeedShop { namespace cli {

/**
 * Parser result class.
 *
 *  Describes the results from parsing the target
 *  portion of an OSS command.
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class Target {

    public:

//	/** Constructor. */
//	Target();
//
//	/** Destructor. */
//	~Target();

//    	/** Return list of xxx. */
//    	const vector<xxx> * getxxxList() const
//	{
//	    return xxx;
//	}
//


    private:
    	/** What command are we representing */
    	oss_cmd_enum dm_comand_type;

    	/** Container of process Ids as integers */
    	vector<parse_range_t> dm_pid_list;
    	/** Container of rank Ids as integers */
    	vector<parse_range_t> dm_rank_list;
    	/** Container of thread Ids as integers */
    	vector<parse_range_t> dm_thread_list;

    	/** Container of files as strings */
    	vector<string> dm_file_list;
    	/** Container of clusters as strings */
    	vector<string> dm_cluster_list;

//    	/** Container of parameter info as class param_tuple */
//    	vector<host_t> dm_host_list;	
};

} }

#endif // __SS_Parse_Target_HXX__
