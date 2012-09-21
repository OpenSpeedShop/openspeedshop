/** @file
 *
 * Parser CLI target description and definition of the ParseTarget class.
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
class ParseTarget {

    public:

	/** Constructor. */
	ParseTarget();

//	/** Destructor. */
//	~ParseTarget();

    	/** Does the object have any data. */
	bool used() { return dm_used;}

    	/** Handle list of host names. */
    	std::vector<ParseRange> * getHostList()
	{
	    return &dm_host_list;
	}

    	void pushHostPoint(const char * name);
    	void pushHostPoint(int64_t num);
    	void pushHostRange(const char * name, int64_t num);
    	void pushHostRange(const char * name1, const char * name2);
    	void pushHostRange(int64_t num, const char * name);
    	void pushHostRange(int64_t num1, int64_t num2);

    	/** Handle list of file names. */
    	std::vector<ParseRange> * getFileList()
	{
	    return &dm_file_list;
	}

    	void pushFilePoint(char * name)  {	    
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name);
    	    dm_file_list.push_back(range);
	}
    	void pushFilePoint(int num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num);
    	    dm_file_list.push_back(range);
	}
    	void pushFileRange(const char * name, int64_t num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name,num);
    	    dm_file_list.push_back(range);
	}
    	void pushFileRange(const char * name1, char * name2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name1,name2);
    	    dm_file_list.push_back(range);
	}
    	void pushFileRange(int64_t num, char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num,name);
    	    dm_file_list.push_back(range);
	}
    	void pushFileRange(int64_t num1, int64_t num2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num1,num2);
    	    dm_file_list.push_back(range);
	}

    	/** Handle list of cluster names. */
    	std::vector<ParseRange> * getClusterList()
	{
	    return &dm_cluster_list;
	}
    	void pushClusterPoint(const char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name);
    	    dm_cluster_list.push_back(range);
	}
    	void pushClusterPoint(int64_t num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num);
    	    dm_cluster_list.push_back(range);
	}
    	void pushClusterRange(const char * name, int64_t num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name,num);
    	    dm_cluster_list.push_back(range);
	}
    	void pushClusterRange(const char * name1, const char * name2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name1,name2);
    	    dm_cluster_list.push_back(range);
	}
    	void pushClusterRange(int64_t num, const char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num,name);
    	    dm_cluster_list.push_back(range);
	}
    	void pushClusterRange(int64_t num1, int64_t num2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num1,num2);
    	    dm_cluster_list.push_back(range);
	}

    	/** Handle list of rank range values. */
    	std::vector<ParseRange> * getRankList()
	{
	    return &dm_rank_list;
	}
    	void pushRankPoint(const char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name);
    	    dm_rank_list.push_back(range);
	}
    	void pushRankPoint(int64_t num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num);
    	    dm_rank_list.push_back(range);
	}
    	void pushRankRange(const char * name, int64_t num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name,num);
    	    dm_rank_list.push_back(range);
	}
    	void pushRankRange(const char * name1, const char * name2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name1,name2);
    	    dm_rank_list.push_back(range);
	}
    	void pushRankRange(int64_t num, const char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num,name);
    	    dm_rank_list.push_back(range);
	}
    	void pushRankRange(int64_t num1, int64_t num2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num1,num2);
    	    dm_rank_list.push_back(range);
	}

    	/** Handle list of pid range values. */
    	std::vector<ParseRange> * getPidList()
	{
	    return &dm_pid_list;
	}
    	void pushPidPoint(const char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name);
    	    dm_pid_list.push_back(range);
	}
    	void pushPidPoint(int64_t num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num);
    	    dm_pid_list.push_back(range);
	}
    	void pushPidRange(const char * name, int64_t num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name,num);
    	    dm_pid_list.push_back(range);
	}
    	void pushPidRange(const char * name1, const char * name2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name1,name2);
    	    dm_pid_list.push_back(range);
	}
    	void pushPidRange(int64_t num, const char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num,name);
    	    dm_pid_list.push_back(range);
	}
    	void pushPidRange(int64_t num1, int64_t num2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num1,num2);
    	    dm_pid_list.push_back(range);
	}

    	/** Handle list of thread range values. */
    	std::vector<ParseRange> * getThreadList()
	{
	    return &dm_thread_list;
	}
    	void pushThreadPoint(char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name);
    	    dm_thread_list.push_back(range);
	}
    	void pushThreadPoint(int64_t num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num);
    	    dm_thread_list.push_back(range);
	}
    	void pushThreadRange(char * name, int64_t num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name,num);
    	    dm_thread_list.push_back(range);
	}
    	void pushThreadRange(char * name1, char * name2) {
	    ParseRange range(name1,name2);
    	    dm_thread_list.push_back(range);
	}
    	void pushThreadRange(int64_t num, char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num,name);
    	    dm_thread_list.push_back(range);
	}
    	void pushThreadRange(int64_t num1, int64_t num2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num1,num2);
    	    dm_thread_list.push_back(range);
	}

    private:
    	/** Have any of the fields been used */
    	bool dm_used;

    	/** Container of process Ids as integers */
    	std::vector<ParseRange> dm_pid_list;
    	/** Container of rank Ids as integers */
    	std::vector<ParseRange> dm_rank_list;
    	/** Container of thread Ids as integers */
    	std::vector<ParseRange> dm_thread_list;

    	/** Container of files as strings */
    	std::vector<ParseRange> dm_file_list;
    	/** Container of clusters as strings */
    	std::vector<ParseRange> dm_cluster_list;

    	/** Container of host info as class ParseRange */
    	std::vector<ParseRange> dm_host_list;	

//    protected:
//    	/** Things I don't want to happen so don't define!*/
//	ParseTarget& operator=(const ParseTarget& rhs);
	
};

} }

#endif // __SS_Parse_Target_HXX__
