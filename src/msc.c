/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 * Copyright (c) 2020 Artur Pacholec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdint.h>

#include "bsp.h"
#include "uf2.h"
#include "tusb.h"

#if CFG_TUD_MSC

#define SERIAL0 (*(uint32_t *)0x0080A00C)
#define SERIAL1 (*(uint32_t *)0x0080A040)
#define SERIAL2 (*(uint32_t *)0x0080A044)
#define SERIAL3 (*(uint32_t *)0x0080A048)

typedef struct
{
    uint8_t JumpInstruction[3];
    uint8_t OEMInfo[8];
    uint16_t SectorSize;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FATCopies;
    uint16_t RootDirectoryEntries;
    uint16_t TotalSectors16;
    uint8_t MediaDescriptor;
    uint16_t SectorsPerFAT;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t TotalSectors32;
    uint8_t PhysicalDriveNum;
    uint8_t Reserved;
    uint8_t ExtendedBootSig;
    uint32_t VolumeSerialNumber;
    char VolumeLabel[11];
    uint8_t FilesystemIdentifier[8];
} __attribute__((packed)) FAT_BootBlock;

typedef struct
{
    char name[8];
    char ext[3];
    uint8_t attrs;
    uint8_t reserved;
    uint8_t createTimeFine;
    uint16_t createTime;
    uint16_t createDate;
    uint16_t lastAccessDate;
    uint16_t highStartCluster;
    uint16_t updateTime;
    uint16_t updateDate;
    uint16_t startCluster;
    uint32_t size;
} __attribute__((packed)) DirEntry;

struct TextFile
{
    const char name[11];
    const char *content;
};

const char infoUf2File[] = //
    "UF2 Bootloader " UF2_VERSION "\r\n"
    "Model: " PRODUCT_NAME "\r\n"
    "Board-ID: " BOARD_ID "\r\n";

const char indexFile[] = //
    "<!doctype html>\n"
    "<html>"
    "<body>"
    "<script>\n"
    "location.replace(\"" INDEX_URL "\");\n"
    "</script>"
    "</body>"
    "</html>\n";

// WARNING -- code presumes only one NULL .content for .UF2 file
//            and requires it be the last element of the array
static const struct TextFile info[] =
{
    {.name = "INFO_UF2TXT", .content = infoUf2File },
    {.name = "INDEX   HTM", .content = indexFile },
    {.name = "CURRENT UF2" },
};

#define NUM_FILES (sizeof(info) / sizeof(info[0]))
#define NUM_DIRENTRIES (NUM_FILES + 1) // Code adds volume label as first root directory entry

#define UF2_SIZE (BOARD_FLASH_SIZE / UF2_PAYLOAD_SIZE * UF2_BLOCK_SIZE)
#define UF2_SECTORS (UF2_SIZE / UF2_BLOCK_SIZE)
#define UF2_FIRST_SECTOR (NUM_FILES + 1) // WARNING -- code presumes each non-UF2 file content fits in single sector
#define UF2_LAST_SECTOR (UF2_FIRST_SECTOR + UF2_SECTORS - 1)

#define RESERVED_SECTORS 1
#define ROOT_DIR_SECTORS 4
#define SECTORS_PER_FAT ((NUM_FAT_BLOCKS * 2 + (UDI_MSC_BLOCK_SIZE - 1)) / UDI_MSC_BLOCK_SIZE)

#define START_FAT0 RESERVED_SECTORS
#define START_FAT1 (START_FAT0 + SECTORS_PER_FAT)
#define START_ROOTDIR (START_FAT1 + SECTORS_PER_FAT)
#define START_CLUSTERS (START_ROOTDIR + ROOT_DIR_SECTORS)

// all directory entries must fit in a single sector
// because otherwise current code overflows buffer
#define DIRENTRIES_PER_SECTOR (512 / sizeof(DirEntry))
STATIC_ASSERT(NUM_DIRENTRIES < DIRENTRIES_PER_SECTOR * ROOT_DIR_SECTORS);

