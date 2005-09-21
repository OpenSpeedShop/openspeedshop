

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
gen_perms(char *new_pat)
{

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
void
permutate(int num_fish, int num_buckets, char *pat)
{
    int next_shift; 	// The next fish to be shifted.
    int last_bucket;	// This is the wall
    int last_fish;  	// Fish that gets moved each loop.
    int next_bucket;	// Next slot to put last fish.
    int *fish_array = (int *)malloc(sizeof(int)*num_fish);
    char new_pat[26];
    
    // initialize the fish array for the first
    // set of patterns. We don't care about the
    // last fish because it will be set later.
    last_bucket = num_buckets-1;
    last_fish = num_fish-1;

    if (num_fish == 2) {
    	next_bucket = 0;
    }
    else {
    	next_bucket = num_fish-2;
    }

    for (int i=0; i<(num_fish-1); ++i) {
    	fish_array[i] = i;
    }
    
    // This is the fish that will move to the
    // left when the last fish hits the wall.
    next_shift = num_fish-2;
    
    // Loop until combinations run out.
    while (1) {

    	if (next_bucket != last_bucket) {
    	    fish_array[last_fish] = ++next_bucket;
	}
	else {
	    // special case 2. We always shift the first fish.
	    if (num_fish == 2) {
	    	fish_array[0] = fish_array[0] +1;
		next_bucket = fish_array[0];
		fish_array[last_fish] = ++next_bucket;
	    }
	    else {
	    
	    	fish_array[next_shift] = fish_array[next_shift] +1;
	    	if (fish_array[next_shift] > last_bucket)
	    	    return;
	    	
	    	if (next_shift ==0)
	    	    next_shift = fish_array[num_fish-2];
	    	else
	    	    --next_shift;

	    }

    	    if (fish_array[num_fish-2] == last_bucket) {
    	    	return;
    	    }

	}
	// Put together the new pattern
	for (int i=0; i<num_fish; ++i) {
	    new_pat[i] = pat[fish_array[i]];
	}
	new_pat[num_fish] = '\0';
	
	// This is a kluge. I need to figure out how
	// it could happen that new_pat is smaller than
	// num_fish.
	if ((strlen(new_pat)) < num_fish)
	    return;;
	
	gen_perms(new_pat);
    }
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
    	    	permutate (i,len,new_pat);
		cout << endl;
		break;
	}
    }
    
}

