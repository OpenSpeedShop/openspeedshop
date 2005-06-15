static
long int seedi = 1;

void my_srand(int i){
  seedi = i;
}

long int my_rand()
/* See "Random Number Generators: Good Ones Are Hard To Find", */
/*     Park & Miller, CACM 31#10 October 1988 pages 1192-1201. */
/***********************************************************/
/* THIS IMPLEMENTATION REQUIRES AT LEAST 32 BIT INTEGERS ! */
/***********************************************************/
#define _A_MULTIPLIER  16807L
#define _M_MODULUS     2147483647L /* (2**31)-1 */
#define _Q_QUOTIENT    127773L     /* 2147483647 / 16807 */
#define _R_REMAINDER   2836L       /* 2147483647 % 16807 */
{
	long lo;
	long hi;
	long test;

	hi = seedi / _Q_QUOTIENT;
	lo = seedi % _Q_QUOTIENT;
	test = _A_MULTIPLIER * lo - _R_REMAINDER * hi;
	if (test > 0) {
		seedi = test;
	} else {
		seedi = test + _M_MODULUS;
	}
	//	return ( (float) seedi / _M_MODULUS);
#if 0
	return seedi>>16;
#else
	return (seedi>>8) & 0x7fff;
#endif
}

