# This is a minimal sed script used to strip out
# Epytext markups in the python code so the runtime
# help output looks reasonable.
#
# Here is what I am currently looking for. As our 
# python code gets more complicated and or our use
# of epytext gets more sophisicated we will need to
# add and alter this script or use some other method
# for preprocessing the python. <text> just represents
# the stuff we are NOT deleting.
#
#   B{<text>}
#   C{<text>}
#   I{<text>}
#   L{<text>}
#   M{<text>}
#   S{<text>}
#   U{<text>}
#   X{<text>}
#
#   @param

s/[BCILMSUX]{//g
s/}//g
s/@param//g
