/*
 *	nreq.c - cawf(1) processing of nroff requests
 */

/*
 *	Copyright (c) 1991 Purdue University Research Foundation,
 *	West Lafayette, Indiana 47907.  All rights reserved.
 *
 *	Written by Victor A. Abell <abe@cc.purdue.edu>,  Purdue	University
 *	Computing Center.  Not derived from licensed software; derived from
 *	awf(1) by Henry Spencer of the University of Toronto.
 *
 *	Permission is granted to anyone to use this software for any
 *	purpose on any computer system, and to alter it and redistribute
 *	it freely, subject to the following restrictions:
 *
 *	1. The author is not responsible for any consequences of use of
 *	   this software, even if they arise from flaws in it.
 *
 *	2. The origin of this software must not be misrepresented, either
 *	   by explicit claim or by omission.  Credits must appear in the
 *	   documentation.
 *
 *	3. Altered versions must be plainly marked as such, and must not
 *	   be misrepresented as being the original software.  Credits must
 *	   appear in the documentation.
 *
 *	4. This notice may not be removed or altered.
 */

#include "cawf.h"
#include <ctype.h>


/*
 * Prototypes for request processing functions.
 */

_PROTOTYPE(static void nr_UL,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_Ub,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_Uc,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_Uf,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_Ur,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_ad,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_bp,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_br,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_ce,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_di,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_ds,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_fi,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_fl,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_ft,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_it,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_na,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_nf,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_ns,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_rm,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_rn,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_rr,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_rs,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_tl,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_tm,(unsigned char *line, int brk));
_PROTOTYPE(static void nr_tr,(unsigned char *line, int brk));

_PROTOTYPE(static void nr_nil,(unsigned char *line, int brk));


/*
 * NrReqt[] - nroff request processing table
 *
 * CAUTION: place new entries in their proper alphabetical order, since
 *	    this table is processed with a binary search.
 */

static struct nr_req {
	char *nm;			/* nroff request */
	void (*fun)();			/* processing function */
} NrReqt[] = {
	{ "\\\"",	nr_nil },	/* backslash-quote */
	{ "^#",		nr_UL  },
	{ "^=",		nr_UL  },
	{ "^b",		nr_Ub  },
	{ "^c",		nr_Uc  },
	{ "^d",		nr_nil },
	{ "^e",		nr_nil },
	{ "^f",		nr_Uf  },
	{ "^r",		nr_Ur  },
	{ "^x",		nr_nil },
	{ "ad",		nr_ad  },
	{ "bp",		nr_bp  },
	{ "br",		nr_br  },
	{ "ce",		nr_ce  },
	{ "di",		nr_di  },
	{ "ds",		nr_ds  },
	{ "fi",		nr_fi  },
	{ "fl",		nr_fl  },
	{ "ft",		nr_ft  },
	{ "i0",		nr_nil },
	{ "it",		nr_it  },
	{ "lg",		nr_nil },
	{ "li",		nr_nil },
	{ "na",		nr_na  },
	{ "nf",		nr_nf  },
	{ "ns",		nr_ns  },
	{ "ps",		nr_nil },
	{ "rm",		nr_rm  },
	{ "rn",		nr_rn  },
	{ "rr",		nr_rr  },
	{ "rs",		nr_rs  },
	{ "tl",		nr_tl  },
	{ "tm",		nr_tm  },
	{ "tr",		nr_tr  },
	{ "vs",		nr_nil },
};
/*
 * Nreq(line, brk) - process miscellaneous nroff requests from line
 *		     buffer with request status of (brk)
 */

void
Nreq(line, brk)
	unsigned char *line;
	int brk;
{
	unsigned char c[3];		/* command buffer */
	int cmp, hi, low, mid;		/* binary search indixes */

	c[0] = c[1] = c[2] = '\0';
	if ((c[0] = line[1]) != '\0')
		c[1] = line[2];

	low = mid = 0;
	hi = (sizeof(NrReqt) / sizeof(struct nr_req)) - 1;
	while (low <= hi) {
		mid = (low + hi) / 2;
		if ((cmp = strcmp((char *)c, NrReqt[mid].nm)) < 0)
			hi = mid - 1;
		else if (cmp > 0)
			low = mid + 1;
		else {

#if	defined(_BCC)
#pragma warn -pro
#endif	/* defined(_BCC) */

			(void) (*NrReqt[mid].fun)(line, brk);

#if	defined(_BCC)
#pragma warn -pro
#endif	/* defined(_BCC) */

			return;
		}
	}
    /*
     * Unknown request starting with a '.' or '\''..
     */
	Error(WARN, LINE, " unknown request", NULL);
}


