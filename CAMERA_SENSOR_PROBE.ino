#include "esp_camera.h"
#include "esp_log.h"
#include "esp32-hal-rgb-led.h"
#include <Wire.h>
#include <stdarg.h>

#define RGB_LED_PIN     48          // onboard WS2812 on this board

#define PWDN_GPIO_NUM   -1
#define RESET_GPIO_NUM  -1
#define XCLK_GPIO_NUM   15
#define SIOD_GPIO_NUM   4
#define SIOC_GPIO_NUM   5
#define Y9_GPIO_NUM     16
#define Y8_GPIO_NUM     17
#define Y7_GPIO_NUM     18
#define Y6_GPIO_NUM     12
#define Y5_GPIO_NUM     10
#define Y4_GPIO_NUM     8
#define Y3_GPIO_NUM     9
#define Y2_GPIO_NUM     11
#define VSYNC_GPIO_NUM  6
#define HREF_GPIO_NUM   7
#define PCLK_GPIO_NUM   13

static void led(uint8_t r, uint8_t g, uint8_t b) {
  neopixelWrite(RGB_LED_PIN, r, g, b);
}

// print to both consoles
static void say(const char *fmt, ...) {
  char buf[256];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  Serial.println(buf);
  Serial.flush();
  ESP_LOGI("PROBE", "%s", buf);
}

static uint8_t sccbRead(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return 0xFF;
  if (Wire.requestFrom((int)addr, 1) != 1) return 0xFF;
  return Wire.read();
}

static const char *ovName(uint16_t pid) {
  switch (pid) {
    case 0x2642: return "OV2640 (supports JPEG)";
    case 0x3660: return "OV3660 (supports JPEG)";
    case 0x5640: return "OV5640 (supports JPEG)";
    case 0x7673: return "OV7670 (NO JPEG - RGB565/YUV only)";
    case 0x7721: return "OV7725 (NO JPEG - RGB565/YUV only)";
    default:     return "unknown / not an OV sensor";
  }
}

static void probeSccb() {
  say("[1] SCCB scan, SDA=GPIO4 SCL=GPIO5");
  Wire.begin(SIOD_GPIO_NUM, SIOC_GPIO_NUM, 100000);

  const uint8_t candidates[] = {0x30, 0x3C, 0x21, 0x2A, 0x6E, 0x20};
  int found = 0;

  for (uint8_t i = 0; i < sizeof(candidates); i++) {
    uint8_t addr = candidates[i];
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() != 0) continue;

    found++;
    uint8_t a = sccbRead(addr, 0x0A);
    uint8_t b = sccbRead(addr, 0x0B);
    uint8_t c = sccbRead(addr, 0x00);
    uint8_t d = sccbRead(addr, 0x01);
    uint16_t pid = ((uint16_t)a << 8) | b;

    say("    device 0x%02X  reg0A/0B=0x%02X 0x%02X (PID 0x%04X: %s)",
        addr, a, b, pid, ovName(pid));
    say("                reg00/01=0x%02X 0x%02X", c, d);
  }

  if (found == 0) {
    say("    NO device answered - wiring / ribbon / sensor power problem");
  }
  Wire.end();
}

void setup() {
  led(0, 40, 0);                    // green: alive
  Serial.begin(115200);
  say("=== PROBE v2 alive (banner) ===");
  delay(2000);

  say("PSRAM total=%u free=%u", (unsigned)ESP.getPsramSize(),
      (unsigned)ESP.getFreePsram());

  probeSccb();
  led(40, 40, 0);                   // yellow: scan done

  say("[2] esp_camera_init() with PIXFORMAT_RGB565");

  camera_config_t config = {};
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size   = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_RGB565;
  config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count     = 2;

  esp_err_t err = esp_camera_init(&config);
  say("[2] result 0x%x (%s)", err, err == ESP_OK ? "OK" : "FAILED");
  if (err != ESP_OK) {
    led(40, 0, 0);                  // red: init failed
    say("    0x105 = probe failed (wiring)   0x106 = not supported");
    return;
  }
  led(0, 0, 40);                    // blue: camera up

  sensor_t *s = esp_camera_sensor_get();
  uint16_t pid = ((uint16_t)s->id.PID << 8) | s->id.VER;
  say("[3] MIDH=0x%02X MIDL=0x%02X PID=0x%02X VER=0x%02X -> 0x%04X",
      s->id.MIDH, s->id.MIDL, s->id.PID, s->id.VER, pid);
  say("[3] looks like: %s", ovName(pid));
  say("[3] current pixformat %d (2=RGB565 3=YUV422 4=JPEG 5=GRAYSCALE)",
      (int)s->pixformat);

  int jpeg = s->set_pixformat(s, PIXFORMAT_JPEG);
  say("[4] set_pixformat(JPEG) -> %d : %s", jpeg,
      jpeg == 0 ? "JPEG WORKS" : "JPEG NOT SUPPORTED");
  if (jpeg != 0) s->set_pixformat(s, PIXFORMAT_RGB565);

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    say("[5] frame capture FAILED");
  } else {
    say("[5] frame OK %u bytes %ux%u format %d", (unsigned)fb->len,
        fb->width, fb->height, (int)fb->format);
    esp_camera_fb_return(fb);
  }
  say("=== probe finished, copy these lines ===");
}

void loop() {
  static uint32_t n = 0;
  delay(3000);
  camera_fb_t *fb = esp_camera_fb_get();
  if (fb) {
    say("heartbeat %u: frame %u bytes %ux%u", (unsigned)++n,
        (unsigned)fb->len, fb->width, fb->height);
    esp_camera_fb_return(fb);
  } else {
    say("heartbeat %u: frame FAILED", (unsigned)++n);
  }
}
