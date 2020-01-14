/*
 * Our own RC4 based on the "original" as posted to sci.crypt in 1994 and
 * tweaked for  performance  on  x86-64.   OpenSSL is probably faster for
 * decrypting larger amounts of data but we are more interested in a very
 * fast key setup.  On Intel and AMD x64, I have seen up to 50% speedups.
 *
 * The speed  improvement  (if you see one) is due to OpenSSL's  (or your
 * distributor's) choice of type for RC4_INT. Some systems perform bad if
 * this is defined as char. Others perform bad if it's not. If needed, we
 * could move JOHN_RC4_INT to arch.h
 *
 * Syntax is same as OpenSSL;
 * just #include "rc4.h"  instead of  <openssl/rc4.h>
 *
 * Put together by magnum in 2011, 2013. No Rights Reserved.
 */

#include "rc4.h"

#define swap_byte(a, b) { RC4_INT tmp = (*a); (*a) = (*b); (*b) = tmp; }

#define swap_state(n) { \
	index2 = (kp[index1] + state[(n)] + index2) & 255; \
	swap_byte(&state[(n)], &state[index2]); \
	if (++index1 == keylen) index1 = 0; \
}

void RC4_set_key(RC4_KEY *ctx, RC4_INT keylen, const unsigned char *kp)
{
	RC4_INT index1;
	RC4_INT index2;
	RC4_INT *state;
	int counter;

	state = &ctx->state[0];
	for (counter = 0; counter < 256; counter++)
		state[counter] = counter;
	ctx->x = 0;
	ctx->y = 0;
	index1 = 0;
	index2 = 0;
	for (counter = 0; counter < 256; counter += 4) {
		swap_state(counter);
		swap_state(counter + 1);
		swap_state(counter + 2);
		swap_state(counter + 3);
	}
}

void RC4(RC4_KEY *ctx, RC4_INT len, const unsigned char *in, unsigned char *out)
{
	RC4_INT x;
	RC4_INT y;
	RC4_INT *state;
	RC4_INT counter;

	x = ctx->x;
	y = ctx->y;

	state = &ctx->state[0];
	for (counter = 0; counter < len; counter ++) {
		x = (x + 1) & 255;
		y = (state[x] + y) & 255;
		swap_byte(&state[x], &state[y]);
		*out++ = *in++ ^ state[(state[x] + state[y]) & 255];
	}
	ctx->x = x;
	ctx->y = y;
}

void RC4_single(void *key, unsigned int keylen, const unsigned char *in, int len, unsigned char *out)
{
	unsigned char *kp = (unsigned char*)key;
	unsigned int i;
	RC4_INT x = 0;
	RC4_INT y = 0;
	RC4_INT index1 = 0;
	RC4_INT index2 = 0;
	RC4_INT state[256];

	for (i = 0; i < 256; i += 4) {
		state[i] = i;
		state[i + 1] = i + 1;
		state[i + 2] = i + 2;
		state[i + 3] = i + 3;
	}

	for (i = 0; i < 256; i += 4) {
		swap_state(i);
		swap_state(i + 1);
		swap_state(i + 2);
		swap_state(i + 3);
	}

	while (len--) {
		x = (x + 1) & 255;
		y = (state[x] + y) & 255;
		swap_byte(&state[x], &state[y]);
		*out++ = *in++ ^ state[(state[x] + state[y]) & 255];
	}
}




/*
 *  Name:     crc16Table - crc-16 lookup table
 *
 *  Purpose:  To speed-up CRC-16 calculation
 *
 */
uint16_t crc16Table[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440, 
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40, 
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841, 
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40, 
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41, 
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641, 
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040, 
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240, 
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441, 
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41, 
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840, 
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40, 
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640, 
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041, 
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240, 
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441, 
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840, 
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41, 
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640, 
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041, 
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241, 
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440, 
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841, 
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40, 
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41, 
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

/*
 *  Name:     crc16Gen - Compute CRC
 *
 *  Purpose:  To compute a CRC using x**16 + x**15 + x**2 + 1 polynomial
 *	     generator
 *
 *  Params:
 *	     iDatap:	address of source sequence
 *
 *	     uLength:	length of source sequence
 *
 *  Return:   CRC
 *
 *  Note:     According to the number of information bits and control bits in a
 *	     transmission sequence, this coding method can detect 2 errors. The
 *	     CRC bytes are swapped at the end to maintain consistency with the
 *	     MB-12 convention that the MSB is stored first.
 *
 *  Abort:    None.
 *
 */
uint16_t crc16Gen(uint8_t *iDatap, uint32_t uLength)
{
    uint16_t uCrc, uByte;
    uint32_t i;

    uCrc = 0;
    if (uLength != 0) {
        for (i = 0; i < uLength; i++) {
            uByte = *(uint8_t *) iDatap++;
            uCrc = ((uCrc >> 8) ^ crc16Table[(uByte ^ uCrc) & 0xFF]);
        }
    }
    return (((uCrc >> 8) | (uCrc << 8)) & 0xFFFF);
}

uint16_t crc16Update(uint16_t oriCrc, uint8_t *iDatap, uint32_t uLength)
{
    uint16_t uByte, uCrc;
    uint32_t i;

    uCrc = ((oriCrc >> 8) | (oriCrc << 8)) & 0xFFFF;

    if (uLength != 0) {
        for (i = 0; i < uLength; i++) {
            uByte = *(uint8_t *) iDatap++;
            uCrc = ((uCrc >> 8) ^ crc16Table[(uByte ^ uCrc) & 0xFF]);
        }
    }
    return (((uCrc >> 8) | (uCrc << 8)) & 0xFFFF);
}