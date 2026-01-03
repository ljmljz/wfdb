/* file: lomb.c		G. Moody	12 February 1992
			Last revised:	  27 July 2010
-------------------------------------------------------------------------------
lomb: Lomb periodogram of real data
Copyright (C) 1992-2010 George B. Moody

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, see <http://www.gnu.org/licenses/>.

You may contact the author by e-mail (wfdb@physionet.org) or postal mail
(MIT Room E25-505A, Cambridge, MA 02139 USA).  For updates to this software,
please visit PhysioNet (http://www.physionet.org/).
_______________________________________________________________________________

The default input to this program is a text file containing a sampled time
series, presented as two columns of numbers (the sample times and the sample
values).  The intervals between consecutive samples need not be uniform (in
fact, this is the most significant advantage of the Lomb periodogram over other
methods of power spectral density estimation).  This program writes the Lomb
periodogram (the power spectral density estimate derived from the input time
series) on the standard output.

The original version of this program was based on the algorithm described in
Press, W.H, and Rybicki, G.B., Astrophysical J. 338:277-280 (1989).  It has
been rewritten using the versions of this algorithm presented in Numerical
Recipes in C (Press, Teukolsky, Vetterling, and Flannery;  Cambridge U. Press,
1992).

This version agrees with 'fft' output (amplitude spectrum up to the Nyquist
frequency with total power equal to the variance);  thanks to Joe Mietus.
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef BSD
# include <string.h>
#else           /* for Berkeley UNIX only */
# include <strings.h>
# define strchr index
#endif

static long lmaxarg1, lmaxarg2;
#define LMAX(a,b) (lmaxarg1 = (a),lmaxarg2 = (b), (lmaxarg1 > lmaxarg2 ? \
						   lmaxarg1 : lmaxarg2))
static long lminarg1, lminarg2;
#define LMIN(a,b) (lminarg1 = (a),lminarg2 = (b), (lminarg1 < lminarg2 ? \
						   lminarg1 : lminarg2))
#define MOD(a,b)	while (a >= b) a -= b
#define MACC 4
#define SIGN(a,b) ((b) > 0.0 ? fabs(a) : -fabs(a))
static float sqrarg;
#define SQR(a) ((sqrarg = (a)) == 0.0 ? 0.0 : sqrarg*sqrarg)
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

/* Function prototypes */
static void fasper(float x[], float y[], unsigned long n, float ofac, float hifac,
                   float wk1[], float wk2[], unsigned long nwk, unsigned long *nout,
                   unsigned long *jmax, float *prob);
static void spread(float y, float yy[], unsigned long n, float x, int m);
static void avevar(float data[], unsigned long n, float *ave, float *var);
static void realft(float data[], unsigned long n, int isign);
static void four1(float data[], unsigned long nn, int isign);
static char *prog_name(char *s);
static void help(void);
static long input(void);
static void zeromean(unsigned long n);

/* Global variables (kept minimal) */
static char *pname;
static float pwr;
static FILE *ifile;
static float *x, *y, *wk1, *wk2;
static long nmax = 512L;

