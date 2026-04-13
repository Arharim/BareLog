#include "blog_flash.h"
#include "blog_config.h"
#include "stm32f10x.h"

static volatile uint16_t flash_pos;

#ifndef FLASH_KEY1
#	define FLASH_KEY1 ((uint32_t)0x45670123u)
#endif
#ifndef FLASH_KEY2
#	define FLASH_KEY2 ((uint32_t)0xCDEF89ABu)
#endif

static void flash_unlock(void)
{
	if ((FLASH->CR & (1u << 7u)) != 0u)
	{
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
	}
}

static void flash_lock(void)
{
	FLASH->CR |= (1u << 7u);
}

static void flash_wait_ready(void)
{
	while ((FLASH->SR & (1u << 0u)) != 0u)
	{
	}
}

static void flash_erase_page(uint32_t addr)
{
	flash_wait_ready();

	FLASH->CR |= (1u << 1u);
	*(volatile uint16_t *)addr = 0u;

	flash_wait_ready();
	FLASH->CR &= ~(1u << 1u);
}

static void flash_write_halfword(uint32_t addr, uint16_t data)
{
	flash_wait_ready();

	FLASH->CR |= (1u << 0u);
	*(volatile uint16_t *)addr = data;

	flash_wait_ready();
	FLASH->CR &= ~(1u << 0u);
}

void blog_flash_init(void)
{
	flash_pos = 0u;

	flash_unlock();
	flash_erase_page(BLOG_FLASH_SECTOR_ADDR);
	flash_lock();
}

uint16_t blog_flash_write(const uint8_t *data, uint16_t len)
{
	uint16_t i;

	flash_unlock();

	for (i = 0u; i < len; i++)
	{
		if (flash_pos >= BLOG_FLASH_SECTOR_SIZE)
		{
			break;
		}

		uint32_t addr = BLOG_FLASH_SECTOR_ADDR + (uint32_t)flash_pos;
		flash_write_halfword(addr, (uint16_t)data[i]);
		flash_pos++;
	}

	flash_lock();

	return i;
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
		data[i] = (uint8_t)(*(volatile uint16_t *)addr);
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
	flash_erase_page(BLOG_FLASH_SECTOR_ADDR);
	flash_lock();
}
