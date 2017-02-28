
#include "math.h"

// Copied from:
// https://github.com/FastLED/FastLED/blob/master/colorutils.cpp
// Modified to have a 16-bit index, for more precise colors (though only
// 12 bits are actually used).
CRGB ColorFromPalette16( const CRGBPalette16& pal, uint16_t index, uint8_t brightness, TBlendType blendType)
{
    uint8_t hi4 = index >> 12; // take the highest 4 bits
    uint8_t lo8 = index >> 4;  // take the next-to-highest 8 bits, ignore the lowest 4 bits

    // const CRGB* entry = &(pal[0]) + hi4;
    // since hi4 is always 0..15, hi4 * sizeof(CRGB) can be a single-byte value,
    // instead of the two byte 'int' that avr-gcc defaults to.
    // So, we multiply hi4 X sizeof(CRGB), giving hi4XsizeofCRGB;
    uint8_t hi4XsizeofCRGB = hi4 * sizeof(CRGB);
    // We then add that to a base array pointer.
    const CRGB* entry = (CRGB*)( (uint8_t*)(&(pal[0])) + hi4XsizeofCRGB);

    uint8_t blend = lo8 && (blendType != NOBLEND);

    uint8_t red1   = entry->red;
    uint8_t green1 = entry->green;
    uint8_t blue1  = entry->blue;


    if( blend ) {

        if( hi4 == 15 ) {
            entry = &(pal[0]);
        } else {
            entry++;
        }

        uint8_t f2 = lo8;
        uint8_t f1 = 255 - f2;

        //    rgb1.nscale8(f1);
        uint8_t red2   = entry->red;
        red1   = scale8_LEAVING_R1_DIRTY( red1,   f1);
        red2   = scale8_LEAVING_R1_DIRTY( red2,   f2);
        red1   += red2;

        uint8_t green2 = entry->green;
        green1 = scale8_LEAVING_R1_DIRTY( green1, f1);
        green2 = scale8_LEAVING_R1_DIRTY( green2, f2);
        green1 += green2;

        uint8_t blue2  = entry->blue;
        blue1  = scale8_LEAVING_R1_DIRTY( blue1,  f1);
        blue2  = scale8_LEAVING_R1_DIRTY( blue2,  f2);
        blue1  += blue2;

        cleanup_R1();
    }

    if( brightness != 255) {
        if( brightness ) {
            brightness++; // adjust for rounding
            // Now, since brightness is nonzero, we don't need the full scale8_video logic;
            // we can just to scale8 and then add one (unless scale8 fixed) to all nonzero inputs.
            if( red1 )   {
                red1 = scale8_LEAVING_R1_DIRTY( red1, brightness);
#if !(FASTLED_SCALE8_FIXED==1)
                red1++;
#endif
            }
            if( green1 ) {
                green1 = scale8_LEAVING_R1_DIRTY( green1, brightness);
#if !(FASTLED_SCALE8_FIXED==1)
                green1++;
#endif
            }
            if( blue1 )  {
                blue1 = scale8_LEAVING_R1_DIRTY( blue1, brightness);
#if !(FASTLED_SCALE8_FIXED==1)
                blue1++;
#endif
            }
            cleanup_R1();
        } else {
            red1 = 0;
            green1 = 0;
            blue1 = 0;
        }
    }

    return CRGB( red1, green1, blue1);
}

