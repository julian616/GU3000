//original by NORITAKE
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>


#include "VFD_GU3000.h"


void VFD_GU3000::GU3000_back() {
    commandd(0x08);
}

void VFD_GU3000::GU3000_forward() {
    commandd(0x09);
}

void VFD_GU3000::GU3000_lineFeed() {
    commandd(0x0a);
}

void VFD_GU3000::GU3000_home() {
    commandd(0x0b);
}

void VFD_GU3000::GU3000_carriageReturn() {
    commandd(0x0d);
}

void VFD_GU3000::GU3000_setCursor(unsigned x, unsigned y) {
    commandd(0x1f);
    commandd('$');
    command_xy(x, y);
}

void VFD_GU3000::GU3000_clearScreen() {
    commandd(0x0c);
}

void VFD_GU3000::GU3000_clearLine() {
    commandd(0x18);
}

void VFD_GU3000::GU3000_clearLineEnd() {
    commandd(0x019);
}

void VFD_GU3000::GU3000_cursorOn() {
    commandd(0x1f, 'C', 1);
}

void VFD_GU3000::GU3000_cursorOff() {
    commandd(0x1f, 'C', 0);
}
/*
void VFD_GU3000::GU3000_init() {
    _delay_ms(500);
    initPort();
    command(0x1b);
    command('@');
}

void initPort()
{
  RAISE(OUT);
  DIRECTION(OUT,1);
}
*/
void VFD_GU3000::GU3000_useMultibyteChars(bool enable) {
    us_command('g', 0x02);
    commandd(enable);
}

void VFD_GU3000::GU3000_setMultibyteCharset(uint8_t code) {
    us_command('g', 0x0f);
    commandd(code);
}

void VFD_GU3000::GU3000_useCustomChars(bool enable) {
    commandd(0x1b, '%', enable);
}

static inline uint8_t getColumn(const uint8_t *src, int col) {
    uint8_t out = 0;
    for (int i=0; i<8; i++)
        if (src[i] & (1<<(4-col))) out += 1<<(7-i);
    return out;
}

void VFD_GU3000::GU3000_defineCustomChar(unsigned code, FontFormat format, const uint8_t *data) {
    if (format == _16x16Format || format == _32x32Format) {
        us_command('g', format);
        commandd(code >> 8);
        commandd(code);
        print((const char*)data, format==_16x16Format? 32: 128);
        return;
    }
    
    commandd(0x1b, '&', format & 0xf);
    commandd(code);
    commandd(code);
    
    switch (format) {
    case CUUFormat:
        commandd(5);
        for (uint8_t i=0; i<5; i++)
            commandd(getColumn(data, i));
        break;
    case _6x8Format:
        commandd(6);
        print((const char*)data, 6);
        break;
    case _8x16Format:
        commandd(8);
        print((const char*)data, 16);
        break;
    case _12x24Format:
        commandd(12);
        print((const char*)data, 36);
        break;
    case _16x32Format:
        commandd(16);
        print((const char*)data, 64);
        break;
    default: ;
    }
}

void VFD_GU3000::GU3000_deleteCustomChar(unsigned code, FontFormat format) {
    if (format == _16x16Format || format == _32x32Format) {
        us_command('g', format+1);
        commandd(code >> 8);
        commandd(code);
    } else {
        commandd(0x01b, '?', format & 0xf);
        commandd(code);
    }
}

void VFD_GU3000::GU3000_setAsciiVariant(AsciiVariant code) {
    if (code < 0x0d)
        commandd(0x1b, 'R', code);
}

void VFD_GU3000::GU3000_setCharset(Charset code) {
    if (code < 0x05 || (0x10 <= code && code <= 0x13))
        commandd(0x1b, 't', code);
}

void VFD_GU3000::GU3000_setScrollMode(ScrollMode mode) {
    commandd(0x1f);
    commandd(mode);
}

void VFD_GU3000::GU3000_setHorizScrollSpeed(uint8_t speed) {
    commandd(0x1f, 's', speed);
}

void VFD_GU3000::GU3000_invertOff() {
    commandd(0x1f, 'r', 0);
}

void VFD_GU3000::GU3000_invertOn() {
    commandd(0x1f, 'r', 1);
}

void VFD_GU3000::GU3000_setCompositionMode(CompositionMode mode) {
    commandd(0x1f, 'w', mode);
}

void VFD_GU3000::GU3000_setScreenBrightness(unsigned level) {
    if (level <= 100)
        commandd(0x1f, 'X', 0x10 + (level*10 + 120)/125);
}

