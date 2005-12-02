/*******************************************************************************
 * Copyright 2002 Silicon Graphics, Inc.  ALL RIGHTS RESERVED
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS LEGEND
 *
 * Use, duplication or disclosure by the Government is subject to restrictions
 * as set forth in FAR 52.227.19(c)(2) or subparagraph (c)(1)(ii) of the Rights
 * in Technical Data and Computer Software clause at DFARS 252.227-7013 and/or
 * in similar or successor clauses in the FAR, or the DOD or NASA FAR
 * Supplement. Unpublished -- rights reserved under the Copyright Laws of the
 * United States. Contractor/manufacturer is Silicon Graphics, Inc.,
 * 2011 N. Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF SGI
 * 
 * The copyright notice above does not evidence any actual or intended
 * publication or disclosure of this source code, which includes information
 * that is the confidential and/or proprietary, and is a trade secret, of
 * Silicon Graphics, Inc. Any use, duplication or disclosure not specifically
 * authorized in writing by Silicon Graphics is strictly prohibited.
 * ANY DUPLICATION, MODIFICATION, DISTRIBUTION,PUBLIC PERFORMANCE, OR PUBLIC
 * DISPLAY OF THIS SOURCE CODE WITHOUT THE EXPRESS WRITTEN CONSENT OF SILICON
 * GRAPHICS, INC. IS STRICTLY PROHIBITED.  THE RECEIPT OR POSSESSION OF THIS
 * SOURCE CODE AND/OR INFORMATION DOES NOT CONVEY ANY RIGHTS TO REPRODUCE,
 * DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL
 * ANYTHING THAT IT MAY DESCRIBE, IN WHOLE OR IN PART.
 *******************************************************************************
 * mkwrapper.c
 *
 * SpeedShop utility command for generating MPI function wrappers. This command
 * takes three arguments:
 *
 *     mkwrapper MPI-Function-Name MPI-Header-File-Name Template-File-Name
 *
 * The utility will read the MPI header file named MPI-Header-File-Name and look
 * for the MPI-Function-Name function. Upon locating this function the utility
 * will parse the function's declaration to determine its argument list. Then
 * the template file Template-File-Name is read and is copied to standard out
 * stream with the following substitutions being performed along the way:
 *
 *     @@
 *
 *         Considered an escape sequence for `@'.
 *  
 *     @FUNCTION@
 *
 *         Prints the MPI function name. This is directly copied from the first
 *         argument of the command.
 *
 *         Example:  MPI_Send
 *
 *     @FORTRAN_FUNCTION@
 *
 *         Prints the FORTRAN version of the MPI function name. This is created
 *         by converting the first argument of the command to lower case and
 *         then appending an `_' character.
 *
 *         Example:  mpi_send_
 *
 *     @TYPED_ARGS@
 *
 *         Prints a typed argument list for the MPI function. This is built from
 *         the arguments found in the MPI header file.
 *
 *         Example:  void * arg1, int arg2, MPI_Datatype arg3,
 *                   int arg4, int arg5, MPI_Comm arg6
 *
 *     @FORTRAN_TYPED_ARGS@
 *
 *         Prints the FORTRAN version of the typed argument list. Same as above,
 *         but with an additional test to insure that all arguments are passed
 *         by reference instead of value (FORTRAN doesn't pass by value).
 *
 *         Example:  void * arg1, int * arg2, MPI_Datatype * arg3,
 *                   int * arg4, int * arg5, MPI_Comm * arg6
 *
 *     @ARGS@
 *
 *         Prints the argument list for the MPI function. This is built from the
 *         arguments found in the MPI header file.
 *
 *         Example:  arg1, arg2, arg3, arg4, arg5, arg6
 *
 * The command handles any and all errors by printing an error message to the
 * standard error stream and exiting with an exit code of `1'. Otherwise an exit
 * code of `0' is returned.
 ******************************************************************************/

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>



static char* ApplicationName = NULL;

typedef struct {
    char* name;
    int fd;
    char* start;
    size_t size;
} TextFile;



