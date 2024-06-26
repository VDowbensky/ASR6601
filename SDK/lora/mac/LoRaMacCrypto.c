/*!
 * \file      LoRaMacCrypto.c
 *
 * \brief     LoRa MAC layer cryptography implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "utilities.h"
#include "secure-element.h"

#include "LoRaMacParser.h"
#include "LoRaMacSerializer.h"
#include "LoRaMacCrypto.h"


/*
 * Frame direction definition for uplink communications
 */
#define UPLINK                          0

/*
 * Frame direction definition for downlink communications
 */
#define DOWNLINK                        1

/*
 * CMAC/AES Message Integrity Code (MIC) Block B0 size
 */
#define MIC_BLOCK_BX_SIZE               16

/*
 * Number of security context entries
 */
#define NUM_OF_SEC_CTX                  5

/*
 * Maximum size of the message that can be handled by the crypto operations
 */
#define CRYPTO_MAXMESSAGE_SIZE          256

/*
 * Maximum size of the buffer for crypto operations
 */
#define CRYPTO_BUFFER_SIZE              CRYPTO_MAXMESSAGE_SIZE + MIC_BLOCK_BX_SIZE

/*
 * Key-Address item
 */
typedef struct sKeyAddr
{
    /*
     * Address identifier
     */
    AddressIdentifier_t AddrID;
    /*
     * Application session key
     */
    KeyIdentifier_t AppSkey;
    /*
     * Network session key
     */
    KeyIdentifier_t NwkSkey;
    /*
     * Rootkey (Multicast only)
     */
    KeyIdentifier_t RootKey;
}KeyAddr_t;

/*
 * Non volatile module context.
 */
static LoRaMacCryptoNvmData_t* CryptoNvm;

/*
 * Key-Address list
 */
static KeyAddr_t KeyAddrList[NUM_OF_SEC_CTX] =
    {
        { MULTICAST_0_ADDR, MC_APP_S_KEY_0, MC_NWK_S_KEY_0, MC_KEY_0 },
        { MULTICAST_1_ADDR, MC_APP_S_KEY_1, MC_NWK_S_KEY_1, MC_KEY_1 },
        { MULTICAST_2_ADDR, MC_APP_S_KEY_2, MC_NWK_S_KEY_2, MC_KEY_2 },
        { MULTICAST_3_ADDR, MC_APP_S_KEY_3, MC_NWK_S_KEY_3, MC_KEY_3 },
        { UNICAST_DEV_ADDR, APP_S_KEY, S_NWK_S_INT_KEY, NO_KEY }
    };

/*
 * Encrypts the payload
 *
 * \param[IN]  keyID            - Key identifier
 * \param[IN]  address          - Address
 * \param[IN]  dir              - Frame direction ( Uplink or Downlink )
 * \param[IN]  frameCounter     - Frame counter
 * \param[IN]  size             - Size of data
 * \param[IN/OUT]  buffer       - Data buffer
 * \retval                      - Status of the operation
 */
