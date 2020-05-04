/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020, Dave Marples <dave@marples.net>
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
#include "csr.h"
#include "irq.h"
#include "spi.h"
#include "uart.h"
#include "uf2.h"

//#define FAMILY_DEBUG 1

#ifdef FAMILY_DEBUG
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...) {}
#endif

enum { IO_LED, IO_HWRESET, IO_RED, IO_GREEN, IO_BLUE, IO_BUTTON } iopins;

// Indicator of if next reboot is to be to bootloader or app
static bool rebootToBootloader = false;

// Overall time maintenance
static volatile uint32_t _system_ticks = 0;

// Flag indicating boot mode (set up in the linker file
extern uint32_t _bootloader_dbl_tap;

// Led flashing support
static uint32_t _next_blink_event;                          /* Time in ticks for next blink crossover */
static uint32_t _blink_interval;                            /* Addition of ticks for next event */

// Indicator that no Sector is currently cached
#define NO_CACHE        0xffffffff

#define MIN(a,b) ((a<b)?a:b)

// Sectors are 4K and individual writable parts are 256 bytes maximum. We hold the
// 4K until the sector changes, then write it back to flash if it's changed. We
// explicitly don't write it back if it hasn't changed, in the (mistaken?) belief
// that most sectors don't change from flash to flash...which is completely dependent
// on their type of content.

#define SECTOR_SIZE      0x1000                             /* 4K Sector */
#define SECTOR_SIZE_MASK (SECTOR_SIZE-1)                    /* Mask for in-sector offset */
#define ADDR_SECTOR(a) ((a)&(~SECTOR_SIZE_MASK))            /* Get sector */
#define ADDR_OFFSET(a) ((a)&SECTOR_SIZE_MASK)               /* Get offset in current sector */
#define SAME_SECTOR(a,b) (ADDR_SECTOR(a)==ADDR_SECTOR(b))   /* Check for if we moved sector */

extern void *_flash_start;                                  /* Provided by the link file */

#define FILESYSTEM_BLOCK_SIZE 256
#define USER_IMAGE_OFFSET 0x80000                           /* No user stuff below 0x80000 */
#define FLASH_BASE  ((uint32_t)&_flash_start) 
#define IMAGE_BASE (FLASH_BASE+USER_IMAGE_OFFSET)

/* Initially we aren't holding a sector */
static bool _flash_sector_dirty     = false;
static uint32_t _flash_sector_addr  = NO_CACHE;
static uint8_t  _flash_cache[SECTOR_SIZE] __attribute__((aligned(4)));


// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

// Flashing Related code

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------
static inline uint32_t _lba2addr(uint32_t block)

/* Turn LBA into address in memory (by implication, in the flash) */

{
  return  IMAGE_BASE + block * FILESYSTEM_BLOCK_SIZE;
}
// ---------------------------------------------------------------------------------------------
static void _burn_sector(void)

/* Unconditionally write sector to flash (assuming it's outside the 'safety zone') */
  
{
  if (ADDR_SECTOR(_flash_sector_addr) < IMAGE_BASE)
    {
      /* Not in the safe zone to burn, just forget it */
      return;
    }

  uint32_t flash_ofs = _flash_sector_addr - FLASH_BASE;
  spiInit();
  spiBeginErase4(flash_ofs);
  while(spiIsBusy());

  DBG("Burn Offset %8x %d\n",flash_ofs, SECTOR_SIZE);

  /* We can only write 256 octets at a time, so batch them up */
  uint32_t r=SECTOR_SIZE;
  uint8_t *p=_flash_cache;
  do
    {
      uint32_t w = spiBeginWrite(flash_ofs, p, r);
      while(spiIsBusy());
      r-=w;
      p+=w;
      flash_ofs+=w;
    }
  while (r);
  spiFree();
  DBG("Burn complete\n");
}
// ---------------------------------------------------------------------------------------------
void board_flash_flush(void)
{
    if ((_flash_sector_addr == NO_CACHE) || (_flash_sector_dirty == false))
        return;
  
    DBG(">>>Flush: %08x\n", _flash_sector_addr);

    _burn_sector();
    _flash_sector_dirty = false;
}
// ---------------------------------------------------------------------------------------------
uint32_t board_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks)
{
    uint32_t src = _lba2addr(block);
    memcpy(dest, (uint8_t*) src, FILESYSTEM_BLOCK_SIZE * num_blocks);
    return 0;
}
// ---------------------------------------------------------------------------------------------
uint32_t board_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks)

{
  uint32_t const addr = _lba2addr(lba);
  uint32_t const len  = num_blocks*FILESYSTEM_BLOCK_SIZE;

  /* Someone is trying to load something, let the user know */
  _blink_interval = BOARD_LOADING_INTERVAL;
  
  /* Flush any old cache and get the original content of the sector */
  if (!SAME_SECTOR(addr,_flash_sector_addr))
    {
      board_flash_flush();
      DBG("\nRead: %08x [lba %d]\n",ADDR_SECTOR(addr),lba);
      _flash_sector_addr = ADDR_SECTOR(addr);
      _flash_sector_dirty = false;
      memcpy(_flash_cache, (uint8_t*) ADDR_SECTOR(addr), SECTOR_SIZE);      
    }

  if (memcmp(&_flash_cache[ADDR_OFFSET(addr)],src,len)!=0)
    {
      DBG("W");
      /* Now update the right part of this sector with the new data */
      memcpy(&_flash_cache[ADDR_OFFSET(addr)],src,len);
      _flash_sector_dirty = true;
    }
  else
    {
      DBG("S");
    }

  return 0;
}
// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------
static void _led_write(bool state)
{
  gpio_out_write((gpio_out_read()&(~(1<<IO_LED)))|((state ? LED_STATE_ON : (1-LED_STATE_ON))<<IO_LED));
}
// ---------------------------------------------------------------------------------------------
static void _reset(bool active)

