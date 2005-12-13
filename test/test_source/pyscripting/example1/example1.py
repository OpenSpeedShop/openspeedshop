import openss

# Create a FileList object with the name of
# the application to analyze.
my_file = openss.FileList("../../../executables/fred/fred")

# Determine the type of experiment to run.
my_exptype = openss.ExpTypeList("pcsamp")

# Register the experiment to be run.
my_id = openss.expCreate(my_file,my_exptype)

# Run the instrumented application
openss.expGo()

# expGo is the only asynchronous command. We may need
# to wait until our app is done before harvesting
# the data.
openss.wait()

# Describe the type of view we want and get the 
# results of the experiment.
my_viewtype = openss.ViewTypeList()
my_viewtype += "pcsamp"
result = openss.expView(my_id,my_viewtype)

# Print out the results. In this case the results are
# in a double array. Normally either the structure of
# the return value will be known or one will have to 
# query each of the object elements.
r_count = len(result)
for row_ndx in range(r_count):
    print " "
    row =result[row_ndx]
    c_count = len(row)
    for rel_ndx in range(c_count):
        print row[rel_ndx]

# Cleanup any intermediate openss files.
openss.exit()

