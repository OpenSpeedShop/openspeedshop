# Use the OpenSpeedShop GUI and/or the CLI to examine the 256 process run of LU
# use: openss -f lu.C.256-pcsamp.openss
# to use the GUI to examine the performance data.
# use: openss -cli -f lu.C.256-pcsamp.openss
# to use the CLI to examine the performance data.
# CLI commands of interest:   expstatus -v all - show status of experiment
#                             expview          - show default view
#                             expview -m loadbalance  - show default load balance view
#                             expview -v statements   - show default per statement view
#                             expview -v linkedobject - show default per library view
#                             expview -r nn           - show performance data for rank rr
# see QuickStartGuide for more CLI commands

