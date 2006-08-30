
#include "BPatch.h"
#include "BPatch_function.h"

BPatch bpatch;

int main(int argc, char* argv[])
{
    int count = 0;

    // Display usage information when necessary
    if(argc != 2) {
	printf("Usage: %s <pid>\n", argv[0]);
	exit(1);
    }
    
    // Attach to the running program
    BPatch_thread* appThread = bpatch.attachProcess(NULL, atoi(argv[1]));

    // Read the program's image and get the associated image object
    BPatch_image* appImage = appThread->getImage();

    BPatch_Vector<BPatch_module *> *bp_module_table = appImage->getModules();


    for (int i = 0; i < bp_module_table->size(); i++) {

          char name[1024];
          (*bp_module_table)[i]->getName(name, sizeof(name));
          printf("%s[%d]:  found module %s\n",
                __FILE__, __LINE__, name );

        // Iterate over each module-scope variable in this module
         BPatch_Vector<BPatch_variableExpr*> bp_variables;
         (*bp_module_table)[i]->getVariables(bp_variables);
         for(int i = 0; i < bp_variables.size(); ++i) {

              if(bp_variables[i] != NULL && strstr(bp_variables[i]->getName(), "MPIR") ) {
                    printf("%s[%d]:  found variable %s\n",
                         __FILE__, __LINE__, bp_variables[i]->getName());
              }
         } // end for var

    } // end for modules

    printf("End of variable mutator\n");
}
