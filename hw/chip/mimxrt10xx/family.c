/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020, Artur Pacholec
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

#include <stdio.h>

#include "tusb.h"
#include "bsp.h"
#include "clock_config.h"

#include "fsl_device_registers.h"
#include "fsl_flexspi.h"
#include "fsl_cache.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"

volatile uint32_t system_ticks = 0;

// FLASH
#define NO_CACHE        0xffffffff

#define SECTOR_SIZE 0x1000 /* 4K */
#define FLASH_PAGE_SIZE 256
#define FILESYSTEM_BLOCK_SIZE 256

#define NOR_CMD_LUT_SEQ_IDX_READ_NORMAL 7
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST 13
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD 0
#define NOR_CMD_LUT_SEQ_IDX_READSTATUS 1
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE 2
#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR 3
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE 6
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD 4
#define NOR_CMD_LUT_SEQ_IDX_READID 8
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG 9
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG 12
#define NOR_CMD_LUT_SEQ_IDX_ERASECHIP 5

#define CUSTOM_LUT_LENGTH 60
#define FLASH_QUAD_ENABLE 0x02
#define FLASH_BUSY_STATUS_POL 1
#define FLASH_BUSY_STATUS_OFFSET 0
#define FLASH_ERROR_STATUS_MASK 0x0e

static uint32_t _flash_page_addr = NO_CACHE;
static uint8_t  _flash_cache[SECTOR_SIZE] __attribute__((aligned(4)));

flexspi_device_config_t deviceconfig =
{
    .flexspiRootClk       = 133000000,
    .flashSize            = (BOARD_FLASH_SIZE / 1024),
    .CSIntervalUnit       = kFLEXSPI_CsIntervalUnit1SckCycle,
    .CSInterval           = 2,
    .CSHoldTime           = 3,
    .CSSetupTime          = 3,
    .dataValidTime        = 0,
    .columnspace          = 0,
    .enableWordAddress    = 0,
    .AWRSeqIndex          = 0,
    .AWRSeqNumber         = 0,
    .ARDSeqIndex          = NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD,
    .ARDSeqNumber         = 1,
    .AHBWriteWaitUnit     = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
    .AHBWriteWaitInterval = 0,
};

const uint32_t customLUT[CUSTOM_LUT_LENGTH] =
{
    /* Normal read mode -SDR */
    /* Normal read mode -SDR */
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x03, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Fast read mode - SDR */
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x0B, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Fast read quad mode - SDR */
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xEB, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD + 1] = FLEXSPI_LUT_SEQ(
        kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x06, kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 0x04),

    /* Read extend parameters */
    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUS] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x81, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Write Enable */
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x06, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Erase Sector  */
    [4 * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x20, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),

    /* Page Program - single mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x02, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Page Program - quad mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x32, kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, 0x18),
    [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD + 1] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

    /* Read ID */
    [4 * NOR_CMD_LUT_SEQ_IDX_READID] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x9F, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Enable Quad mode */
    [4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x31, kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),

    /* Read status register */
    [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x05, kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

    /* Erase whole chip */
    [4 * NOR_CMD_LUT_SEQ_IDX_ERASECHIP] =
        FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0xC7, kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
};

const uint8_t dcd_data[] = { 0x00 };

status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address);
status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t dstAddr, const uint32_t *src);
status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base);
status_t flexspi_nor_erase_chip(FLEXSPI_Type *base);
status_t flexspi_nor_get_vendor_id(FLEXSPI_Type *base, uint8_t *vendorId);
void flexspi_nor_flash_init(FLEXSPI_Type *base);

static inline uint32_t lba2addr(uint32_t block)
{
  return FlexSPI_AMBA_BASE + block * FILESYSTEM_BLOCK_SIZE;
}

void board_flash_flush(void)
{
    status_t status;

    if (_flash_page_addr == NO_CACHE)
        return;

    //printf("%s: page 0x%08lX\r\n", __func__, _flash_page_addr);

    // Skip if data is the same
    if (memcmp(_flash_cache, (void *)_flash_page_addr, SECTOR_SIZE) != 0) {
        //nrf_nvm_safe_flash_page_write(_flash_page_addr, _flash_cache);

        volatile uint32_t sector_addr = (_flash_page_addr - FlexSPI_AMBA_BASE);

        __disable_irq();
        status = flexspi_nor_flash_erase_sector(FLEXSPI, sector_addr);
        __enable_irq();
        if (status != kStatus_Success) {
            printf("Page erase failure %ld!\r\n", status);
            return;
        }

        for (int i = 0; i < SECTOR_SIZE / FLASH_PAGE_SIZE; ++i) {
            __disable_irq();
            status = flexspi_nor_flash_page_program(FLEXSPI, sector_addr + i * FLASH_PAGE_SIZE, (void *)_flash_cache + i * FLASH_PAGE_SIZE);
            __enable_irq();
            DCACHE_CleanInvalidateByRange(_flash_page_addr, SECTOR_SIZE);
            if (status != kStatus_Success) {
                printf("Page program failure %ld!\r\n", status);
                return;
            }
        }
    }
}

