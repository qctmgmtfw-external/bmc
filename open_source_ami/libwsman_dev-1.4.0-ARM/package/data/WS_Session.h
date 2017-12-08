#ifndef _SESSION_H
#define _SESSION_H

#include <time.h>
#include <stdint.h>

#define UNIQ_STR_LEN (36 + 1)

typedef struct Session
{
	char username[128];
	char password[128];

	char user_ip[32];
	int user_port;

	uint16_t id;
	char unique_text[UNIQ_STR_LEN];		// a unique random string for this session

	// tells whether this session can expire? this is only for increase_session_expiry and resume_session_expiry during
	// firmware upload as it takes this.
	// TODO: This is IBM specific. Should move to common
	uint8_t can_expire;

	unsigned long int expiry_time; 		// in sec --> from cim
	unsigned long int counter; 			// in sec --> maintained locally
	int password_expired;				// true(1) / false(0) whether this user's password has expired

	uint8_t active;		// stores the number of requests currently being processed in this session

	//User's initialize handle and cleanup
	void *handle;
	void *cim_handle;	// cim handle returned after call to client_config

	// it contains checksum of the top node in the drawing order
	int drawing_order;

} SESSION_T;

extern unsigned int g_sessions_length;
extern SESSION_T *g_sessions[];


// creates session and returns newly created session id
SESSION_T *create_session(char *username, char *password);
void update_counter();
int update_session(SESSION_T *session);

int has_session_expired(SESSION_T *session);

int delete_session(SESSION_T *session);

SESSION_T *get_session(char *stext);

#endif //_SESSION_H

