////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////


#include <assert.h>
#include <dpcl.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX  256
#endif

static std::string typeToString(DataExpNodeType type)
{
    switch(type) {	
    case DEN_actual_param_type: return "DEN_actual_param_type";
    case DEN_array_type: return "DEN_array_type";
    case DEN_class_type: return "DEN_class_type";
    case DEN_default_type: return "DEN_default_type";
    case DEN_enum_type: return "DEN_enum_type";
    case DEN_error_type: return "DEN_error_type";
    case DEN_float32_type: return "DEN_float32_type";
    case DEN_float64_type: return "DEN_float64_type";
    case DEN_function_type: return "DEN_function_type";
    case DEN_int16_type: return "DEN_int16_type";
    case DEN_int32_type: return "DEN_int32_type";
    case DEN_int64_type: return "DEN_int64_type";
    case DEN_int8_type: return "DEN_int8_type";
    case DEN_pointer_type: return "DEN_pointer_type";
    case DEN_reference_type: return "DEN_reference_type";
    case DEN_return_value_type: return "DEN_return_value_type";
    case DEN_struct_type: return "DEN_struct_type";
    case DEN_uint16_type: return "DEN_uint16_type";
    case DEN_uint32_type: return "DEN_uint32_type";
    case DEN_uint64_type: return "DEN_uint64_type";
    case DEN_uint8_type: return "DEN_uint8_type";
    case DEN_union_type: return "DEN_union_type";
    case DEN_unspecified_type: return "DEN_unspecified_type";
    case DEN_user_type: return "DEN_user_type";
    case DEN_void_type: return "DEN_void_type";
    case DEN_list_item: return "DEN_list_item";
    default: return "<unknown>";
    }
}

static void stderrCallback(GCBSysType sys, GCBTagType,
			   GCBObjType, GCBMsgType msg)
{
    char* ptr = (char*)msg;
    for(int i = 0; i < sys.msg_size; ++i, ++ptr)
	fputc(*ptr, stderr);
    fflush(stderr);
}

static void stdoutCallback(GCBSysType sys, GCBTagType,
			   GCBObjType, GCBMsgType msg)
{
    char* ptr = (char*)msg;
    for(int i = 0; i < sys.msg_size; ++i, ++ptr)
	fputc(*ptr, stdout);
    fflush(stdout);    
}

int main(int argc, char* argv[])
{
    AisStatus retval;

    // Display usage information when necessary
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " <a.out> <args>" << std::endl;
        return 1;
    }
    
    // Initialize DPCL
    Ais_initialize();
    if(getenv("DEBUG_DPCLD") != NULL) {
        char hostname[HOST_NAME_MAX];
        assert(gethostname(hostname, sizeof(hostname)) == 0);
        Ais_blog_on(hostname, LGL_detail, LGD_daemon, NULL, 0);
    }
    
    // Create the process
    Process process;
    extern char** environ;
    retval = process.bcreate(NULL, argv[1], (const char**)&(argv[1]),  (const char**)environ,
			     stdoutCallback, NULL, stderrCallback, NULL);
    assert(retval.status() == ASC_success);
    
    // Get the program object associated with this process
    SourceObj program = process.get_program_object();
    
    // Iterate over each module in this program
    for(int m = 0; m < program.child_count(); ++m) {
	SourceObj module = program.child(m);

	// Get the name of this module
	char name[module.module_name_length() + 1];
	module.module_name(name, sizeof(name));	

	// Ignore everything but "mutatee.cxx"
	if(strcmp(name, "mutatee.cxx"))
	    continue;

	// Expand this module
	retval = program.child(m).bexpand(process);
	assert(retval.status() == ASC_success);

	// Output
	std::cout << "Module \"" << name << "\"" << std::endl;

	// Iterate over each data entity in this module
	for(int d = 0; d < module.child_count(); ++d)
	    if(module.child(d).src_type() == SOT_data) {
		SourceObj data = module.child(d);
		
		// Get the name of this (variable) data entity
		char name[data.get_variable_name_length() + 1];
		data.get_variable_name(name, sizeof(name));

		// Output
		std::cout << "    " << name << " ("
			  << typeToString(data.get_data_type().get_node_type())
			  << ")" << std::endl;
		
	    }

    }

    // Destroy the process
    retval = process.bdestroy();
    assert(retval.status() == ASC_success);
    
    // Success!
    return 0;
}
