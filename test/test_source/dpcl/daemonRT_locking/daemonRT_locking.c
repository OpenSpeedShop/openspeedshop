
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__ia64__)
#include <asm/bitops.h>
#include <asm/intrinsics.h>
#elif defined(__x86_64__)
#define CONFIG_SMP
#include <asm-x86_64/bitops.h>
#include <asm-x86_64/system.h>
#endif


typedef int* atomic_p;
typedef enum { false = 0, true = 1 } boolean_t;

boolean_t _check_lock(atomic_p word_addr, int old_val, int new_val)
{
#if defined(__i386__)

    asm ("\
        movl 16(%ebp), %ecx;\
        movl 12(%ebp), %eax;\
        movl  8(%ebp), %edx;\
        lock; cmpxchgl %ecx, (%edx);\
        movl $0, %eax;\
        setne %al;\
    ");

#elif defined(__ia64__)

    int prev_val = cmpxchg_acq(word_addr, old_val, new_val);
    return prev_val != old_val;

#elif defined(__x86_64__)
    /* locks are performed in the _cmpxchg subroutine when
       CONFIG_SMP is defined - as it is above */

    int prev_val = cmpxchg(word_addr, old_val, new_val);
    return prev_val != old_val;


#else
#error "Linux on this platform isn't supported yet!"
#endif

}

int _safe_fetch(atomic_p word_addr)
{
#if defined(__i386__)

    asm ("\
        movl 8(%ebp), %ecx;\
        movl (%ecx), %eax;\
    ");

#elif defined(__ia64__)

    /*
     * A volatile is used here to force the GCC compiler to use a load that
     * provides "acquire" rather than "unordered" memory ordering semantics. See
     * the Intel Itanium Architecture Software Development Manual, Volume 1
     * ("Application Architecture"), Part I ("Application Architecture Guide"),
     * Section 4.4.7 ("Memory Access Ordering") for additional details on memory
     * ordering semantics.
     */
    volatile int* addr = (volatile int*)word_addr;
    return *addr;
#elif defined(__x86_64__)


    /*
     * A volatile is used here to force the GCC compiler to use a load that
     * provides "acquire" rather than "unordered" memory ordering semantics. See
     * the Intel Itanium Architecture Software Development Manual, Volume 1
     * ("Application Architecture"), Part I ("Application Architecture Guide"),
     * Section 4.4.7 ("Memory Access Ordering") for additional details on memory
     * ordering semantics.
     */
    volatile int* addr = (volatile int*)word_addr;
    return *addr;

#else
#error "Linux on this platform isn't supported yet!"
#endif

}

void _clear_lock(atomic_p word_addr, int val)
{
#if defined(__i386__)

    asm ("\
        movl 12(%ebp), %eax;                    \
        movl  8(%ebp), %ecx;\
        movl %eax, (%ecx);\
    ");

#elif defined(__ia64__)

    /*
     * A volatile is used here to force the GCC compiler to use a store that
     * provides "release" rather than "unordered" memory ordering semantics. See
     * the Intel Itanium Architecture Software Development Manual, Volume 1
     * ("Application Architecture"), Part I ("Application Architecture Guide"),
     * Section 4.4.7 ("Memory Access Ordering") for additional details on memory
     * ordering semantics.
     */
    volatile int* addr = (volatile int*)word_addr;
    *addr = val;

#elif defined(__x86_64__)

    /*
     * A volatile is used here to force the GCC compiler to use a store that
     * provides "release" rather than "unordered" memory ordering semantics. See
     * the Intel Itanium Architecture Software Development Manual, Volume 1
     * ("Application Architecture"), Part I ("Application Architecture Guide"),
     * Section 4.4.7 ("Memory Access Ordering") for additional details on memory
     * ordering semantics.
     */
    volatile int* addr = (volatile int*)word_addr;
    *addr = val;

#else
#error "Linux on this platform isn't supported yet!"
#endif

}



typedef struct { int lock; int value; } object_t;

void LockObject(object_t* object)
{
  int tries = 0;
  pthread_t my_tid = pthread_self();
  
  while(_check_lock((atomic_p)&object->lock, 0, my_tid) != false) {
    pthread_t locking_tid = _safe_fetch((atomic_p)&object->lock);    
    printf("[TID %d]: Already locked by TID %d (Retry #%d)\n",
	   my_tid, locking_tid, ++tries);   
  }
}

void UnlockObject(object_t* object)
{
  _clear_lock((atomic_p)&object->lock, 0);
}



object_t TheCounter = { 0, 0 };

void* f(void* arg)
{
  int i, j, num_iterations = *((int*)arg);
  
  for(i = 0; i < num_iterations; ++i) {
    LockObject(&TheCounter);
    TheCounter.value++;
    UnlockObject(&TheCounter);
    for(j = 0; j < 10000; ++j);
  }  
}

int main(int argc, char* argv[])
{
  int i, num_threads = 0, num_iterations = 0;
  pthread_t* threads = NULL;
  
  if(argc != 3) {
    printf("Usage: %s <num_threads> <num_iterations>\n", argv[0]);
    exit(1);
  }
  
  num_threads = atoi(argv[1]);
  num_iterations = atoi(argv[2]);

  threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));

  for(i = 0; i < num_threads; ++i)
    pthread_create(&(threads[i]), NULL, f, (void*)&num_iterations);  

  for(i = 0; i < num_threads; ++i)
    pthread_join(threads[i], NULL);
  
  free(threads);

  printf("TheCounter.value = %d\n", TheCounter.value);
}
