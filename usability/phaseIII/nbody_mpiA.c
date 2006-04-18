/**
 ** nbody-mpi.c
 **
 ** Calculates the solution to a 3D n-body problem in parallel using MPI.
 **
 ** The number of particles to simulate and the number of iterations, are given
 ** as constants at the top of this file. Each particle is assigned a mass of
 ** 1.0 and a random position by the "master" processor. The master processor
 ** then evenly divides particles amongst the available processors. During each
 ** iteration, processors calculate the new positions of particles assigned to
 ** them. Final particle positions are relayed back to the master processor once
 ** all iterations have been completed.
 **
 ** Forces are calculated within each iteration with a ring pipeline where the
 ** number of pipeline stages is equal to the number of available processors.
 ** During a given stage, each processor is calculating the force exerted by one
 ** set of particles upon the particles on that processor. Consider an example
 ** with 2500 particles and 4 processors. Each processor (0, 1, 2, 3) is given
 ** a set (A, B, C, D) of 625 particles. The force calculations done during the
 ** pipeline stages would look like:
 **
 **           +-------------------------------------------------------------+
 **           |                                                             |
 **           +- Processor 0 <- Processor 1 <- Processor 2 <- Processor 3 <-+
 **
 **  Stage 0      A on A          B on B         C on C         D on D
 **  Stage 1      B on A          C on B         D on C         A on D
 **  Stage 2      C on A          D on B         A on C         B on D
 **  Stage 3      D on A          A on B         B on C         C on D
 **
 ** A processor receives one message from the processor on its "right" and sends
 ** one message to the processor on its "left" during each stage. All processors
 ** know the total force acting on each of their particles after the pipeline
 ** stages are completed. New particle positions can then be calculated on each
 ** processor independently and in parallel. Variable time steps are used to
 ** keep things from getting out of control. The new time step is synchronized
 ** across all processors before beginning the next iteration.
 **
 ** This application was designed for testing MPI performance tools. Thus it was
 ** written to contain both collective and point-to-point operations. And in the
 ** grand tradition of physics simulation codes everywhere, it is written as a
 ** single, giant, main() function. It does NO error checking and produces NO
 ** output. It just calculates!
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>



static const int MasterProcessor = 0;      /* Number of the master processor  */

static const int NumParticles = 2500;      /* Number of particles             */
static const int NumIterations = 50;       /* Number of iterations            */
static const double Infinity = 1.0E96;     /* Infinitely large value          */
static const double MinTimeStep = 1.0E-6;  /* Minimum allowed time step       */



typedef struct {
    double mass;
    double x, y, z;
} particle_t;



