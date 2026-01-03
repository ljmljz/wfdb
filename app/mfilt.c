/* file: mfilt.c	G. Moody	 27 June 1993
			Last revised:    24 April 2020

-------------------------------------------------------------------------------
mfilt: General-purpose median filter for database records
Copyright (C) 1993-2006 George B. Moody

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

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wfdb/wfdb.h>

static char *pname;	/* name by which this program was invoked */
static char *nrec;	/* name of record to be created */
static int flen;	/* filter length */
static int median;	/* offset of median element within vtemp array after sorting */
static int nsig;	/* number of signals to be filtered */
static int **vin;	/* pointers to input vectors */
static int *vtemp;	/* temporary array for calculating medians */
static int *vout;	/* output vector */
static WFDB_Time from = 0L; /* first sample to be processed */
static WFDB_Time to = 0L; /* (if > 0) sample following last sample to be processed */
static WFDB_Time spm;	/* samples per minute */
static WFDB_Time tt;	/* time to print next progress indicator */

static char *prog_name(char *s);
static void help(void);
static void init(int argc, char *argv[]);
static void memerr(void);

int icmp(int *x, int *y)
{
    return (*y - *x);
}

int main(int argc, char *argv[])
{
    int i = 0, j, s;
    WFDB_Time t;

    init(argc, argv);	/* read and interpret command line */
    if (from < median) {	/* preload array with copies of sample 0 */
	(void)getvec(vin[0]);	
	for (i = 1; i <= median - from; i++)
	    for (s = 0; s < nsig; s++)
		vin[i][s] = vin[0][s];
    }
    else if (isigsettime(from - median) < 0)
	exit(2);
    for ( ; i < flen; i++)
	(void)getvec(vin[i]);
    for (t = from; to <= 0L || t < to; t++) {
	for (s = 0; s < nsig; s++) {
	    for (j = 0; j < flen; j++)
		vtemp[j] = vin[j][s];
	    qsort((char *)vtemp, flen, sizeof(int), (int (*)(const void *, const void *))icmp);
	    if (flen & 1)	/* odd length -- median is middle element */
		vout[s] = vtemp[median];
	    else    /* even length -- median is avg. of two middle elements */
		vout[s] = (vtemp[median] + vtemp[median-1])/2;
	}
	if (++i >= flen)
	    i = 0;
	if (putvec(vout) < 0 || getvec(vin[i]) < 0)
	    break;
	if (t > tt) {
	    (void)fprintf(stderr, ".");
	    tt += spm;
	}
    }
    if (to <= 0L) to = t + flen - median;
    for (; t < to; t++)
	(void)putvec(vout);	/* pad output to length of input record */
    (void)fprintf(stderr, "\n");
    if (nrec) (void)newheader(nrec);
    wfdbquit();
    exit(0);
    return 0;
}

