/* Modification 1: exclude even integers */
/* Modification 2: remove broadcast */

#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)

int main(int argc, char *argv[])
{
	unsigned long int    count;        /* Local prime count */
	double elapsed_time; /* Parallel execution time */
	unsigned long int    first;        /* Index of first multiple */
	unsigned long int    global_count=0; /* Global prime count */
	unsigned long long int    high_value;   /* Highest value on this proc */
	unsigned long int    i;
	int    id;           /* Process ID number */
	unsigned long int    index;        /* Index of current prime */
	unsigned long long int    low_value;    /* Lowest value on this proc */
	char  *marked;       /* Portion of 2,...,'n' */
	unsigned long long int    n;            /* Sieving from 2, ..., 'n' */
	int    p;            /* Number of processes*/
	unsigned long int    proc0_size;   /* Size of proc 0's subarray */
	unsigned long int    prime;        /* Current prime */
	unsigned long int    size;         /* Elements in 'marked' */
	unsigned long long int    m;
	int    loc;
	char  *primes;
	unsigned long int    primes_size;

	MPI_Init(&argc, &argv);

	/* Start the timer */

	MPI_Comm_rank (MPI_COMM_WORLD, &id);
	MPI_Comm_size (MPI_COMM_WORLD, &p);
	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time = -MPI_Wtime();

	if (argc != 2) {
		if (!id) printf("Command line: %s <n>\n", argv[0]);
		MPI_Finalize();
		exit(1);
	}

	n = atoll(argv[1]);
	m = (n-3)/2 + 1;

	/* Figure out this process's share of the array, as
	well as the integers represented by the first and
	last array elements */

	low_value = 2 * BLOCK_LOW(id, p, m) + 3;
	high_value = 2 * BLOCK_HIGH(id, p, m) + 3;
	size = BLOCK_SIZE(id, p, m);

	/* Allocate this process' share of the array */

	marked = (char *) malloc(size);

	if (marked == NULL) {
		printf("Cannot allocate enough memory\n");
		MPI_Finalize();
		exit(1);
	}
	for (i=0; i<size; i++) marked[i] = 0;

	primes_size = (sqrt(n) - 3)/2 + 1;
	primes = (char *) malloc(primes_size);
	if (primes == NULL) {
		printf("Cannot allocate enough memory\n");
		free(marked);
		MPI_Finalize();
		exit(1);
	}
	for (i=0; i<primes_size; i++) primes[i] = 0;

	index = 0;
	prime = 3;
	do {
		for (i = (prime*prime-3)/2; i < primes_size; i += prime)
			primes[i] = 1;
		while (primes[++index]);
		prime = 2*index + 3;
	} while (prime*prime <= sqrt(n));

	index = 0;
	prime = 3;
	do {
		if (prime*prime > low_value)
			first = (prime*prime-3)/2 - (low_value-3)/2;
		else {
			loc = low_value % prime;
			if (!loc) first = 0;
			else {
				first = prime - loc;
				if (!((low_value+first)%2))
					first = (first+prime)/2;
				else first /= 2;
			}
		}
		for (i=first; i <size; i+=prime)
			marked[i] = 1;
		while (primes[++index]);
		prime = 2*index + 3;
	} while (prime*prime <= n);
	count = 0;
	for (i=0; i<size; i++)
		if (!marked[i]) count++;
	MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	/* Stop the timer */

	elapsed_time += MPI_Wtime();

	/* Print the results */

if (!id) {
	printf ("There are %ld primes less than or equal to %lld\n",
		global_count+1, n);
	printf ("SIEVE (%d) %10.6f\n", p, elapsed_time);
}
MPI_Finalize ();
return 0;
}