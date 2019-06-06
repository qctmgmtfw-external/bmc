 /******************************************************************************
 *
 * Encode.h
 * Contains declaration of wrapper functions for Base32/Base64 Encoding/Decoding
 * algorithm to support backward compatibility.
 *
 *******************************************************************************/

#ifndef __ENCODE_H
#define __ENCODE_H

#include "stdio.h"
#include "string.h"

/*****************************************************************************/
/*
 * Decode a buffer from "string" and into "outbuf"
 * @brief Base64 decoding wrapper function
 * @param[out] outbuf Resulting decoded data
 * @param[in]  string Base64 encoded string
 * @param[in]  outlen Length of the decoded data
 * @return Error code
 */
int Decode64(char *outbuf, char *string, int outlen);

/******************************************************************************/
/*
 * Encode a buffer from "string" into "outbuf"
 * @brief Base64 encoding wrapper function
 * @param[in] string Input data to encode
 * @param[out] outbuf NULL-terminated string encoded with Base64 algorithm
 * @param[in] outlen Length of the encoded string (optional parameter)
 */
void Encode64(char *outbuf, char *string, int outlen);

//******************************************************************************/
/*
 * Encode n character from buffer "string" into "outbuf"
 * @brief Base64 encoding wrapper function
 * @param[out] outbuf NULL-terminated string encoded with Base64 algorithm
 * @param[in]  string Input data to encode
 * @param[in]  outlen Length of the encoded string
 * @param[in]  inlen Length of the input string to encode
 */
void Encode64nChar(char *outbuf, char *string, int outlen, int inlen);

//******************************************************************************/
/*
 * Encode a buffer "in" into "out"
 * @brief Base32 encoding wrapper function
 * @param[out] out NULL-terminated string encoded with Base32 algorithm
 *             -1 on Error
 * @param[in]  in Input data to encode
 * @param[in]  inlen Length of the input string to encode
 */
int Encode32(unsigned char* in, int inLen, unsigned char* out);

//******************************************************************************/
/*
 * Decode a buffer from "in" and into "out"
 * @brief Base32 decoding wrapper function
 * @param[out] out NULL-terminated string decoded with Base32 algorithm
 *             -1 on Error
 * @param[in]  in Input data to decode
 * @param[in]  inLen Length of the input string to decode
 */
int Decode32(unsigned char* in, int inLen, unsigned char* out);

unsigned int  GetEncode32Length(int bytes);

#endif  /* __ENCODE_H */
