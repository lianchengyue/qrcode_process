#ifndef BASE64_H
#define BASE64_H

/*************************************************
Function:       base64_to_ascii
Description:    decodes string from base64 to ascii
Input:          const char *in: the input string (NIL-terminated)
int inlen: length of input string
int maxlen: the output buffer size limit, 0 to ignore
Output:         unsigned char *out: decoded output string
Return:         length of output string on successful
less than 0 on error occur, then the output is invalid
Others:
 *************************************************/
int base64_decode(unsigned char *out, const char *in, int inlen, int maxlen);

/*************************************************
Function:       ascii_to_base64
Description:    encodes string from ascii to base64
Input:          const char *in: the input string (NIL-terminated)
int inlen: length of input string
int maxlen: the output buffer size limit, 0 to ignore
Output:         unsigned char *out: decoded output string
Return:         length of output string on successful
less than 0 on error occur, then the output is invalid
Others:
 *************************************************/
int base64_encode(char *out, const unsigned char *in, int inlen, int maxlen);


int get_length_after_base64(FILE * fp_in);
//encode
void encode(FILE * fp_in, FILE * fp_out);
void encode(FILE * fp_in, FILE * fp_out, char *dir, char* filename);
void encode(FILE * fp_in, char *str_out);
//decode
void decode(FILE * fp_in, FILE * fp_out);
void decode(char *str_in, FILE * fp_out);

#endif // BASE64_H
