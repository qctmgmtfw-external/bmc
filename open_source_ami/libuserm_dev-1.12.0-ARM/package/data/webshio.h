/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* webshio.h
*
******************************************************************/
extern struct spwd *__webspw_dup(const struct spwd *);
//extern void __spw_set_changed(void);
extern int webspw_close(void);
extern int webspw_file_present(void);
extern const struct spwd *webspw_locate(const char *);
extern int webspw_lock(void);
extern int webspw_name(const char *);
extern const struct spwd *webspw_next(void);
extern int webspw_open(int);
extern int webspw_remove(const char *);
extern int webspw_rewind(void);
extern int webspw_unlock(void);
extern int webspw_update(const struct spwd *);
