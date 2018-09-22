#include <Arduino.h>
#include "OctoWS2811.h"

extern "C" {
    #include "font.h"
}

//#define TEST_MODE
#define SIZE_X 32
#define SIZE_Y 8

#ifdef TEST_MODE
    #define COLOR_PURPLE    0x110011
    #define COLOR_GOLD      0x111100
    #define COLOR_GREEN     0x001100
#else
    #define COLOR_PURPLE    0xFF00FF
    #define COLOR_GOLD      0xFFDD00
    #define COLOR_GREEN     0x00FF00
#endif

// ---------------------------- Function Prototypes ---------------------------

int16_t xyToPixel(int16_t x, int16_t y);
void setPixelXY(int16_t x, int16_t y, uint32_t color);
void fillDisplay(uint32_t color);
void showChar(uint8_t ch, uint32_t color, int16_t posX, int16_t posY);
void showText(const char * msg, uint32_t color, int16_t posX, int16_t posY);
void scrollUp(const char * msg, uint32_t color, uint16_t posX, uint16_t wait);
void scrollDown(const char * msg, uint32_t color, uint16_t posX, uint16_t wait);
void scrollHorizontal(const char * msg, uint32_t color, int16_t startPosX, int16_t endPosX, int16_t posY, int8_t dir, uint16_t wait);

// ------------------------------ Initializations -----------------------------

DMAMEM int displayMemory[SIZE_X * 6];
int drawingMemory[SIZE_X * 6];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(SIZE_X, displayMemory, drawingMemory, config);

uint16_t pixelMap[SIZE_X][SIZE_Y];      // array with number of every pixel
uint32_t frameBuffer[SIZE_X][SIZE_Y];   // display buffer

// ------------------------------- Main Program -------------------------------

int main(){
    Serial.begin(115200);
    leds.begin();
    leds.show();

    // create mapping array
    uint16_t x, y;
    uint16_t pixel = leds.numPixels() - SIZE_X;
    for(y = 0; y < SIZE_Y; y++){
        for(x = 0; x < SIZE_X; x++){
            pixelMap[x][y] = pixel++;
        }
        pixel -= 2 * SIZE_X;
    }

    uint32_t colors[] = {COLOR_PURPLE, COLOR_GOLD, COLOR_GREEN};
    uint32_t cycleCount = 0;

    while(1){
        scrollHorizontal("NEW ORLEANS", colors[cycleCount++ % 3], SIZE_X, -66, 0, -1, 40);
        
        scrollDown("CLASS", COLOR_PURPLE, 2, 30);
        delay(1000);
        scrollUp("OF", COLOR_GOLD, 11, 30);
        delay(1000);
        scrollDown("2022", COLOR_GREEN, 5, 30);
        delay(1500);
    }
}

// ----------------------------- Helper Functions -----------------------------

int16_t xyToPixel(int16_t x, int16_t y){
    if((x > -1) && (x < SIZE_X) && (y > -1) && (y < SIZE_Y)){
        return pixelMap[x][y];
    }else{
        return -1;
    }
}

void setPixelXY(int16_t x, int16_t y, uint32_t color){
    int16_t pixel = xyToPixel(x, y);
    if(pixel > -1){
        leds.setPixel(pixel, color);
    }
}

void fillDisplay(uint32_t color){
    uint16_t pixel;
    for(pixel = 0; pixel < leds.numPixels(); pixel++){
        leds.setPixel(pixel, color);
    }
}

void showChar(uint8_t ch, uint32_t color, int16_t posX, int16_t posY){
    uint16_t x, y;
    for(x = 0; x < FONT_W; x++){
        for(y = 0; y < FONT_H; y++){
            if((alphabet[ch-0x20][x] >> y) & 1){
                setPixelXY(x + posX, y + posY, color);
            }
        }
    }
}

void showText(const char * msg, uint32_t color, int16_t posX, int16_t posY){
    uint16_t i = 0;
    while(msg[i] != '\0'){
        showChar(msg[i], color, posX + (i * 6), posY);
        i++;
    }
}

void scrollUp(const char * msg, uint32_t color, uint16_t posX, uint16_t wait){
    int16_t y;
    for(y = SIZE_Y; y >= 0; y--){
        fillDisplay(0);
        showText(msg, color, posX, y);
        leds.show();
        delay(wait);
    }
}

void scrollDown(const char * msg, uint32_t color, uint16_t posX, uint16_t wait){
    int16_t y;
    for(y = -FONT_H; y <= 0; y++){
        fillDisplay(0);
        showText(msg, color, posX, y);
        leds.show();
        delay(wait);
    }
}

void scrollHorizontal(const char * msg, uint32_t color, int16_t startPosX, int16_t endPosX, int16_t posY, int8_t dir, uint16_t wait){
    int16_t x;
    
    for(x = startPosX; x != (endPosX + dir); x += dir){
        fillDisplay(0);
        showText(msg, color, x, posY);
        leds.show();
        delay(wait);
    }
}