/*
 * Adjust - "^[.']ad"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */

static void
nr_ad(line, brk)
	unsigned char *line;
	int brk;
{
	Pass3(NOBREAK, (unsigned char *)"both", -1, NULL, 0);
}


/*
* Begin new page - "^[.']bp"
*/

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(BCC) */


static void
nr_bp(line, brk)
	unsigned char *line;
	int brk;
{
	Pass3(brk, (unsigned char *)"need", -1, NULL, 999);
}


/*
* Break - "^[.']br"
*/

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(BCC) */


static void
nr_br(line, brk)
	unsigned char *line;
	int brk;
{
	Pass3(brk, (unsigned char *)"flush", -1, NULL, 0);
}


/*
 * Center - "^[.']ce"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(BCC) */


static void
nr_ce(line, brk)
	unsigned char *line;
	int brk;
{
	unsigned char *s;			/* string poiner */

	if ((s = Field(2, line, 0)) != NULL)
		Centering = atoi((char *)s);
	else
		Centering = 1;
	Pass3(brk, (unsigned char *)"flush", -1, NULL, 0);
}


/*
 * Diversion on and off - "^[.']di"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(BCC) */


static void
nr_di(line, brk)
	unsigned char *line;
	int brk;
{
	Pass3(DOBREAK, (unsigned char *)"flush", -1, NULL, 0);
	Divert ^= 1;
}


/*
 * Define string - "^[.']ds"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(BCC) */


static void
nr_ds(line, brk)
	unsigned char *line;
	int brk;
{
	unsigned char buf[MAXLINE];	/* temporary buffer */
	unsigned char nm[4], nm1[4];	/* name buffers */
	unsigned char *s1, *s2, *s4,	/* temporary string pointers */
		       *s5;

	if (Asmname(&line[3], nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	if ((s4 = Field(3, line, 0)) == NULL)
		s4 = (unsigned char *)"";
/*
 * Convert "\\b" to '\b' and "\\\\" to '\\'.  Pass all other sequences,
 * beginning with '\\', unmodified.
 */
	s1 = buf;
	while ((s5 = (unsigned char *)strchr((char *)s4, '\\')) != NULL) {
		while (s5 > s4)
			*s1++ = *s4++;
		s4 = ++s5;
		if (*s5 == '\\')
			*s1++ = '\\';
		else if (*s5 == 'b')
			*s1++ = '\b';
		else {
			*s1++ = '\\';
			*s1++ = *s5;
		}
		if (*s4)
			s4++;
	}

#if	defined(_BCC)
#pragma warn -pia
#endif	/* defined(_BCC) */

	while (*s1++ = *s4++)
		;

#if	defined(_BCC)
#pragma warn +pia
#endif	/* defined(_BCC) */

/*
 * Install the string and see if it's an indirect reference -- e.g.,
 *
 *	.ds S1 \*(S2
 */
	s2 = Findstr(nm, buf, 1);
	while (*s2 == '\\' && *(s2 + 1) == '*') {
		s2++;
		(void) Asmcode(&s2, nm1);
		s2 = Findstr(nm1, NULL, 0);
	}
	if (Hdft) {

/*
 * Look for names LH, LF, CH, CF, RH, RF.
 */
		if ((nm[0]=='L' || nm[0]=='C' || nm[0]=='R')
		&&  (nm[1]=='F' || nm[1]=='H')) {
			(void) sprintf((char *)buf, "%s", (char *)nm);
			Pass3(NOBREAK, buf, -1, s2, 0);
		}
	}
}



/*
 * Fill - "^[.']fi"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_fi(line, brk)
	unsigned char *line;
	int brk;
{
	Fill = 1;
	Pass3(brk, (unsigned char *)"flush", -1, NULL, 0);
}


/*
 * Flush - "^[.']fl"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_fl(line, brk)
	unsigned char *line;
	int brk;
{
	Pass3(brk, (unsigned char *)"flush", -1, NULL, 0);
}


/*
 * Font - "^[.']ft <font_name>"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_ft(line, brk)
	unsigned char *line;
	int brk;
{
	int i;				/* temporary index */

	if (line[3] == '\0' || line[4] == '\0')
		line[4] = 'P';
	if (line[4] == 'P') {
		Font[0] = Prevfont;
		return;
	}
	for (i = 0; Fcode[i].nm; i++) {
		if (Fcode[i].nm == line[4])
		break;
	}
	if (Fcode[i].status == '\0') {
		Error(WARN, LINE, " bad font code", NULL);
		return;
	}
	Prevfont = Font[0];
	Font[0] = line[4];
}


/*
 * Input trap - "^[.']it [1 <request>]"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_it(line, brk)
	unsigned char *line;
	int brk;

{
	unsigned char buf[MAXLINE];	/* temporary buffer */
	unsigned char *s1, *s2;		/* temporary string pointers */

	if ((s1 = Field(2, line, 0)) == NULL) {
		Free(&Aftnxt);
		return;
	}
	if (atoi((char *)s1) != 1) {
		Error(WARN, LINE, " first .it arg must be 1", NULL);
		return;
	}
	if ((s2 = Field(3, line, 0)) == NULL)
		Free(&Aftnxt);
	else {
		(void) sprintf((char *)buf, "%s,%s",
			(Aftnxt == NULL) ? "" : (char *)Aftnxt,
			(char *)s2);
		Free(&Aftnxt);
		Aftnxt = Newstr(buf);
	}
}