static void init(int argc, char *argv[])
{
    char *irec = "16", *ofname, *orec = "16", *p;
    int format, i;
    static int gvmode = 0;
    static WFDB_Siginfo *si, *so;

    pname = prog_name(argv[0]);
    for (i = 1; i < argc; i++) {
	if (*argv[i] == '-') switch (*(argv[i]+1)) {
	  case 'f':	/* starting time */
	    if (++i >= argc) {
		(void)fprintf(stderr, "%s: time must follow -f\n", pname);
		exit(1);
	    }
	    from = i;
	    break;
	  case 'h':	/* help requested */
	    help();
	    exit(0);
	    break;
	  case 'H':	/* operate in WFDB_HIGHRES mode */
	    gvmode = WFDB_HIGHRES;
	    break;
	  case 'i':	/* input record name */
	    if (++i >= argc) {
		(void)fprintf(stderr, "%s: record name must follow -i\n",
			      pname);
		exit(1);
	    }
	    irec = argv[i];
	    break;
	  case 'l':	/* filter length */
	    if (++i >= argc || (flen = atoi(argv[i])) < 1) {
		(void)fprintf(stderr,
			      "%s: filter length (> 0) must follow -l\n",
			      pname);
		exit(1);
	    }
	    break;
	  case 'n':	/* new record name */
	    if (++i >= argc) {
		(void)fprintf(stderr, "%s: record name must follow -n\n",
			      pname);
		exit(1);
	    }
	    nrec = argv[i];
	    break;
	  case 'o':	/* output record name */
	    if (++i >= argc) {
		(void)fprintf(stderr, "%s: record name must follow -o\n",
			      pname);
		exit(1);
	    }
	    orec = argv[i];
	    break;
	  case 't':	/* end time */
	    if (++i >= argc) {
		(void)fprintf(stderr, "%s: time must follow -t\n", pname);
		exit(1);
	    }
	    to = i;
	    break;
	  default:
	    (void)fprintf(stderr, "%s: unrecognized option %s\n", pname,
			  argv[i]);
	    exit(1);
	}
	else {
	    (void)fprintf(stderr, "%s: unrecognized argument %s\n", pname,
			  argv[i]);
	    exit(1);
	}
    }
    if (flen < 1) {
	help();
	exit(1);
    }
    median = flen/2;

    if (gvmode == 0 && (p = getenv("WFDBGVMODE")))
	gvmode = atoi(p);
    setgvmode(gvmode|WFDB_GVPAD);

    if ((nsig = isigopen(irec, NULL, 0)) <= 0)
	exit(2);
    if ((si = (WFDB_Siginfo *)malloc(nsig * sizeof(WFDB_Siginfo))) == NULL ||
	(so = (WFDB_Siginfo *)malloc(nsig * sizeof(WFDB_Siginfo))) == NULL ||
	(vtemp = (int *)calloc((unsigned)flen, sizeof(int))) == NULL ||
	(vin = (int **)calloc((unsigned)flen, sizeof(int *))) == NULL ||
	(vout = (int *)malloc(nsig * sizeof(int))) == NULL)
	memerr();
    for (i = 0; i < flen; i++)
	if ((vin[i] = (int *)calloc((unsigned)nsig, sizeof(int))) == NULL)
	    memerr();
    if (isigopen(irec, si, (unsigned)nsig) != nsig)
	exit(2);

    if (nrec) {
	if ((ofname = (char *)malloc(strlen(nrec)+5)) == NULL)
	    memerr();
	(void)sprintf(ofname, "%s.dat", nrec);
	format = si[0].fmt;
	for (i = 0; i < nsig; i++) {
	    si[i].fname = ofname;
	    si[i].fmt = format;
	    si[i].spf = 1;
	}
	if (osigfopen(si, (unsigned)nsig) < nsig)
	    exit(2);
	setsampfreq(sampfreq(NULL));
    }
    else if (osigopen(orec, so, (unsigned)nsig) != nsig)
	exit(2);

    if (from > 0L) {
	if ((from = strtim(argv[from])) < 0L) from = -from;
    }
    if (to > 0L) {
	if ((to = strtim(argv[to])) < 0L) to = -to;
	if (to > 0L && to <= from) {
	    (void)fprintf(stderr, "%s: improper interval specified\n", pname);
	    exit(1);
	}
    }
    spm = strtim("1:0");	/* samples per minute */
    tt = from + spm;		/* time to print next progress indicator */
}

static void memerr(void)
{
    (void)fprintf(stderr, "%s: insufficient memory\n", pname);
    exit(2);
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
    return (p+1);
}

static const char *help_strings[] = {
 "usage: %s -l N [OPTIONS ...]\n",
"where N is the filter length (output is median of N samples in each signal),",
 "and OPTIONS may include:",
 " -f TIME     begin at specified time",
 " -h          print this usage summary",
 " -H          read multifrequency signals in high resolution mode",
 " -i IREC     read signals from record IREC (default: 16)",
 " -n NREC     create a header file, using record name NREC and signal",
 "              specifications from IREC",
 " -o OREC     produce output signal file(s) as specified by the header file",
 "              for record OREC (default: 16)",
 " -t TIME     stop at specified time",
NULL
};

void help(void)
{
    int i;

    (void)fprintf(stderr, help_strings[0], pname);
    for (i = 1; help_strings[i] != NULL; i++)
	(void)fprintf(stderr, "%s\n", help_strings[i]);
}