#ifndef BLOG_FLASH_H
#define BLOG_FLASH_H

#include <stdint.h>

#ifndef BLOG_FLASH_SECTOR_ADDR
#	define BLOG_FLASH_SECTOR_ADDR 0x0800F000u
#endif

#ifndef BLOG_FLASH_SECTOR_SIZE
#	define BLOG_FLASH_SECTOR_SIZE 1024u
#endif

void blog_flash_init(void);
uint16_t blog_flash_write(const uint8_t *data, uint16_t len);
void blog_flash_read(uint32_t offset, uint8_t *data, uint16_t len);
uint16_t blog_flash_used(void);
void blog_flash_reset(void);

#endif
