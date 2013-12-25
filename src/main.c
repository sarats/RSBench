#include "multibench.h"

int main(int argc, char * argv[])
{
	// =====================================================================
	// Initialization & Command Line Read-In
	// =====================================================================

	int version = 0;
	int max_procs = omp_get_num_procs();
	int n_nuclides;
	int nthreads;
	int lookups;
	HM_size HM;
	double p_rrr;
	double p_UEG;
	
	srand(time(NULL));
	
	// Process CLI Fields
	Inputs input = read_CLI( argc, argv );
	
	// Set CLI variables
	nthreads =     input.nthreads;
	n_nuclides =   input.n_nuclides;
	lookups =      input.lookups;
	HM =      input.HM;

	// Set number of OpenMP Threads
	omp_set_num_threads(nthreads); 
	
	// =====================================================================
	// Calculate Estimate of Memory Usage
	// =====================================================================
	
	// =====================================================================
	// Print-out of Input Summary
	// =====================================================================
		
	logo(version);
	center_print("INPUT SUMMARY", 79);
	border_print();
	
	
		
	border_print();
	center_print("INITIALIZATION", 79);
	border_print();

	// =====================================================================
	// Prepare Resonance Paremeter Grids
	// =====================================================================
	
	// Allocate & fill energy grids
	
	// Sort grids by energy
	
	// Get material data
	
	// =====================================================================
	// Cross Section (XS) Parallel Lookup Simulation Begins
	// =====================================================================
	
	// OpenMP compiler directives - declaring variables as shared or private
	#pragma omp parallel default(none) \
	private(i, thread, p_energy, mat, seed) \
	shared( max_procs, n_isotopes, n_gridpoints, \
	energy_grid, nuclide_grids, lookups, nthreads, \
	mats, concs, num_nucs, mype, vhash) 
	{	
		#pragma omp critical
		{
			counter_init(&eventset, &num_papi_events);
		}
		#endif
		
		double macro_xs_vector[5];
		thread = omp_get_thread_num();
		seed   = (thread+1)*19+17;

		#pragma omp for schedule(dynamic)
		for( i = 0; i < lookups; i++ )
		{
			// Status text
			if( INFO && mype == 0 && thread == 0 && i % 1000 == 0 )
				printf("\rCalculating XS's... (%.0lf%% completed)",
						(i / ( (double)lookups / (double) nthreads )) / (double) nthreads * 100.0);
		
			// Randomly pick an energy and material for the particle
			p_energy = rn(&seed);
			mat      = pick_mat(&seed); 
			
			// debugging
			//printf("E = %lf mat = %d\n", p_energy, mat);
				
			// This returns the macro_xs_vector, but we're not going
			// to do anything with it in this program, so return value
			// is written over.
			calculate_macro_xs( p_energy, mat, n_isotopes,
			                    n_gridpoints, num_nucs, concs,
			                    energy_grid, nuclide_grids, mats,
                                macro_xs_vector );
		}

		#ifdef PAPI
		if( mype == 0 && thread == 0 )
		{
			printf("\n");
			border_print();
			center_print("PAPI COUNTER RESULTS", 79);
			border_print();
			printf("Count          \tSmybol      \tDescription\n");
		}
		{
		#pragma omp barrier
		}
		counter_stop(&eventset, num_papi_events);
		#endif

	}
	
	// =====================================================================
	// Print / Save Results and Exit
	// =====================================================================

	return 0;
}