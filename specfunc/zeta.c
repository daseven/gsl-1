/* Author:  G. Jungman
 * RCS:     $Id$
 */
#include <gsl_math.h>
#include <gsl_errno.h>
#include "gsl_sf_chebyshev.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_zeta.h"

#define LogTwoPi_  1.8378770664093454835606594728111235279723


/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/

/* chebyshev fit for (s(x)-1)Zeta[s(x)],  s(x)= (x+1)/2,  -1 <= x <= 1 */
static double zeta_xlt1_data[14] = {
  1.48018677156931561235192914649,
  0.250120625398894264719999381666,
  0.0099113750213536077424376146719,
 -0.000120847596566764103298330914680,
 -4.7585866367662556504652535281e-6,
  2.22299466944663918555614413609e-7,
 -2.22374964980302571213090565816e-9,
 -1.01732265132290283194207990278e-10,
  4.3756643450424558284466248449e-12,
 -6.2229632593100551465504090814e-14,
 -6.6116201003272207115277520305e-16,
  4.9477279533373912324518463830e-17,
 -1.04298190934561897196600035218e-18,
  6.9925216166580021051464412040e-21,
};
static struct gsl_sf_ChebSeries zeta_xlt1_cs = {
  zeta_xlt1_data,
  13,
  -1, 1,
  (double *)0,
  (double *)0
};

/* chebyshev fit for (s(x)-1)Zeta[s(x)],  s(x)= (19x+21)/2,  -1 <= x <= 1 */
static double zeta_xgt1_data[30] = {
  19.3918515726724119415911269006,
   9.1525329692510756181581271500,
   0.242789765886737998536527015482,
  -0.133900068826202733831664132878,
   0.057782706406502859557841020184,
  -0.0187625983754002298566409700423,
   0.0039403014258320354840823803150,
  -0.000058150827315812796359888250536,
  -0.00037561489072148207045945492833,
   0.000189253054810921434909299963724,
  -0.000054903219969551349611509035920,
   8.7086484008939038610413331863e-6,
   6.4609477924811889068410083425e-7,
  -9.6749773915059089205835337136e-7,
   3.6585400766767257736982342461e-7,
  -8.4592516427275164351876072573e-8,
   9.9956786144497936572288988883e-9,
   1.42600364209511181124571448418e-9,
  -1.17619688233828791953803209478e-9,
   3.7114575899785204664648987295e-10,
  -7.4756855194210961661210215325e-11,
   7.8536934209183700456512982968e-12,
   9.9827182259685539619810406271e-13,
  -7.5276687030192221587850302453e-13,
   2.19550263939642799889178786544e-13,
  -4.1934859852834647427576319246e-14,
   4.6341149635933550715779074274e-15,
   2.37424885090483401068303094027e-16,
  -2.72765163881247861193238243915e-16,
   7.8473570134636044722154797225e-17
};
static struct gsl_sf_ChebSeries zeta_xgt1_cs = {
  zeta_xgt1_data,
  29,
  -1, 1,
  (double *)0,
  (double *)0
};


/* assumes s > 0,  product representation */
static inline double riemann_zeta_prod(double s)
{
  double f2 = 1. - pow(2.,-s);
  double f3 = 1. - pow(3.,-s);
  double f5 = 1. - pow(5.,-s);
  double f7 = 1. - pow(7.,-s);
  return 1./(f2*f3*f5*f7);
}

/* assumes s >= 0 and s != 1.0 */
static inline double riemann_zeta_sgt0(double s)
{
  if(s < 1.0) {
    return gsl_sf_cheb_eval(2.*s - 1, &zeta_xlt1_cs) / (s - 1.);
  }
  else if(s <= 20.0) {
    double x = (2.*s - 21.)/19.;
    return gsl_sf_cheb_eval(x, &zeta_xgt1_cs) / (s - 1.);
  }
  else {
    return riemann_zeta_prod(s);
  }
}

