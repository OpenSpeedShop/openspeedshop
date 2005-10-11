/** @file
 *
 * Parser range description.
 *
 */

#ifndef __OpenSpeedShop_Parse_Range_HXX__
#define __OpenSpeedShop_Parse_Range_HXX__

namespace OpenSpeedShop { namespace cli {

typedef enum {
    VAL_STRING,
    VAL_NUMBER
} val_enum_t;

typedef struct {
    // union is not possible with type string
    string name;
    int num;
    val_enum_t tag; /** Determines with field to be used */
} parse_val_t;

typedef struct {
    parse_val_t start_range;	/** Begin of range or point */
    parse_val_t end_range;  	/** End of Range */
    bool   is_range;	    	/** Is this a range or single point */
} parse_range_t;

/**
 * Parser result class.
 *
 *  Describes the results from parsing one OSS command.
 *
 * @todo    Copy constructors.
 *          Unit testing.
 */
class ParseRange {

    public:

//	/** Constructor. */
	ParseRange(int num);
	ParseRange(char *);
	ParseRange(char * name, int num);
	ParseRange(char * name1, char * name2);
	ParseRange(int num, char * name);
	ParseRange(int num1, int num2);

//	/** Destructor. */
//	~ParseRange();

    	parse_range_t *getRange()
	{
	    return &dm_range;
	}

    private:
    	/** range struct to fill */
    	parse_range_t dm_range;

};

} }

#endif // __OpenSpeedShop_Parse_Range_HXX__
