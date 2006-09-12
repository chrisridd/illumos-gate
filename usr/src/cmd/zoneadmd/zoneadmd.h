/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef	_ZONEADMD_H
#define	_ZONEADMD_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Multi-threaded programs should avoid MT-unsafe library calls (i.e., any-
 * thing which could try to acquire a user-level lock unprotected by an atfork
 * handler) between fork(2) and exec(2).  See the pthread_atfork(3THR) man
 * page for details.  In particular, we want to avoid calls to zerror() in
 * such situations, as it calls setlocale(3c) which is susceptible to such
 * problems.  So instead we have the child use one of the special exit codes
 * below when needed, and the parent look out for such possibilities and call
 * zerror() there.
 *
 * Since 0, 1 and 2 are generally used for success, general error, and usage,
 * we start with 3.
 */
#define	ZEXIT_FORK		3
#define	ZEXIT_EXEC		4
#define	ZEXIT_ZONE_ENTER	5

#define	DEVFSADM	"devfsadm"
#define	DEVFSADM_PATH	"/usr/sbin/devfsadm"

#define	EXEC_PREFIX	"exec "
#define	EXEC_LEN	(strlen(EXEC_PREFIX))

typedef struct zlog {
	FILE *logfile;	/* file to log to */

	/*
	 * The following are used if logging to a buffer.
	 */
	char *log;	/* remaining log */
	size_t loglen;	/* size of remaining log */
	char *buf;	/* underlying storage */
	size_t buflen;	/* total len of 'buf' */
	char *locale;	/* locale to use for gettext() */
} zlog_t;

extern zlog_t logsys;

extern mutex_t lock;
extern mutex_t msglock;
extern boolean_t in_death_throes;
extern boolean_t bringup_failure_recovery;
extern char *zone_name;
extern char boot_args[BOOTARGS_MAX];
extern char bad_boot_arg[BOOTARGS_MAX];
extern boolean_t zone_isnative;


extern void zerror(zlog_t *, boolean_t, const char *, ...);
extern char *localize_msg(char *locale, const char *msg);

/*
 * Eventstream interfaces.
 */
typedef enum {
	Z_EVT_NULL = 0,
	Z_EVT_ZONE_BOOTING,
	Z_EVT_ZONE_REBOOTING,
	Z_EVT_ZONE_HALTED,
	Z_EVT_ZONE_READIED,
	Z_EVT_ZONE_UNINSTALLING,
	Z_EVT_ZONE_BOOTFAILED,
	Z_EVT_ZONE_BADARGS
} zone_evt_t;

extern int eventstream_init();
extern void eventstream_write(zone_evt_t evt);

/*
 * Virtual platform interfaces.
 */
extern zoneid_t vplat_create(zlog_t *, boolean_t);
extern int vplat_bringup(zlog_t *, boolean_t, zoneid_t);
extern int vplat_teardown(zlog_t *, boolean_t);

/*
 * Console subsystem routines.
 */
extern int init_console_slave(zlog_t *);
extern void destroy_console_slave(void);
extern void reset_slave_terminal(zlog_t *);
extern int init_console(zlog_t *);
extern void serve_console(zlog_t *);

/*
 * Contract handling.
 */
extern int init_template(void);

/*
 * Routine to manage child processes.
 */
extern int do_subproc(zlog_t *, char *);

#ifdef __cplusplus
}
#endif

#endif /* _ZONEADMD_H */