uint32_t board_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks)
{
    // Must write out anything in cache before trying to read.
    board_flash_flush();

    uint32_t src = lba2addr(block);
    memcpy(dest, (uint8_t*) src, FILESYSTEM_BLOCK_SIZE * num_blocks);
    return 0;
}

uint32_t board_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks)
{
  //  printf("%s: src %p, lba %ld, num %ld\r\n", __func__, src, lba, num_blocks);

    while (num_blocks) {
        uint32_t const addr      = lba2addr(lba);
        uint32_t const page_addr = addr & ~(SECTOR_SIZE - 1);

        uint32_t count = 8 - (lba % 8); // up to page boundary
        count = MIN(num_blocks, count);

        if (page_addr != _flash_page_addr) {
            // Write out anything in cache before overwriting it.

            board_flash_flush();

            _flash_page_addr = page_addr;

            // Copy the current contents of the entire page into the cache.
            memcpy(_flash_cache, (void *)page_addr, SECTOR_SIZE);
        }

        // Overwrite part or all of the page cache with the src data.
        memcpy(_flash_cache + (addr & (SECTOR_SIZE - 1)), src, count * FILESYSTEM_BLOCK_SIZE);

        // adjust for next run
        lba        += count;
        src        += count * FILESYSTEM_BLOCK_SIZE;
        num_blocks -= count;
    }

    return 0; // success
}

void SysTick_Handler(void)
{
    system_ticks++;
}

uint32_t board_millis(void)
{
    return system_ticks;
}

void board_reset(void)
{
    NVIC_SystemReset();
}

void board_led_write(bool state)
{
    GPIO_PinWrite(LED_GPIO_PORT, LED_GPIO_PIN, state ? LED_STATE_ON : (1-LED_STATE_ON));
}

#include "fsl_lpuart.h"

void board_init(void)
{
    // Init clock
    BOARD_BootClockRUN();
    SystemCoreClockUpdate();

    // Enable IOCON clock
    CLOCK_EnableClock(kCLOCK_Iomuxc);

    // LED
    IOMUXC_SetPinMux(PIN_LED, PIN_LED_MUX);
    IOMUXC_SetPinConfig(PIN_LED, PIN_LED_CFG);
    gpio_pin_config_t led_config = { kGPIO_DigitalOutput, 0, kGPIO_NoIntmode };
    GPIO_PinInit(LED_GPIO_PORT, LED_GPIO_PIN, &led_config);
    board_led_write(false);

    // UART
    IOMUXC_SetPinMux( PIN_UART_TX, PIN_UART_TX_MUX);
    IOMUXC_SetPinMux( PIN_UART_RX, PIN_UART_RX_MUX);
    IOMUXC_SetPinConfig(PIN_UART_TX, PIN_UART_TX_CFG);
    IOMUXC_SetPinConfig(PIN_UART_RX, PIN_UART_RX_CFG);
    lpuart_config_t uart_config;
    LPUART_GetDefaultConfig(&uart_config);
    uart_config.baudRate_Bps = 115200;
    uart_config.enableTx = true;
    uart_config.enableRx = true;
    LPUART_Init(UART_PORT, &uart_config, (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U));

    //------------- USB0 -------------//
    // Clock
    CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
    CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);

    USBPHY_Type* usb_phy = USBPHY;

    // Enable PHY support for Low speed device + LS via FS Hub
    usb_phy->CTRL |= USBPHY_CTRL_SET_ENUTMILEVEL2_MASK | USBPHY_CTRL_SET_ENUTMILEVEL3_MASK;

    // Enable all power for normal operation
    usb_phy->PWD = 0;

    // TX Timing
    uint32_t phytx = usb_phy->TX;
    phytx &= ~(USBPHY_TX_D_CAL_MASK | USBPHY_TX_TXCAL45DM_MASK | USBPHY_TX_TXCAL45DP_MASK);
    phytx |= USBPHY_TX_D_CAL(0x0C) | USBPHY_TX_TXCAL45DP(0x06) | USBPHY_TX_TXCAL45DM(0x06);
    usb_phy->TX = phytx;

    //----------- FLEXSPI ----------//
    IOMUXC_SetPinMux(PIN_SS0, PIN_SS0_MUX);
    IOMUXC_SetPinMux(PIN_DATA1, PIN_DATA1_MUX);
    IOMUXC_SetPinMux(PIN_DATA2, PIN_DATA2_MUX);
    IOMUXC_SetPinMux(PIN_DATA0, PIN_DATA0_MUX);
    IOMUXC_SetPinMux(PIN_SCLK, PIN_SCLK_MUX);
    IOMUXC_SetPinMux(PIN_DATA3, PIN_DATA3_MUX);
#ifdef PIN_DQS
    IOMUXC_SetPinMux(PIN_DQS, PIN_DQS_MUX);
