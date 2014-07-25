#include <Adafruit_NeoPixel.h>

const int LED_PIN = 1;
const int NEOPIXEL_PIN = 2;
const int NEOPIXEL_N = 16;
const int FULL_WHACK = 255; // brightness

int R, G, B;
int hue = 0;

long t0 = 0;

Adafruit_NeoPixel strip(NEOPIXEL_N, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void blink()
{
    for (int i = 0; i < 3; i++) {
	digitalWrite(LED_PIN, HIGH);
	delay(250);
	digitalWrite(LED_PIN, LOW);
	delay(250);
    }
}

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    delay(10);
    blink();

    strip.begin();
    strip.setBrightness(FULL_WHACK);
    strip.show(); // Initialize all pixels to 'off'
}

void hueToRGB(int hue, int brightness)
{
    unsigned int scaledHue = (hue * 6);
    unsigned int segment = scaledHue / 256; // segment 0 to 5 around the color wheel
    unsigned int segmentOffset = scaledHue - (segment * 256); // position within the segment
    unsigned int complement = 0;
    unsigned int prev = (brightness * ( 255 - segmentOffset)) / 256;
    unsigned int next = (brightness * segmentOffset) / 256;
    
    switch (segment) {
    case 0:	// red
	R = brightness;
	G = next;
	B = complement;
	break;

    case 1:
	// yellow
	R = prev;
	G = brightness;
	B = complement;
	break;
    
    case 2:
	// green
	R = complement;
	G = brightness;
	B = next;
	break;

    case 3:
	// cyan
	R = complement;
	G = prev;
	B = brightness;
	break;

    case 4:
	// blue
	R = next;
	G = complement;
	B = brightness;
	break;

    case 5:	
	// magenta
    default:
	R = brightness;
	G = complement;
	B = prev;
	break;
    }
}

void set(uint32_t c)
{
    for (uint16_t i = 0; i < strip.numPixels(); ++i)
	strip.setPixelColor(i, c);
    strip.show();
    delay(2000);
}

void loop()
{
    long t = 0;
    /*
    strip.setBrightness(FULL_WHACK);
    
    set(0xff0000);
    set(0x00ff00);
    set(0x0000ff);

    set(0x00ffff);
    set(0xff00ff);
    set(0xffff00);
    */

    if ((t = millis()) - t0 > 1000) {
	t0 = t;

	if (++hue == 256)
	    hue = 0;

	hueToRGB(hue, 255);

	for (uint16_t i = 0; i < strip.numPixels(); ++i) {
	    strip.setPixelColor(i, R, G, B);
	}
	strip.setBrightness(FULL_WHACK);
	strip.show();
	delay(20);
    }

}

/*
 * Local variables:
 * mode: c++
 * End:
 */