int main(int argc, char *argv[])
{
    int aflag = 1, i, sflag = 0, zflag = 0;
    unsigned long n, nout, jmax, maxout;
    float prob;

    pname = prog_name(argv[0]);
    for (i = 1; i < argc; i++) {
        if (*argv[i] == '-') switch (*(argv[i]+1)) {
          case 'h':       /* print help and exit */
            help();
            exit(0);
            break;
          case 's':       /* smooth output */
            sflag = 1;
            break;
          case 'P':       /* output powers instead of amplitudes */
            aflag = 0;
            break;
          case 'z':       /* zero-mean the input */
            zflag = 1;
            break;
          case '\0':      /* read data from standard input */
            ifile = stdin;
            break;
          default:
            fprintf(stderr, "%s: unrecognized option %s ignored\n",
                    pname, argv[i]);
            break;
        }
        else if (i == argc-1) { /* last argument: input file name */
            if ((ifile = fopen(argv[i], "rt")) == NULL) {
                fprintf(stderr, "%s: can't open %s\n", pname, argv[i]);
                exit(2);
            }
        }
    }
    if (ifile == NULL) {
        help();
        exit(1);
    }

    /* Read input. */
    n = input();

    /* Zero-mean the input if requested. */
    if (zflag) zeromean(n);

    /* Compute the Lomb periodogram. */
    fasper(x-1, y-1, n, 4.0, 2.0, wk1-1, wk2-1, 64*nmax, &nout, &jmax, &prob);

    /* Write the results.  Output only up to Nyquist frequency, so that the
       results are directly comparable to those obtained using conventional
       methods.  The normalization is by half the number of output samples; the
       sum of the outputs is (approximately) the mean square of the inputs.

       Note that the Nyquist frequency is not well-defined for an irregularly
       sampled series.  Here we use half of the mean sampling frequency, but
       the Lomb periodogram can return (less reliable) estimates of frequency
       content for frequencies up to half of the maximum sampling frequency in
       the input.  */

    maxout = nout/2;
    if (sflag) {        /* smoothed */

        pwr /= 4;

        if (aflag)      /* smoothed amplitudes */
            for (n = 0; n < maxout; n += 4)
                printf("%g\t%g\n", wk1[n],
                      sqrt((wk2[n]+wk2[n+1]+wk2[n+2]+wk2[n+3])/(nout/(8.0*pwr))));

        else            /* smoothed powers */
            for (n = 0; n < maxout; n += 4)
                printf("%g\t%g\n", wk1[n],
                      (wk2[n]+wk2[n+1]+wk2[n+2]+wk2[n+3])/(nout/(8.0*pwr)));

    }
    else {              /* oversampled */

        if (aflag)      /* amplitudes */
            for (n = 0; n < maxout; n++)
                printf("%g\t%g\n", wk1[n], sqrt(wk2[n]/(nout/(2.0*pwr))));

        else            /* powers */
            for (n = 0; n < maxout; n++)
                printf("%g\t%g\n", wk1[n], wk2[n]/(nout/(2.0*pwr)));

    }

    free(x);
    free(y);
    free(wk1);
    free(wk2);

    exit(0);
}

static void fasper(float x[], float y[], unsigned long n, float ofac, float hifac,
                   float wk1[], float wk2[], unsigned long nwk, unsigned long *nout,
                   unsigned long *jmax, float *prob)
{
    void avevar(float data[], unsigned long n, float *ave, float *var);
    void realft(float data[], unsigned long n, int isign);
    void spread(float y, float yy[], unsigned long n, float x, int m);
    unsigned long j, k, ndim, nfreq, nfreqt;
    float ave, ck, ckk, cterm, cwt, den, df, effm, expy, fac, fndim, hc2wt,
          hs2wt, hypo, pmax, sterm, swt, var, xdif, xmax, xmin;

    *nout = 0.5*ofac*hifac*n;
    nfreqt = ofac*hifac*n*MACC;
    nfreq = 64;
    while (nfreq < nfreqt) nfreq <<= 1;
    ndim = nfreq << 1;
    if (ndim > nwk) {
        fprintf(stderr, "%s: workspaces too small\n", pname);
        exit(1);
    }
    avevar(y, n, &ave, &var);
    xmax = xmin = x[1];
    for (j = 2; j <= n; j++) {
        if (x[j] < xmin) xmin = x[j];
        if (x[j] > xmax) xmax = x[j];
    }
    xdif = xmax - xmin;
    for (j = 1; j <= ndim; j++) wk1[j] = wk2[j] = 0.0;
    fac = ndim/(xdif*ofac);
    fndim = ndim;
    for (j = 1; j <= n; j++) {
        ck = (x[j] - xmin)*fac;
        MOD(ck, fndim);
        ckk = 2.0*(ck++);
        MOD(ckk, fndim);
        ckk++;
        spread(y[j] - ave, wk1, ndim, ck, MACC);
        spread(1.0, wk2, ndim, ckk, MACC);
    }
    realft(wk1, ndim, 1);
    realft(wk2, ndim, 1);
    df = 1.0/(xdif*ofac);
    pmax = -1.0;
    for (k = 3, j = 1; j <= (*nout); j++, k += 2) {
        hypo = sqrt(wk2[k]*wk2[k] + wk2[k+1]*wk2[k+1]);
        hc2wt = 0.5*wk2[k]/hypo;
        hs2wt = 0.5*wk2[k+1]/hypo;
        cwt = sqrt(0.5+hc2wt);
        swt = SIGN(sqrt(0.5-hc2wt), hs2wt);
        den = 0.5*n + hc2wt*wk2[k] + hs2wt*wk2[k+1];
        cterm = SQR(cwt*wk1[k] + swt*wk1[k+1])/den;
        sterm = SQR(cwt*wk1[k+1] - swt*wk1[k])/(n - den);
        wk1[j] = j*df;
        wk2[j] = (cterm+sterm)/(2.0*var);
        if (wk2[j] > pmax) pmax = wk2[(*jmax = j)];
    }
    expy = exp(-pmax);
    effm = 2.0*(*nout)/ofac;
    *prob = effm*expy;
    if (*prob > 0.01) *prob = 1.0 - pow(1.0 - expy, effm);
}

