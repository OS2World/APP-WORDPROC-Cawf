/* The <ansi.h> header attempts to decide whether the compiler has enough
 * conformance to Standard C for Minix to take advantage of.  If so, the
 * symbol _ANSI is defined (as 31415).  Otherwise _ANSI is not defined
 * here, but it may be defined by applications that want to bend the rules.
 * The magic number in the definition is to inhibit unnecessary bending
 * of the rules.  (For consistency with the new '#ifdef _ANSI" tests in
 * the headers, _ANSI should really be defined as nothing, but that would
 * break many library routines that use "#if _ANSI".)

 * If _ANSI ends up being defined, a macro
 *
 *	_PROTOTYPE(function, params)
 *
 * is defined.  This macro expands in different ways, generating either
 * ANSI Standard C prototypes or old-style K&R (Kernighan & Ritchie)
 * prototypes, as needed.  Finally, some programs use _CONST, _VOIDSTAR etc
 * in such a way that they are portable over both ANSI and K&R compilers.
 * The appropriate macros are defined here.
 */

#if	!defined(_ANSI_H)
#define _ANSI_H

# if	__STDC__ == 1
#define _ANSI		31459	/* compiler claims full ANSI conformance */
# endif	/* __STDC++ == 1 */

# if	defined(__GNUC__)
#define _ANSI		31459	/* gcc conforms enough even in non-ANSI mode */
# endif	/* defined(__GNUC__) */

# if	defined(_ANSI)

/* Keep everything for ANSI prototypes. */
#define	_PROTOTYPE(function, params)	function params

#define	_VOIDSTAR	void *
#define	_VOID		void
#define	_CONST		const
#define	_VOLATILE	volatile

#if	!defined(_SIZET)
#define _SIZET		size_t
#endif	/* !defined(_SIZET) */

# else	/* !defined(_ANSI) */

/* Throw away the parameters for K&R prototypes. */
#define	_PROTOTYPE(function, params)	function()

#define	_VOIDSTAR	void *
#define	_VOID		void
#define	_CONST
#define	_VOLATILE
#define _SIZET		int

# endif	/* defined(_ANSI) */

#endif	/* !defined(ANSI_H) */