/*
 * Comment - "^[.']\\" - do nothing
 *
 * Debug - "^[.']\^d" - do nothing
 *
 * Finalization - "[.']\^e" - do nothing
 *
 * Error file - "^[.']\^x <name>" - do nothing
 *
 * "^[.']i0", "^[.']lg" and "^[.']li" - do nothing
 *
 * Point size - "^[.']ps" - do nothing
 *
 * Vertical spacing - "^[.']vs" - do nothing
 *
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_nil(line, brk)
	unsigned char *line;
	int brk;
{
}


/*
 * No adjust "^[.']na"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_na(line, brk)
	unsigned char *line;
	int brk;
{
	Pass3(NOBREAK, (unsigned char *)"left", -1, NULL, 0);
}


/*
 * No fill - "^[.']nf"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_nf(line, brk)
	unsigned char *line;
	int brk;
{
	Fill = 0;
	Pass3(brk, (unsigned char *)"flush", -1, NULL, 0);
}


/*
 * No space - "^[.']ns"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_ns(line, brk)
	unsigned char *line;
	int brk;
{
	Pass3(NOBREAK, (unsigned char *)"nospace", -1, NULL, 0);
}


/*
 * Remove macro or string - "^[.']rm"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_rm(line, brk)
	unsigned char *line;
	int brk;
{
	int i;				/* temporary index */
	unsigned char nm[4];		/* name buffer */

	if (Asmname(&line[3], nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	if ((i = Findmacro(nm, 0)) >= 0) {
		Delmacro(i);
		return;
			}
	(void) Findstr(nm, NULL, 0);
		if (Sx >= 0) {
			Delstr(Sx);
			return;
	}
	Error(WARN, LINE, " no macro/string", NULL);
}


/*
 * Rename macro or string - "^[.']rn"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_rn(line, brk)
	unsigned char *line;
	int brk;
{
	int i, j;			/* temporary indexes */
	unsigned char nm[4], nm1[4];	/* name buffers */
	unsigned char *s1;		/* temporary string pointer */

	if ((s1 = Field(2, line, 0)) == NULL ||  Asmname(s1, nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	if ((s1 = Field(3, line, 0)) == NULL ||  Asmname(s1, nm1) == 0) {
		Error(WARN, LINE, " no new name", NULL);
		return;
	}
	if ((i = Findmacro(nm, 0)) >= 0) {
		if ((j = Findmacro(nm1, 0)) >= 0)
			Delmacro(j);
		j = Findmacro(nm1, 1);
		Macrotab[j].bx = Macrotab[i].bx;
		Macrotab[i].bx = -1;
		Macrotab[j].ct = Macrotab[i].ct;
		Macrotab[i].ct = 0;
		Delmacro(i);
		return;
	}
	(void) Findstr(nm, NULL, 0);
	if ((i = Sx) >= 0) {
		(void) Findstr(nm1, Str[i].str, 1);
		Delstr(i);
		return;
	}
	if (Findmacro(nm1, 0) < 0)
		(void) Findmacro(nm1, 1);
}


/*
 * Remove register - "^[.']rr"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_rr(line, brk)
	unsigned char *line;
	int brk;
{
	int i;				/* temporary index */
	unsigned char nm[4];		/* name buffer */

	if (Asmname(&line[3], nm) == 0) {
		Error(WARN, LINE, " no name", NULL);
		return;
	}
	if ((i = Findnum(nm, 0, 0)) < 0) {
		Error(WARN, LINE, " no register", NULL);
		return;
	}
	Delnum(i);
}


/*
 * Resume space - "^[.']rs"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_rs(line, brk)
	unsigned char *line;
	int brk;
{
	Pass3(NOBREAK, (unsigned char *)"yesspace", -1, NULL, 0);
}


/*
 * Three part title - "^[.'tl]"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_tl(line, brk)
	unsigned char *line;
	int brk;
{
	unsigned char buf[MAXLINE];	/* temporary buffer */
	unsigned char *be, *bp;		/* buffer pointers */
	int i, j, k, n;			/* temporary indexes */
	unsigned char sep;		/* title separator */
	unsigned char *tl, *tm, *tp,	/* title string pointers */
		      *tr, *ttl;

	for (bp = &line[3]; *bp == ' ' || *bp == '\t'; bp++)
		;
	tl = ttl = Newstr(bp);
	if ((sep = *tl++) == '\0') {

bad_title:
		Error(WARN, LINE, " bad 3-part title", NULL);
		Free(&ttl);
		return;
	}
	for (tm = tl, tr = NULL; *tm && *tm != sep; tm++)
		;
	if (*tm) {
		*tm++ = '\0';
		for (tr = tm; *tr && *tr != sep; tr++)
			;
		if (*tr) {
			*tr++ = '\0';
			for (tp = tr; *tp && *tp != sep; tp++)
				;
			if (*tp)
				*tp = '\0';
		}
	}
	i = LenprtHF(tl, Thispg, 0, NULL, 0) + LenprtHF(tm, Thispg, 0, NULL, 0)
	  + LenprtHF(tr, Thispg, 0, NULL, 0) + 2;
	if (i > (sizeof(buf) - 2) || i > LT)
		goto bad_title;
	j = (LT - i - Pgoff) / 2 + 1;
	n = LT - Pgoff - i - j + 2;
	be = &buf[sizeof(buf) - 1];
	for (bp = buf, k = 0; k < Pgoff; k++)
		*bp++ = ' ';
	if (tl)
		(void) LenprtHF(tl, Thispg, 1, &bp, be - bp);
	while (j--)
		*bp++ = ' ';
	if (tm)
		(void) LenprtHF(tm, Thispg, 1, &bp, be - bp);
	while (n--)
		*bp++ = ' ';
	if (tr)
		(void) LenprtHF(tr, Thispg, 1, &bp, be - bp);
	*bp = '\0';
	Pass3(RAWLINE, (unsigned char *)"title3", -1, buf, 0);
	Free(&ttl);
}