static void OpenTextFile(TextFile* file)
{
    struct stat status;
    
    /* Preconditions */
    assert(ApplicationName != NULL);
    assert(file != NULL);
    assert(file->name != NULL);
    
    /* Open the specified file */
    file->fd = open(file->name, O_RDONLY);
    if (file->fd == -1) {
	fprintf(stderr, "\n%s: unable to open `%s'\n\n",
		ApplicationName, file->name);
	exit(1);
    }
    
    /* Query the size of the file */
    if (fstat(file->fd, &status) == -1) {
	fprintf(stderr, "\n%s: fstat() of `%s' failed\n\n",
		ApplicationName, file->name);
	exit(1);
    }
    file->size = (size_t) status.st_size + 1;
    
    /* Map the file into our address space */
    file->start = (char*) mmap(NULL, file->size,
			       PROT_READ | PROT_WRITE, MAP_PRIVATE,
			       file->fd, 0);
    if (file->start == MAP_FAILED) {
	fprintf(stderr, "\n%s: mmap() of `%s' failed\n\n",
		ApplicationName, file->name);
	exit(1);
    }
    
    /* Turn the mapped file into one giant null-terminated string */
    (file->start)[file->size - 1] = '\0';    
}



static void CloseTextFile(TextFile* file)
{    
    /* Preconditions */
    assert(ApplicationName != NULL);
    assert(file != NULL);
    assert(file->name != NULL);
    assert(file->fd >= 0);
    assert(file->start != MAP_FAILED);
    assert(file->size > 0);
    
    /* Unmap the file from our address space */
    if (munmap(file->start, file->size) == -1) {
	fprintf(stderr, "\n%s: munmap() failed for `%s'\n\n",
		ApplicationName, file->name);
	exit(1);
    }
    
    /* Close the file */
    if (close(file->fd) == -1) {
	fprintf(stderr, "\n%s: unable to close `%s'\n\n",
		ApplicationName, file->name);
	exit(1);
    }    
}



static char** FindArguments(TextFile* file, char* function_name)
{
    int num_arguments;
    char** arguments;
    char* function;
    char* open_paren;
    char* close_paren;
    char* arg;
    char* next;

    /* Preconditions */
    assert(ApplicationName != NULL);
    assert(file != NULL);
    assert(file->name != NULL);
    assert(file->fd >= 0);
    assert(file->start != MAP_FAILED);
    assert(file->size > 0);
    assert(function_name != NULL);
    
    /* Allocate the initially empty argument list */
    num_arguments = 1;
    arguments = (char**) malloc(num_arguments * sizeof(char*));
    if (arguments == NULL) {
	fprintf(stderr, "\n%s: memory allocation failed\n\n", ApplicationName);
	exit(1);
    }
    arguments[num_arguments - 1] = NULL;
    
    /* Locate the specified function name in the text file */
    function = strstr(file->start, function_name);
    if (function == NULL) {
	fprintf(stderr, "\n%s: parsing error seeking `%s'\n\n",
		ApplicationName, function_name);
	exit(1);
    }
    
    /* Locate the first `(' and ')' following the function name */
    open_paren = strstr(function, "(");
    if (open_paren == NULL) {
	fprintf(stderr, "\n%s: parsing error seeking `('\n\n",
		ApplicationName);
	exit(1);
    }
    close_paren = strstr(function, ")");
    if (close_paren == NULL) {
	fprintf(stderr, "\n%s: parsing error seeking `('\n\n",
		ApplicationName);
	exit(1);
    }

    /* Parse the list of argument types for this function */    
    for (arg = open_paren + 1; arg < close_paren; arg = next + 1) {
	
	/* Eat any white space before the next argument */
	for(; isspace(*arg) != 0; ++arg);
	
	/* Look for the next `,' or close parenthesis */
	for(next = arg + 1; (*next != ',') && (next != close_paren); ++next);
	
	/* Terminate the argument type string with a NULL character */
	*next = '\0';

	/* Expand the argument list by one entry */
	++num_arguments;
	arguments = (char**) realloc(arguments, num_arguments * sizeof(char*));
	if (arguments == NULL) {
	    fprintf(stderr, "\n%s: memory allocation failed\n\n",
		    ApplicationName);
	    exit(1);
	}
	arguments[num_arguments - 1] = NULL;
	
	/* Enter this argument into the argument list */
	arguments[num_arguments - 2] = arg;
    }
    
    /* Return the argument list to the caller */
    return arguments;    
}



