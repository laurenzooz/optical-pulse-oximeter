

#ifndef LV_CONF_H
#define LV_CONF_H

#define TFT_HOR_RES   240
#define TFT_VER_RES   240
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0

#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1

#define LV_USE_CHART 1
#define LV_USE_SLIDER 1

#define LV_USE_FONT 1
#define LV_USE_FONT_MONTSERRAT_10  1
#define LV_FONT_MONTSERRAT_12  1
#define LV_FONT_MONTSERRAT_14  1
#define LV_FONT_MONTSERRAT_16  1
#define LV_FONT_MONTSERRAT_18  1
#define LV_FONT_MONTSERRAT_20  1
#define LV_FONT_MONTSERRAT_22  1
#define LV_FONT_MONTSERRAT_24  1


#define LV_USE_DISP_BUF_RW 1  // Allow read and write access to buffers
#define LV_DISP_DEF_BUF_SIZE (TFT_HOR_RES * 10)  // Buffer for 10 lines (adjust as needed)


#define LV_USE_REFR_PERIOD 1
#define LV_REFR_PERIOD 30

#endif /* LV_CONF_H */



