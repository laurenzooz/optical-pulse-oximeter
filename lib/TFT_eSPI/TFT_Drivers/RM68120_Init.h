// Initialisation for RM68120

//ENABLE PAGE 1
writeRegister8(0xF000, 0x55);
writeRegister8(0xF001, 0xAA);
writeRegister8(0xF002, 0x52);
writeRegister8(0xF003, 0x08);
writeRegister8(0xF004, 0x01);

//GAMMA SETING  RED
writeRegister8(0xD400, 0x00);
writeRegister8(0xD401, 0x00);
writeRegister8(0xD402, 0x1b);
writeRegister8(0xD403, 0x44);
writeRegister8(0xD404, 0x62);
writeRegister8(0xD405, 0x00);
writeRegister8(0xD406, 0x7b);
writeRegister8(0xD407, 0xa1);
writeRegister8(0xD408, 0xc0);
writeRegister8(0xD409, 0xee);
writeRegister8(0xD40A, 0x55);
writeRegister8(0xD40B, 0x10);
writeRegister8(0xD40C, 0x2c);
writeRegister8(0xD40D, 0x43);
writeRegister8(0xD40E, 0x57);
writeRegister8(0xD40F, 0x55);
writeRegister8(0xD410, 0x68);
writeRegister8(0xD411, 0x78);
writeRegister8(0xD412, 0x87);
writeRegister8(0xD413, 0x94);
writeRegister8(0xD414, 0x55);
writeRegister8(0xD415, 0xa0);
writeRegister8(0xD416, 0xac);
writeRegister8(0xD417, 0xb6);
writeRegister8(0xD418, 0xc1);
writeRegister8(0xD419, 0x55);
writeRegister8(0xD41A, 0xcb);
writeRegister8(0xD41B, 0xcd);
writeRegister8(0xD41C, 0xd6);
writeRegister8(0xD41D, 0xdf);
writeRegister8(0xD41E, 0x95);
writeRegister8(0xD41F, 0xe8);
writeRegister8(0xD420, 0xf1);
writeRegister8(0xD421, 0xfa);
writeRegister8(0xD422, 0x02);
writeRegister8(0xD423, 0xaa);
writeRegister8(0xD424, 0x0b);
writeRegister8(0xD425, 0x13);
writeRegister8(0xD426, 0x1d);
writeRegister8(0xD427, 0x26);
writeRegister8(0xD428, 0xaa);
writeRegister8(0xD429, 0x30);
writeRegister8(0xD42A, 0x3c);
writeRegister8(0xD42B, 0x4A);
writeRegister8(0xD42C, 0x63);
writeRegister8(0xD42D, 0xea);
writeRegister8(0xD42E, 0x79);
writeRegister8(0xD42F, 0xa6);
writeRegister8(0xD430, 0xd0);
writeRegister8(0xD431, 0x20);
writeRegister8(0xD432, 0x0f);
writeRegister8(0xD433, 0x8e);
writeRegister8(0xD434, 0xff);

