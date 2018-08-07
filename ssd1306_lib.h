
#ifndef SSD1306_LIB_H_
#define SSD1306_LIB_H_

void ssd1306Init();
void sendCommand (unsigned char command);
void sendData (unsigned char params [], unsigned char flag);
void setCursor (unsigned char x, unsigned char p);
void darwPixel (unsigned char x, unsigned char y, unsigned char clear);
void fillDisplay(unsigned char param);
void drawImage(unsigned char x, unsigned char y, unsigned char sx,
                       unsigned char sy, const unsigned char img[],
                       unsigned char invert);
void draw6x8Str(unsigned char x, unsigned char p, const char str[],
                        unsigned char invert, unsigned char underline);
void draw12x16Str(unsigned char x, unsigned char y, const char str[],
                          unsigned char invert);

#endif /* SSD1306_LIB_H_ */
