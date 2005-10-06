/** @file
 *
 * Load messages pertaining to this module by hand.
 *
 * This should result in the save output as example1.
 * The only difference is in how we load the message
 * database.
 *
 */

#include <vector>
#include <string>
#include <iostream>

using namespace std;

#include "SS_Message_Element.hxx"
#include "SS_Message_Czar.hxx"

using namespace OpenSpeedShop;

#include "data1.dxx"

extern SS_Message_Czar& theMessageCzar();

/**
 * Function:  load_messages()
 * 
 * Load messages for the module into the
 * message czar.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
load_messages() 
{

    // Get reference of the message czar.
    SS_Message_Czar& czar = theMessageCzar();
    
    
    // Load messages.
    // Table ends when topic is NULL.
    int ndx = 0;
    while (example1_msg[ndx].topic != NULL) {
    	SS_Message_Element element;
    	message_element_t *p_element = &example1_msg[ndx++];
	
    	// Numeric id
    	if (p_element->errnum)
    	    element.set_id(p_element->errnum);
    
    	// Look up name
    	if (p_element->keyword)
    	    element.set_keyword(p_element->keyword);
    
    	// Related keywords
    	for (int i=0;i<p_element->related_cnt;++i) {
    	    element.add_related(p_element->related[i]);
    	}
    
    	// General topic
    	if (p_element->topic)
    	    element.set_topic(p_element->topic);

    	// Brief, one line description
    	if (p_element->brief)
    	    element.set_brief(p_element->brief);

    	// More than one line description
    	for (int i=0;i<p_element->normal_cnt;++i) {
    	    element.add_normal(p_element->normal[i]);
    	}

    	// A wordy explaination
    	for (int i=0;i<p_element->detail_cnt;++i) {
    	    element.add_verbose(p_element->detailed[i]);
    	}

    	// An example of usage
    	for (int i=0;i<p_element->example_cnt;++i) {
    	    element.add_example(p_element->example[i]);
    	}



	czar.Add_Help(element);
    }
    
}


/**
 * Function:  dump_messages()
 * 
 * Dump messages for the module to stdout.
 * The message utility does not dictate what you do
 * with the message data..
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
void
dump_messages() 
{
    // Get reference of the message czar.
    SS_Message_Czar& czar = theMessageCzar();
    
    // We're just going to use the same array
    // that we used to load the database.

    int i = 0;
    while (example1_msg[i].topic != NULL) {
	string name(example1_msg[i].keyword);
    	vector <SS_Message_Element *> element;
	    
    	czar.Find_By_Keyword(name.c_str(), &element);
    
    	if (element.begin() == element.end()) {
	    cout << "No help for " + name << endl;
    	}

    	cout << "****************" << endl;
	cout << name << endl;
    	cout << "****************" << endl;
    	vector <SS_Message_Element*>:: iterator k;
    	for (k = element.begin();
    	    k != element.end();
	    ++k) {
	    SS_Message_Element *p_el = *k;
	    vector<string> * const p_string = p_el->get_normal_list();
    	    for (vector <string> :: iterator i=p_string->begin();
    	     	 i!= p_string->end();
	     	 ++i) {
		cout << *i << endl;
	    }
    	}
	++i;
    }
}

/**
 * Function:  main()
 * 
 * Driver for example1 test.
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
int 
main()
{

    load_messages();

    dump_messages();

}
