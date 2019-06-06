/*
    Copyright (C) 2011 Raphters authors,
    
    This file is part of Raphters.
    
    Raphters is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Raphters is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "regex.h"
#include "stdlib.h"

#include "qdecoder.h"


#define GET 1
#define POST 2
#define PUT 3
#define HEAD 4
#define DELETE 5
#define PATCH 6

extern void (*error_handler)(const char *);

void pre_call_hook(qentry_t* req) __attribute__ ((weak));
void post_call_hook(qentry_t* req) __attribute__ ((weak));

void pre_authorized_call_hook(qentry_t* req, qentry_t* sess) __attribute__ ((weak));
void post_authorized_call_hook(qentry_t* req, qentry_t* sess) __attribute__ ((weak));


struct handler {
    void (*func)(regmatch_t[]);
    int method;
    const char *regex_str;
    regex_t regex;
    size_t nmatch;
    struct handler *next;
};
typedef struct handler handler;

struct mh {
    char url[256];
    unsigned long size;
    //long size;
    struct mh *next;
    struct mh *prev;
};
typedef struct mh map_handler;

#define START_HANDLER(NAME, METHOD, REGEX, NUM, MATCHES, HEADERS) \
void NAME##_func(); \
handler NAME##_data = {.func=NAME##_func,.method=METHOD,.regex_str=REGEX,.regex={.__buffer=0,.__allocated=0,.__used=0,.__syntax=0,.__fastmap=0,.__translate=0,.__can_be_null=0,.__regs_allocated=0,.__fastmap_accurate=0,.__no_sub=0,.__not_bol=0,.__not_eol=0,.__newline_anchor=0},.nmatch=NUM,.next=NULL};\
handler *NAME = &NAME##_data; \
void NAME##_func(regmatch_t MATCHES[]) { \
        qentry_t *req = qcgireq_parse(NULL, 0); \
        int headers = HEADERS; \
        if(pre_call_hook) pre_call_hook(req); 


#define END_HANDLER \
		if(headers){}; \
		if(post_call_hook) post_call_hook(req); \
        req->free(req); \
}

#define SUCCESS_OUTPUT(JSON_STRING) \
		if(headers) { \
				qcgires_setcontenttype(req, "application/json"); \
		} \
		printf("%s", JSON_STRING);

#define ERROR_OUTPUT(HTTP_STATUS, JSON_ERROR_STRING) \
		printf("Status: %s \n", HTTP_STATUS); \
		if(headers) { \
				qcgires_setcontenttype(req, "application/json"); \
		} \
		printf("%s", JSON_ERROR_STRING);

#define START_HANDLER_AUTHORIZED(NAME, METHOD, REGEX, NUM, MATCHES, HEADERS) \
void NAME##_func(); \
handler NAME##_data = {.func=NAME##_func,.method=METHOD,.regex_str=REGEX,.regex={.__buffer=0,.__allocated=0,.__used=0,.__syntax=0,.__fastmap=0,.__translate=0,.__can_be_null=0,.__regs_allocated=0,.__fastmap_accurate=0,.__no_sub=0,.__not_bol=0,.__not_eol=0,.__newline_anchor=0},.nmatch=NUM,.next=NULL};\
handler *NAME = &NAME##_data; \
void NAME##_func(regmatch_t MATCHES[]) { \
        qentry_t *req = qcgireq_parse(NULL, 0); \
        int headers = HEADERS; \
        char *getenvptr=getenv("HTTP_X_CSRFTOKEN");\
        if(pre_call_hook) pre_call_hook(req); \
        qentry_t *sess = qcgisess_init(req, NULL); \
        if(sess->getint(sess, "authorized")!=1 || getenvptr == NULL || (strncmp(sess->getstr(sess, "CSRFTOKEN", false), getenvptr, strlen(sess->getstr(sess, "CSRFTOKEN", false))) != 0 )) { \
                json_object *jresp; \
                printf("Status: 401 Unauthorized \n"); \
                qcgires_setcontenttype(req, "application/json"); \
                jresp = json_object_new_object(); \
                json_object_object_add(jresp, "cc", json_object_new_int(7)); \
                json_object_object_add(jresp, "error", json_object_new_string("Invalid Authentication")); \
                printf("%s", json_object_to_json_string(jresp)); \
                json_object_put(jresp); \
                goto end_handler_auth; \
        } \
        if(pre_authorized_call_hook) pre_authorized_call_hook(req, sess);


#define END_HANDLER_AUTHORIZED \
end_handler_auth: \
		if(headers){}; \
		if(post_authorized_call_hook) post_authorized_call_hook(req, sess); \
		qcgisess_save(sess); \
		sess->free(sess); \
		        if(post_call_hook) post_call_hook(req); \
		        req->free(req); \
		}

		#define START_HANDLER_AUTHORIZED_POLL(NAME, METHOD, REGEX, NUM, MATCHES, HEADERS) \
		void NAME##_func(); \
		handler NAME##_data = {.func=NAME##_func,.method=METHOD,.regex_str=REGEX,.regex={.__buffer=0,.__allocated=0,.__used=0,.__syntax=0,.__fastmap=0,.__translate=0,.__can_be_null=0,.__regs_allocated=0,.__fastmap_accurate=0,.__no_sub=0,.__not_bol=0,.__not_eol=0,.__newline_anchor=0},.nmatch=NUM,.next=NULL};\
		handler *NAME = &NAME##_data; \
		void NAME##_func(regmatch_t MATCHES[]) { \
		        qentry_t *req = qcgireq_parse(NULL, 0); \
		        int headers = HEADERS; \
		        char *getenvptr=getenv("HTTP_X_CSRFTOKEN");\
		        if(pre_call_hook) pre_call_hook(req); \
		        qcgisess_validate(req);\
		        qentry_t *sess = qcgisess_init(req, NULL); \
		        if(sess->getint(sess, "authorized")!=1 || getenvptr == NULL || (strncmp(sess->getstr(sess, "CSRFTOKEN", false), getenvptr, strlen(sess->getstr(sess, "CSRFTOKEN", false))) != 0 )) { \
		                json_object *jresp; \
		                printf("Status: 401 Unauthorized \n"); \
		                qcgires_setcontenttype(req, "application/json"); \
		                jresp = json_object_new_object(); \
		                json_object_object_add(jresp, "cc", json_object_new_int(7)); \
		                json_object_object_add(jresp, "error", json_object_new_string("Invalid Authentication")); \
		                printf("%s", json_object_to_json_string(jresp)); \
		                json_object_put(jresp); \
		                goto end_handler_auth; \
		        } \
		        if(pre_authorized_call_hook) pre_authorized_call_hook(req, sess);

		#define END_HANDLER_AUTHORIZED_POLL \
		end_handler_auth: \
		        if(headers){}; \
		        if(post_authorized_call_hook) post_authorized_call_hook(req, sess); \
		        pollqcgisess_save(sess); \
        sess->free(sess); \
        if(post_call_hook) post_call_hook(req); \
        req->free(req); \
}


#define START_HANDLER_AUTHORIZED_UPLOAD(NAME, METHOD, REGEX, NUM, MATCHES, HEADERS, BASEPATH, MAXSIZEBYTES, CLEARTIME) \
map_handler mh##NAME = {.url=REGEX,.size=(unsigned long)MAXSIZEBYTES,.next=NULL}; \
void NAME##_func(); \
handler NAME##_data = {.func=NAME##_func,.method=METHOD,.regex_str=REGEX,.regex={.__buffer=0,.__allocated=0,.__used=0,.__syntax=0,.__fastmap=0,.__translate=0,.__can_be_null=0,.__regs_allocated=0,.__fastmap_accurate=0,.__no_sub=0,.__not_bol=0,.__not_eol=0,.__newline_anchor=0},.nmatch=NUM,.next=NULL};\
handler *NAME = &NAME##_data; \
void NAME##_func(regmatch_t MATCHES[]) { \
    int headers = HEADERS; \
    unsigned long clen = 0; \
	char *enviptr=getenv("CONTENT_LENGTH");\
    qentry_t *req = qcgireq_setoption(NULL, true, BASEPATH, CLEARTIME); \
    if(enviptr != NULL) { \
        clen = strtoul(enviptr, NULL, 10); \
    } \
    if(clen<=0) { \
        json_object *jresp_nolen; \
        printf("Status: 411 Length Required \r\n"); \
        qcgires_setcontenttype(req, "application/json"); \
        jresp_nolen = json_object_new_object(); \
        json_object_object_add(jresp_nolen, "cc", json_object_new_int(5)); \
        json_object_object_add(jresp_nolen, "error", json_object_new_string("Invalid Content Length received")); \
        printf("%s", json_object_to_json_string(jresp_nolen)); \
        json_object_put(jresp_nolen); \
        goto end_handler; \
    } \
    if(clen>(unsigned long)MAXSIZEBYTES) { \
        json_object *jresp_lenerr; \
        printf("Status: 413 Request Entity Too Large \r\n"); \
        qcgires_setcontenttype(req, "application/json"); \
        jresp_lenerr = json_object_new_object(); \
        json_object_object_add(jresp_lenerr, "cc", json_object_new_int(6)); \
        json_object_object_add(jresp_lenerr, "error", json_object_new_string("Invalid Content Length received")); \
        printf("%s", json_object_to_json_string(jresp_lenerr)); \
        json_object_put(jresp_lenerr); \
        goto end_handler; \
    } \
	if(req==NULL) qcgires_error(req, "Can't set options."); \
	req = qcgireq_parse(req, 0); \
	if(pre_call_hook) pre_call_hook(req); \
	qentry_t *sess = qcgisess_init(req, NULL); \
	enviptr=getenv("HTTP_X_CSRFTOKEN");\
	if(sess->getint(sess, "authorized")!=1 ||  enviptr == NULL || (strncmp(sess->getstr(sess, "CSRFTOKEN", false), enviptr, strlen(sess->getstr(sess, "CSRFTOKEN", false))) != 0 )) { \
			json_object *jresp; \
			printf("Status: 401 Unauthorized \n"); \
			qcgires_setcontenttype(req, "application/json"); \
			jresp = json_object_new_object(); \
			json_object_object_add(jresp, "cc", json_object_new_int(7)); \
			json_object_object_add(jresp, "error", json_object_new_string("Invalid Authentication")); \
			printf("%s", json_object_to_json_string(jresp)); \
            json_object_put(jresp); \
			goto end_handler_auth_upload; \
	} \
	if(pre_authorized_call_hook) pre_authorized_call_hook(req, sess);
	/*qentry_t *sess = qcgisess_init(req, NULL); \*/
	/*	
        if(sess->getint(sess, "authorized")!=1) { \
                json_object *jresp; \
                jresp = json_object_new_object(); \
                json_object_object_add(jresp, "cc", json_object_new_int(7)); \
                json_object_object_add(jresp, "error", json_object_new_string("Invalid Authentication")); \
                printf("%s", json_object_to_json_string(jresp)); \
                return; \
	}*/


#define END_HANDLER_AUTHORIZED_UPLOAD \
end_handler_auth_upload: \
		if(headers){}; \
		if(post_authorized_call_hook) post_authorized_call_hook(req, sess); \
		qcgisess_save(sess); \
		sess->free(sess); \
end_handler: \
		if(post_call_hook) post_call_hook(req); \
        req->free(req); \
}
        /*sess->free(sess); \*/

void init_handlers();
void cleanup_handlers();

void dispatch();

void add_handler(handler *);

#endif
