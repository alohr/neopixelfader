#include <Adafruit_NeoPixel.h>
#include <IRremote.h>

const int IR_DECODER_PIN = 2;
const int NEOPIXEL_PIN = 3;
const int LED_PIN = 4;

const int BRIGHTNESS_PIN = 1;
const int NEOPIXEL_N = 8;

enum {
    ON_OFF       = 0x0c,
    MUTE         = 0x0d,
    TV_AV        = 0x0b,
    VOLUME_UP    = 0x10,
    VOLUME_DOWN  = 0x11,
    CHANNEL_UP   = 0x20,
    CHANNEL_DOWN = 0x21,
};

struct State {
    State() :
	hue(0),
	red(0),
	green(0),
	blue(0),
	toggle(0),
	brightness(BRIGHTNESS_DEFAULT),
	fadeDelayMillis(FADE_DELAY_DEFAULT) {}

    int hue;
    int red, green, blue;

    int toggle;
    int brightness;
    int fadeDelayMillis;

    static const int BRIGHTNESS_FULL_WHACK = 255;
    static const int BRIGHTNESS_DEFAULT = BRIGHTNESS_FULL_WHACK / 4;
    static const int BRIGHTNESS_INCREMENT = 1;
    static const int BRIGHTNESS_MAX = BRIGHTNESS_FULL_WHACK - 10;
    static const int BRIGHTNESS_MIN = 10;

    static const int FADE_DELAY_DEFAULT = 200;
    static const int FADE_DELAY_INCREMENT = 20;
    static const int FADE_DELAY_MIN = 20;
    static const int FADE_DELAY_MAX = 2000;
};

Adafruit_NeoPixel strip(NEOPIXEL_N, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
IRrecv irrecv(IR_DECODER_PIN);
decode_results irresults;
State state;

long t_fade0 = 0;
long t_brightness0 = 0;

void blink()
{
    for (int i = 0; i < 3; i++) {
	digitalWrite(LED_PIN, HIGH);
	delay(250);
	digitalWrite(LED_PIN, LOW);
	delay(250);
    }
}

void setBrightness(const State *state)
{
    strip.setBrightness(state->brightness);
    strip.show();
}

void setup()
{
    // indicator led
    pinMode(LED_PIN, OUTPUT);
    delay(10);
    blink();

    // ir receiver
    irrecv.enableIRIn();

    // neopixel strip
    strip.begin();
    setBrightness(&state);
    strip.show(); // initialize all pixels to 'off'
}

void hueToRGB(State *state, int brightness)
{
    unsigned int scaledHue = (state->hue * 6);
    unsigned int segment = scaledHue / 256; // segment 0 to 5 around the color wheel
    unsigned int segmentOffset = scaledHue - (segment * 256); // position within the segment
    unsigned int complement = 0;
    unsigned int prev = (brightness * ( 255 - segmentOffset)) / 256;
    unsigned int next = (brightness * segmentOffset) / 256;
    
    switch (segment) {
    case 0:	// red
	state->red = brightness;
	state->green = next;
	state->blue = complement;
	break;

    case 1:
	// yellow
	state->red = prev;
	state->green = brightness;
	state->blue = complement;
	break;
    
    case 2:
	// green
	state->red = complement;
	state->green = brightness;
	state->blue = next;
	break;

    case 3:
	// cyan
	state->red = complement;
	state->green = prev;
	state->blue = brightness;
	break;

    case 4:
	// blue
	state->red = next;
	state->green = complement;
	state->blue = brightness;
	break;

    case 5:	
	// magenta
    default:
	state->red = brightness;
	state->green = complement;
	state->blue = prev;
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

void irinterpret(State *state, const decode_results *r)
{
    if (r->decode_type == RC5) {
	int toggle = (r->value & 0x800) != 0;

	if (toggle != state->toggle) {
	    // s->toggle = toggle;

	    // switch (r->value & 0xff) {
	    // case ON_OFF:
	    // 	s->run1 = (s->run1 != 0) ? 0 : MOTOR1_MIN;
	    // 	break;
	    // }
	}

	switch (r->value & 0xff) {
	case TV_AV:
	    // // turn servo left (min)
	    // s->turn = -STEERING_DIFF_US / STEERING_STEP_US;
	    // s->timestamp = millis();
	    break;
	case MUTE:
	    break;

	case CHANNEL_UP:
	    state->fadeDelayMillis =
		max(state->fadeDelayMillis - State::FADE_DELAY_INCREMENT, State::FADE_DELAY_MIN);
	    break;

	case CHANNEL_DOWN:
	    state->fadeDelayMillis =
		min(state->fadeDelayMillis + State::FADE_DELAY_INCREMENT, State::FADE_DELAY_MAX);
	    break;

	case VOLUME_UP:
	    state->brightness = 
		min(state->brightness + State::BRIGHTNESS_INCREMENT, State::BRIGHTNESS_MAX);
	    break;

	case VOLUME_DOWN:
	    state->brightness = 
		max(state->brightness - State::BRIGHTNESS_INCREMENT, State::BRIGHTNESS_MIN);
	    break;
	}
    }
}

void loop()
{
    long t_fade = 0;
    long t_brightness = 0;


    if (irrecv.decode(&irresults)) {
	irinterpret(&state, &irresults);
	irrecv.resume(); 
    }
    
    if ((t_brightness = millis()) - t_brightness0 > 100) {
	t_brightness0 = t_brightness;
	setBrightness(&state);
    }

    if ((t_fade = millis()) - t_fade0 > state.fadeDelayMillis) {
	t_fade0 = t_fade;

	if (++state.hue == 256)
	    state.hue = 0;

	hueToRGB(&state, 255);

	for (uint16_t i = 0; i < strip.numPixels(); ++i) {
	    strip.setPixelColor(i, state.red, state.green, state.blue);
	}

	strip.show();
	delay(20);
    }
}

// Local variables:
// mode: c++
// End:
