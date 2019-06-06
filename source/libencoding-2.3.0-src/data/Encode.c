 /****************************************************************
 *
 * Encode.c
 * Contains wrapper functions for Base32/Base64 Encoding/Decoding
 * algorithm to support backward compatibility.
 *
 *****************************************************************/


#include "string.h"
#include "stdio.h"

#include "Encode.h"
#include "CyoDecode.h"
#include "CyoEncode.h"

#define REQ_TEMP_BUFF_SIZE( X )  (X % 8) != 0 ? X + ( 8 - ( X % 8 )): X
#define MAX_BUF_SIZE    256
/******************************************************************************/
/*
 * Encode a buffer from "string" into "outbuf"
 * @brief Base64 encoding wrapper function
 * @param[in] string Input data to encode
 * @param[out] outbuf NULL-terminated string encoded with Base64 algorithm
 * @param[in] outlen Length of the encoded string (optional parameter)
 */

void Encode64(char *outbuf, char *string, int outlen)
{
    Encode64nChar(outbuf, string, outlen, strlen(string));
}

//******************************************************************************/
/*
 * Encode n character from buffer "string" into "outbuf"
 * @brief Base64 encoding wrapper function
 * @param[out] outbuf NULL-terminated string encoded with Base64 algorithm
 * @param[in]  string Input data to encode
 * @param[in]  outlen Length of the encoded string
 * @param[in]  inlen Length of the input string to encode
 */

void Encode64nChar(char *outbuf, char *string, int outlen, int inlen)
{
    if ( NULL != string && NULL != outbuf && 0 != outlen)
    {
        if ( 0 == inlen ) {
            *outbuf = '\0'; 
            return; 
        }

        if ( outlen >= ( 4*((inlen+2)/3) ) )
        {
            cyoBase64Encode(outbuf, string, inlen);
            outbuf[outlen] = '\0';
        }
        else  
        {
            printf("%s() Insufficient out buffer for inLen=%d outLen=%d (need=%d)\n", __FUNCTION__, inlen, outlen, (4*((inlen+2)/3)) ); 
        }
    }
}

/*****************************************************************************/
/*
 * Decode a buffer from "string" and into "outbuf"
 * @brief Base64 decoding wrapper function
 * @param[out] outbuf Resulting decoded data
 * @param[in]  string Base64 encoded string
 * @param[in]  outlen Length of the decoded data
 * @return code: -1 Error or
 *               length of output buffer
 */
int Decode64(char *outbuf, char *string, int outlen)
{
    int ret = 0;
//  int inputlength = 0;

    if ( NULL == string || NULL == outbuf)
        return -1;

    if ( 0 == strlen(string) ) { 
        *outbuf = '\0'; 
        return 0; 
    } 

#if 0
    inputlength = strlen(string);
    if (outlen < ( 3*((inputlength+3)/4) ) ) {
        printf("%s() Insufficient buffer for InLen=%d Outlen=%d (need=%d)\n", __FUNCTION__, inputlength, outlen, ( 3*((inputlength+3)/4) ));
        return -1;
    }
#endif

    ret = cyoBase64Decode(outbuf, string, strlen(string));
    outbuf[outlen] = '\0';

    return ret;
}


//******************************************************************************/
/*
 * Encode a buffer "in" into "out"
 * @brief Base32 encoding wrapper function
 * @param[out] out NULL-terminated string encoded with Base32 algorithm
 *             -1 on Error
 * @param[in]  in Input data to encode
 * @param[in]  inlen Length of the input string to encode
 */

int Encode32(unsigned char* in, int inLen, unsigned char* out)
{
    int ret = 0;

    if ( !in || !out )
        return -1;

    if ( 0 == inLen ) {
        *out = '\0'; 
        return 0; 
    }

    ret = cyoBase32Encode((char *)out, in, inLen);
    out[GetEncode32Length(inLen)] = '\0';

    return ret;
}


//******************************************************************************/
/*
 * Decode a buffer from "in" and into "out"
 * @brief Base32 decoding wrapper function
 * @param[out] out NULL-terminated string decoded with Base32 algorithm
 *             -1 on Error
 * @param[in]  in Input data to decode
 * @param[in]  inlen Length of the input string to decode
 */

int Decode32(unsigned char* in, int inLen, unsigned char* out)
{
    int ret = 0;
    int temp_len = REQ_TEMP_BUFF_SIZE(inLen);
    unsigned char temp_in[ MAX_BUF_SIZE ] = {0}; 

    if ( !in || !out )
        return 0;

    if ( 0 == inLen ) {
        *out = '\0'; 
        return 1; 
    }

    memset( temp_in, 0, temp_len );
    memcpy( temp_in, in, inLen);
    memset( temp_in + inLen, '=', ( temp_len - inLen ));        // making the input buffer length multiple of 8

    ret = cyoBase32Decode(out, (char *)temp_in, temp_len);

    return ret;
}

unsigned int GetEncode32Length(int bytes)
{
    int bits = bytes * 8;
    unsigned int length = bits / 5;

    if((bits % 5) > 0)
    {
        length++;
    }

    return length;
}
