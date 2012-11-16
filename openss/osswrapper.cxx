#include "BPatch.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_process.h"
#include "BPatch_image.h"
#include "BPatch_function.h"
#include "BPatch_point.h"

#include <cassert>
#include <cstdio>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <set>
#include <map>
#include <utility>

#define warning_printf(format, ...) \
   do { \
     if (print_warnings) \
        fprintf(stderr, "Warning: " format, ## __VA_ARGS__);   \
     had_warning++; \
   } while (0)

#define error_printf(format, ...) \
   do { \
     fprintf(stderr, "Error: " format, ## __VA_ARGS__);  \
     had_error++; \
   } while (0)

#define GUARD_CHECK_NAME "dwrapCheckWrapGuard"

using namespace std;
using namespace Dyninst;

static set<string> libraries_to_add;
struct WrapDesc {
  std::string wrapper;
  std::string wrappee;
  std::string invented_wrappee_name;
};

static vector< WrapDesc > functions_to_wrap;
static string output_file;
static string output_dir;
static string input_file;
static bool rewrite_libs = true;

static bool print_warnings = false;
static unsigned int had_error = 0;
static unsigned int had_warning = 0;

static BPatch_addressSpace *addrspace = NULL;
static BPatch bpatch;

static bool parseArgs(int argc, char *argv[]);
static bool findFunction(string func_s, BPatch_function* &func, bool err_on_fail = false);
static bool wrapFunction(WrapDesc desc);
static bool addLibrary(string libname);
static bool openBinary();
static bool writeBinary();
static void printResults();
static void printHelp(char *argv[]);

std::map<std::string, BPatch_object *> loadedLibs;

int main(int argc, char *argv[]) {
   bool wrapped_a_function = false;
   bool result = parseArgs(argc, argv);
   if (!result) {
      printHelp(argv);
      return -1;
   }

   result = openBinary();
   if (!result) goto done;

   for (set<string>::iterator i = libraries_to_add.begin(); i != libraries_to_add.end(); i++) {
      result = addLibrary(*i);
      if (!result) goto done;
   }

   for (vector<WrapDesc>::iterator i = functions_to_wrap.begin(); i != functions_to_wrap.end(); i++) {
      result = wrapFunction(*i);
      if (result) 
         wrapped_a_function = true;
   }
   if (!wrapped_a_function)
      warning_printf("Did not find any functions to wrap\n");
   
   result = writeBinary();
   if (!result) goto done;

  done:
   printResults();

   return had_error ? -1 : 0;
}

static bool findFunction(string func_s, BPatch_function* &func, bool err_on_fail)
{
   assert(addrspace);
   BPatch_image *image = addrspace->getImage();

   BPatch_Vector<BPatch_function *> funcs;
   image->findFunction(func_s.c_str(), funcs, true, true, true);
   if (!funcs.size()) {
      if (err_on_fail)
         error_printf("Could not find function %s.\n", func_s.c_str());
      else
         warning_printf("Could not find function %s.\n", func_s.c_str());
      return false;
   }
   if (funcs.size() > 1) {
      if (err_on_fail)
         error_printf("Target function %s was not unique, had %d copies.\n", func_s.c_str(), (int) funcs.size());
      else
         warning_printf("Target function %s was not unique, had %d copies.\n", func_s.c_str(), (int) funcs.size());
      return false;
   }
   func = funcs[0];
   return true;
}

#if (DYNINST_MAJOR == 8)

#include "Symtab.h"

static bool wrapFunction(WrapDesc desc)
{
   BPatch_function *wrappee = NULL, *wrapper = NULL;
   Dyninst::SymtabAPI::Symbol *wrapper_clone = NULL;

   bool result = findFunction(desc.wrappee, wrappee);
   if (!result)
      return false;
   result = findFunction(desc.wrapper, wrapper);
   if (!result)
      return false;


// let "mpi_bsend_original" be the name of the invented call in the wrapper
// let mod be the BPatch_module that represents the wrapper library

   // Go from the BPatch_module to its symbol table as represented by a Symtab
  // When you update, use this line instead
//   SymtabAPI::Symtab *obj_symtab = SymtabAPI::convert(wrapper->getModule()->getObject());
   SymtabAPI::Symtab *obj_symtab = SymtabAPI::convert(wrapper->getModule())->exec();


   // Look up the symbol for the invented wrapper call
   std::vector<SymtabAPI::Symbol *> syms;
   // The "true" says "look in undefined symbols too", so it's important
   obj_symtab->findSymbol(syms, desc.invented_wrappee_name, SymtabAPI::Symbol::ST_UNKNOWN, SymtabAPI::anyName, false, false, true);
   // This might be a return false; we didn't find the symbol. 
   assert(syms.size());
   // We may find entries in both the static and dynamic symbol tables. Take the first one definitely, then
   // look for a dynamic one if we can
   SymtabAPI::Symbol *sym = syms[0];
   for (unsigned i = 0; i < syms.size(); ++i) {
     if (syms[i]->isInDynSymtab()) {
       sym = syms[i];
     }
   }

   result = addrspace->wrapFunction(wrappee, wrapper, sym);
/*   result = addrspace->wrapFunction(*wrappee, *wrapper); */
   if (!result) {
      warning_printf("DyninstAPI failed to wrap function %s with %s, using invented name %s\n", 
                      desc.wrappee.c_str(), desc.wrapper.c_str(), desc.invented_wrappee_name.c_str());
      return false;
   }

   return true;
}
#else
static bool wrapFunction(string wrappee_s, string wrapper_s)
{
   BPatch_function *wrappee = NULL, *wrapper = NULL;
   BPatch_function *guardCheck = NULL;

   bool result = findFunction(wrappee_s, wrappee);
   if (!result)
      return false;
   result = findFunction(wrapper_s, wrapper);
   if (!result)
      return false;
   result = findFunction(GUARD_CHECK_NAME, guardCheck, true);
   if (!result)
      return false;

   //AST looks like: 'if (GUARD_CHECK_NAME() == 1) funcJump wrapper'
   BPatch_Vector<BPatch_snippet *> args;
   BPatch_funcCallExpr gcheck_call(*guardCheck, args);
   BPatch_constExpr one((signed int) 1);
   BPatch_boolExpr gcheck_test(BPatch_eq, gcheck_call, one);
   BPatch_funcJumpExpr wrapper_jump(*wrapper);
   BPatch_ifExpr gtest(gcheck_test, wrapper_jump);

   BPatch_Vector<BPatch_point *> *points = wrappee->findPoint(BPatch_locEntry);
   if (!points || points->empty()) {
      error_printf("DyninstAPI internal error.  Function %s with no entry points.\n", wrappee_s.c_str());
      return false;
   }
   BPatchSnippetHandle *hndl = addrspace->insertSnippet(gtest, *points);
   if (!hndl) {
      error_printf("DyninstAPI internal error.  Could not instrument function %s.\n", wrappee_s.c_str());
      return false;
   }
   return true;
}
#endif


static bool addLibrary(string libname)
{
   BPatch_object *result = addrspace->loadLibrary(libname.c_str());
   if (!result) {
      error_printf("DyninstAPI failed to load library %s\n", libname.c_str());
      return false;
   }
   loadedLibs[libname] = result;

   return true;
}

static bool openBinary() {
   BPatch_binaryEdit *binedit = bpatch.openBinary(input_file.c_str(), rewrite_libs);
   if (!binedit) {
      error_printf("DyninstAPI could not open file %s\n", input_file.c_str());
      return false;
   }
   addrspace = static_cast<BPatch_addressSpace *>(binedit);

   bpatch.setTrampRecursive(true);
   bpatch.setInstrStackFrames(false);
   bpatch.setSaveFPR(false);

   return true;
}

static bool writeBinary() {
   if (!output_dir.empty()) {
      int result = chdir(output_dir.c_str());
      if (result == -1) {
         int error = errno;
         error_printf("Failed to chdir to output directory %s: %s\n", output_dir.c_str(), strerror(error));
         return false;
      }
   }

   assert(addrspace);
   BPatch_binaryEdit *binedit = dynamic_cast<BPatch_binaryEdit *>(addrspace);
   assert(binedit);

   bool result = binedit->writeFile(output_file.c_str());
   if (!result) {
      error_printf("DyninstAPI failed to output binary to file %s\n", output_file.c_str());
      return false;
   }

   return true;
}


#define CHECK_NEXT  do { \
     if (i+1 >= argc) { \
       error_printf("%s must be followed by an argument\n", argv[i]); \
       break; \
     } \
   } while (0)
    
static bool parseArgs(int argc, char *argv[]) {
   for (int i = 1; i < argc; i++) {
      if ((strcmp(argv[i], "--wrap") == 0) || strcmp(argv[i], "-w") == 0) {
         CHECK_NEXT;

         char *line = strdup(argv[++i]);
         for (char *s = strtok(line, ","); s; s = strtok(NULL, ",")) {
            char *equal = strchr(line, '=');
            if (!equal) {
               error_printf("Invalid string in --wrap argument\n");
               break;
            }
            *equal = '\0';
            WrapDesc desc;
            desc.wrappee = s;
            desc.wrapper = equal+1;
            desc.invented_wrappee_name = desc.wrappee + "_original";
            functions_to_wrap.push_back(desc);
            *equal = '=';
         }
         free(line);
      }
      else if (strcmp(argv[i], "--library") == 0 || strcmp(argv[i], "-l") == 0) {
         CHECK_NEXT;
         char *line = strdup(argv[++i]);
         for (char *s = strtok(line, ","); s; s = strtok(NULL, ",")) {
            libraries_to_add.insert(s);
         }
         free(line);
      }
      else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) {
         CHECK_NEXT;
         output_file = string(argv[++i]);
      }
      else if (strcmp(argv[i], "--directory") == 0 || strcmp(argv[i], "-d") == 0) {
         CHECK_NEXT;
         output_dir = string(argv[++i]);
      }
      else if (strcmp(argv[i], "--warnings") == 0 || strcmp(argv[i], "-W") == 0) {
         print_warnings = true;
      }
      else if (strcmp(argv[i], "--exec_only") == 0) {
         rewrite_libs = false;
      }
      else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
         printHelp(argv);
         exit(0);
      }
      else if (*argv[i] != '-') {
         if (!input_file.empty()) {
            error_printf("Multiple input files specified: %s %s\n", input_file.c_str(), argv[i]);
            break;
         }
         input_file = argv[i];
      }
      else {
         error_printf("Unrecognized command line option: %s\n", argv[i]);
         break;
      }
   }

   if (!had_error && input_file.empty()) {
      error_printf("Must specify an input file\n");
   }
   if (!had_error && output_file.empty()) {
      error_printf("Must specify an output file\n");
   }

   return !had_error;
}

