#include <stdio.h>

#include <vector>
#include <iostream>

#include "SS_Input_Manager.hxx"
#include "SS_Cmd_Execution.hxx"

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;

extern FILE *yyin;
extern int yyparse (void);
ParseResult *p_parse_result;


/**
 * Method: s_dumpRange()
 * 
 * Dump range and point lists
 * 
 *     
 * @return  void.
 *
 * @todo    Error handling.
 *
 */
static void 
s_dumpRange(vector<ParseRange> *p_list, char *label, bool is_hex, bool newline)
{
    vector<ParseRange>::iterator iter;
    
    if (is_hex){
    	cout.setf(ios_base::hex,ios_base::basefield);
    	cout.setf(ios_base::showbase);
    }
    else {
    	cout.setf(ios_base::dec,ios_base::basefield);
    	cout.unsetf(ios_base::showbase);
    }
    
    if (p_list->begin() != p_list->end())
    	    cout << "\t" << label << ": " ;
    if (newline)
    	cout << endl;

    for (iter=p_list->begin();iter != p_list->end(); iter++) {
    	
	if (newline)
	    cout << "\t\t";
    	parse_range_t *p_range = iter->getRange();
    	if (p_range->is_range) {
    	    parse_val_t *p_val1 = &p_range->start_range;
    	    parse_val_t *p_val2 = &p_range->end_range;
    	    if (p_val1->tag == VAL_STRING) {
    	    	cout << p_val1->name << ":";
    	    }
    	    else {
    	    	cout << p_val1->num << ":";
    	    }
    	    if (p_val2->tag == VAL_STRING) {
    	    	cout << p_val2->name << " ";
    	    }
    	    else {
    	    	cout << p_val2->num << " ";
    	    }
    	}
    	else {
    	    parse_val_t *p_val1 = &p_range->start_range;
    	    if (p_val1->tag == VAL_STRING) {
    	    	cout << p_val1->name << " ";
    	    }
    	    else {
    	    	cout << p_val1->num << " ";
    	    }
    	}
	
	if (newline)
    	    cout << endl ;

    }
    if (p_list->begin() != p_list->end())
    	    cout << endl ;

}

/**
 * Function: parse_line
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
void
parse_line(char *input_line)
{
int ret;

    // Give yacc access to ParseResult object.
    /*
    	Will this be destroyed at the end of the function
	or do I need to new and delete it?
     */
    ParseResult parse_result = ParseResult();
    p_parse_result = &parse_result;

    printf("\n****************************\n%s\n",input_line);

    yyin = fopen("/usr/tmp/jack.tmp","w+");
    fprintf(yyin,"%s\n", input_line);
    rewind(yyin);

    ret = yyparse();

    // testing code
    if (!p_parse_result->syntax_error())
    	p_parse_result->dumpInfo();
    // Syntax error.
    else {
    	char *cmd_name = p_parse_result->GetCommandname();
	
//    	cout << "SYNTAX ERROR!!!" << endl;
//	if (cmd_name)
//	    cout << "Command: " << cmd_name << endl;

    	s_dumpRange(p_parse_result->getErrorList(), 
	    	    cmd_name, 
		    false /* is_hex */,
		    true /* newline */);
    }
    
    fclose(yyin); 
    //delete p_parse_result;
}

/**
 * Function: main
 * 
 * .
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
int 
main (int argc ,char ** argv) {
    int i;
    char input_line[512]; /*  */
    char *p_line;
    FILE *stream = NULL;
    

    if  (argc !=2) {
    	printf("Usage: %s <oss_commandfile>\n", argv[0]);
	return 1;
    }
    else {
    	stream = fopen(argv[1],"r");
	if (stream == NULL) {
    	    printf("Unable to open %s for reading\n", argv[1]);
    	    return 1;
	}
	p_line = fgets(input_line,512,stream);
	while(p_line) {
    	    int len = strlen(input_line);

    	    if (input_line[len-1] == '\n')
    	    	input_line[len-1] = '\0';

	    parse_line(p_line);
	    p_line = fgets(input_line,512,stream);
	}

	fclose(stream);
    }

    return 0;
 }

