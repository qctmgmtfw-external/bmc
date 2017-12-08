/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

/* ***************************-*- andrew-c -*-****************************** */
/* Filename:    unix.h                                                       */
/* Author:      Andrew McCallum (andrewm@ami.com)                            */
/* Format:      Indent: 4 spaces, no tabs                                    */
/* Created:     04/14/2003                                                   */
/* Modified:    06/19/2003                                                   */
/* Description: Public headers for the unix functions library.               */
/* ************************************************************************* */

#ifndef UNIX_H__
#define UNIX_H__

#include <stdio.h>
#include <signal.h>
#include <sys/sem.h>

#ifdef __cplusplus
extern "C" {
#endif

/* All of these functions should be exported, but most are not used */
/* by the library itself.                                           */
/*@-exportlocal@*/

/** \file unix.h
    \brief Public headers for the unix functions library
*/


/** \mainpage Documentation for libunix
 *
 *  \author Andrew McCallum (andrewm@ami.com)

 *  \section intro Overview
 *
 *  This library contains some general unix functions which many
 *  applications will need to use.  The library exists so that one well
 *  tested, easy to use version of these functions can exist, rather than
 *  everybody implementing their own version.
*/


/** \name Control and Concurrency */
/** \{ */

/** \brief Obtain a write lock on a file
 *
 *  Obtain exclusive access to an open file.  While this lock is held, any
 *  other calls to \c file_lock_write or \ref file_lock_read will block until
 *  you call \ref file_unlock, or the calling process exits.  This call will
 *  block if any other process holds a lock (read or write) on the file.
 *
 *  \param fd The file descriptor of the file to lock.  If you are using
 *  ANSI C file streams (\c fopen(2) and friends) then use \c fileno(3) to
 *  obtain the file descriptor for your FILE stream.
 *
 *  \retval -1 Failure (Invalid fd, no write permission for file, etc.  See
 *  the man page for \c fcntl(2) for other error conditions.)
 *  \retval 0 Success
 *
 *  \note This function is implemented with \c fcntl(2) for maximum
 *  portability.  Mixing calls to this function with calls to \c flock(2)
 *  is not recommended, and is likely to create confusing bugs on some
 *  systems.
 *
 *  \warning This function will return with an error if the calling process
 *  receives a signal that is handled without the SA_RESTART flag set.  See
 *  \ref connect_signal and \ref connect_signal_with_flags for details.
 *
 *  \bug This function will fail with errno set to \c ENOLCK if the passed
 *  file descriptor references a file with the setgid bit set, but not the
 *  group execution bit set.  This is a failing of \c fcntl(2), not this
 *  library.
 */
/*@unused@*//*@external@*/extern inline int file_lock_write( int fd )
/*@globals errno@*/
/*@modifies errno@*/;


/** \brief Obtain a write lock on a file
 *
 *  As \ref file_lock_write, except that it times out after \a timeout
 *  microseconds.  If the function times out without obtaining the lock,
 *  it will return -1 with errno set to \c EAGAIN
 *
 *  \param fd The file descriptor of the file to lock.  If you are using
 *  ANSI C file streams (\c fopen(2) and friends) then use \c fileno(3) to
 *  obtain the file descriptor for your FILE stream.
 *  \param timeout The maximum time (in microseconds) to wait for the lock
 *  before giving up and returning an error.
 *
 *  \retval -1 Failure (Invalid fd, no write permission for file, etc.  See
 *  the man page for \c fcntl(2) for other error conditions.)
 *  \retval 0 Success
 *
 *  \note This function is implemented with \c fcntl(2) for maximum
 *  portability.  Mixing calls to this function with calls to \c flock(2)
 *  is not recommended, and is likely to create confusing bugs on some
 *  systems.
 *
 *  \warning This function will return with an error if the calling process
 *  receives a signal that is handled without the SA_RESTART flag set.  See
 *  \ref connect_signal and \ref connect_signal_with_flags for details.
 *
 *  \bug This function will fail with errno set to \c ENOLCK if the passed
 *  file descriptor references a file with the setgid bit set, but not the
 *  group execution bit set.  This is a failing of \c fcntl(2), not this
 *  library.
 */
/*@unused@*//*@external@*/extern int
file_lock_write_timeout( int fd, unsigned long timeout )
/*@globals fileSystem, errno@*/
/*@modifies fileSystem, errno@*/;


/** \brief Obtain a read lock on a file
 *
 *  Obtain shared read access to an open file.  While this lock is held, any
 *  calls to \ref file_lock_write will block until you call \ref file_unlock,
 *  or the locking process exits.  Calls to \c file_lock_read by other
 *  processes will succeed while this lock is held.  This call will block
 *  only if another process holds an exclusive write lock
 *  (see \ref file_lock_write).
 *
 *  \param fd The file descriptor of the file to lock.  If you are using
 *  ANSI C file streams (\c fopen(2) and friends) then use \c fileno(3) to
 *  obtain the file descriptor for your FILE stream.
 *
 *  \retval -1 Failure (Invalid fd, no read permission for file, etc.  See
 *  the man page for \c fcntl(2) for other error conditions.)
 *  \retval 0 Success
 *
 *  \note This function is implemented with \c fcntl(2) for maximum
 *  portability.  Mixing calls to this function with calls to \c flock(2)
 *  is not recommended, and is likely to create confusing bugs on some
 *  systems.
 *
 *  \warning This function will return with an error if the calling process
 *  receives a signal that is handled without the SA_RESTART flag set.  See
 *  \ref connect_signal and \ref connect_signal_with_flags for details.
 *
 *  \bug This function will fail with errno set to \c ENOLCK if the passed
 *  file descriptor references a file with the setgid bit set, but not the
 *  group execution bit set.  This is a failing of \c fcntl(2), not this
 *  library.
 */
/*@unused@*//*@external@*/extern inline int file_lock_read( int fd )
/*@globals errno@*/
/*@modifies errno@*/;


/** \brief Unlock a locked file
 *
 *  Unlock a file locked with \ref file_lock_write or \ref file_lock_read.
 *  When this function is called, any processes blocking trying to get a lock
 *  via \ref file_lock_write or \ref file_lock_read will have the chance to
 *  do so, based on the rules described in their function descriptions.
 *
 *  \param fd The file descriptor of the file to unlock.  If you are using
 *  ANSI C file streams (\c fopen(2) and friends) then use \c fileno(3) to
 *  obtain the file descriptor for your FILE stream.
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 *
 *  \note This function is implemented with \c fcntl(2) for maximum
 *  portability.  Mixing calls to this function with calls to \c flock(2)
 *  is not recommended, and is likely to create confusing bugs on some
 *  systems.
 */
/*@unused@*//*@external@*/extern inline int file_unlock( int fd )
/*@globals errno@*/
/*@modifies errno@*/;

/** \brief The maximum length of a semaphore managed using create_mutex */
#define SEM_NAME_MAX        ( (size_t)32 )

/** \brief Mutex Handle Structure
 */

typedef struct {

	char	MutexName [SEM_NAME_MAX];		/* Name of the mutex */
	int	OwnerPID;				/* Process ID that created this mutex */
	int	NestingCount;				/* Nesting Count of mutex lock in the same process */
	int	SemID;					/* Internal Semaphore ID for this Mutex */
	key_t	Key;				//remember the key for the mutex for easy access later
	int	Reserved;				/* Reserved */

} Mutex_T;

/** \brief Macro for Mutex handle declaration
 *  This macro is used to declare mutex handle used by the functions below
 *  This macro declares and initializes the handle with default values
 *  which are used by the mutex functions for error checking.
 */
//#define DECLARE_MUTEX(HANDLE_NAME) Mutex_T HANDLE_NAME = { {0}, -1, 0, -1, 0 }
#define DECLARE_MUTEX(HANDLE_NAME) Mutex_T* HANDLE_NAME

#if 0  // OLD IMPLEMENTATION
/** \brief Create a system wide mutex
 *
 *  Create a mutex and initialize it to the "unlocked" state.  If the mutex
 *  already exists, this function will return a new handle to the already
 *  existing mutex.  The mutex will be accessible to any process running
 *  with the same UID or GID as the caller, and any process running as root.
 *
 *
 *  \param mutex_name A text string containing the name of the mutex to
 *  create.  This string should not be longer than \ref SEM_NAME_MAX
 *  characters long.
 *
 *  \param pHandle pointer to the Mutex Handle structure.
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 *
 *  \note If not destroyed with \ref mutex_destroy, this mutex will persist
 *  after the process exits.
 */
/*@unused@*//*@external@*/extern int mutex_create( Mutex_T *pHandle, char *mutex_name )
/*@globals fileSystem, errno@*/
/*@modifies fileSystem, errno@*/;


/** \brief Destroy a system wide mutex
 *
 *  Destroy an existing mutex.  This function will destroy any mutex created
 *  by a process with the same UID or GID as the process that created it,
 *  and any mutex at all if the calling process is running as root.
 *  The mutex is destroyed even if it is locked by another process, or other
 *  processes are waiting for the lock.  Blocked processes waiting for the
 *  lock will immediately get an error, and future accesses to this mutex
 *  will fail with an error.
 *
 *  \param pHandle The mutex handle created with \ref mutex_create.
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*//*@external@*/extern int mutex_destroy( Mutex_T *pHandle )
/*@globals fileSystem, errno@*/
/*@modifies fileSystem, errno@*/;


/** \brief Lock a mutex
 *
 *  Obtain an exclusive lock on mutex created with \a pHandle.  If the mutex does not
 *  exist (it was not created using \ref mutex_create, or has been already
 *  destroyed using \ref mutex_destroy), this function returns an error.
 *  If the mutex is already locked, \c mutex_lock will block until the mutex
 *  is available. Blocking is based on the timeoutval parameter. If the timeout
 *  is 0 then the it is blocked else this value will be timeout in seconds
 *
 *  \param pHandle The mutex handle created with \ref mutex_create to be locked.
 *  \param timeoutval The timeout value in seconds for blocking. 0 provides
 *                    infinite blocking
 *
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*//*@external@*/extern int mutex_lock( Mutex_T *pHandle, long timeoutval )
/*@globals fileSystem, errno@*/
/*@modifies fileSystem, errno@*/;


/** \brief Unlock a mutex
 *
 *  Release an exclusive lock on the mutex created with handle \a pHandle.  This
 *  function fails only if the mutex is destroyed while the lock is held.
 *
 *  \param pHandle The mutex handle created with \ref mutex_create to be unlocked.
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*//*@external@*/extern int mutex_unlock(  Mutex_T *pHandle )
/*@globals fileSystem, errno@*/
/*@modifies fileSystem, errno@*/;

/** \} */

#endif

extern Mutex_T* mutex_create_recursive(char* MutexName);
extern int mutex_lock_recursive(Mutex_T* pmutex,long timeoutval);
extern int mutex_unlock_recursive(Mutex_T* pmutex);
extern int mutex_destroy_recursive(Mutex_T* pmutex);


/** \name Security Functions */
/** \{ */

/** \brief Get a pseudo-random real number suitable for security applications
 *
 *  This function returns a pseudo-random double between zero and one.
 *  The numbers produced from this function are considered cryptographically
 *  secure.
 *
 *  \return Random number
 *
 *  \note On Linux and most BSD platforms, this function is implemented
 *  through use of /dev/urandom.
 */
/*@unused@*//*@external@*/extern double crypto_rand_real( void )
/*@globals fileSystem@*/
/*@modifies fileSystem@*/;


/** \brief Get a pseudo-random integer suitable for security applications
 *
 *  This function returns a pseudo-random unsigned integer between zero and
 *  \a x - 1.  The numbers produced from this function are considered
 *  cryptographically secure.
 *
 *  \param x The random number generated will be between 0 and \a x - 1.
 *
 *  \return Random number
 *
 *  \note On Linux and most BSD platforms, this function is implemented
 *  through use of /dev/urandom.
 */
/*@unused@*//*@external@*/extern unsigned int crypto_rand_int( unsigned int x )
/*@globals fileSystem@*/
/*@modifies fileSystem@*/;


/** \brief Permanently give up root privileges
 *
 *  Call this function if your program is running with root privileges,
 *  but you will not need them for the remainder of program execution.
 *  For example, if you only need root privileges to open a file, it is
 *  safe to drop root privilege after opening said file - you can continue
 *  to write/read to/from it so long as the file descriptor is open.
 *  Use of this function is encouraged for minimizing damage caused by
 *  exploitation of a security hole.
 *
 *  \retval 0 Success
 *  \retval -1 Failure
 */
/*@unused@*//*@external@*/
extern int discard_root_priv( void )
/*@globals internalState, errno@*/
/*@modifies internalState, errno@*/;


#ifdef _POSIX_SAVED_IDS
/** \brief In a setuid application, temporarily give up root privilege
 *
 *  If your application runs setuid root (or setuid anyone else for that
 *  matter), you can call this application to revert the application's
 *  privilege level back to that of the user running the application (IE,
 *  switch the real UID with the effective UID).  You can revert back to
 *  the previous level of privilege by calling \ref regain_root_priv later.
 *  Use of this function is recommended for use in applications that will
 *  run with an effective UID other than that of the user.  As with
 *  \ref discard_root_priv, you may still continue to read/write from/to
 *  file descriptors opened with higher privileges, and it reduces the
 *  risk of total system compromise in the event that the application is
 *  exploited.
 *
 *  \retval 0 Success
 *  \retval -1 Failure
 *
 *  \warning Internally, the implementation of this function is the same as
 *  for \ref regain_root_priv.  Therefore, calling this function twice is the
 *  same as doing \ref drop_root_priv and then \ref regain_root_priv.  It is
 *  preferable to use \ref drop_root_priv and \ref regain_root_priv separately
 *  however, to make your code easier to understand.  Also, there is no
 *  guarantee that the implementation of these two functions will stay the
 *  same in the future.
 *
 *  \note This function is only available if \c _POSIX_SAVED_IDS is defined
 *  on your system.
 */
/*@unused@*//*@external@*/
extern int drop_root_priv( void )
/*@globals internalState, errno@*/
/*@modifies internalState, errno@*/;


/** \brief In a setuid application, regain root privilege
 *
 *  If your setuid application previously called \ref drop_root_priv, call
 *  this function to regain the previous level of privilege (usually root).
 *  See the description of \ref drop_root_priv for a more detailed discussion
 *  of why using these functions in your setuid application is a good idea.
 *
 *  \retval 0 Success
 *  \retval -1 Failure
 *
 *  \note This function is only available if \c _POSIX_SAVED_IDS is defined
 *  on your system.
 */
/*@unused@*//*@external@*/
extern int regain_root_priv( void )
/*@globals internalState, errno@*/
/*@modifies internalState, errno@*/;
#endif

/** \} */


/** \name Initialization */
/** \{ */

/** \brief Perform cleanup of potentially dangerous user settings
 *
 *  This function resets a number of parameters which are inherited from the
 *  parent process, be it a shell, some launcher app, or a malicious program
 *  using some flavor of \c exec(3).  It clears and resets the environment to
 *  \ref default_environment, sets the umask to make files read/write only for
 *  the user, resets all the signal handlers to their default values, and
 *  prevents core files from being created (to avoid giving away sensitive
 *  data in memory such as passwords if the program crashes).
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 *
 *  \note If you use core files for debugging, you may want to disable use of
 *  this function during debugging.
 */
/*@unused@*//*@external@*/extern int unix_init( void )
/*@globals fileSystem, internalState, errno@*/
/*@modifies fileSystem, internalState, errno@*/;


/** \brief Cause the calling process to become a daemon
 *
 *  The calling process becomes a daemon - a process running in the background
 *  with no controlling terminal.  The calling process is responsible for
 *  closing any unneeded file handles, sockets, etc before calling this
 *  function.
 *
 *  \retval 0 Success
 *  \retval -1 Failure
 */
/*@unused@*//*@external@*/extern int daemon_init( void )
/*@globals internalState, systemState, errno@*/
/*@modifies internalState, systemState, errno@*/;

/** \} */


/** \name Process Invocation */
/** \{ */

/** \brief Make a call to \c system(3) safely
 *
 *  \c system(3) is a dangerous call to make, because it invokes a shell.
 *  A clever user can sometimes find a way to pervert that call and make
 *  it do something unintended by manipulating the application's environment
 *  variables.  The \ref safe_system function resets the environment to
 *  something safe (see \ref default_environment) before running the
 *  \c system(3) command.  Otherwise, it works the same as \c system(3).
 *
 *  \param command A character string containing the shell command to run
 *
 *  \retval "Return status of command" Success
 *  \retval -1 Failure
 *
 *  \note If you just need to run a single command, use \ref spawn_external
 *  instead.  It uses \c execve(2), which does not invoke a shell, and is
 *  altogether safer.
 */
/*@unused@*//*@external@*/extern int safe_system( const char *command )
/*@globals internalState, systemState, errno@*/
/*@modifies internalState, systemState, errno@*/;


/** \brief Make a call to \c popen(3) safely
 *
 *  \c popen(3) is a dangerous call to make for the same reasons \c system(3)
 *  is dangerous.  The \c safe_popen function resets the environment to
 *  something safe (see \c default_environment) before running the command.
 *  Otherwise it works the same as \c popen(3).
 *
 *  \param command A character string containing the shell command to run
 *  \param type Either "r" for reading or "w" for writing.
 *
 *  \retval "A FILE pointer for read or write" Success
 *  \retval NULL Failure
 *
 *  \note You must still call \c pclose(3) when you are finished with the
 *  FILE stream returned by \c safe_popen.
 */
/*@unused@*//*@external@*//*@null@*//*@dependent@*/
extern FILE *safe_popen( const char *command, const char *type )
/*@globals internalState, systemState, errno@*/
/*@modifies internalState, systemState, errno@*/;


/** \brief Spawn an external process
 *
 *  Run some external program, return immediately, and automatically handle
 *  the return value from the process.  IE, you don't have to call one of the
 *  \c wait(2) functions to reap the process status, and no zombies will be
 *  created.
 *
 *  \param file The path and filename of some program to run
 *  \param arg  The first argument to \a file.  Pass NULL if you want to
 *              call \a file with no arguments.
 *  \param ...  Additional arguments to \a file.  Pass NULL as the last argument.
 *
 *  \retval 0 Success
 *  \retval -1 Failure
 *
 *  \warning The last argument MUST be a NULL, so the function knows where the
 *           argument list ends.  For example: \n
 *           spawn_external( "/bin/tar", "xvzf", "somefile.tgz", NULL );
 */
/*@unused@*//*@external@*/
extern int spawn_external( const char *file, const char *arg, ... )
/*@globals internalState, systemState, errno@*/
/*@modifies internalState, systemState, errno@*/;

/** \} */


/** \name PID Functions */
/** \{ */

/** \brief Save the PID of the calling process
 *
 *  Save the PID of the calling process in a file named process_name
 *  in a predefined location (\ref PID_FILE_PATH).  The PID is saved in human
 *  readable format for easy debugging.  The length of \a process_name
 *  should not be greater than \ref MAX_PID_FILE.
 *
 *  \param process_name Name of the calling process
 *
 *  \retval 0 Success
 *  \retval -1 Failure
 */
/*@unused@*//*@external@*/extern int save_pid( char *process_name )
/*@globals systemState, internalState, errno@*/
/*@modifies systemState, errno@*/;


/** \brief Get the PID for process_name
 *
 *  Retrieve the PID for a process whose PID was previously
 *  saved using \ref save_pid.  If the PID was not saved with
 *  \ref save_pid, the function will search for \a process_name
 *  in the running process list if the library was compiled
 *  with \c UNIXTOOLS defined.
 *
 *  \param process_name Name of the process whose PID you want.
 *  \param process_pid Pointer to a pid_t that will be filled by the function
 *                     with the pid of \a process_name.  This variable is
 *                     undefined if \c get_pid_by_name returns -1.
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*//*@external@*/
extern int get_pid_by_name( char *process_name, /*@out@*/pid_t *process_pid )
/*@globals internalState, fileSystem, errno@*/
/*@modifies process_pid, internalState, fileSystem, errno@*/;


/** \brief Delete the saved PID file for a process
 *
 *  Delete the saved PID file for some process with name \a process_name.
 *  This does not have to be the same as the name of the calling process.
 *
 *  \param process_name Name of the process whose PID file you want
 *                      to delete.  This must be the same as when it
 *                      was saved using \ref save_pid.
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*//*@external@*/extern int delete_pid_file( char *process_name )
/*@globals systemState, internalState, errno@*/
/*@modifies systemState, errno@*/;

/** \} */


/** \name Signal Functions */
/** \{ */

/** \brief Connect a handler to a signal
 *
 *  \param signum The signal to which you want to connect a handler
 *  \param handler Function returning void and taking an int argument.
 *                 This function will be called every time signal
 *                 \a signum is recevied by the program.  The int argument
 *                 will have the value \a signum when called.
 *  \param old_action The previous action that was connected to this signal
 *                    is returned if this pointer is non-NULL.  You should
 *                    save the old signal handler if you want to restore
 *                    the default signal handler (IE, if you want to
 *                    unregister a signal handler).
 *
 *  \note The signal handler will stay registered indefinitely - there is no
 *        need to reregister the signal handler every time the handler runs
 *        as with some implementations of the \c signal(2) function.
 *
 *  \note This function registers signal handlers using the \c SA_RESTART
 *  flag.  This means that some system calls that are interrupted by the
 *  handled signal will be restarted automatically.  Ordinarily, this is a
 *  good thing.  If this behaviour is not desired, use
 *  \ref connect_signal_with_flags and do not set \c SA_RESTART in the
 *  flags field.
 *
 *  \warning Please note that there are two signals which cannot be caught
 *  under any circumstances - \c SIGKILL and \c SIGSTOP.
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*/
/*@external@*/extern
int connect_signal( int signum, void (*handler)(int),
                    /*@out@*//*@null@*/struct sigaction *old_action )
/*@globals systemState, errno@*/
/*@modifies systemState, old_action, errno@*/;


/** \brief Connect a handler to a signal with extra flags
 *
 *  \param signum The signal to which you want to connect a handler
 *  \param handler Function returning void and taking an int argument.
 *                 This function will be called every time signal
 *                 \a signum is recevied by the program.  The int argument
 *                 will have the value \a signum when called.
 *  \param flags Bitmask of specific desired behaviours in the signal handler.
 *               See the \c sa_flags field in \c sigaction(2) for details.
 *  \param old_action The previous action that was connected to this signal
 *                    is returned if this pointer is non-NULL.  You should
 *                    save the old signal handler if you want to restore
 *                    the default signal handler (IE, if you want to
 *                    unregister a signal handler).
 *
 *  \warning Please note that there are two signals which cannot be caught
 *  under any circumstances - \c SIGKILL and \c SIGSTOP.
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*/
/*@external@*/extern int
connect_signal_with_flags( int signum, void (*handler)(int), int flags,
                           /*@out@*//*@null@*/struct sigaction *old_action )
/*@globals systemState, errno@*/
/*@modifies systemState, old_action, errno@*/;


/** \brief Connect a handler to all signals
 *
 *  This function connects a single handler to all catchable signals.
 *  If desired, you can save all the old handlers, and restore them later
 *  via \ref restore_all_signals.
 *
 *  \param handler Function returning void and taking an int argument.
 *                 This function will be called every time a signal
 *                 is recevied by the program.  The int argument
 *                 will have the value of the signal received when called.
 *  \param old_actions A pointer to an array of sigaction structs used to
 *                     store the previous signal action data for each signal.
 *                     this should be an array of sigaction structs with \c NSIG
 *                     items, or NULL if you don't want to restore the previous
 *                     signal handlers.
 *
 *  \note The signal handler will stay registered indefinitely - there is no
 *        need to reregister the signal handler every time the handler runs
 *        as with some implementations of the \c signal(2) function.
 *
 *  \note This function registers signal handlers using the \c SA_RESTART
 *  flag.  This means that some system calls that are interrupted by the
 *  handled signal will be restarted automatically.
 *
 *  \warning Please note that there are two signals which cannot be caught
 *  under any circumstances - \c SIGKILL and \c SIGSTOP.
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*/
/*@external@*/extern
int catch_all_signals( void (*handler)(int),
                       /*@out@*//*@null@*/struct sigaction *old_actions )
/*@globals systemState, errno@*/
/*@modifies systemState, old_actions, errno@*/;


/** \brief Disconnect a handler from a signal
 *
 *  Disconnect a handler from a signal previously connected via
 *  \ref connect_signal.  You need to have access to the previous
 *  sigaction struct (IE, you didn't pass a NULL for \a old_action
 *  when you called \ref connect_signal) to use this function.
 *
 *  \param signum The signal whose handler you are disconnecting
 *  \param old_action The previous action that was connected to this signal.
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*//*@external@*/extern
int disconnect_signal( int signum, struct sigaction *old_action )
/*@globals systemState, errno@*/
/*@modifies systemState, errno@*/;


/** \brief Restore all signal handlers to their previous values
 *
 *  This function can be called after a call to \ref catch_all_signals
 *  to restore the previous signal handlers to all signals.  You need
 *  the array of sigaction structs used when you called
 *  \ref catch_all_signals (IE, you didn't pass a NULL for \a old_actions
 *  when you called \ref catch_all_signals) to use this function.
 *
 *  \param old_actions The array of previous actions that were connected
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*//*@external@*/extern
int restore_all_signals( struct sigaction *old_actions )
/*@globals systemState, errno@*/
/*@modifies systemState, errno@*/;


/** \brief Send a signal to a PID
 *
 *  Send signal \a signum to the process with PID \a process.
 *  This is basically a wrapper around the \c kill(2) system call.
 *
 *  \param process The pid of the process you wish to signal
 *  \param signum The signal you wish to send
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@external@*//*@unused@*/extern int send_signal( pid_t process, int signum )
/*@globals errno@*/
/*@modifies errno@*/;


/** \brief Send a signal to a process
 *
 *  Send signal \a signum to the process named \a process.
 *  This function will attempt to read the PID from the PID file
 *  (set through \ref save_pid()) if it can, and fall back to searching
 *  for the PID in the process list if UNIXTOOLS is defined at library
 *  compile time.
 *
 *  \param process The name of a process you want to signal
 *  \param signum The signal you wish to send
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*//*@external@*/extern
int send_signal_by_name( char *process, int signum )
/*@globals internalState, systemState, errno@*/
/*@modifies internalState, systemState, errno@*/;

/** \} */

/** \name Sleep Functions */
/** \{ */

/* These functions call nanosleep, which is a part of POSIX.1b, but not yet */
/* supported by splint.  Because nanosleep modifies errno, it should be in  */
/* the globals and modifies lists.  Splint doesn't know that, so we have to */
/* suppress the globals use warning.                                        */


/** \brief Sleep without interruptions
 *
 *  Sleep for a number of seconds, restarting if interrupted by a signal.
 *
 *  \param seconds The number of whole seconds to sleep
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@-globuse@*/
/*@unused@*//*@external@*/extern
void deep_sleep( long int seconds )
/*@globals internalState, errno@*/
/*@modifies internalState, errno@*/;


/** \brief Sleep without interruptions
 *
 *  Sleep for a number of microseconds, restarting if interrupted by a signal.
 *
 *  \param useconds The number of microseconds to sleep
 *
 *  \retval -1 Failure
 *  \retval 0 Success
 */
/*@unused@*//*@external@*/extern
void deep_usleep( unsigned long useconds )
/*@globals internalState, errno@*/
/*@modifies internalState, errno@*/;


/**
 * @fn IsMounted
 * @brief Find whether the given directory is mounted to given file
 * 			type with given mount option
 * @param[in] pMntDir - Mount directory to verify.
 * @param[in] pFileType - File type (Optional).
 * @param[in] pMntOpt - Mount options (Optional).
 * @return  1 if Mounted,
 * 			0 if not Mounted.
 */
extern int
IsMounted (const char *pMntDir, const char *pFileType, const char *pMntOpt);

/*@=globuse@*/

/*@=exportlocal@*/

#ifdef __cplusplus
}
#endif

#endif