/*
 * Message - "^[.']tm"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_tm(line, brk)
	unsigned char *line;
	int brk;
{
	Pass3(MESSAGE, Inname, -1, (line[3] == ' ') ? &line[4] : &line[3], NR);
}


/*
 * Translate - "^[.']tr abcd..."
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_tr(line, brk)
	unsigned char *line;
	int brk;
{
	unsigned char buf[MAXLINE];	/* temporary buffer */
	int i, j;			/* temporary indexes */
	unsigned char nm[4], nm1[4];	/* name buffers */
	unsigned char *s1, *s2;		/* temporary string pointers */
	int trin, trout;		/* types: 0 = char; 1 = named char */
	unsigned char xbuf[MAXLINE];	/* temporary buffer */

	if (line[3] != ' ') {
		Error(WARN, LINE, " unknown translation", NULL);
		return;
	}
	for (s1 = &line[4]; *s1;) {
	    nm[1] = nm[2] = '\0';
	    s2 = s1 + 1;
	/*
	 * Assemble the input value.
	 */
	    if (*s1 == '\\' && (*s2 == '*' || *s2 == '(')) {
		if (*s2 == '(') {
	    /*
	     * Input is named character -- "\(xx".
	     */
		    trin = 1;
		    s1 = s2 + 1;
		    if ((nm[0] = *s1) != '\0') {
			s1++;
			if ((nm[1] = *s1) != '\0')
			    s1++;
		    }
		} else {
	    /*
	     * Input is interpolated string -- "\*x" or "\*(xx".
	     */
		    s1 = Asmcode(&s2, nm);
		    if (*s1)
			s1++;
		    s2 = Findstr(nm, NULL, 0);
		    if (*s2 != '\0') {
			if ((strlen((char *)s2) + strlen((char *)s1) + 1)
			>= MAXLINE)
			    Error(WARN, LINE, " string too long: ", (char *)nm);
			else {
			    (void) sprintf((char *)buf, "%s%s",
				(char *)s2, (char *)s1);
			    (void) strcpy((char *)xbuf, (char *)buf);
			    s1 = xbuf;
			}
		    }
		    continue;
		}
	    } else {

	    /*
	     * Input is a simple character.
	     */
		trin = 0;
		if ((nm[0] = *s1) != '\0')
		    s1++;
	    }
	/*
	 * Assemble the output value.
	 */

assemble_output:
	    nm1[1] = nm1[2] = '\0';
	    if (*s1 == '\0') {

	    /*
	     * Supply a space if there is no output character.
	     */
		trout = 0;
		nm1[0] = ' ';
	    } else {
		s2 = s1 + 1;
		if (*s1 == '\\' && (*s2 == '(' || *s2 == '*')) {
		    if (*s2 == '(') {
		/*
		 * The output is a named character -- "\(xx".
		 */
			trout = 1;
			s1 = s2 + 1;
			if ((nm1[0] = *s1) != '\0') {
			    s1++;
			    if ((nm1[1] = *s1) != '\0')
				s1++;
			}
		    } else {
		/*
		 * The output is an interpolated string -- * "\*x" or "\*(xx".
		 */
			s1 = Asmcode(&s2, nm1);
			if (*s1)
			    s1++;
			s2 = Findstr(nm1, NULL, 0);
			if (*s2 != '\0') {
		    /*
		     * Interpolate a string value.
		     */
			    if ((strlen((char *)s2) + strlen((char *)s1) + 1)
			    >= MAXLINE)
				Error(WARN, LINE, " string too long: ",
				    (char *)nm);
			    else {
				(void) sprintf((char *)buf, "%s%s", (char *)s2,
				    (char *)s1);
				(void) strcpy((char *)xbuf, (char *)buf);
			        s1 = xbuf;
			    }
			}
			goto assemble_output;
		    }
		} else {
		    trout = 0;
		    if ((nm1[0] = *s1) != '0')
			s1++;
		    else
			nm1[0] = ' ';
		}
	    }
	/*
	 * Do the translation.
	 */
	    switch (trin) {

	    case 0:			/* simple char */
		switch (trout) {

		case 0:			/* to simple char */
		    Trtbl[(int)nm[0]] = nm1[0];
		    break;
		case 1:			/* to named char */
		    if ((i = Findchar(nm1, 0, NULL, 0)) < 0
		    ||  strlen((char *)Schar[i].str) != 1)
			Error(WARN, LINE, " bad named character: ",
			    (char *)nm1);
		    else
			Trtbl[(int)nm[0]] = *(Schar[i].str);
		    break;
		}
		break;
	    case 1:			/* named char */
		if ((i = Findchar(nm, 0, NULL, 0)) < 0)
		    Error(WARN, LINE, " unknown named character: ", (char *)nm);
		else {
		    switch (trout) {

		    case 0:		/* to simple char */
			Free(&Schar[i].str);
			Schar[i].str = Newstr(nm1);
			Schar[i].len = 1;
			break;
		    case 1:		/* to named char */
			if ((j = Findchar(nm1, 0, NULL, 0)) < 0)
			    Error(WARN, LINE, " unknown named character: ",
				(char *)nm1);
			else
			    (void) Findchar(nm, Schar[j].len, Schar[j].str, 1);
			break;
		    }
		}
		break;
	    }
	}
}


