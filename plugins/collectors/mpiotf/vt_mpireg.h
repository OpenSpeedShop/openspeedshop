/**
 * VampirTrace
 * http://www.tu-dresden.de/zih/vampirtrace
 *
 * Copyright (c) 2005-2007, ZIH, TU Dresden, Federal Republic of Germany
 *
 * Copyright (c) 1998-2005, Forschungszentrum Juelich GmbH, Federal
 * Republic of Germany
 *
 * See the file COPYRIGHT in the package base directory for details
 **/

#ifndef _VT_MPIREG_H
#define _VT_MPIREG_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#define VT__MPI_ABORT                              0
#define VT__MPI_ADDRESS                            1
#define VT__MPI_ALLGATHER                          2
#define VT__MPI_ALLGATHERV                         3
#define VT__MPI_ALLREDUCE                          4
#define VT__MPI_ALLTOALL                           5
#define VT__MPI_ALLTOALLV                          6
#define VT__MPI_ATTR_DELETE                        7
#define VT__MPI_ATTR_GET                           8
#define VT__MPI_ATTR_PUT                           9  
#define VT__MPI_BARRIER                           10
#define VT__MPI_BCAST                             11
#define VT__MPI_BSEND                             12
#define VT__MPI_BSEND_INIT                        13
#define VT__MPI_BUFFER_ATTACH                     14
#define VT__MPI_BUFFER_DETACH                     15 
#define VT__MPI_CANCEL                            16
#define VT__MPI_CART_COORDS                       17
#define VT__MPI_CART_CREATE                       18
#define VT__MPI_CART_GET                          19
#define VT__MPI_CART_MAP                          20
#define VT__MPI_CART_RANK                         21
#define VT__MPI_CART_SHIFT                        22 
#define VT__MPI_CART_SUB                          23
#define VT__MPI_CARTDIM_GET                       24 
#define VT__MPI_COMM_COMPARE                      25
#define VT__MPI_COMM_CREATE                       26
#define VT__MPI_COMM_DUP                          27
#define VT__MPI_COMM_FREE                         28
#define VT__MPI_COMM_GROUP                        29
#define VT__MPI_COMM_RANK                         30
#define VT__MPI_COMM_REMOTE_GROUP                 31
#define VT__MPI_COMM_REMOTE_SIZE                  32
#define VT__MPI_COMM_SIZE                         33
#define VT__MPI_COMM_SPLIT                        34
#define VT__MPI_COMM_TEST_INTER                   35
#define VT__MPI_DIMS_CREATE                       36 
#define VT__MPI_ERRHANDLER_CREATE                 37
#define VT__MPI_ERRHANDLER_FREE                   38
#define VT__MPI_ERRHANDLER_GET                    39 
#define VT__MPI_ERRHANDLER_SET                    40
#define VT__MPI_ERROR_CLASS                       41 
#define VT__MPI_ERROR_STRING                      42
#define VT__MPI_FINALIZE                          43
#define VT__MPI_GATHER                            44
#define VT__MPI_GATHERV                           45
#define VT__MPI_GET_COUNT                         46
#define VT__MPI_GET_ELEMENTS                      47
#define VT__MPI_GET_PROCESSOR_NAME                48
#define VT__MPI_GET_VERSION                       49
#define VT__MPI_GRAPH_CREATE                      50
#define VT__MPI_GRAPH_GET                         51
#define VT__MPI_GRAPH_MAP                         52
#define VT__MPI_GRAPH_NEIGHBORS                   53
#define VT__MPI_GRAPH_NEIGHBORS_COUNT             54 
#define VT__MPI_GRAPHDIMS_GET                     55
#define VT__MPI_GROUP_COMPARE                     56
#define VT__MPI_GROUP_DIFFERENCE                  57
#define VT__MPI_GROUP_EXCL                        58 
#define VT__MPI_GROUP_FREE                        59
#define VT__MPI_GROUP_INCL                        60
#define VT__MPI_GROUP_INTERSECTION                61
#define VT__MPI_GROUP_RANGE_EXCL                  62
#define VT__MPI_GROUP_RANGE_INCL                  63
#define VT__MPI_GROUP_RANK                        64 
#define VT__MPI_GROUP_SIZE                        65    
#define VT__MPI_GROUP_TRANSLATE_RANKS             66
#define VT__MPI_GROUP_UNION                       67
#define VT__MPI_IBSEND                            68
#define VT__MPI_INIT                              69
#define VT__MPI_INITIALIZED                       70
#define VT__MPI_INTERCOMM_CREATE                  71
#define VT__MPI_INTERCOMM_MERGE                   72
#define VT__MPI_IPROBE                            73  
#define VT__MPI_IRECV                             74
#define VT__MPI_IRSEND                            75
#define VT__MPI_ISEND                             76
#define VT__MPI_ISSEND                            77
#define VT__MPI_KEYVAL_CREATE                     78
#define VT__MPI_KEYVAL_FREE                       79
#define VT__MPI_OP_CREATE                         80
#define VT__MPI_OP_FREE                           81
#define VT__MPI_PACK                              82
#define VT__MPI_PACK_SIZE                         83
#define VT__MPI_PCONTROL                          84 
#define VT__MPI_PROBE                             85
#define VT__MPI_RECV                              86
#define VT__MPI_RECV_INIT                         87
#define VT__MPI_REDUCE                            88
#define VT__MPI_REDUCE_SCATTER                    89 
#define VT__MPI_REQUEST_FREE                      90 
#define VT__MPI_RSEND                             91
#define VT__MPI_RSEND_INIT                        92
#define VT__MPI_SCAN                              93
#define VT__MPI_SCATTER                           94
#define VT__MPI_SCATTERV                          95
#define VT__MPI_SEND                              96
#define VT__MPI_SEND_INIT                         97
#define VT__MPI_SENDRECV                          98
#define VT__MPI_SENDRECV_REPLACE                  99
#define VT__MPI_SSEND                            100 
#define VT__MPI_SSEND_INIT                       101
#define VT__MPI_START                            102
#define VT__MPI_STARTALL                         103
#define VT__MPI_TEST                             104
#define VT__MPI_TEST_CANCELLED                   105
#define VT__MPI_TESTALL                          106
#define VT__MPI_TESTANY                          107
#define VT__MPI_TESTSOME                         108   
#define VT__MPI_TOPO_TEST                        109                
#define VT__MPI_TYPE_COMMIT                      110 
#define VT__MPI_TYPE_CONTIGUOUS                  111
#define VT__MPI_TYPE_EXTENT                      112
#define VT__MPI_TYPE_FREE                        113 
#define VT__MPI_TYPE_HINDEXED                    114
#define VT__MPI_TYPE_HVECTOR                     115
#define VT__MPI_TYPE_INDEXED                     116
#define VT__MPI_TYPE_LB                          117
#define VT__MPI_TYPE_SIZE                        118
#define VT__MPI_TYPE_STRUCT                      119
#define VT__MPI_TYPE_UB                          120
#define VT__MPI_TYPE_VECTOR                      121 
#define VT__MPI_UNPACK                           122
#define VT__MPI_WAIT                             123
#define VT__MPI_WAITALL                          124   
#define VT__MPI_WAITANY                          125
#define VT__MPI_WAITSOME                         126
#define VT__MPI_WTICK                            127
#define VT__MPI_WTIME                            128
#define VT__MPI_ACCUMULATE                       129
#define VT__MPI_GET                              130
#define VT__MPI_PUT                              131
#define VT__MPI_WIN_COMPLETE                     132
#define VT__MPI_WIN_CREATE                       133
#define VT__MPI_WIN_FENCE                        134
#define VT__MPI_WIN_FREE                         135
#define VT__MPI_WIN_GET_GROUP                    136
#define VT__MPI_WIN_LOCK                         137
#define VT__MPI_WIN_POST                         138
#define VT__MPI_WIN_START                        139
#define VT__MPI_WIN_TEST                         140
#define VT__MPI_WIN_UNLOCK                       141
#define VT__MPI_WIN_WAIT                         142
#define VT__MPI_ALLTOALLW                        143
#define VT__MPI_EXSCAN                           144
#define VT__MPI_REGID_NUM                        145

extern int     vt_mpi_regid[VT__MPI_REGID_NUM];

EXTERN void    vt_mpi_register();

#endif









