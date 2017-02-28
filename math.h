
#pragma once

#include <FastLED.h>

CRGB ColorFromPalette16(const CRGBPalette16& pal,
                        uint16_t index,
                        uint8_t brightness=255,
                        TBlendType blendType=LINEARBLEND);
