#ifndef PCRS_H_INCLUDED
#define PCRS_H_INCLUDED

/*********************************************************************
 *
 * File        :  $Source: /cvsroot/ijbswa/current/pcrs.h,v $
 *
 * Purpose     :  Header file for pcrs.c
 *
 * Copyright   :  see pcrs.c
 *
 * Revisions   :
 *    $Log: pcrs.h,v $
 *    Revision 1.11  2002/03/08 14:18:23  oes
 *    Fixing -Wconversion warnings
 *
 *    Revision 1.10  2002/03/08 13:44:48  oes
 *    Hiding internal functions, preventing double inclusion of pcre.h
 *
 *    Revision 1.9  2001/08/18 11:35:29  oes
 *    - Introduced pcrs_strerror()
 *    - added pcrs_execute_list()
 *
 *    Revision 1.8  2001/08/15 15:32:50  oes
 *    Replaced the hard limit for the maximum number of matches
 *    by dynamic reallocation
 *
 *    Revision 1.7  2001/08/05 13:13:11  jongfoster
 *    Making parameters "const" where possible.
 *
 *    Revision 1.6  2001/07/29 18:52:06  jongfoster
 *    Renaming _PCRS_H, and adding "extern C {}"
 *
 *    Revision 1.5  2001/07/18 17:27:00  oes
 *    Changed interface; Cosmetics
 *
 *    Revision 1.4  2001/06/29 13:33:19  oes
 *    - Cleaned up, commented and adapted to reflect the
 *      changes in pcrs.c
 *    - Introduced the PCRS_* flags
 *
 *    Revision 1.3  2001/06/09 10:58:57  jongfoster
 *    Removing a single unused #define which referenced BUFSIZ
 *
 *    Revision 1.2  2001/05/25 11:03:55  oes
 *    Added sanity check for NULL jobs to pcrs_exec_substitution
 *
 *    Revision 1.1.1.1  2001/05/15 13:59:02  oes
 *    Initial import of version 2.9.3 source tree
 *
 *    Revision 1.4  2001/05/11 01:57:02  rodney
 *    Added new file header standard w/RCS control tags.
 *
 *    revision 1.3  2001/05/08 02:38:13  rodney
 *    Changed C++ "//" style comment to C style comments.
 *
 *    revision 1.2  2001/04/30 02:39:24  rodney
 *    Made this pcrs.h file conditionally included.
 *
 *    revision 1.1  2001/04/16 21:10:38  rodney
 *    Initial checkin
 *
 *********************************************************************/

#define PCRS_H_VERSION "$Id: pcrs.h,v 1.11 2002/03/08 14:18:23 oes Exp $"


#ifndef _PCRE_H
#include <pcre.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Constants:
 */

#define FALSE 0
#define TRUE 1

/* Capacity */
#define PCRS_MAX_SUBMATCHES  33     /* Maximum number of capturing subpatterns allowed. MUST be <= 99! FIXME: Should be dynamic */
#define PCRS_MAX_MATCH_INIT  40     /* Initial amount of matches that can be stored in global searches */
#define PCRS_MAX_MATCH_GROW  1.6    /* Factor by which storage for matches is extended if exhausted */

/* Error codes */
#define PCRS_ERR_NOMEM     -10      /* Failed to acquire memory. */
#define PCRS_ERR_CMDSYNTAX -11      /* Syntax of s///-command */
#define PCRS_ERR_STUDY     -12      /* pcre error while studying the pattern */
#define PCRS_ERR_BADJOB    -13      /* NULL job pointer, pattern or substitute */
#define PCRS_WARN_BADREF   -14      /* Backreference out of range */

/* Flags */
#define PCRS_GLOBAL          1      /* Job should be applied globally, as with perl's g option */
#define PCRS_TRIVIAL         2      /* Backreferences in the substitute are ignored */
#define PCRS_SUCCESS         4      /* Job did previously match */


/*
 * Data types:
 */

/* A compiled substitute */

typedef struct {
  char  *text;                                   /* The plaintext part of the substitute, with all backreferences stripped */
  int    backrefs;                               /* The number of backreferences */
  int    block_offset[PCRS_MAX_SUBMATCHES];      /* Array with the offsets of all plaintext blocks in text */
  size_t block_length[PCRS_MAX_SUBMATCHES];      /* Array with the lengths of all plaintext blocks in text */
  int    backref[PCRS_MAX_SUBMATCHES];           /* Array with the backref number for all plaintext block borders */
  int    backref_count[PCRS_MAX_SUBMATCHES + 2]; /* Array with the number of references to each backref index */
} pcrs_substitute;


/*
 * A match, including all captured subpatterns (submatches)
 * Note: The zeroth is the whole match, the PCRS_MAX_SUBMATCHES + 0th
 * is the range before the match, the PCRS_MAX_SUBMATCHES + 1th is the
 * range after the match.
 */

typedef struct {
  int    submatches;                               /* Number of captured subpatterns */
  int    submatch_offset[PCRS_MAX_SUBMATCHES + 2]; /* Offset for each submatch in the subject */
  size_t submatch_length[PCRS_MAX_SUBMATCHES + 2]; /* Length of each submatch in the subject */
} pcrs_match;


/* A PCRS job */

typedef struct PCRS_JOB {
  pcre *pattern;                            /* The compiled pcre pattern */
  pcre_extra *hints;                        /* The pcre hints for the pattern */
  int options;                              /* The pcre options (numeric) */
  int flags;                                /* The pcrs and user flags (see "Flags" above) */
  pcrs_substitute *substitute;              /* The compiled pcrs substitute */
  struct PCRS_JOB *next;                    /* Pointer for chaining jobs to joblists */
} pcrs_job;


/*
 * Prototypes:
 */

/* Main usage */
extern pcrs_job        *pcrs_compile_command(const char *command, int *errptr);
extern pcrs_job        *pcrs_compile(const char *pattern, const char *substitute, const char *options, int *errptr);
extern int              pcrs_execute(pcrs_job *job, const char *subject, size_t subject_length, char **result, size_t *result_length);
extern int              pcrs_execute_list(pcrs_job *joblist, char *subject, size_t subject_length, char **result, size_t *result_length);

/* Freeing jobs */
extern pcrs_job        *pcrs_free_job(pcrs_job *job);
extern void             pcrs_free_joblist(pcrs_job *joblist);

/* Info on errors: */
extern const char *pcrs_strerror(const int error);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ndef PCRS_H_INCLUDED */

/*
  Local Variables:
  tab-width: 3
  end:
*/