/* We can either reset *this* firmware or boot the application - that's decided here */

{
  if ((active) && (rebootToBootloader))
    {
      ctrl_reset_write(1);
    }
  else
    {
      gpio_out_write((gpio_in_read()&(~(1<<IO_HWRESET)))|(active?0:(1<<IO_HWRESET)));
    }
}
// ---------------------------------------------------------------------------------------------
static bool _button_pressed(void)

{
  return ((gpio_in_read()&(1<<IO_BUTTON))==0);
}

// ---------------------------------------------------------------------------------------------
void _init_io(void)

{
  _reset(false);
  _led_write(false);
  gpio_oe_write( (1<<IO_LED) | (1<<IO_HWRESET) | (1<<IO_RED) | (1<<IO_GREEN) | (1<<IO_BLUE) );
}
// ---------------------------------------------------------------------------------------------
void isr(void)
{
    unsigned int irqs;
    static bool led_state;
    irqs = irq_pending() & irq_getmask();

    if (irqs & (1 << USB_INTERRUPT))
    {
      tud_int_handler(0);
    }

    if (irqs & (1 << TIMER0_INTERRUPT))
    {
      _system_ticks++;
      timer0_ev_pending_write(1);

      if ((_blink_interval) && (_system_ticks>=_next_blink_event))
        {
          _led_write(led_state=!led_state);
          _next_blink_event=_system_ticks+_blink_interval;
        }
    }

    if(irqs & (1 << UART_INTERRUPT))
      uart_isr();
}
// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------
//
// External Interface
//
// ---------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------
uint32_t board_millis(void)

/* Return time since board start in mS */
  
{
    return _system_ticks;
}
// ---------------------------------------------------------------------------------------------
void board_reset(void)

/* Reset the board */
  
{
  DBG("Reset request\n");
  board_flash_flush();
  board_delay_ms(500);
  _reset(true);
}
// ---------------------------------------------------------------------------------------------
void board_reset_to_bootloader(bool toBootloader)

/* Called to ensure that next reset is into bootloader */

{
  if (toBootloader)
    {
      rebootToBootloader = true;
      _bootloader_dbl_tap = DBL_TAP_MAGIC;
    }
  else
    {
      rebootToBootloader = false;
      _bootloader_dbl_tap = 0;      
    }
}
// ---------------------------------------------------------------------------------------------
void board_init(void)

/* Perform essential board init */

{
  irq_setmask(0);

  // Initialise timer
  timer0_en_write(0);
  uint32_t t = CONFIG_CLOCK_FREQUENCY / 1000; // 1000 Hz tick
  timer0_reload_write(t);
  timer0_load_write(t);
  timer0_en_write(1);
  timer0_ev_enable_write(1);
  timer0_ev_pending_write(1);
  irq_setmask(irq_getmask() | (1 << TIMER0_INTERRUPT));

  // Make uart fly
  uart_init();
  
  // Make interrupts happen
  irq_setie(1);

}
// ---------------------------------------------------------------------------------------------
void board_delay_ms(uint32_t ms)

/* Perform static delay in mS */

{
  uint32_t p = board_millis();

  while (board_millis()-p < ms);
}
// ---------------------------------------------------------------------------------------------
void board_check_app_start(void)

{
  // On this chip, we always go into the app if we're started
  // but we can still use this routine to set up family specific things.

  _init_io();
  return;
}
// ---------------------------------------------------------------------------------------------
void board_led_blinking_task(void)

{
  // For this CPU this function is done in the ISR
}
// ---------------------------------------------------------------------------------------------
void board_check_tinyuf2_start(void)

/* Check if we're to start the bootloader or the application (based on button held down) */

{
  uint32_t tend = board_millis()+BOARD_BOOTING_WAIT;

  // If there's nothing sane in the main memory or if we've been here
  // before and we're coming back into the bootloader again then jump
  // to the uf2 bootloader directly
  
  if ((_bootloader_dbl_tap==DBL_TAP_MAGIC) ||
      ((*(uint32_t *)IMAGE_BASE)==0xffffffff))
    {
      _bootloader_dbl_tap=0;
      return;
    }

#ifdef BOARD_LED_ON_UF2_START
  _blink_interval = BOARD_BOOTING_INTERVAL;
#endif
  while (board_millis()<tend)
    {

      if (!_button_pressed())
        {
          // Button isn't pressed, so let's pack up and boot
          _blink_interval = BOARD_OFF_INTERVAL;
          _led_write(false);
          board_reset();
        }
    }
  _blink_interval = BOARD_BLINK_INTERVAL;
  return; // stay in bootloader
}
// ---------------------------------------------------------------------------------------------