static void spread(float y, float yy[], unsigned long n, float x, int m)
{
    int ihi, ilo, ix, j, nden;
    static int nfac[11] = { 0, 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880 };
    float fac;

    if (m > 10) {
        fprintf(stderr, "%s: factorial table too small\n", pname);
        exit(1);
    }
    ix = (int)x;
    if (x == (float)ix) yy[ix] += y;
    else {
        ilo = LMIN(LMAX((long)(x - 0.5*m + 1.0), 1), n - m + 1);
        ihi = ilo + m - 1;
        nden = nfac[m];
        fac = x - ilo;
        for (j = ilo + 1; j <= ihi; j++) fac *= (x - j);
        yy[ihi] += y*fac/(nden*(x - ihi));
        for (j = ihi-1; j >= ilo; j--) {
            nden = (nden/(j + 1 - ilo))*(j - ihi);
            yy[j] += y*fac/(nden*(x - j));
        }
    }
}

static void avevar(float data[], unsigned long n, float *ave, float *var)
{
    unsigned long j;
    float s, ep;

    for (*ave = 0.0, j = 1; j <= n; j++) *ave += data[j];
    *ave /= n;
    *var = ep = 0.0;
    for (j = 1; j <= n; j++) {
        s = data[j] - (*ave);
        ep += s;
        *var += s*s;
    }
    *var = (*var - ep*ep/n)/(n-1);
    pwr = *var;
}

static void realft(float data[], unsigned long n, int isign)
{
    void four1(float data[], unsigned long nn, int isign);
    unsigned long i, i1, i2, i3, i4, np3;
    float c1 = 0.5, c2, h1r, h1i, h2r, h2i;
    double wr, wi, wpr, wpi, wtemp, theta;

    theta = 3.141592653589793/(double)(n>>1);
    if (isign == 1) {
        c2 = -0.5;
        four1(data, n>>1, 1);
    } else {
        c2 = 0.5;
        theta = -theta;
    }
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0 + wpr;
    wi = wpi;
    np3 = n+3;
    for (i = 2; i <= (n>>2); i++) {
        i4 = 1 + (i3 = np3 - (i2 = 1 + (i1 = i + i - 1)));
        h1r =  c1*(data[i1] + data[i3]);
        h1i =  c1*(data[i2] - data[i4]);
        h2r = -c2*(data[i2] + data[i4]);
        h2i =  c2*(data[i1] - data[i3]);
        data[i1] =  h1r + wr*h2r - wi*h2i;
        data[i2] =  h1i + wr*h2i + wi*h2r;
        data[i3] =  h1r - wr*h2r + wi*h2i;
        data[i4] = -h1i +wr*h2i + wi*h2r;
        wr = (wtemp = wr)*wpr - wi*wpi + wr;
        wi = wi*wpr + wtemp*wpi + wi;
    }
    if (isign == 1) {
        data[1] = (h1r = data[1]) + data[2];
        data[2] = h1r - data[2];
    } else {
        data[1] = c1*((h1r = data[1]) + data[2]);
        data[2] = c1*(h1r - data[2]);
        four1(data, n>>1, -1);
    }
}