/* zeta(n) */
#define ZETA_POS_TABLE_NMAX   100
static double zeta_pos_int_table[ZETA_POS_TABLE_NMAX+1] = {
 -0.50000000000000000000000000000,       /* zeta(0) */
  0.0 /* FIXME: DirectedInfinity() */,   /* zeta(1) */
  1.64493406684822643647241516665,       /* ...     */
  1.20205690315959428539973816151,
  1.08232323371113819151600369654,
  1.03692775514336992633136548646,
  1.01734306198444913971451792979,
  1.00834927738192282683979754985,
  1.00407735619794433937868523851,
  1.00200839282608221441785276923,
  1.00099457512781808533714595890,
  1.00049418860411946455870228253,
  1.00024608655330804829863799805,
  1.00012271334757848914675183653,
  1.00006124813505870482925854511,
  1.00003058823630702049355172851,
  1.00001528225940865187173257149,
  1.00000763719763789976227360029,
  1.00000381729326499983985646164,
  1.00000190821271655393892565696,
  1.00000095396203387279611315204,
  1.00000047693298678780646311672,
  1.00000023845050272773299000365,
  1.00000011921992596531107306779,
  1.00000005960818905125947961244,
  1.00000002980350351465228018606,
  1.00000001490155482836504123466,
  1.00000000745071178983542949198,
  1.00000000372533402478845705482,
  1.00000000186265972351304900640,
  1.00000000093132743241966818287,
  1.00000000046566290650337840730,
  1.00000000023283118336765054920,
  1.00000000011641550172700519776,
  1.00000000005820772087902700889,
  1.00000000002910385044497099687,
  1.00000000001455192189104198424,
  1.00000000000727595983505748101,
  1.00000000000363797954737865119,
  1.00000000000181898965030706595,
  1.00000000000090949478402638893,
  1.00000000000045474737830421540,
  1.00000000000022737368458246525,
  1.00000000000011368684076802278,
  1.00000000000005684341987627586,
  1.00000000000002842170976889302,
  1.00000000000001421085482803161,
  1.00000000000000710542739521085,
  1.00000000000000355271369133711,
  1.00000000000000177635684357912,
  1.00000000000000088817842109308,
  1.00000000000000044408921031438,
  1.00000000000000022204460507980,
  1.00000000000000011102230251411,
  1.00000000000000005551115124845,
  1.00000000000000002775557562136,
  1.00000000000000001387778780973,
  1.00000000000000000693889390454,
  1.00000000000000000346944695217,
  1.00000000000000000173472347605,
  1.00000000000000000086736173801,
  1.00000000000000000043368086900,
  1.00000000000000000021684043450,
  1.00000000000000000010842021725,
  1.00000000000000000005421010862,
  1.00000000000000000002710505431,
  1.00000000000000000001355252716,
  1.00000000000000000000677626358,
  1.00000000000000000000338813179,
  1.00000000000000000000169406589,
  1.00000000000000000000084703295,
  1.00000000000000000000042351647,
  1.00000000000000000000021175824,
  1.00000000000000000000010587912,
  1.00000000000000000000005293956,
  1.00000000000000000000002646978,
  1.00000000000000000000001323489,
  1.00000000000000000000000661744,
  1.00000000000000000000000330872,
  1.00000000000000000000000165436,
  1.00000000000000000000000082718,
  1.00000000000000000000000041359,
  1.00000000000000000000000020680,
  1.00000000000000000000000010340,
  1.00000000000000000000000005170,
  1.00000000000000000000000002585,
  1.00000000000000000000000001292,
  1.00000000000000000000000000646,
  1.00000000000000000000000000323,
  1.00000000000000000000000000162,
  1.00000000000000000000000000081,
  1.00000000000000000000000000040,
  1.00000000000000000000000000020,
  1.00000000000000000000000000010,
  1.00000000000000000000000000005,
  1.00000000000000000000000000003,
  1.00000000000000000000000000001,
  1.00000000000000000000000000001,
  1.00000000000000000000000000000,
  1.00000000000000000000000000000,
  1.00000000000000000000000000000
};
#define ZETA_NEG_TABLE_NMAX  99
#define ZETA_NEG_TABLE_SIZE  50
static double zeta_neg_int_table[ZETA_NEG_TABLE_SIZE] = {
 -0.083333333333333333333333333333,      /* zeta(-1) */
  0.0083333333333333333333333333333,     /* zeta(-3) */
 -0.0039682539682539682539682539683,     /* ...      */
  0.0041666666666666666666666666667,
 -0.0075757575757575757575757575758,
  0.0210927960927960927960927960928,
 -0.083333333333333333333333333333,
  0.44325980392156862745098039216,
 -3.05395433027011974380395433027,
  26.4562121212121212121212121212,
 -281.460144927536231884057971014,
  3607.5105463980463980463980464,
 -54827.583333333333333333333333,
  974936.82385057471264367816092,
 -2.00526957966880789461434622725e7,
  4.7238486772162990196078431373e8,
 -1.26357247959166666666666666667e10,
  3.8087931125245368811553022079e11,
 -1.28508504993050833333333333333e13,
  4.8241448354850170371581670362e14,
 -2.00403106565162527381084216632e16,
  9.1677436031953307756992753623e17,
 -4.5979888343656503490437943262e19,
  2.51804719214510956970890233202e21,
 -1.50017334921539287337114401515e23,
  9.6899578874635940656497942895e24,
 -6.7645882379292820990945242302e26,
  5.0890659468662289689766332916e28,
 -4.1147288792557978697665486068e30,
  3.5666582095375556109684574609e32,
 -3.3066089876577576725680214670e34,
  3.2715634236478716264211227016e36,
 -3.4473782558278053878256455080e38,
  3.8614279832705258893092720200e40,
 -4.5892974432454332168863989006e42,
  5.7775386342770431824884825688e44,
 -7.6919858759507135167410075972e46,
  1.08136354499716546963540333511e49,
 -1.60293645220089654060671023458e51,
  2.50194790415604628436566614985e53,
 -4.1067052335810212479752045004e55,
  7.0798774408494580617452972433e57,
 -1.28045468879395087901908497563e60,
  2.42673403923335240780208920671e62,
 -4.8143218874045769355129570066e64,
  9.9875574175727530680652777408e66,
 -2.16456348684351856313351361598e69,
  4.8962327039620553206849224516e71,     /* ...        */
 -1.15490239239635196639542716916e74,    /* zeta(-97)  */
  2.83822495706937069592641563365e76     /* zeta(-99)  */
};


