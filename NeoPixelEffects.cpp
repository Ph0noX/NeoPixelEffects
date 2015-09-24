/*-------------------------------------------------------------------------
  Arduino library to provide a variety of effects when using Adafruit's
  NeoPixel library along with NeoPixels and other compatible hardware.
  This library is a work in progress and it's main purpose to help get
  my coding back on track after a long absence. Wish me luck!
  Written by Nolan Moore.
  -------------------------------------------------------------------------
  This file is part of the NeoPixel Effects library.
  NeoPixel is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.
  NeoPixel is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with NeoPixel.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/

#include <NeoPixelEffects.h>

NeoPixelEffects::NeoPixelEffects(CRGB *ledset, Effect effect, int pixstart, int pixend, int aoe, unsigned long delay, CRGB color_crgb, bool repeat, bool dir) :
  _pixset(ledset), _color_fg(color_crgb), _color_bg(CRGB::Black), _repeat(repeat), _direction(dir), _counter(0), _subtype(0)
{
  setRange(pixstart, pixend);
  setAreaOfEffect(aoe);
  setDelay(delay);
  setEffect(effect);
  // *assoc_effects = NULL;
}

NeoPixelEffects::NeoPixelEffects()
{
  *_pixset = NULL;
  _effect = NONE;
  _status = INACTIVE;
  _pixstart = 0;
  _pixend = 0;
  _pixrange = 1;
  _pixaoe = 1;
  _pixcurrent = _pixstart;
  _counter = 0;
  _subtype = 0;
  _delay = 0;
  _lastupdate = 0;
  _color_fg = CRGB::Black;
  _color_bg = CRGB::Black;
  _repeat = true;
  _direction = FORWARD;
}

NeoPixelEffects::~NeoPixelEffects()
{
  *_pixset = NULL;
}

void NeoPixelEffects::setEffect(Effect effect)
{
  _effect = effect;
  if (_direction == FORWARD) {
    _pixcurrent = _pixstart;
    _counter = 0;
  } else {
    _pixcurrent = _pixend;
    _counter = 100;
  }
  _lastupdate = 0;
}

void NeoPixelEffects::update()
{
  if (_effect != NONE || _status == ACTIVE) {
    unsigned long timenow = millis();
    if (timenow - _lastupdate > _delay) {
      _lastupdate = timenow;
      switch (_effect) {
        case COMET:
          updateCometEffect();
          break;
        case LARSON:
          _subtype = 1;
          updateCometEffect();
          break;
        case CHASE:
          _subtype = 0;
          updateChaseEffect();
          break;
        case PULSE:
          updatePulseEffect();
          break;
        case STATIC:
          _subtype = 0;
          updateStaticEffect();
          break;
        case RANDOM:
          _subtype = 1;
          updateStaticEffect();
          break;
        case FADE:
          updateFadeOutEffect();
          break;
        case FILLIN:
          updateFillInEffect();
          break;
        case GLOW:
          updateGlowEffect();
          break;
        case RAINBOWWAVE:
          updateRainbowWaveEffect();
          break;
        case STROBE:
          updateStrobeEffect();
          break;
        case SINEWAVE:
          updateWaveEffect();
          break;
        case TALKING:
          updateTalkingEffect();
          break;
        // case FIREWORK:
        //   updateFireworkEffect();
        //   break;
        // case SPARKLEFILL:
        //   updateSparkleFillEffect();
        //   break;
        default:
          break;
      }
    }
  }
}

void NeoPixelEffects::updateCometEffect()
{
  if (_subtype > 0) {
    if (_repeat) _repeat = false;
  }

  for (int j = 0; j <= _pixaoe; j++) {
    int tpx;
    bool showpix = true;
    if (_direction == FORWARD) {
      tpx = _pixcurrent + j;
      if (tpx > _pixend) {
        if (_repeat) {
          tpx = _pixcurrent + j - _pixrange + 1;
        } else {
          showpix = false;
        }
      }
    } else {
      tpx = _pixcurrent - j;
      if (tpx < _pixstart) {
        if (_repeat) {
          tpx = _pixcurrent - j + _pixrange;
        } else {
          showpix = false;
        }
      }
    }

    if (showpix) {
      float ratio = j / (float)_pixaoe;
      CRGB tailcolor = CRGB(_color_fg.r * ratio, _color_fg.g * ratio, _color_fg.b * ratio);

      _pixset[tpx] = tailcolor;
    }
  }

  if (_direction == FORWARD) {
    if (_pixcurrent == _pixend) {
      if (_repeat) {
        _pixcurrent = _pixstart;
      } else {
        if (_subtype > 0) {
          setDirection(REVERSE);
        } else {
          stop();
        }
      }
    } else {
      _pixcurrent++;
    }
  } else {
    if (_pixcurrent == _pixstart) {
      if (_repeat) {
        _pixcurrent = _pixend;
      } else {
        if (_subtype > 0) {
          setDirection(FORWARD);
        } else {
          stop();
        }
      }
    } else {
      _pixcurrent--;
    }
  }
}

void NeoPixelEffects::updateChaseEffect()
{
  for (int j = _pixstart; j <= _pixend; j++) {
    if (_counter % 2 == 0) {
      if (j % 2 == 0) {
        _pixset[j] = _color_fg;
      } else {
        _pixset[j] = _color_bg;
      }
    } else {
      if (j % 2 == 0) {
        _pixset[j] = _color_bg;
      } else {
        _pixset[j] = _color_fg;
      }
    }
  }
  _counter++;
}

void NeoPixelEffects::updatePulseEffect()
{
  CRGB pulsecolor;

  float ratio = _counter / 100.0;
  pulsecolor.r = _color_fg.r * ratio;
  pulsecolor.g = _color_fg.g * ratio;
  pulsecolor.b = _color_fg.b * ratio;

  if (_direction == FORWARD) {
    _counter++;
    if (_counter >= 100) setDirection(REVERSE);
  } else {
    _counter--;
    if (_counter <= 0) setDirection(FORWARD);
  }

  for (int i = _pixstart; i <= _pixend; i++) {
    _pixset[i] = pulsecolor;
  }
}

void NeoPixelEffects::updateStaticEffect()
{
  random16_add_entropy(random(65535));

  for (int i = _pixstart; i <= _pixend; i++) {
    CRGB randomcolor;
    if (_subtype == 0) {
      float random_ratio = random8(101) / 100.0;
      randomcolor.r = _color_fg.r * random_ratio;
      randomcolor.g = _color_fg.g * random_ratio;
      randomcolor.b = _color_fg.b * random_ratio;
    } else {
      randomcolor.r = 255 * random8(101) / 100.0;
      randomcolor.g = 255 * random8(101) / 100.0;
      randomcolor.b = 255 * random8(101) / 100.0;
    }
    _pixset[i] = randomcolor;
  }
}

void NeoPixelEffects::updateFadeOutEffect()
{
  if (_counter == 0) _counter = 100;
  float ratio = _counter / 100.0;

  CRGB fadecolor;
  for (int i = _pixstart; i <= _pixend; i++) {
    fadecolor = CRGB(_pixset[i].r * ratio, _pixset[i].g * ratio, _pixset[i].b * ratio);
    _pixset[i] = fadecolor;
  }

  _counter--;

  if (_counter <= 0 || fadecolor == CRGB(0,0,0)) {
    stop();
  }
}

void NeoPixelEffects::updateFillInEffect()
{
  _pixset[_pixcurrent] = _color_fg;
  if (_direction == FORWARD) {
    if (_pixcurrent != _pixend) {
      _pixcurrent++;
    } else {
      stop();
    }
  } else {
    if (_pixcurrent != _pixstart) {
      _pixcurrent--;
    } else {
      stop();
    }
  }
}

void NeoPixelEffects::updateGlowEffect()
{
  // Ensure glow_area_half is always even
  if (_pixrange % 2 == 0 && _pixaoe % 2 != 0) {
    _pixaoe++;
  } else if (_pixrange % 2 != 0 && _pixaoe % 2 == 0) {
    _pixaoe--;
  }
  Serial.println(_pixaoe);

  float ratio = _counter / 100.0;
  CRGB glowcolor;
  glowcolor.r = _color_fg.r * ratio;
  glowcolor.g = _color_fg.g * ratio;
  glowcolor.b = _color_fg.b * ratio;

  if (_direction == FORWARD) {
    _counter++;
    if (_counter >= 100) setDirection(REVERSE);
  } else {
    _counter--;
    if (_counter <= 0) {
      setDirection(FORWARD);
      if (!_repeat) {
        stop();
      }
    }
  }

  int glow_area_half = (_pixrange - _pixaoe) / 2;
  for (int i = 0; i < glow_area_half ; i++) {
    uint8_t denom = glow_area_half + 1 - i;
    CRGB tempcolor = CRGB(glowcolor.r / denom, glowcolor.g / denom, glowcolor.b / denom);
    _pixset[_pixstart + i] = tempcolor;
    _pixset[_pixend - i] = tempcolor;
  }
  for (int i = 0; i < _pixaoe; i++) {
    _pixset[_pixstart + glow_area_half + i] = glowcolor;
  }
}

// void NeoPixelEffects::updateFireworkEffect()
// {
//   static NeoPixelEffects *m = new NeoPixelEffects(_pixset, COMET, _pixstart,                  _pixend - _pixaoe - 4, 2,         25,     CRGB::White,  false, FORWARD);
//   static NeoPixelEffects *s1 = new NeoPixelEffects(_pixset, NONE, _pixend - 2 * _pixaoe - 4,  _pixend - _pixaoe - 2, stars_aoe, _delay, _color_fg, false, REVERSE);
//   static NeoPixelEffects *s2 = new NeoPixelEffects(_pixset, NONE, _pixend - _pixaoe - 2,      stars_end,             stars_aoe, _delay, _color_fg, false, FORWARD);
//   static NeoPixelEffects *sf = new NeoPixelEffects(_pixset, NONE, _pixend - 2 * _pixaoe - 4,  _pixend,               1,         7,      _color_fg, false, REVERSE);
//
//   if (_counter == 0 && m->getEffect() == NONE) {
//     delay(200);
//     s1->setEffect(FILLIN);
//     s2->setEffect(FILLIN);
//     _counter = 1;
//   } else if (_counter == 1 && s1->getEffect() == NONE && s2->getEffect() == NONE) {
//     delay(200);
//     sf->setEffect(FADE);
//     _counter = 2;
//   } else if (_counter == 2 && sf->getEffect() == NONE) {
//     delete(m);
//     delete(s1);
//     delete(s2);
//     delete(sf);
//     setEffect(NONE);
//   }
//
//   m->update();
//   s1->update();
//   s2->update();
//   sf->update();
// }

// void NeoPixelEffects::updateSparkleFillEffect()
// {
//   static NeoPixelEffects *sparkles = new NeoPixelEffects(_pixset, STATIC, (_direction==FORWARD)?_pixstart,  (_direction==FORWARD)?_pixstart:_pixend, 1, _delay, _color_fg, false, _direction);
//
//   sparkles->update();
//
//   if (_direction == FORWARD) {
//     if (_pixcurrent != _pixend) {
//       _pixcurrent++;
//       sparkles->setRange(_pixstart, _pixcurrent);
//     }
//   } else {
//     if (_pixcurrent != _pixstart) {
//       _pixcurrent--;
//       sparkles->setRange(_pixcurrent, _pixend);
//     }
//   }
//
//   if (_status == INACTIVE) {
//     delete(sparkles);
//     clear();
//     setEffect(NONE);
//   }
// }

void NeoPixelEffects::updateRainbowWaveEffect()
{
  float ratio = 255.0  / _pixrange;

  for (int i = _pixstart; i <= _pixend; i++) {
    CRGB color = CHSV((uint8_t)((_counter + i) * ratio), 255, 255);
    _pixset[i] = color;
  }
  _counter = (_direction) ? _counter + 1 : _counter - 1;
}

void NeoPixelEffects::updateStrobeEffect()
{
  CRGB strobecolor;
  if (_counter % 2 == 0) {
    strobecolor = _color_fg;
  } else {
    strobecolor = _color_bg;
  }

  for (int j = _pixstart; j <= _pixend; j++) {
    _pixset[j] = strobecolor;
  }
  _counter++;
}

void NeoPixelEffects::updateWaveEffect()
{
  for (int i = _pixstart; i <= _pixend; i++) {
    float ratio;
    if (!_subtype) {
      // ratio = sin8(_counter * (i - _pixstart) / _pixrange) / 255.0; // Shrinking wave
      ratio = sin8(255 * (i - _pixstart) / _pixrange) / 255.0; // Shrinking wave
    } else {
      // ratio = triwave8(_counter * (i - _pixstart) / _pixrange) / 255.0; // Shrinking wave
    }

    CRGB wavecolor = CRGB(_color_fg.r * ratio, _color_fg.g * ratio, _color_fg.b * ratio);
    _pixset[i] = wavecolor;
  }
  _counter = (_direction) ? _counter + 1 : _counter - 1;
}

// void NeoPixelEffects::updateTalkingEffect1()
// {
//   static uint8_t init = 1;
//   static uint8_t next_bright = 0;
//   static uint16_t next_delay = 0;
//   static unsigned long lastupdate = 0;
//   static uint8_t max_b = 0;
//   static uint8_t min_b = 0;
//   static uint8_t current_b = 0;
//   static int delta_b = 0;
//
//   if (init) {
//     initTalkingEffect(next_bright, next_delay, max_b, min_b, current_b);
//     init = 0;
//   }
//
//   unsigned long now = millis();
//
//   if (now - lastupdate > next_delay) {
//     // Updater delay and brightness
//     lastupdate = now;
//     next_delay = random16(100, 250);
//     next_bright = random8(min_b + 50, max_b);
//
//     // Even if we are currently moving down slope, make sure next brightness is higher
//     while (next_bright - current_b <= 0) {
//       next_bright = random8(min_b + 50, max_b);
//     }
//     delta_b = next_bright - current_b;
//   }
//
//   if (delta_b > 0) {
//     if (_direction == FORWARD) {
//       current_b++;
//       delta_b = next_bright - current_b;
//     } else {
//       current_b--;
//       delta_b = current_b - min_b;
//     }
//   } else {
//     if (current_b == next_bright) {
//       setDirection(REVERSE);
//       delta_b = current_b - min_b;
//     } else {
//       setDirection(FORWARD);
//     }
//   }
//
//   // Set colors
//   float ratio = current_b / 255.0;
//   CRGB pulsecolor = CRGB(_color_fg.r * ratio, _color_fg.g * ratio, _color_fg.b * ratio);
//   for (int i = _pixstart; i <= _pixend; i++) {
//     _pixset[i] = pulsecolor;
//   }
// }
//
// void NeoPixelEffects::initTalkingEffect1(uint8_t &bright, uint16_t &delay, uint8_t &maxb, uint8_t &minb, uint8_t &nowb)
// {
//   maxb = max(_color_fg.r, max(_color_fg.g, _color_fg.b));
//   minb = _color_bg.r;
//   nowb = minb;
//   _direction = FORWARD;
//   _delay = 0;
//   _color_bg = CRGB(30, 0, 0);
//
//   bright = random8(minb + 50, maxb);
//   delay = random16(100, 250);
// }

void NeoPixelEffects::updateTalkingEffect()
{
  static uint8_t init = 1;
  static uint8_t target_pix = 0;
  static uint16_t next_update = 0;
  static unsigned long lastupdate = 0;

  if (init) {
    _pixcurrent = 0;
    _direction = FORWARD;
    init = 0;
  }

  unsigned long now = millis();

  if (now - lastupdate > next_update) {
    lastupdate = now;
    next_update = random16(150, 450);
    target_pix = random8(3, _pixrange / 2);
    _direction = (target_pix > _pixcurrent) ? FORWARD : REVERSE;
  }

  if (_pixcurrent != target_pix) {
    if (_direction == FORWARD) {
      _pixcurrent++;
    } else {
      _pixcurrent--;
    }
  } else {
    if (target_pix != 0) {
      target_pix = 0;
      _direction = REVERSE;
    } else {
      if (_direction == REVERSE) {
        _direction = FORWARD;
      }
    }
  }

  clear();
  if (_pixcurrent != 0) {
    for (int i = 0; i < _pixcurrent; i++) {
      _pixset[_pixstart + (_pixrange / 2) - 1 - i] = _color_fg;
      _pixset[_pixstart + (_pixrange / 2) + i] = _color_fg;
    }
  } else {
    CRGB dim1 = CRGB(_color_fg.r * 0.2,_color_fg.g * 0.2, _color_fg.b * 0.2);
    CRGB dim2 = CRGB(_color_fg.r * 0.1,_color_fg.g * 0.1, _color_fg.b * 0.1);
    _pixset[_pixstart + _pixrange / 2 - 1] = dim1;
    _pixset[_pixstart + _pixrange / 2] = dim1;
    _pixset[_pixstart + _pixrange / 2 - 2] = dim2;
    _pixset[_pixstart + _pixrange / 2 + 1] = dim2;
  }
}

Effect NeoPixelEffects::getEffect()
{
  return _effect;
}

void NeoPixelEffects::setRange(int pixstart, int pixend)
{
  if (pixstart >= 0 && pixstart <= pixend) {
    _pixstart = pixstart;
    _pixend = pixend;
    _pixrange = _pixend - _pixstart + 1;
  }
  if (_direction == FORWARD) {
    _pixcurrent = _pixstart;
  } else {
    _pixcurrent = _pixend;
  }
  _lastupdate = 0;
}

void NeoPixelEffects::setAreaOfEffect(int aoe)
{
  if (aoe > 0 && aoe <= _pixrange) {
    _pixaoe = aoe;
  }
  _lastupdate = 0;
}

void NeoPixelEffects::setDelayHz(int delay_hz)
{
  if (delay_hz > 0) {
    _delay = (unsigned long)(1.0 / delay_hz * 1000);
  }
  update();
}

void NeoPixelEffects::setDelay(unsigned long delay_ms)
{
  _delay = delay_ms;
  _lastupdate = 0;
  update();
}

void NeoPixelEffects::setColorRGB(uint8_t color_red, uint8_t color_green, uint8_t color_blue)
{
  if (color_red > -1 && color_green > -1 && color_blue > -1) {
    _color_fg.r = (color_red > 255) ? 255 : color_red;
    _color_fg.g = (color_green > 255) ? 255 : color_green;
    _color_fg.b = (color_blue > 255) ? 255 : color_blue;
  }
  _lastupdate = 0;
}

void NeoPixelEffects::setColor(CRGB color_crgb)
{
  _color_fg = color_crgb;
  _lastupdate = 0;
}

void NeoPixelEffects::setBackgroundColor(CRGB color_crgb)
{
  _color_bg = color_crgb;
  _lastupdate = 0;
}

void NeoPixelEffects::setStatus(EffectStatus status)
{
  _status = status;
  _lastupdate = 0;
}

EffectStatus NeoPixelEffects::getStatus()
{
  return(_status);
}

void NeoPixelEffects::setRepeat(bool repeat)
{
  _repeat = repeat;
  _lastupdate = 0;
}

void NeoPixelEffects::setDirection(bool direction)
{
  _direction = direction;
  _lastupdate = 0;
}

void NeoPixelEffects::setSubtype(uint8_t subtype)
{
  _subtype = subtype;
  _lastupdate = 0;
}

void NeoPixelEffects::stop()
{
  setEffect(NONE);
  setStatus(INACTIVE);
}

void NeoPixelEffects::clear()
{
  fill_solid(CRGB::Black);
}

void NeoPixelEffects::fill_solid(CRGB color_crgb)
{
  for (int i = _pixstart; i <= _pixend; i++) {
    _pixset[i] = color_crgb;
  }
}

void NeoPixelEffects::fill_gradient(CRGB color_crgb1, CRGB color_crgb2)
{
  int delta_red = color_crgb1.r - color_crgb2.r;
  int delta_green = color_crgb1.g - color_crgb2.g;
  int delta_blue = color_crgb1.b - color_crgb2.b;

  for (int i = _pixstart; i <= _pixend; i++) {
    float part = (float)(i - _pixstart) / _pixrange;
    uint8_t grad_red = color_crgb1.r - (delta_red * part);
    uint8_t grad_green = color_crgb1.g - (delta_green * part);
    uint8_t grad_blue = color_crgb1.b - (delta_blue * part);
    _pixset[i] = CRGB(grad_red, grad_green, grad_blue);
  }
}