static void DoSubstitution(TextFile* file,
			   char* function_name, char** arguments)    
{
    int i;
    char* copy;
    char* sub;
    char* next;

    /* Preconditions */
    assert(ApplicationName != NULL);
    assert(file != NULL);
    assert(file->name != NULL);
    assert(file->fd >= 0);
    assert(file->start != MAP_FAILED);
    assert(file->size > 0);
    assert(function_name != NULL);
    assert(arguments != NULL);

    /* Copy the file to standard out while performing substitutions */
    for (copy = file->start;
	 copy < (file->start + file->size);
	 copy = sub + 1) {
	
	/* Look for the next `@' or NULL character */
	for(sub = copy; (*sub != '@') && (*sub != '\0'); ++sub);
	
	/* Write previous non-substituted part of the file to standard out */	
	if (fwrite(copy, sizeof(char), (sub - copy), stdout) < (sub - copy)) {
	    fprintf(stderr, "\n%s: fwrite() failed\n\n",
		    ApplicationName);
	    exit(1);	    
	}
	
	/* Continue if we didn't find a `@' */
	if (*sub != '@') {
	    continue;
	}

	/* Skip over the first `@' */
	++sub;
	
	/* Look for the next `@' or NULL character */
	for(next = sub; (*next != '@') && (*next != '\0'); ++next);
	
	/* Continue if we didn't find another `@' */
	if (*next != '@') {
	    fprintf(stderr, "\n%s: found unterminated substitution tag\n\n",
		    ApplicationName);
	    exit(1);	    
	}
	
	/* Terminate the substitution string with a NULL character */
	*next = '\0';
	
	/* Handle the substitution as appropriate */
	if (strlen(sub) == 0) {
	    printf("@");
	}
	else if(strcmp(sub, "FUNCTION") == 0) {
	    printf("%s", function_name);
	}
	else if(strcmp(sub, "FORTRAN_FUNCTION") == 0) {
	    for(i = 0; i < strlen(function_name); i++) {
		printf("%c", tolower(function_name[i]));
	    }
	    printf("_");
	}
	else if(strcmp(sub, "TYPED_ARGS") == 0) {
	    for(i = 0; arguments[i] != NULL; i++) {
		if (i > 0) {
		    printf(", ");
		}
		printf("%s arg%d", arguments[i], i + 1);
	    }
	}
	else if(strcmp(sub, "FORTRAN_TYPED_ARGS") == 0) {
	    for(i = 0; arguments[i] != NULL; i++) {
		if (i > 0) {
		    printf(", ");
		}
		if (strchr(arguments[i], '*') == NULL)
		    printf("%s * arg%d", arguments[i], i + 1);
		else
		    printf("%s arg%d", arguments[i], i + 1);
	    }
	}
	else if(strcmp(sub, "ARGS") == 0) {
	    for(i = 0; arguments[i] != NULL; i++) {
		if (i > 0) {
		    printf(", ");
		}
		printf("arg%d", i + 1);
	    }
	}
	else {
	    fprintf(stderr, "\n%s: unknown substitution tag `%s'\n\n",
		    ApplicationName, sub);
	    exit(1);	    
	}
	
	/* Advance past the end of the completed substitution */
	sub = next;
    }
    fflush(stdout);
}



int main(int argc, char* argv[])
{
    TextFile mpi_header_file, template_file;
    char** arguments;
    
    /* Insure we have the proper number of arguments */
    assert(argc > 0);
    if (argc != 4) {
	fprintf(stderr,
		"\n"
		"Usage:"
		"  %s"
		"  MPI-Function-Name"
		"  MPI-Header-File-Name"
		"  Template-File-Name"
		"\n\n",
		argv[0]);
	exit(1);
    }
    
    /* Squirrel away the arguments for use by the other functions */
    ApplicationName = argv[0];
    mpi_header_file.name = argv[2];
    template_file.name = argv[3];
    
    /* Open the specified MPI header file */
    OpenTextFile(&mpi_header_file);
    
    /* Open the specified template file */
    OpenTextFile(&template_file);
    
    /* Find the MPI function's argument list in the MPI header file */
    arguments = FindArguments(&mpi_header_file, argv[1]);

    /* Copy the template to standard out while performing substitutions */
    DoSubstitution(&template_file, argv[1], arguments);
    
    /* Destroy the argument list */
    free(arguments);
    
    /* Close the MPI header file */
    CloseTextFile(&mpi_header_file);
    
    /* Close the template file */
    CloseTextFile(&template_file);    
}