static void printHelpLine(char *opt1, char *opt2, char *opt_arg, char *message)
{
   if (opt2 && opt_arg) 
      fprintf(stderr, "%s (%s) %s\n", opt1, opt2, opt_arg);
   else if (opt2 && !opt_arg)
      fprintf(stderr, "%s (%s)\n", opt1, opt2);
   else if (opt_arg)
      fprintf(stderr, "%s %s\n", opt1, opt_arg);
   else
      fprintf(stderr, "%s\n", opt1);
   fprintf(stderr, "        %s\n", message);
}

static void printHelp(char *argv[])
{
   fprintf(stderr, "Usage: %s input-file -o output-file [OPTIONS]\n\n", argv[0]);
   printHelpLine("--wrap", "-w", "wrapee=wrapper,...", "Specifies a function, wrappee, that should be wrapped by another function, wrapper");
   printHelpLine("--library", "-l", "libname,...", "Specifies a library that should be added to the binary");
   printHelpLine("--output", "-o", "output-file", "Specifies a name for the outputed executable or library");
   printHelpLine("--directory", "-d", "directory", "Specifies a directory that the output file and any rewritten libraries will be written to");
   printHelpLine("--warnings", "-W", NULL, "Print all warnings");
   printHelpLine("--exec_only", NULL, NULL, "Do not allow any rewriting of the input-file's dependent shared libraries");
   printHelpLine("--help", "-h", NULL, "Print this message");
}

static void printResults()
{
   if (had_error) {
      fprintf(stderr, "Rewriting failed with %d errors and %d warnings.\n", had_error, had_warning);
   }
   else if (print_warnings && had_warning) {
      fprintf(stderr, "Rewriting completed with %d warnings\n", had_warning);
   }
}

