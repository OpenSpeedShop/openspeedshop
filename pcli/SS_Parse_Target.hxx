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
class ParseTarget {

    public:

	/** Constructor. */
	ParseTarget()
	{
	    dm_used = false;
	}

//	/** Destructor. */
//	~ParseTarget();

    	/** Does the object have any data. */
	bool used() { return dm_used;}

    	/** Handle list of host names. */
    	vector<ParseRange> * getHostList()
	{
	    return &dm_host_list;
	}

    	void pushHostPoint(char * name);
    	void pushHostPoint(int num);
    	void pushHostRange(char * name, int num);
    	void pushHostRange(char * name1, char * name2);
    	void pushHostRange(int num, char * name);
    	void pushHostRange(int num1, int num2);

    	/** Handle list of file names. */
    	vector<ParseRange> * getFileList()
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
    	void pushFileRange(char * name, int num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name,num);
    	    dm_file_list.push_back(range);
	}
    	void pushFileRange(char * name1, char * name2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name1,name2);
    	    dm_file_list.push_back(range);
	}
    	void pushFileRange(int num, char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num,name);
    	    dm_file_list.push_back(range);
	}
    	void pushFileRange(int num1, int num2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num1,num2);
    	    dm_file_list.push_back(range);
	}

    	/** Handle list of cluster names. */
    	vector<ParseRange> * getClusterList()
	{
	    return &dm_cluster_list;
	}
    	void pushClusterPoint(char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name);
    	    dm_cluster_list.push_back(range);
	}
    	void pushClusterPoint(int num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num);
    	    dm_cluster_list.push_back(range);
	}
    	void pushClusterRange(char * name, int num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name,num);
    	    dm_cluster_list.push_back(range);
	}
    	void pushClusterRange(char * name1, char * name2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name1,name2);
    	    dm_cluster_list.push_back(range);
	}
    	void pushClusterRange(int num, char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num,name);
    	    dm_cluster_list.push_back(range);
	}
    	void pushClusterRange(int num1, int num2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num1,num2);
    	    dm_cluster_list.push_back(range);
	}

    	/** Handle list of rank range values. */
    	vector<ParseRange> * getRankList()
	{
	    return &dm_rank_list;
	}
    	void pushRankPoint(char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name);
    	    dm_rank_list.push_back(range);
	}
    	void pushRankPoint(int num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num);
    	    dm_rank_list.push_back(range);
	}
    	void pushRankRange(char * name, int num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name,num);
    	    dm_rank_list.push_back(range);
	}
    	void pushRankRange(char * name1, char * name2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name1,name2);
    	    dm_rank_list.push_back(range);
	}
    	void pushRankRange(int num, char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num,name);
    	    dm_rank_list.push_back(range);
	}
    	void pushRankRange(int num1, int num2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num1,num2);
    	    dm_rank_list.push_back(range);
	}

    	/** Handle list of pid range values. */
    	vector<ParseRange> * getPidList()
	{
	    return &dm_pid_list;
	}
    	void pushPidPoint(char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name);
    	    dm_pid_list.push_back(range);
	}
    	void pushPidPoint(int num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num);
    	    dm_pid_list.push_back(range);
	}
    	void pushPidRange(char * name, int num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name,num);
    	    dm_pid_list.push_back(range);
	}
    	void pushPidRange(char * name1, char * name2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name1,name2);
    	    dm_pid_list.push_back(range);
	}
    	void pushPidRange(int num, char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num,name);
    	    dm_pid_list.push_back(range);
	}
    	void pushPidRange(int num1, int num2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num1,num2);
    	    dm_pid_list.push_back(range);
	}

    	/** Handle list of thread range values. */
    	vector<ParseRange> * getThreadList()
	{
	    return &dm_thread_list;
	}
    	void pushThreadPoint(char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name);
    	    dm_thread_list.push_back(range);
	}
    	void pushThreadPoint(int num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num);
    	    dm_thread_list.push_back(range);
	}
    	void pushThreadRange(char * name, int num) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(name,num);
    	    dm_thread_list.push_back(range);
	}
    	void pushThreadRange(char * name1, char * name2) {
	    ParseRange range(name1,name2);
    	    dm_thread_list.push_back(range);
	}
    	void pushThreadRange(int num, char * name) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num,name);
    	    dm_thread_list.push_back(range);
	}
    	void pushThreadRange(int num1, int num2) {
	    this->dm_used = true;   // All insertions need to do this currently
	    ParseRange range(num1,num2);
    	    dm_thread_list.push_back(range);
	}

    private:
    	/** Have any of the fields been used */
    	bool dm_used;

    	/** Container of process Ids as integers */
    	vector<ParseRange> dm_pid_list;
    	/** Container of rank Ids as integers */
    	vector<ParseRange> dm_rank_list;
    	/** Container of thread Ids as integers */
    	vector<ParseRange> dm_thread_list;

    	/** Container of files as strings */
    	vector<ParseRange> dm_file_list;
    	/** Container of clusters as strings */
    	vector<ParseRange> dm_cluster_list;

    	/** Container of host info as class ParseRange */
    	vector<ParseRange> dm_host_list;	

//    protected:
//    	/** Things I don't want to happen so don't define!*/
//	ParseTarget& operator=(const ParseTarget& rhs);
	
};

} }

#endif // __SS_Parse_Target_HXX__