#endif
    
    IOMUXC_SetPinConfig(PIN_SS0, PIN_SS0_CFG);
    IOMUXC_SetPinConfig(PIN_DATA1, PIN_DATA1_CFG);
    IOMUXC_SetPinConfig(PIN_DATA2, PIN_DATA2_CFG);
    IOMUXC_SetPinConfig(PIN_DATA0, PIN_DATA0_CFG);
    IOMUXC_SetPinConfig(PIN_SCLK, PIN_SCLK_CFG);
    IOMUXC_SetPinConfig(PIN_DATA3, PIN_DATA3_CFG);
#ifdef PIN_DQS
    IOMUXC_SetPinConfig(PIN_DQS, PIN_DQS_CFG);
#endif
    SCB_DisableDCache();

    flexspi_nor_flash_init(FLEXSPI);

    status_t status;
    uint8_t vendorID = 0;

    status = flexspi_nor_get_vendor_id(FLEXSPI, &vendorID);
    if (status != kStatus_Success) {
    //    printf("flexspi_nor_get_vendor_id fail %ld\r\n", status);
        return;
    }

    status = flexspi_nor_enable_quad_mode(FLEXSPI);
    if (status != kStatus_Success) {
    //    printf("flexspi_nor_enable_quad_mode fail %ld\r\n", status);
        return;
    }

    // 1ms tick timer
    SysTick_Config(CLOCK_GetCoreSysClkFreq() / 1000);

    for (uint16_t i = 0; i < ADC_ETC_ERROR_IRQ_IRQn; i++) {
        NVIC_SetPriority(i, (1UL << __NVIC_PRIO_BITS) - 1UL);
    }

    NVIC_SetPriority(USB_OTG1_IRQn, 0);
    NVIC_SetPriority(FLEXSPI_IRQn, 1);
    NVIC_SetPriority(SysTick_IRQn, 2);
}


void board_delay_ms(uint32_t ms)
{
#if defined(MIMXRT1011_SERIES)
    SDK_DelayAtLeastUs(ms*1000, SystemCoreClock);
#else
    SDK_DelayAtLeastUs(ms*1000);
#endif
}

void USB_OTG1_IRQHandler(void)
{
#if CFG_TUSB_RHPORT0_MODE & OPT_MODE_HOST
    tuh_isr(0);
#endif

#if CFG_TUSB_RHPORT0_MODE & OPT_MODE_DEVICE
    tud_isr(0);
#endif
}

#include "uf2.h"

extern uint32_t _bootloader_dbl_tap;

void board_check_app_start(void)

{
  // This is the inital check to make sure we're not in the process
  // of rebooting for a clean start of the application. This is
  // deliberately done before any chip configuration so the application
  // receives control of a chip in a as-close-to-clean condition as possible,

  register uint32_t app_start_address = *(uint32_t *)(APP_START_ADDRESS + 4);

  if (_bootloader_dbl_tap != DBL_TAP_MAGIC_QUICK_BOOT)
    return;

  _bootloader_dbl_tap = 0;

  /* Rebase the Stack Pointer */
  __set_MSP(*(uint32_t *)APP_START_ADDRESS);

  /* Rebase the vector table base address */
  SCB->VTOR = ((uint32_t)APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

  /* Jump to application Reset Handler in the application */
  asm("bx %0" ::"r"(app_start_address));  
}

void board_check_tinyuf2_start(void)
{
  register uint32_t app_start_address = *(uint32_t *)(APP_START_ADDRESS + 4);

  /**
   * Test reset vector of application @ APP_START_ADDRESS + 4
   * Sanity check on the Reset_Handler address
   */
  if (app_start_address < APP_START_ADDRESS || app_start_address > BOARD_FLASH_BASE + BOARD_FLASH_SIZE) {
    return; // stay in bootloader
  }

  if (_bootloader_dbl_tap == DBL_TAP_MAGIC) {
    _bootloader_dbl_tap = 0;
    return; // stay in bootloader
  }

#ifdef BOARD_MULTITAP_COUNT
  if (MULTITAP_AMCOUNTING(_bootloader_dbl_tap)) {
    // A multi-tap count is in progress - see if it's expired
    _bootloader_dbl_tap = MULTITAP_SETCOUNT( MULTITAP_GETCOUNT(_bootloader_dbl_tap)+1 );
    if (MULTITAP_GETCOUNT(_bootloader_dbl_tap) == BOARD_MULTITAP_COUNT) {
      _bootloader_dbl_tap = 0;
      return; // We're done, stay in bootloader
    }
  }
  else
    // The multitap is just starting...
    _bootloader_dbl_tap = MULTITAP_SETCOUNT(1);
#else
    _bootloader_dbl_tap = DBL_TAP_MAGIC;
#endif
    
    // Now wait to see if the user wants to intervene...
#ifdef BOARD_LED_ON_UF2_START
    board_led_write(true);
#endif

    board_delay_ms(BOARD_TAP_WAIT);

#ifdef BOARD_LED_ON_UF2_START
    board_led_write(false);
#endif

  // If we made to to here then we should boot into the application
  _bootloader_dbl_tap = DBL_TAP_MAGIC_QUICK_BOOT;
  board_reset();
}