int
main(int argc, char* argv[])
{
    MPI_Datatype type;      /* MPI data type for communicating particle data  */
    int num_processors;     /* Number of processors being used                */
    int processor;          /* My processor number                            */
    int* num;               /* Number of particles on each processor          */
    int* offset;            /* Offset to start of each processor's particles  */
    int buffer_size;        /* Number of particles in pipeline data buffers   */
    particle_t* local;      /* Array containing our local particles           */

        
    /* Initialize MPI */
    MPI_Init(&argc, &argv);

        
    /* Create the MPI data type for communicating particle data */
    MPI_Type_contiguous(4, MPI_DOUBLE, &type);
    MPI_Type_commit(&type);
    
    
    /* Determine the number of procesors being used and our processor number */
    MPI_Comm_size(MPI_COMM_WORLD, &num_processors);
    MPI_Comm_rank(MPI_COMM_WORLD, &processor);


    /* Determine how the particles are allocated to the processors */
    {
	int p;
	
	num = (int*)malloc(num_processors * sizeof(int));
	offset = (int*)malloc(num_processors * sizeof(int));
	
	for(p = 0; p < num_processors; p++)
	    num[p] = (NumParticles / num_processors) +
		((p < (NumParticles % num_processors)) ? 1 : 0);	
	buffer_size = num[0];
	
	offset[0] = 0;
	for(p = 1; p < num_processors; p++)
	    offset[p] = offset[p - 1] + num[p - 1];
    }
    

    /* Distribute the initial particle state */
    {
	int i;
	particle_t* particles = NULL;
	
	if(processor == MasterProcessor) {
	    particles = (particle_t*)malloc(NumParticles * sizeof(particle_t));
	    
	    /*
	      CODE FOR READING INITIAL PARTICLE STATE DATA FROM
	      A FILE COULD BE PLACED HERE INSTEAD OF RANDOMIZATION
	    */

	    /* Randomize the particles */
	    for(i = 0; i < NumParticles; i++) {
		particles[i].mass = 1.0;
		particles[i].x = drand48();
		particles[i].y = drand48();
		particles[i].z = drand48();
	    }
	    
	}
	
	local = (particle_t*)malloc(num[processor] * sizeof(particle_t));
	
	MPI_Scatterv(particles, num, offset, type,
		     local, num[processor], type,
		     MasterProcessor, MPI_COMM_WORLD);
	
	if(processor == MasterProcessor)
	    free(particles);
    }

printf("Processor: %d Ready to rumble!\n", processor );

sleep(30);

printf("Processor: %d rumbling!!\n", processor );
    
    
    /* Actual Simulation */
    {
	int iteration, stage, i, j;
	particle_t* buf_send;
	particle_t* buf_recv;
	double* tfx;
	double* tfy;
	double* tfz;
	double* ox;
	double* oy;
	double* oz;
	
	buf_send = (particle_t*)malloc(buffer_size * sizeof(particle_t));
	buf_recv = (particle_t*)malloc(buffer_size * sizeof(particle_t));	

	tfx = (double*)malloc(num[processor] * sizeof(double));
	tfy = (double*)malloc(num[processor] * sizeof(double));
	tfz = (double*)malloc(num[processor] * sizeof(double));
	ox = (double*)malloc(num[processor] * sizeof(double));
	oy = (double*)malloc(num[processor] * sizeof(double));
	oz = (double*)malloc(num[processor] * sizeof(double));
	
	/* Set the "old" position for each particle to its current position */
	for(i = 0; i < num[processor]; i++) {
	    ox[i] = local[i].x;
	    oy[i] = local[i].y;
	    oz[i] = local[i].z;
	}
	
	/* Time steps */
	for(iteration = 0; iteration < NumIterations; iteration++) {	    
	    
	    double f_max = -Infinity;	    

	    /* Show current iteration number */
	    if(processor == 1)
        {
		fprintf(stdout, "Iteration %d of %d...\n", 
			iteration + 1, NumIterations);
		fflush(stdout);
	    }

	    /* Zero the total force for each particle */
	    for(i = 0; i < num[processor]; i++) {
		tfx[i] = 0.0;
		tfy[i] = 0.0;
		tfz[i] = 0.0;
	    }
	    
	    /* Force computation pipeline */
	    for(stage = 0; stage < num_processors; stage++) {
		
		MPI_Request request[2];
		MPI_Status status[2];
		
		/* Prime the pipeline with our local data for stage zero */
		if(stage == 0)
		    memcpy(buf_send, local,
			   num[processor] * sizeof(particle_t));
		
		/* Issue the send/receive pair for this pipeline stage */
		if(stage < (num_processors - 1)) {
		    MPI_Isend(buf_send, buffer_size, type,
			      (processor - 1 + num_processors) % num_processors,
			      0, MPI_COMM_WORLD, &request[0]);
		    MPI_Irecv(buf_recv, buffer_size, type,
			      (processor + 1 + num_processors) % num_processors,
			      0, MPI_COMM_WORLD, &request[1]);
		}
		
		/* Compute forces */ 		
		for(i = 0; i < num[processor]; i++) {
		    
		    double r_min = +Infinity;
		    double fx = 0.0;
		    double fy = 0.0;			
		    double fz = 0.0;
		    double f = 0.0;
		    
		    for(j = 0; 
			j < num[(processor + stage) % num_processors];
			j++) {
			
			double rx = local[i].x - buf_send[j].x;
			double ry = local[i].y - buf_send[j].y;
			double rz = local[i].z - buf_send[j].z;
			double r = (rx * rx) + (ry * ry) + (rz * rz);
			
			if(r > 0.0) {
			    if(r < r_min)
				r_min = r;
			    fx -= buf_send[j].mass * (rx / r);
			    fy -= buf_send[j].mass * (ry / r);
			    fz -= buf_send[j].mass * (rz / r);
			}
			
		    }
		    
		    tfx[i] += fx;
		    tfy[i] += fy;
		    tfz[i] += fz;

		    /* Rough estimate of 1/m|df/dx| */
		    f = sqrt((fx * fx) + (fy * fy) + (fz * fz)) / r_min;
		    if(f > f_max)
			f_max = f;
		    
		}
		
		/* Complete the send/receive pair for this pipeline stage */
		if(stage < (num_processors - 1)) {
		    MPI_Waitall(2, request, status);
		    memcpy(buf_send, buf_recv,
			   buffer_size * sizeof(particle_t));
		}
		
	    }
	    
	    /*
	     * Compute new positions using a simple leapfrog time integration.
	     * Use a variable step version to simplify time-step control.
	     *
	     * Integration is (a0 * x^+) + (a1 * x) + (a2 * x^-) = f / m
	     *
	     * Stability criteria is roughly 2.0 / sqrt(1/m|df/dx|) >= dt
	     */
	    {
		static double dt_old = 0.001;
		static double dt_now = 0.001;
		double dt_est;
		double dt_new;
		
		double a0 = +2.0 / (dt_now * (dt_old + dt_now));
		double a1 = -2.0 / (dt_old * dt_now);
		double a2 = +2.0 / (dt_old * (dt_old + dt_now));
		
		for(i = 0; i < num[processor]; i++) {
		    
		    double x = local[i].x;
		    double y = local[i].y;
		    double z = local[i].z;
		    
		    local[i].x = (tfx[i] - (a1 * x) - (a2 * ox[i])) / a0;
		    local[i].y = (tfy[i] - (a1 * y) - (a2 * oy[i])) / a0;
		    local[i].z = (tfz[i] - (a1 * z) - (a2 * oz[i])) / a0;
		    
		    ox[i] = x;
		    oy[i] = y;
		    oz[i] = z;
		    
		}
		
		dt_est = 1.0 / sqrt(f_max);
		if(dt_est < MinTimeStep)
		    dt_est = MinTimeStep;
		
		MPI_Allreduce(&dt_est, &dt_new, 1, MPI_DOUBLE,
			      MPI_MIN, MPI_COMM_WORLD);
		
		if(dt_new < dt_now) {
		    dt_old = dt_now;
		    dt_now = dt_new;
		}
		else if(dt_new > (4.0 * dt_now)) {
		    dt_old = dt_now;
		    dt_now *= 2.0;
		}
		
	    }
	    
	}
	
	free(buf_send);
	free(buf_recv);
	free(tfx);
	free(tfy);
	free(tfz);
	free(ox);
	free(oy);
	free(oz);
	
    }
    

    /* Gather the final particle state */
    {
	particle_t* particles = NULL;
	
	if(processor == MasterProcessor)
	    particles = (particle_t*)malloc(NumParticles * sizeof(particle_t));
	
	MPI_Gatherv(local, num[processor], type,
		    particles, num, offset, type,
		    MasterProcessor, MPI_COMM_WORLD);
	
	free(local);
	
	if(processor == MasterProcessor) {

	    /*
	      CODE FOR WRITING FINAL PARTICLE STATE
	      DATA TO A FILE COULD BE PLACED HERE
	    */
	    
	    free(particles);	
	}
    }
    
    
    /* Free the particle distribution arrays */
    free(num);
    free(offset);
    
    
    /* Finalize MPI */
    MPI_Finalize();
    
    
    /* All Done */
    return 0;
}
