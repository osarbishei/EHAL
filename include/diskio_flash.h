/*--------------------------------------------------------------------------
File   : diskio_flash.h

Author : Hoang Nguyen Hoan          Aug. 30, 2016

Desc   : Generic flash disk I/O driver class
         NOTE : Most Flash devices work in MSB bit order
                This implementation only support MSB.
                Make sure that the Flash is configure
                for MSB mode

Copyright (c) 2016, Motsai, all rights reserved

Permission to use, copy, modify, and distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright
notice and this permission notice appear in all copies, and none of the
names : I-SYST or its contributors may be used to endorse or
promote products derived from this software without specific prior written
permission.

For info or contributing contact : nh.hoang at motsai dot com

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------------
Modified by          Date              Description

----------------------------------------------------------------------------*/
#ifndef __DISKIO_FLASH_H__
#define __DISKIO_FLASH_H__

#include <stdint.h>
#include "diskio.h"
#include "serialintrf.h"

#define FLASH_CMD_WRITE             0x2
#define FLASH_CMD_READ              0x3
#define FLASH_CMD_WRDISABLE         0x4
#define FLASH_CMD_READSTATUS        0x5
#define FLASH_CMD_WRENABLE          0x6
#define FLASH_CMD_BLOCK_ERASE       0xD8
#define FLASH_CMD_BULK_ERASE        0xC7

#define FLASH_STATUS_WIP            (1<<0)  // Write In Progress

/**
 * @brief FlashDiskIO_Init callback function
 *        This function is called at initialization to allow special
 *        initialization as require for example setting MSB mode which
 *        is required by this implementation
 *
 * @param   DevNo : Device number or address used by the interface
 * @param   pInterf : Interface used to access the flash (SPI, I2C or whatever
 * @return  true - Success
 *          false - Failed.
 */
typedef bool (*FLASHDISKIO_INIT)(int DevNo, SerialIntrf *pInterf);

typedef struct {
    int         DevNo;          // Device number or address for interface use
    uint64_t    TotalSize;      // Total Flash size in bytes
    uint32_t    EraseSize;      // Min erasable block size in byte
    uint32_t    WriteSize;      // Writable page size in bytes
    int         AddrSize;       // Address size in bytes
    FLASHDISKIO_INIT FlashInit; // Flash initialization function pointer.
                                // Set to NULL if not used
} FLASHDISKIO_CFG;

/*
 * NOTE : Most Flash devices work in MSB bit order. This implementation
 *        only supports MSB mode. Make sure that the Flash is configured
 *        for MSB mode.
 *
 */
class FlashDiskIO : public DiskIO {
public:
	FlashDiskIO() : DiskIO() {}
	virtual ~FlashDiskIO() {}

	bool Init(FLASHDISKIO_CFG &Cfg, SerialIntrf *pInterf,
	          DISKIO_CACHE_DESC *pCacheBlk = NULL, int NbChaceBlk = 0);

    /**
     *
     * @return total disk size in KB
     */
    virtual uint32_t GetSize(void) { return vTotalSize / 1024; }

    /**
	 * Device specific minimum erase size in bytes
	 *
	 * @return
	 */
	virtual uint32_t GetMinEraseSize() { return vEraseSize; }

	/**
	 * Device specific minimum write size in bytes
	 *
	 * @return
	 */
	virtual uint32_t GetMinWriteSize() { return vWriteSize; }

	/**
	 * Mass erase
	 */
	virtual void Erase();

	/**
	 * Erase Flash block.
	 *
	 * @param	BlkNo	: Starting block number to erase.
	 * 			NbBlk	: Number of consecutive blocks to erase
	 */
	virtual void EraseBlock(uint32_t BlkNo, int NbBlk);

	/**
     * Read one sector from physical device
     */
    virtual bool SectRead(uint32_t SectNo, uint8_t *pData);

    /**
     * Write one sector to physical device
     */
    virtual bool SectWrite(uint32_t SectNo, uint8_t *pData);

protected:
    void WriteDisable();
    bool WriteEnable(uint32_t Timeout = 10000);
    bool WaitReady(uint32_t Timeout = 10000);

private:
    uint32_t    vEraseSize;    // Min erasable block size in byte
    uint32_t    vWriteSize;    // Min writable size in bytes
    uint64_t    vTotalSize;    // Total Flash size in bytes
    int         vAddrSize;     // Address size in bytes
    int         vDevNo;
   SerialIntrf *vpInterf;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif	// __DISKIO_FLASH_H__