void VFD_GU3000::GU3000_wait(uint8_t time) {
    us_command('a', 0x01);
    commandd(time);
}

void VFD_GU3000::GU3000_shortWait(uint8_t time) {
    us_command('a', 0x02);
    commandd(time);
}
/*
void VFD_GU3000::GU3000_scrollScreen(unsigned x, unsigned y, unsigned times, uint8_t speed) {
    unsigned pos = (x*NORITAKE_VFD_LINES)+(y/8);
    us_command('a', 0x10);
    command(pos);
    command(pos>>8);
    command(times);
    command(times>>8);
    command(speed);
}
*/
void VFD_GU3000::GU3000_wipeScreen(WipeDirection direction, unsigned speed, uint8_t filler) {
    us_command('a', 0x12);
    commandd(direction);
    commandd(speed);
    commandd(filler);
}

void VFD_GU3000::GU3000_reverseWipeScreen(WipeDirection direction, unsigned speed, unsigned addr) {
    us_command('a', 0x13);
    commandd(direction);
    commandd(speed);
    commandd(addr);
    commandd(addr >> 8);
}

void VFD_GU3000::GU3000_disolveScreen(unsigned speed, unsigned addr) {
    us_command('a', 0x14);
    commandd(speed);
    commandd(addr);
    commandd(addr >> 8);
}

void VFD_GU3000::GU3000_blinkScreen(bool enable, bool reverse, uint8_t onTime, uint8_t offTime, uint8_t times) {
    us_command('a', 0x11);
    commandd(enable? (reverse? 2: 1): 0);
    commandd(onTime);
    commandd(offTime);
    commandd(times);
}

void VFD_GU3000::GU3000_displayOff() {
    us_command('a', 0x40);
    commandd(0);
}

void VFD_GU3000::GU3000_displayOff(uint8_t timer) {
    us_command('a', 0x40);
    commandd(0x11);
    commandd(timer);
    us_command('a', 0x40);
    commandd(0x10);
}

void VFD_GU3000::GU3000_displayOn() {
    us_command('a', 0x40);
    commandd(0x01);
}

void VFD_GU3000::GU3000_boldOn() {
    us_command('g', 0x41);
    commandd(1);
}

void VFD_GU3000::GU3000_boldOff() {
    us_command('g', 0x41);
    commandd(0);
}

void VFD_GU3000::GU3000_setFontStyle(FontStyle style) {
    us_command('g', 0x04);
    commandd(style);
}

void VFD_GU3000::GU3000_setFontSize(FontFormat format, uint8_t x, uint8_t y) {
    if (x<=4 && y<=2) {        
        us_command('g', 0x40);
        commandd(x);
        commandd(y);
        us_command('g', 0x01);
        commandd(format);
    }
}

void VFD_GU3000::GU3000_selectWindow(uint8_t window) {
    us_command('w', 0x01);
    commandd(window);
}

void VFD_GU3000::GU3000_defineWindow(uint8_t window, unsigned x, unsigned y, unsigned width, unsigned height) {
    us_command('w', 0x02);
    commandd(window);
    commandd(0x01);
    command_xy(x, y);
    command_xy(width, height);
}

void VFD_GU3000::GU3000_deleteWindow(uint8_t window) {
    us_command('w', 0x02);
    commandd(window);
    commandd(0);
    command_xy(0, 0);
    command_xy(0, 0);
}

void VFD_GU3000::GU3000_joinScreens() {
    us_command('w', 0x10);
    commandd(0x01);
}

void VFD_GU3000::GU3000_separateScreens() {
    us_command('w', 0x10);
    commandd(0);
}

void VFD_GU3000::print(char c) {
    commandd(c);
}

void VFD_GU3000::print(const char *str) {
    while (*str)
        commandd(*str++);
}

void VFD_GU3000::print(const char *buffer, size_t size) {
    while (size--)
        print(*buffer++);
}

void VFD_GU3000::print(long number, uint8_t base) {
    if (number < 0) {
        print('-');
        number = -number;
    }
    printNumber(number, base);
}

void VFD_GU3000::print(int number, uint8_t base) {
    print((long)number, base);
}

void VFD_GU3000::print(unsigned long number, uint8_t base) {
    printNumber(number, base);
}

void VFD_GU3000::print(unsigned number, uint8_t base) {
    print((unsigned long)number, base);
}

void VFD_GU3000::crlf() {
    GU3000_carriageReturn();
    GU3000_lineFeed();
}