/* coefficients for Maclaurin summation in hzeta()
 * B_{2j}/(2j)!
 */
static double hzeta_c[15] = {
  1.00000000000000000000000000000,
  0.083333333333333333333333333333,
 -0.00138888888888888888888888888889,
  0.000033068783068783068783068783069,
 -8.2671957671957671957671957672e-7,
  2.08767569878680989792100903212e-8,
 -5.2841901386874931848476822022e-10,
  1.33825365306846788328269809751e-11,
 -3.3896802963225828668301953912e-13,
  8.5860620562778445641359054504e-15,
 -2.17486869855806187304151642387e-16,
  5.5090028283602295152026526089e-18,
 -1.39544646858125233407076862641e-19,
  3.5347070396294674716932299778e-21,
 -8.9535174270375468504026113181e-23
};


/*-*-*-*-*-*-*-*-*-*-*-* (semi)Private Implementations *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_hzeta_impl(const double s, const double q, double * result)
{
  if(s <= 1.0 || q <= 0.0) {
    return GSL_EDOM;
  }
  else if(-s * log(q) < GSL_LOG_DBL_MIN) {
    *result = 0.;
    return GSL_EUNDRFLW;
  }
  else {
    /* Euler-Maclaurin summation formula 
       [Moshier, p. 400, with several typo corrections]
     */
    int j, k;
    const int jmax = 12;
    const int kmax = 10;
    const double pmax  = pow(kmax + q, -s);
    double scp = s;
    double pcp = pmax / (kmax + q);
    double ans = pmax*((kmax+q)/(s-1) - 0.5);
    
    for(k=0; k<kmax; k++) {
      ans += pow(k + q, -s);
    }
    ans += pmax; /* avoids one unnecessary call to pow() */

    for(j=0; j<=jmax; j++) {
      double delta = hzeta_c[j] * scp * pcp;
      ans += delta;
      if(fabs(delta/ans) < GSL_MACH_EPS) break;
      scp *= (s+2*j+1)*(s+2*j+2);
      pcp /= (kmax + q)*(kmax + q);
    }
    
    *result = ans;
    return GSL_SUCCESS;
  }
}

