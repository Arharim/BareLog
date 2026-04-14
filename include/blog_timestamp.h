#ifndef BLOG_TIMESTAMP_H
#define BLOG_TIMESTAMP_H

#include <stdint.h>

void blog_timestamp_init(void);
void blog_timestamp_deinit(void);
uint32_t blog_timestamp_get(void);

#endif