void VFD_GU3000::println(char c) {
    print(c);
    crlf();
}

void VFD_GU3000::println(const char *str) {
    print(str);
    crlf();
}

void VFD_GU3000::println(const char *buffer, size_t size) {
    print(buffer, size);
    crlf();
}

void VFD_GU3000::println(long number, uint8_t base) {
    print(number, base);
    crlf();
}

void VFD_GU3000::println(int number, uint8_t base) {
    println((long) number, base);
}

void VFD_GU3000::println(unsigned long number, uint8_t base) {
    print(number, base);
    crlf();
}

void VFD_GU3000::println(unsigned number, uint8_t base) {
    println((unsigned long) number, base);
}

void VFD_GU3000::printNumber(unsigned long number, uint8_t base) {
    if (number/base)
        printNumber(number/base, base);
    number %= base;
    print(number + (number < 10? '0': 'A' - 10));
}

void VFD_GU3000::GU3000_drawImage(unsigned width, uint8_t height, const uint8_t *data) {
    if (height > 128) return;
    us_command('f', 0x11);
    command_xy(width, height);
    commandd((uint8_t) 1);
    for (unsigned i = 0; i<(height/8)*width; i++)
        commandd(data[i]);
}

void VFD_GU3000::GU3000_dot(unsigned x, unsigned y, bool on) {
    us_command('d', 0x10);
    commandd(on);
    commandd(x);
    commandd(x>>8);
    commandd(y);
    commandd(y>>8);
}
void VFD_GU3000::GU3000_shape(Shape shape, unsigned x0, unsigned y0, unsigned x1, unsigned y1, bool on) {
    us_command('d', 0x11);
    commandd(shape);
    commandd(on);
    commandd(x0);
    commandd(x0>>8);
    commandd(y0);
    commandd(y0>>8);
    commandd(x1);
    commandd(x1>>8);
    commandd(y1);
    commandd(y1>>8);
}
////////////////////////////////////////////////////////
void VFD_GU3000::commandd(uint8_t data) {
  const int RDY = 12; //J1-14
  const int WR =  13; //J1-2
  const int DB0 =  2; //J1-10
  const int DB1 =  3; //J1-9
  const int DB2 =  4; //J1-8
  const int DB3 =  5; //J1-7
  const int DB4 =  6; //J1-6
  const int DB5 =  7; //J1-5
  const int DB6 =  8; //J1-4
  const int DB7 =  9; //J1-3
//Add all data ports to an Array 
int outData[8]={DB0,DB1,DB2,DB3,DB4,DB5,DB6,DB7}; 
  pinMode(RDY, OUTPUT);   
  pinMode(WR, OUTPUT);      
  pinMode(DB0, OUTPUT);      
  pinMode(DB1, OUTPUT);      
  pinMode(DB2, OUTPUT);      
  pinMode(DB3, OUTPUT);      
  pinMode(DB4, OUTPUT);      
  pinMode(DB5, OUTPUT);      
  pinMode(DB6, OUTPUT);      
  pinMode(DB7, OUTPUT);
  //RESET 
  delay(250);
    int i=0;
   
   for (byte mask = B00000001 ; mask > 0 ; mask <<= 1) 
   { 
     if (data & mask)        // if bitwise AND resolves to true
     { 
       digitalWrite(outData[i],HIGH);  
     }
    else    //if bitwise and resolves to false
     { 
       digitalWrite(outData[i],LOW);
     }
    i++;
  }
  digitalWrite(WR, 1);
  delayMicroseconds(250);
  digitalWrite(WR, 0); 
 
}

void VFD_GU3000::command_xy(unsigned x, unsigned y) {
    commandd(x);
    commandd(x>>8);
    y /= 8;
    commandd(y);
    commandd(y>>8);
}
void VFD_GU3000::command_xy1(unsigned x, unsigned y) {
    commandd(x);
    commandd(x>>8);
    commandd(y);
    commandd(y>>8);
}

void VFD_GU3000::us_command(uint8_t group, uint8_t cmd) {
   commandd(0x1f);
   commandd(0x28);
   commandd(group);
   commandd(cmd);
}

void VFD_GU3000::commandd(uint8_t prefix, uint8_t group, uint8_t cmd) {
   commandd(prefix);
   commandd(group);
   commandd(cmd);
}

void VFD_GU3000::print(unsigned x, uint8_t y, const char *buffer, uint8_t len) {
    us_command('d', 0x30);
    command_xy1(x, y);
    commandd(0);
    commandd(len);
    while (len--)
        commandd(*buffer++);
}