/*
 * Initialization - "^[.']\^b (fh|HF|NH) [01]"
 *
 * fh = first page header status
 * HF = header/footer status
 * NH = initialize number headers
 *
 * Initialization - "^[.']\^b lf n" -- set footer line count to n
 *
 * Initialization - "^[.']\^b lh n" -- set header line count to n
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_Ub(line, brk)
	unsigned char *line;
	int brk;
{
	int i;				/* temporary index */
	unsigned char *s1, *s2;		/* temporary string pointers */

	if ((s1 = Field(2, line, 0)) == NULL)
		return;
	if ((s2 = Field(3, line, 0)) == NULL)
		i = 0;
	else
		i = atoi((char *)s2);
	if (s1[0] == 'f' && s1[1] == 'h')
		Pass3(NOBREAK, (unsigned char *)"fph", -1, NULL, i);
	else if (s1[0] == 'H' && s1[1] == 'F')
		Hdft = i;
	else if (s1[0] == 'N' && s1[1] == 'H') {
		for (i = 0; i < MAXNHNR; i++)
			Nhnr[i] = 0;
	} else if (s1[0] == 'l' && s1[1] == 'f')
		Botmarg = i;
	else if (s1[0] == 'l' && s1[1] == 'h')
		Topmarg = i;
	else
		Error(WARN, LINE, " unknown initialization", NULL);
}


