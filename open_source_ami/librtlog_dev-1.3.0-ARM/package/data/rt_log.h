#ifndef _RT_LOG_H
#define _RT_LOG_H

typedef enum _level {
	L_DEBUG = 1,
	L_INFO,
	L_ERROR,
	L_CRIT
} m_level;

/*!--------------------------------------------------------------
 * @fn          rt_log_init
 * @brief       Initializes the logging routine.Calling this mutiple times initializes the lib only once
 * 				but increases the reference count. Hence, rt_log_uninit() needs to be called number of times
 * 				init was called
 * @return      returns 0 on success and -1 on failure
 *-----------------------------------------------------------------
*/
int rt_log_init();

/*!--------------------------------------------------------------
 * @fn          _rt_log
 * @brief       Do not call this function directly, insted call rt_log
 * @param[in]   file - name of file where this statement is called
  				lineno - line no of file where this statement is called
  				module - name of module; should be same as given in file
		     	submod - name of sub module inside given module. it can go deeper using '.'
				like ibmc.web, etc.
		     	level - one of L_DEBUG, L_INFO, L_ERROR, L_CRIT
						choosing a level in file displays logs from that level
		     	fmt and ... format string and variable args like in printf
 * @param[out]  NONE
 * @return      returns number of characters logged
 *-----------------------------------------------------------------
*/
int _rt_log(char *file,
			 int lineno,
			 char *module,
			 char *submod,
			 m_level level,
			 const char* fmt, ...);

/*!--------------------------------------------------------------
 * @fn          rt_log
 * @brief       This function logs the given statement based on the module and level given
 * @param[in]   module - name of module; should be same as given in file
		     	submod - name of sub module inside given module. it can go deeper using '.'
				like ibmc.web, etc.
		     	level - one of L_DEBUG, L_INFO, L_ERROR, L_CRIT
						choosing a level in file displays logs from that level
		     	fmt and ... format string and variable args like in printf
 * @param[out]  NONE
 * @return      returns number of characters logged
 *-----------------------------------------------------------------
*/
#define rt_log(module, submod, level, ...)	\
	_rt_log(__FILE__, __LINE__, module, submod, level, __VA_ARGS__)

#endif

/*!--------------------------------------------------------------
 * @fn          rt_log_init
 * @brief       Uninitializes the logging routine. Frees any allocated structures
 * @return      returns 0 on success and -1 on failure
 *-----------------------------------------------------------------
*/
int rt_log_uninit();
