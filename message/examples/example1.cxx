/** @file
 *
 * Load messages pertaining to this module.
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
    int i = 0;
    while (example1_msg[i].topic != NULL) {
    	SS_Message_Element element;
	
	element.set_element(&example1_msg[i]);
	czar.Add_Help(element);
	++i;
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