/* checked OK [GJ] Tue May  5 20:38:59 MDT 1998 */
int gsl_sf_zeta_impl(const double s, double * result)
{
  if(s == 1.0) {
    return GSL_EDOM;
  }

  if(s >= 0.0) {
    *result = riemann_zeta_sgt0(s);
    return GSL_SUCCESS;
  }
  else {
    /* reflection formula, [Abramowitz+Stegun, 23.2.5] */
    double ln_gamma;
    double ln_pre = s * LogTwoPi_;
    double zeta_one_minus_s = riemann_zeta_sgt0(1.0-s);
    double ln_term;
    gsl_sf_lngamma_impl(1.0-s, &ln_gamma);
    ln_term  = ln_pre + ln_gamma;
    if(ln_term < GSL_LOG_DBL_MAX - 1.0) {
      *result = sin(0.5*M_PI*s)/M_PI * exp(ln_term) * zeta_one_minus_s;
      return GSL_SUCCESS;
    }
    else {
      *result = 0.; /* FIXME: should be Inf */
      return GSL_EOVRFLW;
    }
  }
}

/* checked OK [GJ] Tue May  5 20:34:39 MDT 1998 */
int gsl_sf_zeta_int_impl(const int n, double * result)
{
  if(n < 0) {
    if(!GSL_IS_ODD(abs(n))) {
      *result = 0.; /* exactly zero at even negative integers */
      return GSL_SUCCESS;
    }
    else if(n > -ZETA_NEG_TABLE_NMAX) {
      *result = zeta_neg_int_table[-(n+1)/2];
      return GSL_SUCCESS;
    }
    else {
      return gsl_sf_zeta_impl((double)n, result);
    }
  }
  else if(n == 1){
    return GSL_EDOM;
  }
  else if(n <= ZETA_POS_TABLE_NMAX){
    *result = zeta_pos_int_table[n];
    return GSL_SUCCESS;
  }
  else {
    *result = 1.0;
    return GSL_SUCCESS;
  }
}

void testy(void)
{
  double x;
  double q = 1.1;
  int n;
  for(x = 1.0; x <= 30; x += 0.1 ) {
    double y;
    int status = gsl_sf_hzeta_impl(x, q, &y);
    printf("%24.18g  %8.5g   %24.18g  %2d\n", x, q, y, status);
  }
  exit(0);
}


/*-*-*-*-*-*-*-*-*-*-*-* Error Handling Versions *-*-*-*-*-*-*-*-*-*-*-*/

int gsl_sf_zeta_e(const double s, double * result)
{
  int status = gsl_sf_zeta_impl(s, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_zeta_e", status);
  }
  return status;
}

int gsl_sf_hzeta_e(const double s, const double a, double * result)
{
  int status = gsl_sf_hzeta_impl(s, a, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_hzeta_e", status);
  }
  return status;
}

int gsl_sf_zeta_int_e(const int s, double * result)
{
  int status = gsl_sf_zeta_int_impl(s, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_zeta_int_e", status);
  }
  return status;
}


/*-*-*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*-*/

double gsl_sf_zeta(const double s)
{
  double y;
  int status = gsl_sf_zeta_impl(s, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_sf_zeta", status);
  }
  return y;
}

double gsl_sf_hzeta(const double s, const double a)
{
  double y;
  int status = gsl_sf_hzeta_impl(s, a, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_sf_hzeta_e", status);
  }
  return y;
}

double gsl_sf_zeta_int(int n)
{
  double y;
  int status = gsl_sf_zeta_int_impl(n, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("gsl_sf_zeta_int", status);
  }
  return y;
}
