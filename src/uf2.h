#ifndef _UF2_H_
#define _UF2_H_

#include <stdbool.h>

#include "uf2_version.h"

#define UF2_VERSION \
    UF2_VERSION_BASE " SF"

#define UF2_PAYLOAD_SIZE 256
#define UF2_BLOCK_SIZE 512

// All entries are little endian.
#define UF2_MAGIC_START0 0x0A324655UL // "UF2\n"
#define UF2_MAGIC_START1 0x9E5D5157UL // Randomly selected
#define UF2_MAGIC_END 0x0AB16F30UL    // Ditto

// If set, the block is "comment" and should not be flashed to the device
#define UF2_FLAG_NOFLASH 0x00000001
#define UF2_FLAG_FAMILYID_PRESENT 0x00002000

#define UF2_IS_MY_FAMILY(bl) \
    (((bl)->flags & UF2_FLAG_FAMILYID_PRESENT) == 0 || (bl)->familyID == UF2_FAMILY)

typedef struct
{
    // 32 byte header
    uint32_t magicStart0;
    uint32_t magicStart1;
    uint32_t flags;
    uint32_t targetAddr;
    uint32_t payloadSize;
    uint32_t blockNo;
    uint32_t numBlocks;
    uint32_t familyID;

    // raw data;
    uint8_t data[476];

    // store magic also at the end to limit damage from partial block reads
    uint32_t magicEnd;
} UF2_Block;

static inline bool is_uf2_block(void *data)
{
    UF2_Block *bl = (UF2_Block *)data;
    return bl->magicStart0 == UF2_MAGIC_START0 && bl->magicStart1 == UF2_MAGIC_START1 &&
           bl->magicEnd == UF2_MAGIC_END;
}

#define DBL_TAP_MAGIC            0xf01669ef     // Randomly selected, adjusted to have first and last bit set
#define DBL_TAP_MAGIC_QUICK_BOOT 0xf02669ef

// These entries support multitap entry into the bootloader by putting the count into byte 1
#define MULTITAP_MAGIC_COUNTING 0xf01700ef
#define MULTITAP_AMCOUNTING(x)  (((x)&0xFFFF00FF)==MULTITAP_MAGIC_COUNTING)
#define MULTITAP_GETCOUNT(x)    (((x)>>8)&0xff)
#define MULTITAP_SETCOUNT(y)    (MULTITAP_MAGIC_COUNTING|(((y)&0xff)<<8))

// Static block size for all memories
#define UDI_MSC_BLOCK_SIZE 512L

// Needs to be more than ~4200 (to force FAT16)
#define NUM_FAT_BLOCKS 65535

#define CONCAT_1(a, b) a##b
#define CONCAT_0(a, b) CONCAT_1(a, b)
#define STATIC_ASSERT(e) enum { CONCAT_0(_static_assert_, __LINE__) = 1 / ((e) ? 1 : 0) }

#define MAX_BLOCKS (BOARD_FLASH_SIZE / UF2_PAYLOAD_SIZE + 100)

typedef struct {
    uint32_t numBlocks;
    uint32_t numWritten;
    uint8_t writtenMask[MAX_BLOCKS / 8 + 1];
} WriteState;

extern uint32_t reset_millis;

#endif // UF2_H_
