/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2020 Microsoft
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 * Copyright (c) 2020 Artur Pacholec
 * Copyright (c) 2020 Konrad Beckmann
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
 * 
 * 
 * This code is a port of https://github.com/microsoft/uf2-samdx1/blob/master/src/hid.c
 * 
 */

#include "tusb.h"
#include "bsp.h"
#include "uf2.h"
#include "hid.h"
#include "assert.h"

#if CFG_TUD_HID

#if HF2_USE_HID

#ifdef DEBUG
#define log(...) printf(__VA_ARGS__)
#else
#define log(...)
#endif

extern const char infoUf2File[];

typedef struct {
    const uint8_t *buf_in;
    uint32_t buf_size;
    uint32_t size;
    uint8_t serial;
    union {
        uint8_t buf[BOARD_FLASH_PAGE_SIZE + 64];
        HF2_Command cmd;
        HF2_Response resp;
    };
} HID_InBuffer;

static struct {
    HID_InBuffer pkt;
    const uint8_t *ptr;
    uint32_t buf_size;
    uint32_t bytes_left;
    uint8_t flag;
} state;

static uint16_t crcCache[256];

#define CRC16POLY 0x1021

uint16_t add_crc(uint8_t ch, unsigned short crc0) {
    if (!crcCache[1]) {
        for (int ptr = 0; ptr < 256; ptr++) {
            uint16_t crc = (int)ptr << 8;
            for (uint16_t cmpt = 0; cmpt < 8; cmpt++) {
                if (crc & 0x8000)
                    crc = crc << 1 ^ CRC16POLY;
                else
                    crc = crc << 1;
            }
            crcCache[ptr] = crc;
        }
    }

    return ((crc0 << 8) ^ crcCache[((crc0 >> 8) ^ ch) & 0xff]) & 0xffff;
}

// Recieve HF2 message
// Does not block. Will store intermediate data in pkt.
// `serial` flag is cleared if we got a command message.
uint32_t recv_hf2(HID_InBuffer *pkt) {
    assert(pkt->buf_in != NULL && pkt->buf_size > 0);

    uint8_t tag = pkt->buf_in[0];
#if !HF2_USE_HID_SERIAL
    if (tag & 0x80) {
        return 0;
    }
#endif // !HF2_USE_HID_SERIAL
    // serial packets not allowed when in middle of command packet
    assert(pkt->size == 0 || !(tag & HF2_FLAG_SERIAL_OUT));
    memcpy(pkt->buf + pkt->size, pkt->buf_in + 1, tag & HF2_SIZE_MASK);
    pkt->size += tag & HF2_SIZE_MASK;
    assert(pkt->size <= sizeof(pkt->buf));
    tag &= HF2_FLAG_MASK;
    if (tag != HF2_FLAG_CMDPKT_BODY) {
#if HF2_USE_HID_SERIAL
        pkt->serial = tag - 0x40;
#endif // HF2_USE_HID_SERIAL
        uint32_t sz = pkt->size;
        pkt->size = 0;
        return sz;
    }
    return 0;
}

void do_send_hf2(void) {
    uint8_t buf[64];

    if (state.bytes_left == 0) {
        return;
    }

    int s = 63;
    if (state.bytes_left <= 63) {
        s = state.bytes_left;
        if (state.flag == HF2_FLAG_CMDPKT_BODY)
            state.flag = HF2_FLAG_CMDPKT_LAST;
    }
    buf[0] = state.flag | s;
    memcpy(buf + 1, state.ptr, s);
    tud_hid_report(0, buf, s + 1);
    state.ptr += s;
    state.bytes_left -= s;
}

// Send HF2 message async.
// Use command message when flag == HF2_FLAG_CMDPKT_BODY
// Use serial stdout for HF2_FLAG_SERIAL_OUT and stderr for HF2_FLAG_SERIAL_ERR.
void send_hf2(const void *data, uint32_t size, int flag) {
    state.ptr = data;
    state.buf_size = state.bytes_left = size;
    state.flag = flag;
}

void send_hf2_response(HID_InBuffer *pkt, uint32_t size) {
    log("sendresp %ld\n", size);
    send_hf2(pkt->buf, 4 + size, HF2_FLAG_CMDPKT_BODY);
}

static void checksum_pages(HID_InBuffer *pkt, int start, int num) {
    for (int i = 0; i < num; ++i) {
        uint8_t *data = (uint8_t *)start + i * BOARD_FLASH_PAGE_SIZE;
        uint16_t crc = 0;
        for (int j = 0; j < BOARD_FLASH_PAGE_SIZE; ++j) {
            crc = add_crc(*data++, crc);
        }
        pkt->resp.data16[i] = crc;
    }
    send_hf2_response(pkt, num * 2);
}

