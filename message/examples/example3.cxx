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
    if (1) {
    	SS_Message_Element element;
	
    	// Set keyword. You really need to have a keyword.
    	element.set_keyword("bosco");
    
    	// Numeric id
	// This message is not associated with an error number so we won't set this field
    	//element.set_id(p_element->errnum);
    
    	// Related keywords
	// In  case you want references to other messages.
	// I don't currently check to see if these other keywords exist.
    	element.add_related("bozo");
    	element.add_related("dweebo");
    
    	// General topic
	// This is a general classification for this message.
	// It helps for listing related messages per topic.
    	element.set_topic("dismissives");

    	// Brief, one line description
    	element.set_brief("Bosco is a traditional Irish name");

    	// More than one line description
    	element.add_normal("I often use this name to refer to people I don't");
    	element.add_normal("know, but am irritated with such as those that");
    	element.add_normal("don't adhere to commonly accepted driving ettiquette");

    	// A wordy explaination
    	element.add_verbose("I often use this name to refer to people I don't");
    	element.add_verbose("know, but am irritated with such as those that");
    	element.add_verbose("don't adhere to commonly accepted driving ettiquette");
    	element.add_verbose(" ");
    	element.add_verbose("I originally learned of this word/name by listening to");
    	element.add_verbose("children\'s television. Bosco chocolate flavored");
    	element.add_verbose("milk was often advertised.");

    	// An example of usage
    	element.add_example("Someone is tailgating you on the freeway and you:");
    	element.add_example("mutter to no one in particular: ");
    	element.add_example(" ");
    	element.add_example("\tHey bosco! Did you learn to drive on bumper cars");
    	element.add_example("\tin the carnival?");

    	// Submit the message to the database
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
	string name("bosco");
    	vector <SS_Message_Element *> element;
	    
    	czar.Find_By_Keyword(name.c_str(), &element);
    
    	if (element.begin() == element.end()) {
	    cout << "No help for " + name << endl;
    	}

    	cout << "****************" << endl;
	cout << "KEYWORD" << endl;
	cout <<  name << endl;
    	cout << "****************" << endl;
	
	// There may be multiple entries for this keyword
    	vector <SS_Message_Element*>:: iterator k;
    	for (k = element.begin();
    	    k != element.end();
	    ++k) {
	    SS_Message_Element *p_el = *k;
	    
	    // print out topic
    	    cout << endl << "****************" << endl;
	    cout << "TOPIC" << endl;
    	    cout << "****************" << endl;
	
	    cout << p_el->get_topic() << endl;
	    
	    // print out  related keywords
    	    cout << endl << "****************" << endl;
	    cout << "RELATED KEYS" << endl;
    	    cout << "****************" << endl;
	    vector<string> * const p_key_list = p_el->get_related_list();
    	    for (vector <string> :: iterator i=p_key_list->begin();
    	     	 i!= p_key_list->end();
	     	 ++i) {
		cout << *i << endl;
	    }
	    
	    // print out id number (none set so this should -1)
    	    cout << endl << "****************" << endl;
	    cout << "ID NUMBER" << endl;
    	    cout << "****************" << endl;
	    cout << p_el->get_id() << endl;
	    
	    // print out brief message
    	    cout << endl << "****************" << endl;
	    cout << "BRIEF" << endl;
    	    cout << "****************" << endl;
	
	    cout << p_el->get_brief() << endl;
	    
	    // print out normal length message
    	    cout << endl << "****************" << endl;
	    cout << "NORMAL" << endl;
    	    cout << "****************" << endl;
	    vector<string> * const p_normal = p_el->get_normal_list();
    	    for (vector <string> :: iterator i=p_normal->begin();
    	     	 i!= p_normal->end();
	     	 ++i) {
		cout << *i << endl;
	    }
	    
	    // print out verbose length message
    	    cout << endl << "****************" << endl;
	    cout << "VERBOSE/DETAILED" << endl;
    	    cout << "****************" << endl;
	    vector<string> * const p_verbose = p_el->get_verbose_list();
    	    for (vector <string> :: iterator i=p_verbose->begin();
    	     	 i!= p_verbose->end();
	     	 ++i) {
		cout << *i << endl;
	    }
	    
	    // print out example message
    	    cout << endl << "****************" << endl;
	    cout << "EXAMPLE" << endl;
    	    cout << "****************" << endl;
	    vector<string> * const p_example = p_el->get_example_list();
    	    for (vector <string> :: iterator i=p_example->begin();
    	     	 i!= p_example->end();
	     	 ++i) {
		cout << *i << endl;
	    }
	    cout << endl;
    	}
	++i;
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
