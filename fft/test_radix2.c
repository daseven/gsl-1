#include <config.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include <gsl_complex.h>
#include <gsl_errno.h>
#include <gsl_dft_complex.h>
#include <gsl_fft_complex.h>
#include <gsl_fft_real.h>
#include <gsl_fft_halfcomplex.h>
#include <gsl_test.h>

#include "fft_signals.h"

#include "compare.h"

void check_complex_bitreverse_order (size_t n) ;
void check_complex_radix2 (size_t n) ;
void check_real_radix2 (size_t n) ;

/*  Usage: test_radix2 [n]
    Exercise the radix-2 fft routines for length n. By default n runs
    through small powers of two: 1, 2, 4, 8, ... , 1024.
    The exit status indicates success or failure. */

int
main (int argc, char *argv[])
{
  size_t i ;
  size_t n = 0;
  
  if (argc == 2) 
    n = strtol (argv[1], NULL, 0);

  if (n)
    {
      check_complex_bitreverse_order (n) ;
      check_complex_radix2 (n) ;
      check_real_radix2 (n) ;
    }
  else
    {
      for (i = 1 ; i <= 1024 ; i *= 2) 
	{
	  check_complex_bitreverse_order (i) ;
	  check_complex_radix2 (i) ;
	  check_real_radix2 (i) ;
	}
    }

  return gsl_test_summary ();

}


void check_complex_bitreverse_order (size_t n) 
{
  int status ;
  int result ;
  size_t logn, i ;
  gsl_complex *complex_data, *complex_tmp, *complex_reversed_data;

  complex_tmp = (gsl_complex *) malloc (n * sizeof (gsl_complex));
  complex_data = (gsl_complex *) malloc (n * sizeof (gsl_complex));
  complex_reversed_data = (gsl_complex *) malloc (n * sizeof (gsl_complex));
  
  for (i = 0; i < n; i++) 
    {
      complex_data[i].real = i + 1;
      complex_data[i].real = n + i + 1 ;
    }

  memcpy (complex_tmp, complex_data, n * sizeof(gsl_complex)) ;

  result = gsl_fft_binary_logn(n) ;
  
  if (result == -1) {
    abort() ;
  } else {
    logn = result ;
  }

  /* do a naive bit reversal as a baseline for testing the other routines */

  for (i = 0; i < n; i++) 
    {
      
      size_t i_tmp = i ;
      size_t j = 0 ;
      size_t bit ;

      for (bit = 0; bit < logn; bit++)
	{
	  j <<= 1;		/* reverse shift i into j */
	  j |= i_tmp & 1;
	  i_tmp >>= 1;
	}

      complex_reversed_data[j] = complex_data[i] ;
    }

  gsl_fft_complex_bitreverse_order (complex_data, n, logn);
  status = compare_complex_results ("naive bit reverse", 
				    complex_reversed_data,
				    "gsl_fft_complex_bitreverse_order", 
				    complex_data,
				    n, 1e6);

  gsl_test (status, "gsl_fft_complex_bitreverse_order, n = %d", n);

#ifdef UNUSED
  memcpy (complex_data, complex_tmp, n * sizeof(gsl_complex)) ;
  gsl_fft_complex_goldrader_bitreverse_order (complex_data, n);
  status = compare_complex_results ("naive bit reverse", 
				    complex_reversed_data,
				    "gsl_fft_complex_goldrader_bitreverse_order", 
				    complex_data,
				    n, 1e6);
  gsl_test (status, "gsl_fft_complex_goldrader_bitreverse_order, n = %d", n);

  memcpy (complex_data, complex_tmp, n * sizeof(gsl_complex)) ;
  gsl_fft_complex_rodriguez_bitreverse_order (complex_data, n, logn);
  status = compare_complex_results ("naive bit reverse", 
				    complex_reversed_data,
				    "gsl_fft_complex_rodriguez_bit_reverse", 
				    complex_data, 
				    n, 1e6);
  gsl_test (status, "gsl_fft_complex_rodriguez_bitreverse_order, n = %d", n);
#endif

  free (complex_data) ;
  free (complex_tmp) ;
}