void VFD_GU3000::print(unsigned x, uint8_t y, const char *str) {
    print(x, y, str, strlen(str));
}
void VFD_GU3000::print(unsigned x, uint8_t y, char c) {
    print(x, y, &c, 1);
}
void VFD_GU3000::print(unsigned x, uint8_t y, int number, uint8_t base) {
    if (number < 0) {
        print(x, y, '-');
        print(-1, y, (unsigned)-number, base);        
    } else
        print(x, y, (unsigned)number, base);
}
void VFD_GU3000::print(unsigned x, uint8_t y, unsigned number, uint8_t base) {
    char buf[16], *p = buf + sizeof buf;
    do
        *--p = number % base + (number % base < 10? '0': 'A' - 10);
    while (number /= base);
    print(x, y, p, buf + sizeof buf - p);
}
void VFD_GU3000::GU3000_drawImage(unsigned x, uint8_t y, unsigned width, uint8_t height, const uint8_t *data) {
    us_command('d', 0x21);
    command_xy1(x, y);
    command_xy1(width, height);
    commandd(0x01);
    for (unsigned i = 0; i<(height/8)*width; i++)
        commandd(data[i]);
}
void VFD_GU3000::GU3000_drawImage(unsigned x, uint8_t y, ImageMemoryArea area, unsigned long address, uint8_t srcHeight, unsigned width, uint8_t height, unsigned offsetx, unsigned offsety) {
    if (height > 128) return;
    us_command('d', 0x20);
    command_xy1(x, y);
    commandd(area);
    commandd(address);
    commandd(address>>8);
    commandd(address>>16);
    commandd(srcHeight/8);
    commandd(srcHeight/8>>8);
    command_xy1(offsetx, offsety);
    command_xy1(width, height);
    commandd(0x01);
}

void VFD_GU3000::GU3000_drawImage(unsigned x, uint8_t y, ImageMemoryArea area, unsigned long address, unsigned width, uint8_t height) {
    GU3000_drawImage(x, y, area, address, (height + 7) & ~7, width, height, 0, 0);
}

void VFD_GU3000::print_p(const char *str) {
    while (pgm_read_byte(str))
        commandd(pgm_read_byte(str++)); // "commond was writePort"
}
void VFD_GU3000::print_p(unsigned x, uint8_t y, const char *buffer, uint8_t len) {
    us_command('d', 0x30);
    command_xy1(x, y);
    commandd(0);
    commandd(len);
    while (len--)
        commandd(pgm_read_byte(buffer++));
}

void VFD_GU3000::print_p(unsigned x, uint8_t y, const char *str) {
    const char *end = str;
    while (pgm_read_byte(end)) end++;
    print_p(x, y, str, end - str);
}

void VFD_GU3000::GU3000_drawImage_p(unsigned width, uint8_t height, const uint8_t *data) {
    if (height > 128) return;
    us_command('f', 0x11);
    command_xy(width, height);
    commandd((uint8_t) 1);
    for (unsigned i = 0; i<(height/8)*width; i++)
        commandd(pgm_read_byte(data+i));
}

void VFD_GU3000::GU3000_drawImage_p(unsigned x, uint8_t y, unsigned width, uint8_t height, const uint8_t *data) {
    us_command('d', 0x21);
    command_xy1(x, y);
    command_xy1(width, height);
    commandd(0x01);
    for (unsigned i = 0; i<(height/8)*width; i++)
        commandd(pgm_read_byte(data+i));
}
/*
void VFD_GU3000::GU3000_drawImage(ImageMemoryArea area, unsigned long address, uint8_t srcHeight, unsigned width, uint8_t height) {
    if (height > 128) return;
    us_command('f', 0x10);
    command(area);
    command(address);
    command(address>>8);
    command(address>>16);
    command(srcHeight/8);
    command((srcHeight/8)>>8);
    command(width);
    command(width>>8);
    command(height/8);
    command((height/8)>>8);
    command((uint8_t) 1);
}

void VFD_GU3000::GU3000_scrollImage(ImageMemoryArea area, unsigned long address, uint8_t srcHeight, unsigned width, uint8_t height, uint8_t speed) {
    if (height > 128) return;
    us_command('f', 0x90);
    command(area);
    command(address);
    command(address>>8);
    command(address>>16);
    command(srcHeight/8);
    command((srcHeight/8)>>8);
    command(width);
    command(width>>8);
    command(height/8);
    command((height/8)>>8);
    command((uint8_t) 1);
    command(speed);
}
*/