static int is_initialized;
static WriteState write_state;

static const FAT_BootBlock BootBlock =
{
  .JumpInstruction = {0xeb, 0x3c, 0x90},
  .OEMInfo = "UF2 UF2 ",
  .SectorSize = UDI_MSC_BLOCK_SIZE,
  .SectorsPerCluster = 1,
  .ReservedSectors = RESERVED_SECTORS,
  .FATCopies = 2,
  .RootDirectoryEntries = (ROOT_DIR_SECTORS * DIRENTRIES_PER_SECTOR),
  .TotalSectors16 = NUM_FAT_BLOCKS,
  .MediaDescriptor = 0xF8,
  .SectorsPerFAT = SECTORS_PER_FAT,
  .SectorsPerTrack = 1,
  .Heads = 1,
  .PhysicalDriveNum = 0x80, // to match MediaDescriptor of 0xF8
  .ExtendedBootSig = 0x29,
  .VolumeSerialNumber = 0x00420042,
  .VolumeLabel = VOLUME_LABEL,
  .FilesystemIdentifier = "FAT16   ",
};

static void padded_memcpy(char *dst, const char *src, int len)
{
    for (int i = 0; i < len; ++i) {
        if (*src)
            *dst = *src++;
        else
            *dst = ' ';
        dst++;
    }
}

void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    const char rev[] = "1.00";

    memcpy(vendor_id  , VENDOR_NAME, strlen(VENDOR_NAME));
    memcpy(product_id , PRODUCT_NAME, strlen(PRODUCT_NAME));
    memcpy(product_rev, rev, strlen(rev));
}

bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    return true;
}

void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
    *block_count = NUM_FAT_BLOCKS - 1;
    *block_size  = UDI_MSC_BLOCK_SIZE;
}

int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
    memset(buffer, 0, bufsize);

    uint32_t sector_idx = lba;

    if (lba == 0) { // Requested boot block
        memcpy(buffer, &BootBlock, sizeof(BootBlock));
        ((char*)buffer)[510] = 0x55;
        ((char*)buffer)[511] = 0xaa;
    } else if (lba < START_ROOTDIR) {  // Requested FAT table sector
        sector_idx -= START_FAT0;
        if (sector_idx >= SECTORS_PER_FAT)
            sector_idx -= SECTORS_PER_FAT; // second FAT is same as the first...

        if (sector_idx == 0) {
            ((char*)buffer)[0] = 0xf0;
            // WARNING -- code presumes only one NULL .content for .UF2 file
            //            and all non-NULL .content fit in one sector
            //            and requires it be the last element of the array
            for (size_t i = 1; i < NUM_FILES * 2 + 4; ++i) {
                ((char*)buffer)[i] = 0xff;
            }
        }

        for (int i = 0; i < 256; ++i) { // Generate the FAT chain for the firmware "file"
            uint32_t v = sector_idx * 256 + i;
            if (UF2_FIRST_SECTOR <= v && v <= UF2_LAST_SECTOR)
                ((uint16_t *)(void *)buffer)[i] = v == UF2_LAST_SECTOR ? 0xffff : v + 1;
        }
    } else if (lba < START_CLUSTERS) { // Requested sector of the root directory
        sector_idx -= START_ROOTDIR;
        if (sector_idx == 0) {
            DirEntry *d = (void *)buffer;
            padded_memcpy(d->name, BootBlock.VolumeLabel, 11);
            d->attrs = 0x28;
            for (size_t i = 0; i < NUM_FILES; ++i) {
                d++;
                const struct TextFile *inf = &info[i];
                d->size = inf->content ? strlen(inf->content) : UF2_SIZE;
                d->startCluster = i + 2;
                padded_memcpy(d->name, inf->name, 11);
                d->createDate = 0x4d99;
                d->updateDate = 0x4d99;
            }
        }
    } else { // Requested sector from file space
        sector_idx -= START_CLUSTERS;
        // WARNING -- code presumes all but last file take exactly one sector
        if (sector_idx < NUM_FILES - 1) {
            memcpy(buffer, info[sector_idx].content, strlen(info[sector_idx].content));
        } else {
            sector_idx -= NUM_FILES - 1;

            uint32_t addr = sector_idx * UF2_PAYLOAD_SIZE;
            if (addr < BOARD_FLASH_SIZE) {
                addr += BOARD_FLASH_BASE;

                UF2_Block *bl = (void *)buffer;
                bl->magicStart0 = UF2_MAGIC_START0;
                bl->magicStart1 = UF2_MAGIC_START1;
                bl->magicEnd = UF2_MAGIC_END;
                bl->blockNo = sector_idx;
                bl->numBlocks = BOARD_FLASH_SIZE / UF2_PAYLOAD_SIZE;
                bl->targetAddr = addr;
                bl->payloadSize = UF2_PAYLOAD_SIZE;
                bl->flags |= UF2_FLAG_FAMILYID_PRESENT;
                bl->familyID = UF2_FAMILY;

                board_flash_read_blocks(bl->data, sector_idx, 1);
            }
        }
    }

    return bufsize;
}

