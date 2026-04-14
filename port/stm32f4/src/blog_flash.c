#include "blog_flash.h"
#include "blog_config.h"
#include "blog_flash_port.h"
#include "stm32f4xx.h"

static volatile uint16_t flash_pos;

static void flash_unlock(void)
{
	if ((FLASH->CR & FLASH_CR_LOCK) != 0u)
	{
		FLASH->KEYR = 0x45670123u;
		FLASH->KEYR = 0xCDEF89ABu;
	}
}

static void flash_lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;
}

static void flash_wait_ready(void)
{
	while ((FLASH->SR & FLASH_SR_BSY) != 0u)
	{
	}
}

static void flash_erase_sector(uint8_t sector)
{
	flash_wait_ready();

	FLASH->CR &= ~FLASH_CR_SNB;
	FLASH->CR |= ((uint32_t)sector << 3u) & FLASH_CR_SNB;
	FLASH->CR |= FLASH_CR_SER;
	FLASH->CR |= FLASH_CR_STRT;

	flash_wait_ready();
	FLASH->CR &= ~FLASH_CR_SER;
	FLASH->CR &= ~FLASH_CR_SNB;
}

static void flash_program_word(uint32_t addr, uint32_t data)
{
	flash_wait_ready();

	FLASH->CR &= ~FLASH_CR_PSIZE;
	FLASH->CR |= FLASH_CR_PSIZE_1;
	FLASH->CR |= FLASH_CR_PG;

	*(volatile uint32_t *)addr = data;

	flash_wait_ready();
	FLASH->CR &= ~FLASH_CR_PG;
}

void blog_flash_init(void)
{
	flash_pos = 0u;

	flash_unlock();
	flash_erase_sector(BLOG_FLASH_SECTOR_NUM);
	flash_lock();
}

void blog_flash_deinit(void)
{
	flash_lock();
	flash_pos = 0u;
}

uint16_t blog_flash_write(const uint8_t *data, uint16_t len)
{
	uint16_t written = 0u;

	flash_unlock();

	while (written < len && flash_pos + 3u < BLOG_FLASH_SECTOR_SIZE)
	{
		uint32_t addr = BLOG_FLASH_SECTOR_ADDR + (uint32_t)flash_pos;
		uint32_t word = 0xFFFFFFFFu;

		uint16_t i;
		for (i = 0u; i < 4u && (written + i) < len; i++)
		{
			word &= ~((uint32_t)0xFFu << (i * 8u));
			word |= (uint32_t)data[written + i] << (i * 8u);
		}

		flash_program_word(addr, word);
		flash_pos += 4u;
		written += i;
	}

	flash_lock();
	return written;
}

void blog_flash_read(uint32_t offset, uint8_t *data, uint16_t len)
{
	uint16_t i;

	for (i = 0u; i < len; i++)
	{
		if ((offset + (uint32_t)i) >= BLOG_FLASH_SECTOR_SIZE)
		{
			break;
		}

		uint32_t addr = BLOG_FLASH_SECTOR_ADDR + offset + (uint32_t)i;
		data[i] =
		    (uint8_t)(*(volatile uint32_t *)(addr & ~3u) >> ((addr & 3u) * 8u));
	}
}

uint16_t blog_flash_used(void)
{
	return flash_pos;
}

void blog_flash_reset(void)
{
	flash_pos = 0u;

	flash_unlock();
	flash_erase_sector(BLOG_FLASH_SECTOR_NUM);
	flash_lock();
}
