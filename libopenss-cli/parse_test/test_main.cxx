#include <stdio.h>
#include <vector>
#include <iostream>

#include "Python.h"
#include "SS_Input_Manager.hxx"

using namespace std;

#include "SS_Parse_Range.hxx"
#include "SS_Parse_Target.hxx"
#include "SS_Parse_Result.hxx"

using namespace OpenSpeedShop::cli;

extern int yydebug;
extern FILE *yyin;
extern int yyparse (void);
ParseResult *p_parse_result = NULL;

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

    yyin = fopen("/tmp/parse.tmp","w+");
    fprintf(yyin,"%s\n", input_line);
    rewind(yyin);

    ret = yyparse();

    p_parse_result->dumpInfo();

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