void check_complex_radix2 (size_t n) 
{
  int status ;

  gsl_complex *complex_data, *complex_tmp;
  gsl_complex *fft_complex_data, *fft_complex_tmp;

  complex_data = (gsl_complex *) malloc (n * sizeof (gsl_complex));
  complex_tmp = (gsl_complex *) malloc (n * sizeof (gsl_complex));
  fft_complex_data = (gsl_complex *) malloc (n * sizeof (gsl_complex));
  fft_complex_tmp = (gsl_complex *) malloc (n * sizeof (gsl_complex));
  
  gsl_fft_signal_complex_noise (n, complex_data, fft_complex_data);
  memcpy (complex_tmp, complex_data, n * sizeof (gsl_complex));
  gsl_fft_complex_radix2_dif_forward (complex_data, n);
  status = compare_complex_results ("dft", fft_complex_data,
				    "fft of noise", complex_data,
				    n, 1e6);
  gsl_test (status, "gsl_fft_complex_radix2_dif with signal_noise, n = %d", n);


  gsl_fft_signal_complex_noise (n, complex_data, fft_complex_data);
  memcpy (complex_tmp, complex_data, n * sizeof (gsl_complex));
  gsl_fft_complex_radix2_forward (complex_data, n);
  status = compare_complex_results ("dft", fft_complex_data,
				    "fft of noise", complex_data,
				    n, 1e6);
  gsl_test (status, "gsl_fft_complex_radix2_forward with signal_noise, n = %d", n);
  
  /* compute the inverse fft */
  status = gsl_fft_complex_radix2_inverse (complex_data, n);
  status = compare_complex_results ("orig", complex_tmp,
				    "fft_real", complex_data,
				    n, 1e6);
  gsl_test (status, "gsl_fft_complex_radix2_inverse with signal_noise, n = %d", n);

  free (complex_data) ;
  free (complex_tmp) ;
  free (fft_complex_data) ;
  free (fft_complex_tmp) ;
  
}

void check_real_radix2 (size_t n) 
{

  size_t i ;
  int status ;

  double *real_data, *real_tmp;
  double *fft_real_data, *fft_real_tmp;

  gsl_complex *complex_data, *complex_tmp;
  gsl_complex *fft_complex_data, *fft_complex_tmp;

  char length[256];

  real_data = (double *) malloc (n * sizeof (double));
  real_tmp = (double *) malloc (n * sizeof (double));

  fft_real_data = (double *) malloc (n * sizeof (double));
  fft_real_tmp = (double *) malloc (n * sizeof (double));

  complex_data = (gsl_complex *) malloc (n * sizeof (gsl_complex));
  complex_tmp = (gsl_complex *) malloc (n * sizeof (gsl_complex));

  fft_complex_data = (gsl_complex *) malloc (n * sizeof (gsl_complex));
  fft_complex_tmp = (gsl_complex *) malloc (n * sizeof (gsl_complex));
  
  sprintf (length, "n = %d", n);
  
  gsl_fft_signal_real_noise (n, complex_data, fft_complex_data);
  memcpy (complex_tmp, complex_data, n * sizeof (gsl_complex));

  for (i = 0; i < n; i++)
    {
      real_data[i] = complex_data[i].real;
    }

  gsl_fft_real_radix2 (real_data, n);

  complex_data[0].real = real_data[0] ;
  complex_data[0].imag = 0.0 ;
  for (i = 1 ; i < n/2 ; i++) {
    complex_data[i].real = real_data[i] ;
    complex_data[i].imag = real_data[n-i] ;
    complex_data[n-i].real = real_data[i] ;
    complex_data[n-i].imag = -real_data[n-i] ;
  }
  complex_data[n/2].real = real_data[n/2] ;
  complex_data[n/2].imag = 0.0 ;

  status = compare_complex_results ("dft", fft_complex_data,
				    "fft of noise", complex_data,
				    n, 1e6);
  gsl_test (status, "gsl_fft_real_radix2 with signal_noise, n = %d", n);

  status = gsl_fft_halfcomplex_radix2 (real_data, n) ;

   for (i = 0; i < n ; i++) { 
     real_data[i] /= n ; 
   } 

  gsl_fft_real_unpack (real_data, complex_data, n) ;

  status = compare_complex_results ("orig", complex_tmp,
				    "fft inverse", complex_data,
				    n, 1e6);
  gsl_test (status, "gsl_fft_halfcomplex_radix2 with signal_noise, n = %d", n);

  free (complex_data) ;
  free (complex_tmp) ;
  free (fft_complex_data) ;
  free (fft_complex_tmp) ;
  
}