int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
    UF2_Block *bl = (void *)buffer;
    if (!is_uf2_block(bl) || !UF2_IS_MY_FAMILY(bl)) {
        return bufsize;
    }

    if (!is_initialized) {
        write_state.numBlocks = 0xffffffff;
        is_initialized = 1;
    }

    if ((bl->flags & UF2_FLAG_NOFLASH) || bl->payloadSize != UF2_PAYLOAD_SIZE ||
        bl->targetAddr < APP_START_ADDRESS || bl->targetAddr >= (BOARD_FLASH_BASE + BOARD_FLASH_SIZE)) {
    // this happens when we're trying to re-flash CURRENT.UF2 file previously
    // copied from a device; we still want to count these blocks to reset properly
    } else {
      //printf("uf2 write, target: 0x%08lX, size: %ld, block %ld, num blocks %ld\r\n", bl->targetAddr, bl->payloadSize, bl->blockNo, bl->numBlocks);

        board_flash_write_blocks(bl->data, (bl->targetAddr - BOARD_FLASH_BASE) / UF2_PAYLOAD_SIZE, 1);

        if (bl->numBlocks) {
            if (write_state.numBlocks != bl->numBlocks && bl->numBlocks < MAX_BLOCKS) {
                write_state.numBlocks = bl->numBlocks;
            }

            if (bl->blockNo < MAX_BLOCKS) {
                const uint8_t mask = 1 << (bl->blockNo % 8);
                const uint32_t pos = bl->blockNo / 8;
                if (!(write_state.writtenMask[pos] & mask)) {
                    write_state.writtenMask[pos] |= mask;
                    write_state.numWritten++;
                }

                if (write_state.numWritten >= write_state.numBlocks) {
                    board_flash_flush();
                    
                    reset_delay( RESET_DELAY_MS );
                }
            }
        } else {
          // reset after last block received
          reset_delay( RESET_DELAY_MS );          
        }
    }

    return bufsize;
}

int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
    void const *response = NULL;
    uint16_t resplen = 0;

    // most scsi handled is input
    bool in_xfer = true;

    switch (scsi_cmd[0]) {
    case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
        // Host is about to read/write etc ... better not to disconnect disk
        resplen = 0;
        break;

    default:
        // Set Sense = Invalid Command Operation
        tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

        // negative means error -> tinyusb could stall and/or response with failed status
        resplen = -1;
        break;
    }

    // return resplen must not larger than bufsize
    if (resplen > bufsize)
        resplen = bufsize;

    if (response && (resplen > 0)) {
        if (in_xfer) {
            memcpy(buffer, response, resplen);
        } else {
            // SCSI output
        }
    }

    return resplen;
}

#endif
