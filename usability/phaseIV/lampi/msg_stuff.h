      integer maxtasks
      parameter (maxtasks=256)
      integer numtasks, tids(maxtasks), myid, mytid
      common /msg_int/ numtasks,tids,myid,mytid
      character*32 group_name
      common /msg_chr/ group_name
      integer intsize,encoding
      common /pvm_flags/ intsize,encoding

