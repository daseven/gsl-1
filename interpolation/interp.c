/* Author:  G. Jungman
 * RCS:     $Id$
 */
#include <gsl_errno.h>
#include "gsl_interp.h"


int
gsl_interp_eval_impl(const gsl_interp_obj * obj,
                     const double xa[], const double ya[], double x,
                     gsl_interp_accel * a, double * y
                     )
{
  return obj->eval_impl(obj, xa, ya, x, a, y);
}


int
gsl_interp_eval_e(const gsl_interp_obj * obj,
                  const double xa[], const double ya[], double x,
                  gsl_interp_accel * a, double * y
                  )
{
  int status = obj->eval_impl(obj, xa, ya, x, a, y);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_interp_eval_e", status);
  }
  return status;
}

double
gsl_interp_eval(const gsl_interp_obj * obj,
                const double xa[], const double ya[], double x,
                gsl_interp_accel * a
                )
{
  double y;
  int status = obj->eval_impl(obj, xa, ya, x, a, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_interp_eval_e", status);
  }
  return y;
}

int
gsl_interp_eval_noaccel_impl(const gsl_interp_obj * obj,
                             const double xa[], const double ya[], double x,
                             double * y
                             )
{
  return obj->eval_na_impl(obj, xa, ya, x, y);
}


int
gsl_interp_eval_noaccel_e(const gsl_interp_obj * obj,
                          const double xa[], const double ya[], double x,
                          double * y
                          )
{
  int status = obj->eval_na_impl(obj, xa, ya, x, y);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_interp_eval_noaccel_e", status);
  }
  return status;
}

double
gsl_interp_eval_noaccel(const gsl_interp_obj * obj,
                        const double xa[], const double ya[], double x
                        )
{
  double y;
  int status = obj->eval_na_impl(obj, xa, ya, x, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_interp_eval_noaccel_e", status);
  }
  return y;
}


void
gsl_interp_obj_free(gsl_interp_obj * obj)
{
  if(obj != 0)
    obj->free(obj);
}
