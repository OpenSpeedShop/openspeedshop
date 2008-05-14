#include <bfd.h>
#include <stdint.h>
#include "ToolAPI.hxx"
#include "LinkedObject.hxx"

namespace OpenSpeedShop { namespace Framework {

struct BFDFunction {
    std::string func_name;
    Address func_begin;
    Address func_end;

    BFDFunction (const std::string& name,
		  const uint64_t& begin,
		  const uint64_t& end ) :
	func_name(name),
	func_begin(begin),
	func_end(end)
    {
    }
};

struct BFDStatement
{
    uint64_t pc;
    std::string file_name;
    unsigned int lineno;

    BFDStatement (const uint64_t& pc,
	           const std::string& filename,
	           const unsigned int& line) :
	pc(pc),
	file_name(filename),
	lineno(line)
   {
   }
};


typedef std::map<std::string,BFDFunction> FuncMap;
typedef std::vector<BFDStatement> StatementsVec;

class BFDSymbols {

    public:
    FuncMap	  bfd_functions;
    StatementsVec bfd_statements;

    int		getBFDFunctionStatements(PCBuffer*, LinkedObject*);
    int		getFunctionSyms(PCBuffer*, FuncMap*, bfd_vma, bfd_vma);
    int		initBFD(std::string);
    Path	getObjectFile(Path);

    private:

    void	slurp_symtab (bfd *);
    int		dummyprint () { return 0; };
    long	remove_useless_symbols (asymbol **symbols, long count);

    int init_done;

#ifndef NDEBUG
    static bool is_debug_symbols_enabled;
    static bool is_debug_offline_enabled;
#endif

};

} }
