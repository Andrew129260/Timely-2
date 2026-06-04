#pragma once
#include <pebble.h>
typedef struct {
  GColor mask_color;
  GColor background_color;
  GBitmap *bitmap_mask;
  GBitmap *bitmap_background;
  char* text;
  GFont font;
  GTextOverflowMode text_overflow;
  GTextAlignment text_align;
} EffectMask;

typedef struct {
  time_t starttt;
  uint16_t startms;
  uint16_t frame;
} EffectFPS;

typedef struct {
  GColor orig_color;
  GColor offset_color;
  int8_t offset_x;
  int8_t offset_y;
  uint8_t option;
  uint8_t *aplite_visited; 
} EffectOffset;
void effect_invert(GContext* ctx, GRect position, void* param);
void effect_invert_bw_only(GContext* ctx, GRect position, void* param);
void effect_mirror_vertical(GContext* ctx, GRect position, void* param);
void effect_mirror_horizontal(GContext* ctx, GRect position, void* param);