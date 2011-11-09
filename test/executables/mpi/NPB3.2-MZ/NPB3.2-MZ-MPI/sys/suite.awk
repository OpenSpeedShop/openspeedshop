BEGIN { SMAKE = "make" } {
  if ($1 !~ /^#/ &&  NF > 2) {
    printf "cd `echo %s|tr '[a-z]' '[A-Z]'`; %s clean;", $1, SMAKE;
    printf "%s CLASS=%s NPROCS=%s; cd ..\n", SMAKE, $2, $3;
  }
}
