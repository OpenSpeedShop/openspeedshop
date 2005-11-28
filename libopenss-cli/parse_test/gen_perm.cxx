

#include <iterator>
#include <vector>
#include <iostream>

using namespace std;

static char *pattern = {"abcdefghijklmnop"};
static char **input_array = NULL;

/**
 * Function: translate
 * 
 * Translate the patern token into user's string.
 *     
 * @param   token   character representing postition.
 *
 * @return  char * of users string.
 *
 */
char * translate(char token)
{
    int position = (token - 'a') + 1;

    return input_array[position];
}

/**
 * Function: vec
 * 
 * Make vector of characters from string.
 *     
 * @param   xx    	xx.
 *
 * @return  void.
 *
 */
vector<char> vec(char *s)
{
    vector<char> word;

    while (*s!='\0') {
    	word.push_back(*s++);
    };
    
    return word;
}

/**
 * Function: gen_perms
 * 
 * .
 *     
 * @param   clump_size	size of prefex characters.
 * @param   pat    	original pattern.
 *
 * @return  void.
 *
 */
void
gen_perms(char *pat,
    	  int *fish_array,
	  int num_fish)
{

    char new_pat[26];

	// Put together the new pattern
	for (int i=0; i<num_fish; ++i) {
	    new_pat[i] = pat[fish_array[i]];
	}
	new_pat[num_fish] = '\0';
	
    // This produces all permutation of the
    // manufactured string.
    vector<char> word = vec(new_pat);
    int count = strlen(new_pat);
    
    do {
    	cout << "    	    	|   ";
    	for (int i = 0; i<count;++i) {
    	    cout << translate(word[i]) << " ";
    	}
    	cout << endl;

    } while (next_permutation(word.begin(),word.end()));

}

/**
 * Function: purmutate
 * 
 * .
 *     
 * @param   clump_size	size of prefex characters.
 * @param   pat    	original pattern.
 *
 * @return  void.
 *
 */
bool
permutate(  int group_size, 	// Number of fish in net
    	    int bucket_count, 	// Number of buckets for fish
	    int ndx_level,  	// Fish that gets moved each loop.
	    int *pos_array, 	// Where current fish are
	    char *pat)
{
    bool ret = false;
    
    // We need to get deeper
    if (ndx_level < (group_size-1)) {
    	for (int i=pos_array[ndx_level];i<bucket_count;++i) {
	    pos_array[ndx_level] = i;
	    pos_array[ndx_level+1] = i+1;
	    if (1) {
	    	ret = permutate(group_size,
		    	    	bucket_count,
				ndx_level+1,
				pos_array,
				pat);
		if (ret !=true)
		    break; // Go up a level
	    }
	}
    }
    else { // leaf situation, last fish
    	for (int i=pos_array[ndx_level];i<bucket_count;++i) {
	    pos_array[ndx_level] = i;
	    if (ndx_level < group_size) {
	    	gen_perms(pat,pos_array,group_size);
	    }
	    else
	    	return false; // end of the road
	}
    }

    return true;
}

/**
 * Function: main
 * 
 * dump command line permutations for YACC.
 *     
 *
 * @return  int.
 *
 */
int 
main(int argc ,char ** argv)
{
    int len = argc-1;
    char *new_pat = (char *)malloc(sizeof(char)*(len+1));
    
    input_array = argv;
    
    strncpy(new_pat,pattern,len);

    for (int i=0; i<=len; ++i) {
    	switch(i) {
	    case 0:
	    	cout << "    	    	    /* empty */" << endl << endl;
		break;
	    case 1:
	    	for (int j=0;j<len;++j) {
    		    cout << "    	    	|   ";
    	    	    cout << translate(new_pat[j]) << endl;
		}
		cout << endl;
		break;
	    default:
    	    	int *pos_array = (int *)malloc(sizeof(int)*i);
		memset(pos_array,0,sizeof(int)*i);
    	    	permutate(i,
		    	  len,
			  0,	// Fish that gets shifted
			  pos_array,
			  new_pat);
		cout << endl;
		break;
	}
    }
    
}

