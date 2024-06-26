/**
 ******************************************************************************
 * @file    aes.h
 * @author  ASR Tremo Team
 * @version v1.6.2
 * @date    2022-05-28
 * @brief   Header file of crypto module.
 * @addtogroup Tremo_Drivers
 * @{
 * @addtogroup CRYPTO
 * @{
 * @defgroup AES
 * @{
 */

#ifndef __AES_H__
#define __AES_H__
#include "type_define.h"
#include "algorithm.h"

/**
 * @brief AES status definition
 */
enum AES_ENUM
{
    AESSuccess = 0, /*!< success*/
	AESNull,        /*!< null*/
	AESLengthError, /*!< length error*/
	AESLengthZero,  /*!< length is zero*/
	AESStateError,  /*!< state error*/
	AESSeedError,   /*!< seed error*/
    AESKeyLenError, /*!< length of key is not 16, 24 or 32 words*/
	AESCloseError,  /*!< close error*/
	AESAttacked     /*!< attacked*/
};

/**
 * @brief AES code mode
 */
enum AES_ENC_DEC{
    AES_ENC_MODE =0, /*!< encode mode*/
    AES_DEC_MODE,    /*!< decode mode*/
};

/**
 * @brief AES ECB or CBC mode
 */
enum AES_ECB_CBC{
    AES_ECB_MODE = 0, /*!< ECB mode*/
    AES_CBC_MODE,    /*!< CBC mode*/
	AES_OFB_MODE,
	AES_CTR_MODE
};

/**
 * @brief  AES initialize
 * @param  key secret key
 * @param  keymod key length.
 *            @arg 16: 128bit
 *            @arg 24: 192bit
 *            @arg 32: 256bit
 * @param  mode AES mode
 *            @arg 0: ECB
 *            @arg 1: CBC
 *            @arg 2: OFB
 *            @arg 3: CTR
 * @param  IVorNonce IV or NONCE,must be 16 bytes,only needed in CBC mode
 * @retval AESSuccess success
 * @retval AESNull null
 * @retval AESLengthError length error
 * @retval AESLengthZero length is zero
 * @retval AESStateError state error
 * @retval AESSeedError seed error
 * @retval AESKeyLenError length of key is not 16, 24 or 32 words
 * @retval AESCloseError close error
 * @retval AESAttacked attacked
 */
uint8_t aes_init(uint8_t* key, uint8_t keymod,uint8_t mode,uint8_t* IVorNonce);

 /**
 * @brief  AES encryption/decryption
 * @param  in input buffer
 * @param  inLen input length,must be integer multiples of 16
 * @param  En_De encode or decode
 *            @arg 0: encode
 *            @arg 1: decode
 * @param  out output buffer
 * @retval AESSuccess success
 * @retval AESNull null
 * @retval AESLengthError length error
 * @retval AESLengthZero length is zero
 * @retval AESStateError state error
 * @retval AESSeedError seed error
 * @retval AESKeyLenError length of key is not 16, 24 or 32 words
 * @retval AESCloseError close error
 * @retval AESAttacked attacked
 */
uint8_t aes_crypto(uint8_t* in, uint16_t inLen, uint8_t En_De, uint8_t*  out);

 /**
 * @brief  AES close
 * @retval AESSuccess success
 * @retval AESNull null
 * @retval AESLengthError length error
 * @retval AESLengthZero length is zero
 * @retval AESStateError state error
 * @retval AESSeedError seed error
 * @retval AESKeyLenError length of key is not 16, 24 or 32 words
 * @retval AESCloseError close error
 * @retval AESAttacked attacked
 */
uint8_t aes_close(void);

#endif /* ifndef __AES_H__ */

/**
 * @} 
 * @}
 * @}
 */