/*
 * Character definitions - "^[.']\^c"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_Uc(line, brk)
	unsigned char *line;
	int brk;
{
	unsigned char buf[MAXLINE];	/* temporary buffer */
	int i;				/* temporary index */
	unsigned char *s1, *s2, *s4,	/* temporary string pointers */
		      *s5;

	s2 = Field(2, line, 0);
	i = atoi((char *)Field(3, line, 0));
	s4 = Field(4, line, 0);
	if (i < 0 || i > MAXLINE/2 || *s2 == '\0') {
		Error(WARN, LINE, " bad character definition", NULL);
		return;
	}
	if (s4 == NULL)
		s4 = (unsigned char *)"";
	else if (*s4 == '"')
		s4++;
	s1 = buf;
	while ((s5 = (unsigned char *)strchr((char *)s4, '\\')) != NULL) {
		while (s5 > s4)
			*s1++ = *s4++;
		s4 = ++s5;
		if (*s5 == '\\')
			*s1++ = '\\';
		else if (*s5 == 'b')
			*s1++ = '\b';
		if (*s4)
			s4++;
	}

#if	defined(_BCC)
#pragma warn -pia
#endif	/* defined(_BCC) */

	while (*s1++ = *s4++)
        ;

#if	defined(_BCC)
#pragma warn +pia
#endif	/* defined(_BCC) */

	if (*s2 == 'h' && *(s2+1) == 'y')
		(void) Findhy(buf, i, 1);
	else
		(void) Findchar(s2, i, buf, 1);
}


/*
 * Font is OK - "[.']\^f <font_name_character>"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_Uf(line, brk)
	unsigned char *line;
	int brk;
{
	int i;				/* temporary index */

	if (line[3] != '\0' && line[4] != '\0') {
		for (i = 0; Fcode[i].nm; i++) {
			if (line[4] == Fcode[i].nm) {
				Fcode[i].status = '1';
				return;
			}
		}
	}
	Error(WARN, LINE, " unknown font", NULL);
}


/*
 * Resolutions - "[.']\^r cpi horizontal vertical"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_Ur(line, brk)
	unsigned char *line;
	int brk;
{
	unsigned char buf[MAXLINE];	/* temporary buffer */
	int i, j;			/* temporary indexes */
	double tval;			/* temporary value */

	if ((i = atoi((char *)Field(3, line, 0))) <= 0
	||  (j = atoi((char *)Field(4, line, 0))) <= 0) {
		Error(WARN, LINE, " bad cpi resolutions", NULL);
		return;
	}
	tval = (double) (240.0 / (double) i);
	if (Findscale((int)'m', tval, 1) < 0)
		Error(FATAL, LINE, " missing Scal['m']", NULL);
	Scalen = tval;
	if (Scalen <= 0.0) {
		(void) sprintf((char *)buf, " bad Scale['n'] (%f)", Scalen);
		Error(FATAL, LINE, (char *)buf, NULL);
	}
	if (Findscale((int)'n', tval, 1) < 0)
		Error(FATAL, LINE, " missing Scale['n']", NULL);
	Scalev = (double) (240.0 / (double) j);
	if (Scalev <= 0.0) {
		(void) sprintf((char *)buf, " bad Scale['v'] (%f)", Scalen);
		Error(FATAL, LINE, (char *)buf, NULL);
	}
	if (Findscale((int)'v', Scalev, 1) < 0)
		Error(FATAL, LINE, " missing Scale['v']", NULL);
}


/*
 * Set line number and file name - "^[.']\^# <number> <file>"
 *
 * Lock line number and file name - "^[.']\^= <number> <file>"
 */

#if	defined(_BCC)
#pragma	argsused
#endif	/* defined(_BCC) */


static void
nr_UL(line, brk)
	unsigned char *line;
	int brk;
{
	unsigned char *s1;		/* temporary string pointer */

	if ((s1 = Field(2, line, 0)) != NULL)
		P2il = atoi((char *)s1) - 1;
	else
		P2il = 0;
	Lockil = (line[2] == '#') ? 0 : 1;
	Free(&P2name);
	if (Field(3, line, 1) != NULL) {
		P2name = F;
		F = NULL;
	} else
		P2name = NULL;
}