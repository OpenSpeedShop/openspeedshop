# Use the OpenSpeedShop GUI and/or the CLI to examine the 4 process (rank) 4 thread (openMP) run of BT
# use: openss -f bt-mz.B.4-pcsamp-1.openss
# to use the GUI to examine the performance data.
# use: openss -cli -f bt-mz.B.4-pcsamp-1.openss
# to use the CLI to examine the performance data.
# CLI commands of interest:   expstatus -v all - show status of experiment
#                             expview          - show default view
#                             expview -m loadbalance  - show default load balance view
#                             expview -v statements   - show default per statement view
#                             expview -v linkedobject - show default per library view
#                             expview -r nn           - show performance data for rank rr
#                             expview -r nn -m loadbalance  - show load balance of underlying threads for rank rr
# see QuickStartGuide for more CLI commands