void process_core(HID_InBuffer *pkt) {
    uint32_t sz = recv_hf2(pkt);

    if (!sz)
        return;

    uint32_t tmp;

#if HF2_USE_HID_SERIAL
    if (pkt->serial) {
#if USE_LOGS
        if (pkt->buf[0] == 'L') {
            send_hf2(logStoreUF2.buffer, logStoreUF2.ptr, HF2_FLAG_SERIAL_OUT);
        } else
#endif // USE_LOGS
        {
            send_hf2("OK\n", 3, HF2_FLAG_SERIAL_ERR);
        }
        return;
    }
#endif // HF2_USE_HID_SERIAL

    log("HID sz=%ld, CMD=%ld", sz, pkt->cmd.command_id);

    // one has to be careful dealing with these, as they share memory
    HF2_Command *cmd = &pkt->cmd;
    HF2_Response *resp = &pkt->resp;

    uint32_t cmdId = cmd->command_id;
    resp->tag = cmd->tag;
    resp->status16 = HF2_STATUS_OK;

#define checkDataSize(str, add) assert(sz == 8 + sizeof(cmd->str) + (add))

    switch (cmdId) {
    case HF2_CMD_INFO:
        tmp = strlen(infoUf2File);
        memcpy(pkt->resp.data8, infoUf2File, tmp);
        send_hf2_response(pkt, tmp);
        return;

    case HF2_CMD_BININFO:
        resp->bininfo.mode = HF2_MODE_BOOTLOADER;
        resp->bininfo.flash_page_size = BOARD_FLASH_PAGE_SIZE;
        resp->bininfo.flash_num_pages = BOARD_FLASH_SIZE / BOARD_FLASH_PAGE_SIZE;
        resp->bininfo.max_message_size = sizeof(pkt->buf);
        resp->bininfo.uf2_family = UF2_FAMILY;
        send_hf2_response(pkt, sizeof(resp->bininfo));
        return;

    case HF2_CMD_RESET_INTO_APP:
        board_flash_flush();
        reset_millis = board_millis() + 30;
        break;
    case HF2_CMD_RESET_INTO_BOOTLOADER:
        board_reset_to_bootloader(true);
        board_flash_flush();
        reset_millis = board_millis() + 30;
        break;
    case HF2_CMD_START_FLASH:
        // userspace app should reboot into bootloader on this command; we just ignore it
        // userspace can also call hf2_handover() here
        break;
    case HF2_CMD_WRITE_FLASH_PAGE:
        checkDataSize(write_flash_page, BOARD_FLASH_PAGE_SIZE);
        // first send ACK and then start writing, while getting the next packet
        send_hf2_response(pkt, 0);
        if (cmd->write_flash_page.target_addr >= APP_START_ADDRESS) {
            board_flash_write_blocks((uint8_t*) cmd->write_flash_page.data, (cmd->write_flash_page.target_addr - BOARD_FLASH_BASE) / UF2_PAYLOAD_SIZE, 1);
        }
        return;
#if HF2_USE_HID_EXT
    case HF2_CMD_WRITE_WORDS:
        checkDataSize(write_words, cmd->write_words.num_words << 2);
        memcpy((void *)cmd->write_words.target_addr, cmd->write_words.words,
                   cmd->write_words.num_words << 2);
        break;
    case HF2_CMD_READ_WORDS:
        checkDataSize(read_words, 0);
        tmp = cmd->read_words.num_words;
        memcpy(resp->data32, (void *)cmd->read_words.target_addr, tmp << 2);
        send_hf2_response(pkt, tmp << 2);
        return;
#endif // HF2_USE_HID_EXT
    case HF2_CMD_CHKSUM_PAGES:
        checkDataSize(chksum_pages, 0);
        checksum_pages(pkt, cmd->chksum_pages.target_addr, cmd->chksum_pages.num_pages);
        return;

    default:
        // command not understood
        resp->status16 = HF2_STATUS_INVALID_CMD;
        break;
    }

    send_hf2_response(pkt, 0);
}

#endif // HF2_USE_HID

uint16_t tud_hid_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    return 0;
}

void tud_hid_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
#if HF2_USE_HID
    state.pkt.buf_in = buffer;
    state.pkt.buf_size = bufsize;
    process_core(&state.pkt);
#endif // HF2_USE_HID
}

void hf2_hid_task(void)
{
#if HF2_USE_HID
    if (!tud_hid_ready()) {
        return;
    }

    do_send_hf2();
#endif // HF2_USE_HID
}

#endif // CFG_TUD_HID
