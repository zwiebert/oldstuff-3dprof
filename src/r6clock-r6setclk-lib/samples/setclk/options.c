/* $Id: options.c,v 1.1.1.1 2002/01/25 22:14:25 VSi Exp $
 *
 * Command-line options parser.
 *
 * ----------------------------------------------------------------------------
 * LICENSE
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (GPL) as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * To read the license please visit http://www.gnu.org/copyleft/gpl.html
 * ----------------------------------------------------------------------------
 * $Log: options.c,v $
 * Revision 1.1.1.1  2002/01/25 22:14:25  VSi
 * Imported to CVS
 *
 * ----------------------------------------------------------------------------
 *                    Copyright (C) 1997-2002 Vahur Sinij�rv
 * ----------------------------------------------------------------------------
 */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "options.h"

typedef void (*INTPROC)( int );
typedef void (*STRPROC)( char * );
typedef void (*VOIDPROC)( void );

char *opt_errmsg = NULL;

static char *curopt;

/* Messages */
static char *msg_unknown   = "Unknown option %s";
static char *msg_missparam = "Missing parameter for option %s";
static char *msg_invalint  = "Invalid integer %s";
static char *msg_missopt   = "Missing option";
static char *msg_invopt    = "Invalid option %s";


static void _cdecl opt_err( char *fmt, ... )
{
    va_list ap;

    va_start( ap, fmt );
    vfprintf( stderr, fmt, ap );
    fputs( "\n", stderr );
    if (opt_errmsg) fputs( opt_errmsg, stderr );
    exit( -1 );
}

static OPTDEF *findlongop( char *op, OPTDEF *defs, int nops )
{
    int    i;
    OPTDEF *p;

    p = defs;
    for( i = 0; i < nops; i++, p++ )
        if ( !strcmp( p->longname, op ) ) return p;

    return NULL;
}

static OPTDEF *findshortop( char op, OPTDEF *defs, int nops )
{
    int    i;
    OPTDEF *p;

    p = defs;
    for( i = 0; i < nops; i++, p++ )
        if ( p->shortname == op ) return p;

    return NULL;
}

static int process( OPTDEF *p, int i, int n, char **argv )
{
    char *param, *s;
    int  utmp;
    long l;

    if (!p) opt_err( msg_unknown, curopt );

    if ( p->param & OPT_PMASK) {
        if ( i >= n-1 ) opt_err( msg_missparam, curopt );
        param = argv[i+1];

        switch( p->param ) {
        case OPT_EXECINT:
        case OPT_INT:
            l = strtol( param, &s, 10 );
            if (errno || *s || (l > INT_MAX) || (l < INT_MIN))
                opt_err( msg_invalint, param );
            utmp = l;
            if (p->param == OPT_INT) *((int *)(p->var)) = utmp;
            else ((INTPROC)(p->var))( utmp );
            break;
        case OPT_STR:
            *((char **)(p->var)) = param;
            break;
        case OPT_EXECSTR:
            ((STRPROC)(p->var))( param );
            break;
        }
        return 1;
    } else {
        if ( p->param == OPT_EXEC ) ((VOIDPROC)(p->var))();
        else *((int *)(p->var)) |= p->mask;
        return 0;
    }
}

int opt_parse( int argc, char **argv, OPTDEF *opdef, int nops )
{
    int    i, n;
    char   *s;
    char   buf[3];
    OPTDEF *p;

    buf[0] = '-';
    buf[2] = 0;

    for( i = 1; i < argc; i++ ) {
        s = argv[i];
        if ( s[0] == '-' ) {
            if ( s[1] == '-' ) {
                if (!s[2]) return i;                 /* -- means eop */
                curopt = s;
                p = findlongop( &s[2], opdef, nops );
                n = process( p, i, argc, argv );
            }
            else {
                if (!s[1]) opt_err( msg_missopt );
                while (*++s) {
                    buf[1] = *s;
                    curopt = buf;
                    p = findshortop( *s, opdef, nops );

                    if (p && (p->param & OPT_PMASK) && s[1])
                        opt_err( msg_missparam, curopt );

                    n = process( p, i, argc, argv );
                }                                       /* while         */
            }                                           /* if ( s[1]...  */
        } else return i;                               /* if ( s[0]...  */
        i += n;
    }
    return i;
}
                                                                                                                                                                                                                                                                                              