static LoRaMacCryptoStatus_t PayloadEncrypt( uint8_t* buffer, int16_t size, KeyIdentifier_t keyID, uint32_t address, uint8_t dir, uint32_t frameCounter )
{
    if( buffer == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    uint8_t bufferIndex = 0;
    uint16_t ctr = 1;
    uint8_t sBlock[16] = { 0 };
    uint8_t aBlock[16] = { 0 };

    aBlock[0] = 0x01;

    aBlock[5] = dir;

    aBlock[6] = address & 0xFF;
    aBlock[7] = ( address >> 8 ) & 0xFF;
    aBlock[8] = ( address >> 16 ) & 0xFF;
    aBlock[9] = ( address >> 24 ) & 0xFF;

    aBlock[10] = frameCounter & 0xFF;
    aBlock[11] = ( frameCounter >> 8 ) & 0xFF;
    aBlock[12] = ( frameCounter >> 16 ) & 0xFF;
    aBlock[13] = ( frameCounter >> 24 ) & 0xFF;

    while( size > 0 )
    {
        aBlock[15] = ctr & 0xFF;
        ctr++;
        if( SecureElementAesEncrypt( aBlock, 16, keyID, sBlock ) != SECURE_ELEMENT_SUCCESS )
        {
            return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
        }

        for( uint8_t i = 0; i < ( ( size > 16 ) ? 16 : size ); i++ )
        {
            buffer[bufferIndex + i] = buffer[bufferIndex + i] ^ sBlock[i];
        }
        size -= 16;
        bufferIndex += 16;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

/*
 * Prepares B0 block for cmac computation.
 *
 * \param[IN]  msgLen         - Length of message
 * \param[IN]  keyID          - Key identifier
 * \param[IN]  isAck          - True if it is a acknowledge frame ( Sets ConfFCnt in B0 block )
 * \param[IN]  devAddr        - Device address
 * \param[IN]  dir            - Frame direction ( Uplink:0, Downlink:1 )
 * \param[IN]  fCnt           - Frame counter
 * \param[IN/OUT]  b0         - B0 block
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t PrepareB0( uint16_t msgLen, KeyIdentifier_t keyID, bool isAck, uint8_t dir, uint32_t devAddr, uint32_t fCnt, uint8_t* b0 )
{
    if( b0 == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    b0[0] = 0x49;

    if( ( isAck == true ) && ( dir == DOWNLINK ) )
    {
        // confFCnt contains the frame counter value modulo 2^16 of the "confirmed" uplink or downlink frame that is being acknowledged
        uint16_t confFCnt = 0;

        confFCnt = ( uint16_t )( CryptoNvm->FCntList.FCntUp % 65536 );

        b0[1] = confFCnt & 0xFF;
        b0[2] = ( confFCnt >> 8 ) & 0xFF;
    }
    else
    {
        b0[1] = 0x00;
        b0[2] = 0x00;
    }

    b0[3] = 0x00;
    b0[4] = 0x00;

    b0[5] = dir;

    b0[6] = devAddr & 0xFF;
    b0[7] = ( devAddr >> 8 ) & 0xFF;
    b0[8] = ( devAddr >> 16 ) & 0xFF;
    b0[9] = ( devAddr >> 24 ) & 0xFF;

    b0[10] = fCnt & 0xFF;
    b0[11] = ( fCnt >> 8 ) & 0xFF;
    b0[12] = ( fCnt >> 16 ) & 0xFF;
    b0[13] = ( fCnt >> 24 ) & 0xFF;

    b0[14] = 0x00;

    b0[15] = msgLen & 0xFF;

    return LORAMAC_CRYPTO_SUCCESS;
}

/*
 * Computes cmac with adding B0 block in front.
 *
 *  cmac = aes128_cmac(keyID, B0 | msg)
 *
 * \param[IN]  msg            - Message to compute the integrity code
 * \param[IN]  len            - Length of message
 * \param[IN]  keyID          - Key identifier
 * \param[IN]  isAck          - True if it is a acknowledge frame ( Sets ConfFCnt in B0 block )
 * \param[IN]  devAddr        - Device address
 * \param[IN]  dir            - Frame direction ( Uplink:0, Downlink:1 )
 * \param[IN]  fCnt           - Frame counter
 * \param[OUT] cmac           - Computed cmac
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t ComputeCmacB0( uint8_t* msg, uint16_t len, KeyIdentifier_t keyID, bool isAck, uint8_t dir, uint32_t devAddr, uint32_t fCnt, uint32_t* cmac )
{
    if( ( msg == 0 ) || ( cmac == 0 ) )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }
    if( len > CRYPTO_MAXMESSAGE_SIZE )
    {
        return LORAMAC_CRYPTO_ERROR_BUF_SIZE;
    }

    uint8_t micBuff[MIC_BLOCK_BX_SIZE];

    // Initialize the first Block
    PrepareB0( len, keyID, isAck, dir, devAddr, fCnt, micBuff );

    if( SecureElementComputeAesCmac( micBuff, msg, len, keyID, cmac ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }
    return LORAMAC_CRYPTO_SUCCESS;
}

/*!
 * Verifies cmac with adding B0 block in front.
 *
 * \param[IN]  msg            - Message to compute the integrity code
 * \param[IN]  len            - Length of message
 * \param[IN]  keyID          - Key identifier
 * \param[IN]  isAck          - True if it is a acknowledge frame ( Sets ConfFCnt in B0 block )
 * \param[IN]  devAddr        - Device address
 * \param[IN]  dir            - Frame direction ( Uplink:0, Downlink:1 )
 * \param[IN]  fCnt           - Frame counter
 * \param[in]  expectedCmac   - Expected cmac
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t VerifyCmacB0( uint8_t* msg, uint16_t len, KeyIdentifier_t keyID, bool isAck, uint8_t dir, uint32_t devAddr, uint32_t fCnt, uint32_t expectedCmac )
{
    if( msg == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }
    if( len > CRYPTO_MAXMESSAGE_SIZE )
    {
        return LORAMAC_CRYPTO_ERROR_BUF_SIZE;
    }

    uint8_t micBuff[CRYPTO_BUFFER_SIZE];
    memset1( micBuff, 0, CRYPTO_BUFFER_SIZE );

    // Initialize the first Block
    PrepareB0( len, keyID, isAck, dir, devAddr, fCnt, micBuff );

    // Copy the given data to the mic computation buffer
    memcpy1( ( micBuff + MIC_BLOCK_BX_SIZE ), msg, len );

    SecureElementStatus_t retval = SECURE_ELEMENT_ERROR;
    retval = SecureElementVerifyAesCmac( micBuff, ( len + MIC_BLOCK_BX_SIZE ), expectedCmac, keyID );

    if( retval == SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_SUCCESS;
    }
    else if( retval == SECURE_ELEMENT_FAIL_CMAC )
    {
        return LORAMAC_CRYPTO_FAIL_MIC;
    }

    return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
}

/*
 * Gets security item from list.
 *
 * \param[IN]  addrID          - Address identifier
 * \param[OUT] keyItem        - Key item reference
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t GetKeyAddrItem( AddressIdentifier_t addrID, KeyAddr_t** item )
{
    for( uint8_t i = 0; i < NUM_OF_SEC_CTX; i++ )
    {
        if( KeyAddrList[i].AddrID == addrID )
        {
            *item = &( KeyAddrList[i] );
            return LORAMAC_CRYPTO_SUCCESS;
        }
    }
    return LORAMAC_CRYPTO_ERROR_INVALID_ADDR_ID;
}

/*
 * Derives a session key as of LoRaWAN versions prior to 1.1.0
 *
 * \param[IN]  keyID          - Key Identifier for the key to be calculated
 * \param[IN]  joinNonce      - Sever nonce
 * \param[IN]  netID          - Network Identifier
 * \param[IN]  deviceNonce    - Device nonce
 * \retval                    - Status of the operation
 */
static LoRaMacCryptoStatus_t DeriveSessionKey10x( KeyIdentifier_t keyID, uint32_t joinNonce, uint32_t netID, uint16_t devNonce )
{
    uint8_t compBase[16] = { 0 };

    switch( keyID )
    {
        case F_NWK_S_INT_KEY:
        case S_NWK_S_INT_KEY:
        case NWK_S_ENC_KEY:
            compBase[0] = 0x01;
            break;
        case APP_S_KEY:
            compBase[0] = 0x02;
            break;
        default:
            return LORAMAC_CRYPTO_ERROR_INVALID_KEY_ID;
    }

    compBase[1] = ( uint8_t )( ( joinNonce >> 0 ) & 0xFF );
    compBase[2] = ( uint8_t )( ( joinNonce >> 8 ) & 0xFF );
    compBase[3] = ( uint8_t )( ( joinNonce >> 16 ) & 0xFF );

    compBase[4] = ( uint8_t )( ( netID >> 0 ) & 0xFF );
    compBase[5] = ( uint8_t )( ( netID >> 8 ) & 0xFF );
    compBase[6] = ( uint8_t )( ( netID >> 16 ) & 0xFF );

    compBase[7] = ( uint8_t )( ( devNonce >> 0 ) & 0xFF );
    compBase[8] = ( uint8_t )( ( devNonce >> 8 ) & 0xFF );

    if( SecureElementDeriveAndStoreKey( compBase, NWK_KEY, keyID ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

/*
 * Gets the last received frame counter
 *
 * \param[IN]     fCntID       - Frame counter identifier
 * \param[IN]     lastDown     - Last downlink counter value
 *
 * \retval                     - Status of the operation
 */
static LoRaMacCryptoStatus_t GetLastFcntDown( FCntIdentifier_t fCntID, uint32_t* lastDown )
{
    if( lastDown == NULL )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }
    switch( fCntID )
    {
        case N_FCNT_DOWN:
            *lastDown = CryptoNvm->FCntList.NFCntDown;
            CryptoNvm->LastDownFCnt = CryptoNvm->FCntList.NFCntDown;
            break;
        case A_FCNT_DOWN:
            *lastDown = CryptoNvm->FCntList.AFCntDown;
            CryptoNvm->LastDownFCnt = CryptoNvm->FCntList.AFCntDown;
            break;
        case FCNT_DOWN:
            *lastDown = CryptoNvm->FCntList.FCntDown;
            CryptoNvm->LastDownFCnt = CryptoNvm->FCntList.FCntDown;
            break;
#if ( LORAMAC_MAX_MC_CTX > 0 )
        case MC_FCNT_DOWN_0:
            *lastDown = CryptoNvm->FCntList.McFCntDown[0];
            break;
#endif
#if ( LORAMAC_MAX_MC_CTX > 1 )
        case MC_FCNT_DOWN_1:
            *lastDown = CryptoNvm->FCntList.McFCntDown[1];
            break;
#endif
#if ( LORAMAC_MAX_MC_CTX > 2 )
        case MC_FCNT_DOWN_2:
            *lastDown = CryptoNvm->FCntList.McFCntDown[2];
            break;
#endif
#if ( LORAMAC_MAX_MC_CTX > 3 )
        case MC_FCNT_DOWN_3:
            *lastDown = CryptoNvm->FCntList.McFCntDown[3];
            break;
#endif
        default:
            return LORAMAC_CRYPTO_FAIL_FCNT_ID;
    }
    return LORAMAC_CRYPTO_SUCCESS;
}

/*
 * Checks the downlink counter value
 *
 * \param[IN]     fCntID       - Frame counter identifier
 * \param[IN]     currentDown  - Current downlink counter value
 *
 * \retval                     - Status of the operation
 */
static bool CheckFCntDown( FCntIdentifier_t fCntID, uint32_t currentDown )
{
    uint32_t lastDown = 0;
    if( GetLastFcntDown( fCntID, &lastDown ) != LORAMAC_CRYPTO_SUCCESS )
    {
        return false;
    }
    if( ( currentDown > lastDown ) ||
        // For LoRaWAN 1.0.X only. Allow downlink frames of 0
        ( lastDown == FCNT_DOWN_INITAL_VALUE ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*!
 * Updates the reference downlink counter
 *
 * \param[IN]     fCntID        - Frame counter identifier
 * \param[IN]     currentDown   - Current downlink counter value
 *
 * \retval                     - Status of the operation
 */
static void UpdateFCntDown( FCntIdentifier_t fCntID, uint32_t currentDown )
{
    switch( fCntID )
    {
        case N_FCNT_DOWN:
            CryptoNvm->FCntList.NFCntDown = currentDown;
            break;
        case A_FCNT_DOWN:
            CryptoNvm->FCntList.AFCntDown = currentDown;
            break;
        case FCNT_DOWN:
            CryptoNvm->FCntList.FCntDown = currentDown;
            break;
#if ( LORAMAC_MAX_MC_CTX > 0 )
        case MC_FCNT_DOWN_0:
            CryptoNvm->FCntList.McFCntDown[0] = currentDown;
            break;
#endif
#if ( LORAMAC_MAX_MC_CTX > 1 )
        case MC_FCNT_DOWN_1:
            CryptoNvm->FCntList.McFCntDown[1] = currentDown;
            break;
#endif
#if ( LORAMAC_MAX_MC_CTX > 2 )
        case MC_FCNT_DOWN_2:
            CryptoNvm->FCntList.McFCntDown[2] = currentDown;
            break;
#endif
#if ( LORAMAC_MAX_MC_CTX > 3 )
        case MC_FCNT_DOWN_3:
            CryptoNvm->FCntList.McFCntDown[3] = currentDown;
            break;
#endif
        default:
            break;
    }
}

/*!
 * Resets the frame counters
 */
static void ResetFCnts( void )
{
    CryptoNvm->FCntList.FCntUp = 0;
    CryptoNvm->FCntList.NFCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoNvm->FCntList.AFCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoNvm->FCntList.FCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoNvm->LastDownFCnt = CryptoNvm->FCntList.FCntDown;

    for( int32_t i = 0; i < LORAMAC_MAX_MC_CTX; i++ )
    {
        CryptoNvm->FCntList.McFCntDown[i] = FCNT_DOWN_INITAL_VALUE;
    }
}

/*
 *  API functions
 */
LoRaMacCryptoStatus_t LoRaMacCryptoInit( LoRaMacCryptoNvmData_t* nvm )
{
    if( nvm == NULL )
    {
        return LORAMAC_CRYPTO_FAIL_PARAM;
    }

    // Assign non volatile context
    CryptoNvm = nvm;

    // Initialize with default
    memset1( ( uint8_t* )CryptoNvm, 0, sizeof( LoRaMacCryptoNvmData_t ) );

    // Set default LoRaWAN version
    CryptoNvm->LrWanVersion.Fields.Major = 1;
    CryptoNvm->LrWanVersion.Fields.Minor = 1;
    CryptoNvm->LrWanVersion.Fields.Patch = 1;
    CryptoNvm->LrWanVersion.Fields.Revision = 0;

    // Reset frame counters
    ResetFCnts( );

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoSetLrWanVersion( Version_t version )
{
    CryptoNvm->LrWanVersion = version;
    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoGetFCntUp( uint32_t* currentUp )
{
    if( currentUp == NULL )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    *currentUp = CryptoNvm->FCntList.FCntUp + 1;

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoGetFCntDown( FCntIdentifier_t fCntID, uint16_t maxFCntGap, uint32_t frameFcnt, uint32_t* currentDown )
{
    uint32_t lastDown = 0;
    int32_t fCntDiff = 0;
    LoRaMacCryptoStatus_t cryptoStatus = LORAMAC_CRYPTO_ERROR;

    if( currentDown == NULL )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    cryptoStatus = GetLastFcntDown( fCntID, &lastDown );
    if( cryptoStatus != LORAMAC_CRYPTO_SUCCESS )
    {
        return cryptoStatus;
    }

    // For LoRaWAN 1.0.X only, allow downlink frames of 0
    if( lastDown == FCNT_DOWN_INITAL_VALUE )
    {
        *currentDown = frameFcnt;
    }
    else
    {
        // Add difference, consider roll-over
        fCntDiff = ( int32_t )( ( int64_t )frameFcnt - ( int64_t )( lastDown & 0x0000FFFF ) );

        if( fCntDiff > 0 )
        {  // Positive difference
            *currentDown = lastDown + fCntDiff;
        }
        else if( fCntDiff == 0 )
        {  // Duplicate FCnt value, keep the current value.
            *currentDown = lastDown;
            return LORAMAC_CRYPTO_FAIL_FCNT_DUPLICATED;
        }
        else
        {  // Negative difference, assume a roll-over of one uint16_t
            *currentDown = ( lastDown & 0xFFFF0000 ) + 0x10000 + frameFcnt;
        }
    }

    // For LoRaWAN 1.0.X only, check maxFCntGap
    if( CryptoNvm->LrWanVersion.Fields.Minor == 0 )
    {
        if( ( ( int64_t )*currentDown - ( int64_t )lastDown ) >= maxFCntGap )
        {
            return LORAMAC_CRYPTO_FAIL_MAX_GAP_FCNT;
        }
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoSetMulticastReference( MulticastCtx_t* multicastList )
{
    if( multicastList == NULL )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    for( int32_t i = 0; i < LORAMAC_MAX_MC_CTX; i++ )
    {
        multicastList[i].DownLinkCounter = &CryptoNvm->FCntList.McFCntDown[i];
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoSetKey( KeyIdentifier_t keyID, uint8_t* key )
{
    if( SecureElementSetKey( keyID, key ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }
    if( keyID == APP_KEY )
    {
        // Derive lifetime keys
        if( LoRaMacCryptoDeriveMcRootKey( CryptoNvm->LrWanVersion.Fields.Minor, keyID ) != LORAMAC_CRYPTO_SUCCESS )
        {
            return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
        }
        if( LoRaMacCryptoDeriveMcKEKey( MC_ROOT_KEY ) != LORAMAC_CRYPTO_SUCCESS )
        {
            return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
        }
    }
    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoPrepareJoinRequest( LoRaMacMessageJoinRequest_t* macMsg )
{
    if( macMsg == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }
    KeyIdentifier_t micComputationKeyID = NWK_KEY;

    // Add device nonce
#if ( USE_RANDOM_DEV_NONCE == 1 )
    uint32_t devNonce = 0;
    SecureElementRandomNumber( &devNonce );
    CryptoNvm->DevNonce = devNonce;
#else
    CryptoNvm->DevNonce++;
#endif
    macMsg->DevNonce = CryptoNvm->DevNonce;

    // Serialize message
    if( LoRaMacSerializerJoinRequest( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    // Compute mic
    if( SecureElementComputeAesCmac( NULL, macMsg->Buffer, ( LORAMAC_JOIN_REQ_MSG_SIZE - LORAMAC_MIC_FIELD_SIZE ), micComputationKeyID, &macMsg->MIC ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    // Reserialize message to add the MIC
    if( LoRaMacSerializerJoinRequest( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoHandleJoinAccept( JoinReqIdentifier_t joinReqType, uint8_t* joinEUI, LoRaMacMessageJoinAccept_t* macMsg )
{
    if( ( macMsg == 0 ) || ( joinEUI == 0 ) )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    LoRaMacCryptoStatus_t retval = LORAMAC_CRYPTO_ERROR;
    uint8_t decJoinAccept[LORAMAC_JOIN_ACCEPT_FRAME_MAX_SIZE] = { 0 };
    uint8_t versionMinor         = 0;
    uint16_t nonce               = CryptoNvm->DevNonce;

    // Nonce selection depending on JoinReqType
    // JOIN_REQ     : CryptoNvm->DevNonce
    // REJOIN_REQ_0 : RJcount0
    // REJOIN_REQ_1 : CryptoCtx.RJcount1
    // REJOIN_REQ_2 : RJcount0
    if( joinReqType == JOIN_REQ )
    {
        // Nothing to be done
    }

    if( SecureElementProcessJoinAccept( joinReqType, joinEUI, nonce, macMsg->Buffer,
                                        macMsg->BufSize, decJoinAccept,
                                        &versionMinor ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    memcpy1( macMsg->Buffer, decJoinAccept, macMsg->BufSize );

    // Parse the message
    if( LoRaMacParserJoinAccept( macMsg ) != LORAMAC_PARSER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_PARSER;
    }

    uint32_t currentJoinNonce;

    currentJoinNonce = ( uint32_t )macMsg->JoinNonce[0];
    currentJoinNonce |= ( ( uint32_t )macMsg->JoinNonce[1] << 8 );
    currentJoinNonce |= ( ( uint32_t )macMsg->JoinNonce[2] << 16 );

#if( USE_JOIN_NONCE_COUNTER_CHECK == 1 )
    // Check if the JoinNonce is greater as the previous one
    if( currentJoinNonce > CryptoNvm->JoinNonce )
#else
    // Check if the JoinNonce is different from the previous one
    if( currentJoinNonce != CryptoNvm->JoinNonce )
#endif
    {
        CryptoNvm->JoinNonce = currentJoinNonce;
    }
    else
    {
        return LORAMAC_CRYPTO_FAIL_JOIN_NONCE;
    }

    // Derive lifetime keys
    retval = LoRaMacCryptoDeriveMcRootKey( versionMinor, APP_KEY );
    if( retval != LORAMAC_CRYPTO_SUCCESS )
    {
        return retval;
    }

    retval = LoRaMacCryptoDeriveMcKEKey( MC_ROOT_KEY );
    if( retval != LORAMAC_CRYPTO_SUCCESS )
    {
        return retval;
    }

    {
        // Operating in LoRaWAN 1.0.x mode

        uint32_t netID;

        netID = ( uint32_t )macMsg->NetID[0];
        netID |= ( ( uint32_t )macMsg->NetID[1] << 8 );
        netID |= ( ( uint32_t )macMsg->NetID[2] << 16 );

        retval = DeriveSessionKey10x( APP_S_KEY, currentJoinNonce, netID, nonce );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey10x( NWK_S_ENC_KEY, currentJoinNonce, netID, nonce );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey10x( F_NWK_S_INT_KEY, currentJoinNonce, netID, nonce );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }

        retval = DeriveSessionKey10x( S_NWK_S_INT_KEY, currentJoinNonce, netID, nonce );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }
    }

    // Join-Accept is successfully processed
    // Save LoRaWAN specification version
    CryptoNvm->LrWanVersion.Fields.Minor = versionMinor;

    // Reset frame counters
    CryptoNvm->FCntList.FCntUp = 0;
    CryptoNvm->FCntList.FCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoNvm->FCntList.NFCntDown = FCNT_DOWN_INITAL_VALUE;
    CryptoNvm->FCntList.AFCntDown = FCNT_DOWN_INITAL_VALUE;

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoSecureMessage( uint32_t fCntUp, uint8_t txDr, uint8_t txCh, LoRaMacMessageData_t* macMsg )
{
    LoRaMacCryptoStatus_t retval = LORAMAC_CRYPTO_ERROR;
    KeyIdentifier_t payloadDecryptionKeyID = APP_S_KEY;

    if( macMsg == NULL )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    if( fCntUp < CryptoNvm->FCntList.FCntUp )
    {
        return LORAMAC_CRYPTO_FAIL_FCNT_SMALLER;
    }

    // Encrypt payload
    if( macMsg->FPort == 0 )
    {
        // Use network session key
        payloadDecryptionKeyID = NWK_S_ENC_KEY;
    }

    if( fCntUp > CryptoNvm->FCntList.FCntUp )
    {
        retval = PayloadEncrypt( macMsg->FRMPayload, macMsg->FRMPayloadSize, payloadDecryptionKeyID, macMsg->FHDR.DevAddr, UPLINK, fCntUp );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }
    }

    // Serialize message
    if( LoRaMacSerializerData( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    // Compute mic
    {
        // MIC = cmacF[0..3]
        // The IsAck parameter is every time false since the ConfFCnt field is not used in legacy mode.
        retval = ComputeCmacB0( macMsg->Buffer, ( macMsg->BufSize - LORAMAC_MIC_FIELD_SIZE ), NWK_S_ENC_KEY, false, UPLINK, macMsg->FHDR.DevAddr, fCntUp, &macMsg->MIC );
        if( retval != LORAMAC_CRYPTO_SUCCESS )
        {
            return retval;
        }
    }

    // Re-serialize message to add the MIC
    if( LoRaMacSerializerData( macMsg ) != LORAMAC_SERIALIZER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SERIALIZER;
    }

    CryptoNvm->FCntList.FCntUp = fCntUp;

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoUnsecureMessage( AddressIdentifier_t addrID, uint32_t address, FCntIdentifier_t fCntID, uint32_t fCntDown, LoRaMacMessageData_t* macMsg )
{
    if( macMsg == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    if( CheckFCntDown( fCntID, fCntDown ) == false )
    {
        return LORAMAC_CRYPTO_FAIL_FCNT_SMALLER;
    }

    LoRaMacCryptoStatus_t retval = LORAMAC_CRYPTO_ERROR;
    KeyIdentifier_t payloadDecryptionKeyID = APP_S_KEY;
    KeyIdentifier_t micComputationKeyID = S_NWK_S_INT_KEY;
    KeyAddr_t* curItem;

    // Parse the message
    if( LoRaMacParserData( macMsg ) != LORAMAC_PARSER_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_PARSER;
    }

    // Determine current security context
    retval = GetKeyAddrItem( addrID, &curItem );
    if( retval != LORAMAC_CRYPTO_SUCCESS )
    {
        return retval;
    }

    payloadDecryptionKeyID = curItem->AppSkey;
    micComputationKeyID = curItem->NwkSkey;

    // Check if it is our address
    if( address != macMsg->FHDR.DevAddr )
    {
        return LORAMAC_CRYPTO_FAIL_ADDRESS;
    }

    // Compute mic
    bool isAck = macMsg->FHDR.FCtrl.Bits.Ack;
    if( CryptoNvm->LrWanVersion.Fields.Minor == 0 )
    {
        // In legacy mode the IsAck parameter is forced to be false since the ConfFCnt field is not used.
        isAck = false;
    }

    // Verify mic
    retval = VerifyCmacB0( macMsg->Buffer, ( macMsg->BufSize - LORAMAC_MIC_FIELD_SIZE ), micComputationKeyID, isAck, DOWNLINK, address, fCntDown, macMsg->MIC );
    if( retval != LORAMAC_CRYPTO_SUCCESS )
    {
        return retval;
    }

    // Decrypt payload
    if( macMsg->FPort == 0 )
    {
        // Use network session encryption key
        payloadDecryptionKeyID = NWK_S_ENC_KEY;
    }
    retval = PayloadEncrypt( macMsg->FRMPayload, macMsg->FRMPayloadSize, payloadDecryptionKeyID, address, DOWNLINK, fCntDown );
    if( retval != LORAMAC_CRYPTO_SUCCESS )
    {
        return retval;
    }

    UpdateFCntDown( fCntID, fCntDown );

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoDeriveMcRootKey( uint8_t versionMinor, KeyIdentifier_t keyID )
{
    // Prevent other keys than AppKey
    if( keyID != APP_KEY )
    {
        return LORAMAC_CRYPTO_ERROR_INVALID_KEY_ID;
    }
    uint8_t compBase[16] = { 0 };

    if( versionMinor == 1 )
    {
        compBase[0] = 0x20;
    }
    if( SecureElementDeriveAndStoreKey( compBase, keyID, MC_ROOT_KEY ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoDeriveMcKEKey( KeyIdentifier_t keyID )
{
    // Prevent other keys than McRootKey
    if( keyID != MC_ROOT_KEY )
    {
        return LORAMAC_CRYPTO_ERROR_INVALID_KEY_ID;
    }
    uint8_t compBase[16] = { 0 };

    if( SecureElementDeriveAndStoreKey( compBase, keyID, MC_KE_KEY ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}

LoRaMacCryptoStatus_t LoRaMacCryptoDeriveMcSessionKeyPair( AddressIdentifier_t addrID, uint32_t mcAddr )
{
    if( mcAddr == 0 )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    LoRaMacCryptoStatus_t retval = LORAMAC_CRYPTO_ERROR;

    // Determine current security context
    KeyAddr_t* curItem;
    retval = GetKeyAddrItem( addrID, &curItem );
    if( retval != LORAMAC_CRYPTO_SUCCESS )
    {
        return retval;
    }

    // McAppSKey = aes128_encrypt(McKey, 0x01 | McAddr | pad16)
    // McNwkSKey = aes128_encrypt(McKey, 0x02 | McAddr | pad16)

    uint8_t compBaseAppS[16] = { 0 };
    uint8_t compBaseNwkS[16] = { 0 };

    compBaseAppS[0] = 0x01;
    compBaseAppS[1] = mcAddr & 0xFF;
    compBaseAppS[2] = ( mcAddr >> 8 ) & 0xFF;
    compBaseAppS[3] = ( mcAddr >> 16 ) & 0xFF;
    compBaseAppS[4] = ( mcAddr >> 24 ) & 0xFF;

    compBaseNwkS[0] = 0x02;
    compBaseNwkS[1] = mcAddr & 0xFF;
    compBaseNwkS[2] = ( mcAddr >> 8 ) & 0xFF;
    compBaseNwkS[3] = ( mcAddr >> 16 ) & 0xFF;
    compBaseNwkS[4] = ( mcAddr >> 24 ) & 0xFF;

    if( SecureElementDeriveAndStoreKey( compBaseAppS, curItem->RootKey, curItem->AppSkey ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    if( SecureElementDeriveAndStoreKey( compBaseNwkS, curItem->RootKey, curItem->NwkSkey ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_CRYPTO_ERROR_SECURE_ELEMENT_FUNC;
    }

    return LORAMAC_CRYPTO_SUCCESS;
}
