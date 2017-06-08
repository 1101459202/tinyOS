#ifndef TLIB_H
#define TLIB_H

#include <stdint.h>

typedef struct
{
	uint32_t bitmap;
}tBitmap;

uint32_t tBitmapPosCount (void);
void tBitmapInit (tBitmap * bitmap);
void tBitmapSet (tBitmap * bitmap,uint32_t pos);
void tBitmapClear (tBitmap * bitmap,uint32_t pos);
uint32_t tBitmapGetFirstSet (tBitmap * bitmap);
#endif