static void four1(float data[], unsigned long nn, int isign)
{
    unsigned long n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    float tempr, tempi;

    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
        if (j > i) {
            SWAP(data[j], data[i]);
            SWAP(data[j+1], data[i+1]);
        }
        m = n >> 1;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    mmax = 2;
    while (n > mmax) {
        istep = mmax << 1;
        theta = isign*(6.28318530717959/mmax);
        wtemp = sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m = 1; m < mmax; m += 2) {
            for (i = m;i <= n;i += istep) {
                j = i + mmax;
                tempr = wr*data[j] - wi*data[j+1];
                tempi = wr*data[j+1] + wi*data[j];
                data[j] = data[i] - tempr;
                data[j+1] = data[i+1] - tempi;
                data[i] += tempr;
                data[i+1] += tempi;
            }
            wr = (wtemp = wr)*wpr - wi*wpi + wr;
            wi = wi*wpr + wtemp*wpi + wi;
        }
        mmax = istep;
    }
}

static char *prog_name(char *s)
{
    char *p = s + strlen(s);

#ifdef MSDOS
    while (p >= s && *p != '\\' && *p != ':') {
        if (*p == '.')
            *p = '\0';		/* strip off extension */
        if ('A' <= *p && *p <= 'Z')
            *p += 'a' - 'A';	/* convert to lower case */
        p--;
    }
#else
    while (p >= s && *p != '/')
        p--;
#endif
    return (++p);
}

static void help(void)
{
    printf("Usage: %s [OPTIONS] FILE\n", pname);
    printf("Compute Lomb periodogram of irregularly-sampled data\n");
    printf("Options:\n");
    printf("  -h        print this help message\n");
    printf("  -s        smooth output\n");
    printf("  -P        output powers instead of amplitudes\n");
    printf("  -z        zero-mean the input\n");
    printf("  FILE      input file (use '-' for standard input)\n");
    printf("Input format: time value pairs, one per line\n");
    printf("Output format: frequency power/amplitude pairs\n");
}

static long input(void)
{
    long n = 0, maxn = nmax;
    float t, v;

    /* Allocate initial buffers. */
    if ((x = (float *)malloc(maxn * sizeof(float))) == NULL ||
        (y = (float *)malloc(maxn * sizeof(float))) == NULL ||
        (wk1 = (float *)malloc(64 * maxn * sizeof(float))) == NULL ||
        (wk2 = (float *)malloc(64 * maxn * sizeof(float))) == NULL) {
        fprintf(stderr, "%s: insufficient memory\n", pname);
        exit(2);
    }

    /* Read input data. */
    while (fscanf(ifile, "%f %f", &t, &v) == 2) {
        if (n >= maxn) {  /* need to expand buffers */
            maxn *= 2;
            if ((x = (float *)realloc(x, maxn * sizeof(float))) == NULL ||
                (y = (float *)realloc(y, maxn * sizeof(float))) == NULL ||
                (wk1 = (float *)realloc(wk1, 64 * maxn * sizeof(float))) == NULL ||
                (wk2 = (float *)realloc(wk2, 64 * maxn * sizeof(float))) == NULL) {
                fprintf(stderr, "%s: insufficient memory\n", pname);
                exit(2);
            }
        }
        x[n] = t;
        y[n] = v;
        n++;
    }

    return (n);
}

static void zeromean(unsigned long n)
{
    unsigned long i;
    float mean = 0.0;

    for (i = 0; i < n; i++)
        mean += y[i];
    mean /= n;
    for (i = 0; i < n; i++)
        y[i] -= mean;
}