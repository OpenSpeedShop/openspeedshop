/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Message related objects and routines.
 *
 */

#ifndef __OpenSpeedShop_Message_Element_HXX__
#define __OpenSpeedShop_Message_Element_HXX__

namespace OpenSpeedShop {

/** Initial package for feeding SS_Message_Element */
typedef struct {
    int    errnum;  	/** Does this have a numeric id? */
    char  *keyword; 	/** Name to look up */
    char **related;	/** Related keywords */
    int    related_cnt;	/** Number of strings in related */
    char  *topic;     	/** General topic */
    char  *brief;     	/** Brief description (one line) */
    char **normal;	/** Not so brief description */
    int    normal_cnt;	/** Number of strings in normal */
    char **detailed;	/** Most verbose description */
    int    detail_cnt;	/** Number of strings in detailed */
    char **example;	/** Example of use */
    int    example_cnt;	/** Number of strings in example */
} message_element_t;

/**
 * OSS message element class.
 *
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class SS_Message_Element {

    public:

	/** Constructor. */
	SS_Message_Element();

	/** Destructor. */
	~SS_Message_Element();

    	/** Set name/keyword that message is associated with */
	void set_keyword(const char * const);
	void set_keyword(const string);
	void set_keyword(const string,const string);
	void set_keyword(const string,const string,const int);
	string * get_keyword();

    	/** Set general topic that message is associated with */
	void set_topic(const char * const );
	void set_topic(const string);
	string * get_topic();

    	/** Add related keyword */
	void add_related(const char * const );
	void add_related(const string);
	vector<string> * get_related_list();

    	/** Set any id number for this message */
	void set_id(const int);
	int get_id();

    	/** Set the brief string for this message */
	void set_brief(const char * const );
	void set_brief(const string);
	string * get_brief();

    	/** Add line of normal message */
	void add_normal(const char * const );
	void add_normal(const string);
	vector<string> * get_normal_list();

    	/** Add line of verbose message */
	void add_verbose(const char * const );
	void add_verbose(const string);
	vector<string> * get_verbose_list();

    	/** Add line of example */
	void add_example(const char * const );
	void add_example(const string);
	vector<string> * get_example_list();
	
	/** Use message_element_t to initialize element */
	void set_element(const message_element_t * const p_element);

    private:

	/** Keyword string. */
    	string dm_keyword;

	/** Topic string. */
    	string dm_topic;
	
	/** Related keyword strings. */
    	vector <string> dm_related_keyword_list;
	
	/** Possible id number. */
    	int dm_id_num;
	
	/** One line brief message. */
    	string dm_brief_message;
	
	/** Normal multiline message. */
    	vector <string> dm_normal_message_list;
	
	/** Verbose multiline message. */
    	vector <string> dm_verbose_message_list;
	
	/** Multiline example. */
    	vector <string> dm_example_list;
	
};
}  	// namespace OpenSpeedShop
#endif // __OpenSpeedShop_Message_Element_HXX__
