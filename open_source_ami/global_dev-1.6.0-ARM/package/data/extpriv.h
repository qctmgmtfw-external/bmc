#ifndef _RAC_EXT_PRIV_H_
#define _RAC_EXT_PRIV_H_

#define RAC_LOGIN_USER                    0x80000001
#define RAC_CARD_CONFIG_ADMIN             0x80000002
#define RAC_USER_CONFIG_ADMIN             0x80000004
#define RAC_LOG_CLEAR_ADMIN               0x80000008
#define RAC_SERVER_RESET_POWERONOFF_USER  0x80000010
#define RAC_CONSOLE_REDIRECT_USER         0x80000020
#define RAC_VIRTUAL_MEDIA_USER            0x80000040
#define RAC_TEST_ALERT_USER               0x80000080
#define RAC_DEBUG_CMD_ADMIN               0x80000100
#define RAC_PRIV_VALID					  0x80000000

#define HAS_EXT_PRIV(input_priv,priv_to_check)       \
      ( ((input_priv)&RAC_PRIV_VALID) && (((input_priv) & (priv_to_check)) == (priv_to_check)) )


#endif