//GAMMA SETING GREEN
writeRegister8(0xD500, 0x00);
writeRegister8(0xD501, 0x00);
writeRegister8(0xD502, 0x1b);
writeRegister8(0xD503, 0x44);
writeRegister8(0xD504, 0x62);
writeRegister8(0xD505, 0x00);
writeRegister8(0xD506, 0x7b);
writeRegister8(0xD507, 0xa1);
writeRegister8(0xD508, 0xc0);
writeRegister8(0xD509, 0xee);
writeRegister8(0xD50A, 0x55);
writeRegister8(0xD50B, 0x10);
writeRegister8(0xD50C, 0x2c);
writeRegister8(0xD50D, 0x43);
writeRegister8(0xD50E, 0x57);
writeRegister8(0xD50F, 0x55);
writeRegister8(0xD510, 0x68);
writeRegister8(0xD511, 0x78);
writeRegister8(0xD512, 0x87);
writeRegister8(0xD513, 0x94);
writeRegister8(0xD514, 0x55);
writeRegister8(0xD515, 0xa0);
writeRegister8(0xD516, 0xac);
writeRegister8(0xD517, 0xb6);
writeRegister8(0xD518, 0xc1);
writeRegister8(0xD519, 0x55);
writeRegister8(0xD51A, 0xcb);
writeRegister8(0xD51B, 0xcd);
writeRegister8(0xD51C, 0xd6);
writeRegister8(0xD51D, 0xdf);
writeRegister8(0xD51E, 0x95);
writeRegister8(0xD51F, 0xe8);
writeRegister8(0xD520, 0xf1);
writeRegister8(0xD521, 0xfa);
writeRegister8(0xD522, 0x02);
writeRegister8(0xD523, 0xaa);
writeRegister8(0xD524, 0x0b);
writeRegister8(0xD525, 0x13);
writeRegister8(0xD526, 0x1d);
writeRegister8(0xD527, 0x26);
writeRegister8(0xD528, 0xaa);
writeRegister8(0xD529, 0x30);
writeRegister8(0xD52A, 0x3c);
writeRegister8(0xD52B, 0x4a);
writeRegister8(0xD52C, 0x63);
writeRegister8(0xD52D, 0xea);
writeRegister8(0xD52E, 0x79);
writeRegister8(0xD52F, 0xa6);
writeRegister8(0xD530, 0xd0);
writeRegister8(0xD531, 0x20);
writeRegister8(0xD532, 0x0f);
writeRegister8(0xD533, 0x8e);
writeRegister8(0xD534, 0xff);

//GAMMA SETING BLUE
writeRegister8(0xD600, 0x00);
writeRegister8(0xD601, 0x00);
writeRegister8(0xD602, 0x1b);
writeRegister8(0xD603, 0x44);
writeRegister8(0xD604, 0x62);
writeRegister8(0xD605, 0x00);
writeRegister8(0xD606, 0x7b);
writeRegister8(0xD607, 0xa1);
writeRegister8(0xD608, 0xc0);
writeRegister8(0xD609, 0xee);
writeRegister8(0xD60A, 0x55);
writeRegister8(0xD60B, 0x10);
writeRegister8(0xD60C, 0x2c);
writeRegister8(0xD60D, 0x43);
writeRegister8(0xD60E, 0x57);
writeRegister8(0xD60F, 0x55);
writeRegister8(0xD610, 0x68);
writeRegister8(0xD611, 0x78);
writeRegister8(0xD612, 0x87);
writeRegister8(0xD613, 0x94);
writeRegister8(0xD614, 0x55);
writeRegister8(0xD615, 0xa0);
writeRegister8(0xD616, 0xac);
writeRegister8(0xD617, 0xb6);
writeRegister8(0xD618, 0xc1);
writeRegister8(0xD619, 0x55);
writeRegister8(0xD61A, 0xcb);
writeRegister8(0xD61B, 0xcd);
writeRegister8(0xD61C, 0xd6);
writeRegister8(0xD61D, 0xdf);
writeRegister8(0xD61E, 0x95);
writeRegister8(0xD61F, 0xe8);
writeRegister8(0xD620, 0xf1);
writeRegister8(0xD621, 0xfa);
writeRegister8(0xD622, 0x02);
writeRegister8(0xD623, 0xaa);
writeRegister8(0xD624, 0x0b);
writeRegister8(0xD625, 0x13);
writeRegister8(0xD626, 0x1d);
writeRegister8(0xD627, 0x26);
writeRegister8(0xD628, 0xaa);
writeRegister8(0xD629, 0x30);
writeRegister8(0xD62A, 0x3c);
writeRegister8(0xD62B, 0x4A);
writeRegister8(0xD62C, 0x63);
writeRegister8(0xD62D, 0xea);
writeRegister8(0xD62E, 0x79);
writeRegister8(0xD62F, 0xa6);
writeRegister8(0xD630, 0xd0);
writeRegister8(0xD631, 0x20);
writeRegister8(0xD632, 0x0f);
writeRegister8(0xD633, 0x8e);
writeRegister8(0xD634, 0xff);

//AVDD VOLTAGE SETTING
writeRegister8(0xB000, 0x05);
writeRegister8(0xB001, 0x05);
writeRegister8(0xB002, 0x05);
//AVEE VOLTAGE SETTING
writeRegister8(0xB100, 0x05);
writeRegister8(0xB101, 0x05);
writeRegister8(0xB102, 0x05);

//AVDD Boosting
writeRegister8(0xB600, 0x34);
writeRegister8(0xB601, 0x34);
writeRegister8(0xB603, 0x34);
//AVEE Boosting
writeRegister8(0xB700, 0x24);
writeRegister8(0xB701, 0x24);
writeRegister8(0xB702, 0x24);
//VCL Boosting
writeRegister8(0xB800, 0x24);
writeRegister8(0xB801, 0x24);
writeRegister8(0xB802, 0x24);
//VGLX VOLTAGE SETTING
writeRegister8(0xBA00, 0x14);
writeRegister8(0xBA01, 0x14);
writeRegister8(0xBA02, 0x14);
//VCL Boosting
writeRegister8(0xB900, 0x24);
writeRegister8(0xB901, 0x24);
writeRegister8(0xB902, 0x24);
//Gamma Voltage
writeRegister8(0xBc00, 0x00);
writeRegister8(0xBc01, 0xa0);//vgmp=5.0
writeRegister8(0xBc02, 0x00);
writeRegister8(0xBd00, 0x00);
writeRegister8(0xBd01, 0xa0);//vgmn=5.0
writeRegister8(0xBd02, 0x00);
//VCOM Setting
writeRegister8(0xBe01, 0x3d);//3

//ENABLE PAGE 0
writeRegister8(0xF000, 0x55);
writeRegister8(0xF001, 0xAA);
writeRegister8(0xF002, 0x52);
writeRegister8(0xF003, 0x08);
writeRegister8(0xF004, 0x00);
//Vivid Color Function Control
writeRegister8(0xB400, 0x10);
//Z-INVERSION
writeRegister8(0xBC00, 0x05);
writeRegister8(0xBC01, 0x05);
writeRegister8(0xBC02, 0x05);
//*************** add on 20111021**********************//
writeRegister8(0xB700, 0x22);//GATE EQ CONTROL
writeRegister8(0xB701, 0x22);//GATE EQ CONTROL
writeRegister8(0xC80B, 0x2A);//DISPLAY TIMING CONTROL
writeRegister8(0xC80C, 0x2A);//DISPLAY TIMING CONTROL
writeRegister8(0xC80F, 0x2A);//DISPLAY TIMING CONTROL
writeRegister8(0xC810, 0x2A);//DISPLAY TIMING CONTROL
//*************** add on 20111021**********************//
//PWM_ENH_OE =1
writeRegister8(0xd000, 0x01);
//DM_SEL =1
writeRegister8(0xb300, 0x10);
//VBPDA=07h
writeRegister8(0xBd02, 0x07);
//VBPDb=07h
writeRegister8(0xBe02, 0x07);
//VBPDc=07h
writeRegister8(0xBf02, 0x07);

//ENABLE PAGE 2
writeRegister8(0xF000, 0x55);
writeRegister8(0xF001, 0xAA);
writeRegister8(0xF002, 0x52);
writeRegister8(0xF003, 0x08);
writeRegister8(0xF004, 0x02);
//SDREG0 =0
writeRegister8(0xc301, 0xa9);
//DS=14
writeRegister8(0xfe01, 0x94);
//OSC =60h
writeRegister8(0xf600, 0x60);
//TE ON
writeRegister8(0x3500, 0x00);
writeRegister8(0xFFFF, 0xFF);

//SLEEP OUT
writecommand(0x1100);
delay(100);
//DISPLY ON
writecommand(0x2900);
delay(100);

writeRegister16(0x3A00, 0x55);
writeRegister8(0x3600, TFT_MAD_COLOR_ORDER);
