/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */
#define XPOWERS_CHIP_AXP2101
#include <lvgl.h>

/*To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 *You also need to copy `lvgl/examples` to `lvgl/src/examples`. Similarly for the demos `lvgl/demos` to `lvgl/src/demos`.
 Note that the `lv_examples` library is for LVGL v7 and you shouldn't install it for this version (since LVGL v8)
 as the examples and demos are now part of the main LVGL library. */

// #include <examples/lv_examples.h>
// #include <demos/lv_demos.h>

//#define DIRECT_MODE // Uncomment to enable full frame buffer

#include <Arduino_GFX_Library.h>
#include "TCA9554.h"
#include "TouchDrvFT6X36.hpp"

#include "USB.h"

#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"

#include <Wire.h>
#include "es8311.h"
#include "esp_check.h"
#include "SensorQMI8658.hpp"

#include <Arduino.h>

#include "Audio.h"
#include "SD_MMC.h"
int sd_begin_failed=0;

#include "kwcli.h"

//#include <WiFiClientSecure.h>
//#include <HTTPClient.h>

#include <WiFiUdp.h>
#include <WakeOnLan.h>

#include "USBHIDConsumerControl.h"

#include <WiFiManager.h>
#include <time.h>

#include "XPowersLib.h"

#ifndef CONFIG_PMU_SDA
#define CONFIG_PMU_SDA 8
#endif

#ifndef CONFIG_PMU_SCL
#define CONFIG_PMU_SCL 7
#endif

#ifndef CONFIG_PMU_IRQ
#define CONFIG_PMU_IRQ -1
#endif

XPowersPMU power;

#ifndef SENSOR_SDA
#define SENSOR_SDA 8
#endif

#ifndef SENSOR_SCL
#define SENSOR_SCL 7
#endif

#ifndef SENSOR_IRQ
#define SENSOR_IRQ -1
#endif

SensorQMI8658 qmi;

IMUdata gyr;

const uint8_t i2c_sda = CONFIG_PMU_SDA;
const uint8_t i2c_scl = CONFIG_PMU_SCL;
const uint8_t pmu_irq_pin = CONFIG_PMU_IRQ;

const int backlightChannel = 0;
const int backlightFreq = 5000;
const int backlightResolution = 8; // 0–255


KWConfigInterface kwconfig;
KWCLI kwcli(&kwconfig);

static void gpio_init(void)
{
  gpio_config_t gpio_conf = {};
  gpio_conf.intr_type = GPIO_INTR_DISABLE;
  gpio_conf.mode = GPIO_MODE_OUTPUT;
  gpio_conf.pin_bit_mask = 0x1ULL << GPIO_NUM_8;
  gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpio_conf.pull_up_en = GPIO_PULLUP_ENABLE;

  ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&gpio_conf));

  gpio_set_level(GPIO_NUM_8,true);
}

void initPMU() {

  if (!power.begin(Wire, AXP2101_SLAVE_ADDRESS, 8, 7)) {
    Serial.println("PMU not found");
    return;
  }

  // Enable battery detection + ADC
  power.enableBattDetection();
  power.enableBattVoltageMeasure();
  power.enableVbusVoltageMeasure();
  power.enableSystemVoltageMeasure();

  power.disableTSPinMeasure();  // important on Waveshare
}

#include "ui.h"

#define GFX_BL 6  // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

#define SPI_MISO 2
#define SPI_MOSI 1
#define SPI_SCLK 5

#define LCD_CS -1
#define LCD_DC 3
#define LCD_RST -1
#define LCD_HOR_RES 320
#define LCD_VER_RES 480


#define I2C_SDA 8
#define I2C_SCL 7

// I2S GPIOs
#define I2S_SDOUT 16
#define I2S_BCLK 13
#define I2S_LRCK 15
#define I2S_MCLK 12

#define EXAMPLE_SAMPLE_RATE     (48000)
#define EXAMPLE_MCLK_MULTIPLE   (256) // If not using 24-bit data width, 256 should be enough
#define EXAMPLE_MCLK_FREQ_HZ    (EXAMPLE_SAMPLE_RATE * EXAMPLE_MCLK_MULTIPLE)
#define EXAMPLE_VOICE_VOLUME    (70)

Audio audio;
USBHIDConsumerControl Consumer;
WiFiManager wm;

USBHIDMouse Mouse;
bool mouseTouching = false;
int16_t lastTouchX = 0;
int16_t lastTouchY = 0;

WiFiUDP UDP;
WakeOnLan WOL(UDP);

#define SD_MMC_CLK 11
#define SD_MMC_CMD 10
#define SD_MMC_D0 9

static esp_err_t es8311_codec_init(void) {
  es8311_handle_t es_handle = es8311_create(I2C_NUM_0, ES8311_ADDRRES_0);
  ESP_RETURN_ON_FALSE(es_handle, ESP_FAIL, TAG, "es8311 create failed");
  const es8311_clock_config_t es_clk = {
    .mclk_inverted = false,
    .sclk_inverted = false,
    .mclk_from_mclk_pin = true,
    .mclk_frequency = EXAMPLE_MCLK_FREQ_HZ,
    .sample_frequency = EXAMPLE_SAMPLE_RATE
  };

  ESP_ERROR_CHECK(es8311_init(es_handle, &es_clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16));
  // ESP_RETURN_ON_ERROR(es8311_sample_frequency_config(es_handle, EXAMPLE_SAMPLE_RATE * EXAMPLE_MCLK_MULTIPLE, EXAMPLE_SAMPLE_RATE), TAG, "set es8311 sample frequency failed");
  ESP_RETURN_ON_ERROR(es8311_voice_volume_set(es_handle, EXAMPLE_VOICE_VOLUME, NULL), TAG, "set es8311 volume failed");
  ESP_RETURN_ON_ERROR(es8311_microphone_config(es_handle, false), TAG, "set es8311 microphone failed");

  return ESP_OK;
}

uint32_t lastTouchTime = 0;
const uint32_t backlightTimeoutMs = 40000; 
bool backlightOn = true;


enum PlayerState {
  PLAYER_STOPPED,
  PLAYER_PLAYING,
  PLAYER_PAUSED
};

PlayerState playerState = PLAYER_STOPPED;

String currentTrack;
std::vector<String> musicPaths;
std::vector<String> musicTitles;
std::vector<bool> musicIsURL;
uint32_t trackDurationSec = 0;

String rollerscript;
std::vector<String> scriptPaths;
std::vector<String> scriptTitles;
String rollerfavourite;

#define MAX_PIN_LEN 6

bool unlocked=false;
std::vector<String> accounts;
int accountCount;

String enteredPin = "";
String selectedAccount;

std::vector<String> favourite_urls;
int favCount;

uint32_t lastPosSec = 0;

int volume = 12; // 0..21

TCA9554 TCA(0x20);
USBHIDKeyboard Keyboard;

enum ShortcutID {
  // basic
  SC_ENTER,
  SC_BACKSPACE,
  SC_DELETE,
  SC_ESCAPE,
  SC_TAB,
  SC_SPACE,
  SC_UP,
  SC_DOWN,
  SC_RIGHT,
  SC_LEFT,
  SC_VOLUP,
  SC_VOLDOWN,
  SC_MUTE,
  SC_PLAYPAUSE,

  // system
  SC_ALT_F4,
  SC_CTRL_ALT_T,

  // function keys
  SC_F1=100,
  SC_F2,
  SC_F3,
  SC_F4,
  SC_F5,
  SC_F6,
  SC_F7,
  SC_F8,
  SC_F9,
  SC_F10,
  SC_F11,
  SC_F12,

  // windows
  SC_WIN_D=200,
  SC_WIN_L,
  SC_WIN_V,
  SC_WIN_I,


  // ctrl list
  SC_CTRL_A=300,
  SC_CTRL_C,
  SC_CTRL_X,
  SC_CTRL_V,
  SC_CTRL_S,
  SC_CTRL_F,
  SC_CTRL_P,
  SC_CTRL_Z
};

static uint16_t sel_f = 0;
static uint16_t sel_win = 0;
static uint16_t sel_ctrl = 0;

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC /* DC */, LCD_CS /* CS */, SPI_SCLK /* SCK */, SPI_MOSI /* MOSI */, SPI_MISO /* MISO */);
Arduino_GFX *gfx = new Arduino_ST7796(
  bus, LCD_RST /* RST */, 1 /* rotation */, true, LCD_HOR_RES, LCD_VER_RES);

TouchDrvFT6X36 touch;

uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_disp_draw_buf_t draw_buf;
lv_color_t *disp_draw_buf1;
lv_color_t *disp_draw_buf2;
lv_disp_drv_t disp_drv;

//homescreen
extern void ui_Screen1_screen_init(void);
extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_HomePanel;
extern lv_obj_t * ui_startslider;
extern lv_obj_t * ui_batterystatuslabel;
extern lv_obj_t * ui_OTAbutton;
extern lv_obj_t * ui_IPlabel;
extern lv_obj_t * ui_titlebutton;
extern lv_obj_t * ui_poweroffbutton;

//lockscreen
extern void ui_Screen9_screen_init(void);
extern lv_obj_t * ui_Screen9;
extern lv_obj_t * ui_unlockslider;
extern lv_obj_t * ui_batterystatuslabel2;
extern lv_obj_t * ui_configlabel;
extern lv_obj_t * ui_trackname2;
extern lv_obj_t * ui_clocklabel;

//keyboard
extern lv_obj_t * ui_KeyboardTextArea;
extern lv_obj_t * ui_addCBbutton;
extern lv_obj_t * ui_openCBbutton;

//Mouse
extern lv_obj_t * ui_Screen10;
extern lv_obj_t * ui_rightclickbutton;
extern lv_obj_t * ui_leftclickbutton;
extern lv_obj_t * ui_stickyclickbutton;

//shortcuts
extern lv_obj_t * ui_enterbutton;
extern lv_obj_t * ui_delbutton;
extern lv_obj_t * ui_backbutton;
extern lv_obj_t * ui_altf4button;
extern lv_obj_t * ui_tabbutton;
extern lv_obj_t * ui_spacebutton;
extern lv_obj_t * ui_escbutton;
extern lv_obj_t * ui_terminalbutton;
extern lv_obj_t * ui_upbutton;
extern lv_obj_t * ui_downbutton;
extern lv_obj_t * ui_rightbutton;
extern lv_obj_t * ui_leftbutton;
extern lv_obj_t * ui_volupbutton;
extern lv_obj_t * ui_voldownbutton;
extern lv_obj_t * ui_mutebutton;
extern lv_obj_t * ui_playpausebutton;

extern lv_obj_t * ui_flist;
extern lv_obj_t * ui_ctrllist;
extern lv_obj_t * ui_winlist;

extern lv_obj_t * ui_fenter;
extern lv_obj_t * ui_ctrlenter;
extern lv_obj_t * ui_winenter;

//mp3 player
extern void ui_Screen4_screen_init(void);
extern lv_obj_t * ui_Screen4;
extern lv_obj_t * ui_playtimelabel;
extern lv_obj_t * ui_playpauseindicator;
extern lv_obj_t * ui_playposition;
extern lv_obj_t * ui_stopbutton;
extern lv_obj_t * ui_playbutton;
extern lv_obj_t * ui_pausebutton;
extern lv_obj_t * ui_minusbutton;
extern lv_obj_t * ui_plusbutton;
extern lv_obj_t * ui_volumelabel;
extern lv_obj_t * ui_Roller1;

//passkeys
extern lv_obj_t * ui_Screen5;
extern void ui_Screen5_screen_init(void);
extern lv_obj_t * ui_Roller3;
extern lv_obj_t * ui_onebutton;
extern lv_obj_t * ui_twobutton;
extern lv_obj_t * ui_threebutton;
extern lv_obj_t * ui_fourbutton;
extern lv_obj_t * ui_fivebutton;
extern lv_obj_t * ui_sixbutton;
extern lv_obj_t * ui_sevenbutton;
extern lv_obj_t * ui_eightbutton;
extern lv_obj_t * ui_ninebutton;
extern lv_obj_t * ui_zerobutton;
extern lv_obj_t * ui_pinlabel;
extern lv_obj_t * ui_enterbutton2;
extern lv_obj_t * ui_pinbackspacebutton;

//scripts
extern lv_obj_t * ui_Roller4;
extern lv_obj_t * ui_enterbutton3;
extern lv_obj_t * ui_powershellcheckbox;

//favourites
extern lv_obj_t * ui_Roller5;
extern lv_obj_t * ui_enterbutton4;
extern lv_obj_t * ui_winrcheckbox;

//clipboard
extern lv_obj_t * ui_CBprintbutton;
extern lv_obj_t * ui_CBdeletebutton;
extern lv_obj_t * ui_Roller6;
extern lv_obj_t * ui_CBenterbutton;
extern lv_obj_t * ui_CBwolbutton;


/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
#ifndef DIRECT_MODE
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif
#endif  // #ifndef DIRECT_MODE

  lv_disp_flush_ready(disp_drv);
}


void backlight_on()
{
    //if(!backlightOn) {
        //digitalWrite(GFX_BL, HIGH);

      if (lv_scr_act() == ui_Screen9) {
        ledcWrite(GFX_BL, (1<<10)*0.5);   
        
      }else{
        ledcWrite(GFX_BL, (1<<10));
      }

      backlightOn = true;  
    
    lastTouchTime = millis();
}

void backlight_off()
{
  if(backlightOn) {
        //digitalWrite(GFX_BL, LOW);
        ledcWrite(GFX_BL, 0);
        backlightOn = false;
        _ui_screen_change(&ui_Screen9, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, &ui_Screen9_screen_init);
        
    }
}

void init_rollers(){
  String passkeys;
  for(int i=0;i<accountCount;i++){passkeys.concat(accounts[i]); passkeys.concat("\n");}
  if(!passkeys.isEmpty())lv_roller_set_options(ui_Roller3, passkeys.c_str(), LV_ROLLER_MODE_NORMAL);

  String favourites;
  for(int i=0;i<favCount;i++){favourites.concat(favourite_urls[i]); favourites.concat("\n");}
  if(!favourites.isEmpty())lv_roller_set_options(ui_Roller5, favourites.c_str(), LV_ROLLER_MODE_NORMAL);

  if(sd_begin_failed){
    Serial.println("SD-Card mounting failed..");
    return;
  }

  scriptPaths.clear();
  scriptTitles.clear();

  String scriptOptions = "";

  File scriptFolder = SD_MMC.open("/scripts");

  if(scriptFolder)
  {
      File file;

      while(file = scriptFolder.openNextFile())
      {
          if(!file.isDirectory())
          {
              String path = "/scripts/";
              path.concat(file.name());

              if(path.endsWith(".txt"))
              {
                  scriptPaths.push_back(path);
                  scriptTitles.push_back(path.substring(path.lastIndexOf('/') + 1, path.length()-4));
                  scriptOptions += scriptTitles.back();
                  scriptOptions += "\n";
              }
          }

          file.close();
      }

      scriptFolder.close();
  }

  if(scriptOptions.length() > 0)
  {
      lv_roller_set_options(ui_Roller4, scriptOptions.c_str(), LV_ROLLER_MODE_NORMAL);
      lv_roller_set_selected(ui_Roller4, 0, LV_ANIM_OFF);
  }
  else
  {
      lv_roller_set_options(ui_Roller4, "-empty-", LV_ROLLER_MODE_NORMAL);
  }

  Serial.printf("Loaded %d scripts\n", scriptPaths.size());

  musicPaths.clear();
  musicTitles.clear();
  musicIsURL.clear();

  String options = "";

  File urlFile = SD_MMC.open("/music/urls.txt");

  if(urlFile)
  {
      while(urlFile.available())
      {
          String url = urlFile.readStringUntil('|');
          musicTitles.push_back(url);
          options += url;
          options += "\n";

          url = urlFile.readStringUntil('\n');
          url.trim();

          if(url.length() == 0)
              continue;

          musicPaths.push_back(url);
          musicIsURL.push_back(true);

      }
      urlFile.close();
  }

  File folder = SD_MMC.open("/music");

  if(folder){
      File file;
      while(file = folder.openNextFile())
      {
          if(!file.isDirectory())
          {
              String path = "/music/";
              path.concat(file.name());

            if(path.endsWith(".mp3"))
            {
                musicPaths.push_back(path);
                musicTitles.push_back(path.substring(path.lastIndexOf('/') + 1, path.length()-4));
                musicIsURL.push_back(false);

                options += musicTitles.back();
                options += "\n";
            }
          }
          file.close();
      }
      folder.close();
  }

  if(options.length() > 0)
  {
      lv_roller_set_options(ui_Roller1, options.c_str(), LV_ROLLER_MODE_NORMAL);
      lv_roller_set_selected(ui_Roller1, 0, LV_ANIM_OFF);
  }
  else
  {
      lv_roller_set_options(ui_Roller1, "-empty-", LV_ROLLER_MODE_NORMAL);
  }

  Serial.printf("Loaded %d music entries\n", musicPaths.size());

}

void handleMouseTouch(int16_t x, int16_t y, bool touching)
{
    if(!touching)
    {
        mouseTouching = false;
        return;
    }

    // First touch sample
    if(!mouseTouching)
    {
        lastTouchX = x;
        lastTouchY = y;
        mouseTouching = true;
        return;
    }

    float mouseSensitivity = 2.0f;
    int16_t dx = (x - lastTouchX)*mouseSensitivity;
    int16_t dy = (y - lastTouchY)*mouseSensitivity;

    lastTouchX = x;
    lastTouchY = y;

    if(dx != 0 || dy != 0)
        Mouse.move(dx, dy);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  int16_t x[1], y[1];
  uint8_t touched = touch.getPoint(y, x, 1);
  y[0]=320-y[0];

  if (touched) {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = x[0];
    data->point.y = y[0];

    backlight_on();
    bool mouse = (lv_scr_act() == ui_Screen10) && (x[0]>0 && x[0]<477) && (y[0]>55 && y[0]<245);
    handleMouseTouch(x[0],y[0],mouse);
    //backlightOn = true;
    //ledcWrite(backlightChannel, 100);
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void lcd_reset(void) {
  TCA.write1(1, 1);
  delay(10);
  TCA.write1(1, 0);
  delay(10);
  TCA.write1(1, 1);
  delay(200);
}

void send_text_as_keyboard(const char *text)
{
  delay(100);
    while (*text) {
        Keyboard.print(*text);
        delay(36); 
        text++;
    }
    //Keyboard.press(LV_KEY_ENTER);
    Keyboard.releaseAll();
   
}


void textarea_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);

    if (code == LV_EVENT_READY) {
        const char *text = lv_textarea_get_text(ta);
        if (text && strlen(text) > 0) {
            send_text_as_keyboard(text);
            lv_textarea_set_text(ta, "");
        }
    }
}

#define MOUSE_STICKY 73

lv_timer_t* leftclick_delay_timer = nullptr;
lv_timer_t* leftclick_timer = nullptr;
bool leftclick_active = false;
bool leftclick_longpress = false;

void leftclick_timer_cb(lv_timer_t* t)
{
    Mouse.click(MOUSE_LEFT);
}

void leftclick_delay_cb(lv_timer_t* t)
{
    leftclick_active = true;
    leftclick_longpress = true;

    leftclick_timer = lv_timer_create(leftclick_timer_cb, 300, NULL);

    lv_timer_del(t);
    leftclick_delay_timer = nullptr;
}

void mouseclick_btn_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    int mouseAction = (int)(intptr_t)lv_event_get_user_data(e);

    if(mouseAction == MOUSE_RIGHT || mouseAction == MOUSE_LEFT)
    {
        if(code == LV_EVENT_CLICKED)
            Mouse.click(mouseAction);

        return;
    }

    if(mouseAction != MOUSE_STICKY)
        return;

    if(code == LV_EVENT_PRESSED)
    {
        leftclick_longpress = false;

        if(!leftclick_active)
        {
            if(leftclick_delay_timer)
                lv_timer_del(leftclick_delay_timer);

            leftclick_delay_timer = lv_timer_create(leftclick_delay_cb, 1000, NULL);
            lv_timer_set_repeat_count(leftclick_delay_timer, 1);
        }

        return;
    }

    if(code == LV_EVENT_CLICKED)
    {
        // If the 1-second timer hasn't fired yet,
        // this was just a normal short click.
        if(!leftclick_active)
        {
            if(leftclick_delay_timer)
            {
                lv_timer_del(leftclick_delay_timer);
                leftclick_delay_timer = nullptr;
            }

            return;
        }

        // This CLICKED belongs to the long press that activated it.
        if(leftclick_longpress)
        {
            leftclick_longpress = false;
            return;
        }

        // A subsequent normal click stops the auto-clicker.
        if(leftclick_timer)
        {
            lv_timer_del(leftclick_timer);
            leftclick_timer = nullptr;
        }

        leftclick_active = false;
    }
}

void update_play_icon(bool playing)
{
    if (!playing) {
        lv_img_set_src(ui_playpauseindicator, &ui_img_pause_png);
        lv_obj_add_flag(ui_trackname, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_trackname2, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_img_set_src(ui_playpauseindicator, &ui_img_play_png);
        lv_obj_clear_flag(ui_trackname, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_trackname2, LV_OBJ_FLAG_HIDDEN);
    }
}

void update_time_label(uint32_t seconds)
{
    uint32_t m = seconds / 60;
    uint32_t s = seconds % 60;

    char buf[16];
    sprintf(buf, "%02lu:%02lu", m, s);
    lv_label_set_text(ui_playtimelabel, buf);
}

void update_volume_label()
{
    char buf[8];
    sprintf(buf, "%d", volume);
    lv_label_set_text(ui_volumelabel, buf);
}

void update_pin_label()
{
    String stars = "";
    for (int i = 0; i < enteredPin.length(); i++) stars += "*";
    lv_label_set_text(ui_pinlabel, stars.c_str());
}

void send_slow_text(const char* text)
{
    while (*text)
    {
        Keyboard.press(*text);
        delay(20);              // key down time
        Keyboard.release(*text);
        delay(35);              // gap between keys
        text++;
    }
}

void media_press(uint16_t key)
{
    Consumer.press(key);
    delay(10);
    Consumer.release();
}

void open_url(const char* url)
{
  bool winr = lv_obj_has_state(ui_winrcheckbox,LV_STATE_CHECKED);
    // WIN + R
    if(winr){
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('r');
      delay(100);
      Keyboard.releaseAll();
      delay(400);
    }

    send_text_as_keyboard(url);  
    delay(100);

    Keyboard.write(LV_KEY_ENTER);
}

void type_script_file(uint16_t index)
{
    if(index >= scriptPaths.size())
        return;

    File file = SD_MMC.open(scriptPaths[index]);
    Serial.println(scriptPaths[index]);
    if(!file)
    {
        Serial.println("Failed to open script!");
        return;
    }

    String line = "";

    // WIN + R
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    delay(120);
    Keyboard.releaseAll();
    delay(500);
    bool powershell = lv_obj_has_state(ui_powershellcheckbox,LV_STATE_CHECKED);
    if(!powershell){send_text_as_keyboard("cmd");}else{send_text_as_keyboard("powershell");}   
    delay(120);
    Keyboard.write(LV_KEY_ENTER);

    delay(2000); // IMPORTANT: wait for terminal window

    while(file.available())
    {
        char c = file.read();

        if(c == '\n' || c == '\r')
        {
            if(line.length() > 0)
            {
                send_text_as_keyboard(line.c_str());
                delay(100);
                Keyboard.write(LV_KEY_ENTER);
                delay(2000); // wait between commands
                line = "";
            }
        }
        else
        {
            line += c;
        }
    }

    // last line if file doesn't end with newline
    if(line.length() > 0)
    {
        send_text_as_keyboard(line.c_str());
        delay(100);
        Keyboard.write(LV_KEY_ENTER);
    }

    file.close();
}


void send_shortcut(int sc)
{
  switch (sc)
  {
    // ---------- BASIC ----------
    case SC_ENTER:     Keyboard.write(KEY_RETURN); break;
    case SC_BACKSPACE: Keyboard.write(KEY_BACKSPACE); break;
    case SC_DELETE:    Keyboard.write(KEY_DELETE); break;
    case SC_ESCAPE:    Keyboard.write(KEY_ESC); break;
    case SC_TAB:       Keyboard.write(KEY_TAB); break;
    case SC_SPACE:     Keyboard.write(' '); break;
    case SC_UP:     Keyboard.write(KEY_UP_ARROW); break;
    case SC_DOWN:     Keyboard.write(KEY_DOWN_ARROW); break;
    case SC_RIGHT:     Keyboard.write(KEY_RIGHT_ARROW); break;
    case SC_LEFT:     Keyboard.write(KEY_LEFT_ARROW); break;
    case SC_VOLUP:      media_press(HID_USAGE_CONSUMER_VOLUME_INCREMENT); break;
    case SC_VOLDOWN:    media_press(HID_USAGE_CONSUMER_VOLUME_DECREMENT); break;
    case SC_MUTE:       media_press(HID_USAGE_CONSUMER_MUTE); break;
    case SC_PLAYPAUSE:  media_press(HID_USAGE_CONSUMER_PLAY_PAUSE); break;

    // ---------- SYSTEM ----------
    case SC_ALT_F4:
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_F4);
      Keyboard.releaseAll();
      break;

    case SC_CTRL_ALT_T:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press('t');
      Keyboard.releaseAll();
      break;

    // ---------- FUNCTION KEYS ----------
    case SC_F1:  Keyboard.write(KEY_F1); break;
    case SC_F2:  Keyboard.write(KEY_F2); break;
    case SC_F3:  Keyboard.write(KEY_F3); break;
    case SC_F4:  Keyboard.write(KEY_F4); break;
    case SC_F5:  Keyboard.write(KEY_F5); break;
    case SC_F6:  Keyboard.write(KEY_F6); break;
    case SC_F7:  Keyboard.write(KEY_F7); break;
    case SC_F8:  Keyboard.write(KEY_F8); break;
    case SC_F9:  Keyboard.write(KEY_F9); break;
    case SC_F10: Keyboard.write(KEY_F10); break;
    case SC_F11: Keyboard.write(KEY_F11); break;
    case SC_F12: Keyboard.write(KEY_F12); break;

    // ---------- WINDOWS ----------
    case SC_WIN_D:
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('d');
      Keyboard.releaseAll();
      break;

    case SC_WIN_I:
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('i');
      Keyboard.releaseAll();
      break;

    case SC_WIN_L:
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('l');
      Keyboard.releaseAll();
      break;

    case SC_WIN_V:
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('v');
      Keyboard.releaseAll();
      break;

    // ---------- CTRL ----------
    case SC_CTRL_P:
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press('p');
      Keyboard.releaseAll();
      break;

    case SC_CTRL_A:
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press('a');
      Keyboard.releaseAll();
      break;

    case SC_CTRL_C:
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press('c');
      Keyboard.releaseAll();
      break;

    case SC_CTRL_X:
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press('x');
      Keyboard.releaseAll();
      break;

    case SC_CTRL_V:
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press('v');
      Keyboard.releaseAll();
      break;

    case SC_CTRL_F:
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press('f');
      Keyboard.releaseAll();
      break;

    case SC_CTRL_S:
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press('s');
      Keyboard.releaseAll();
      break;

    case SC_CTRL_Z:
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press('z');
      Keyboard.releaseAll();
      break;
  }

  delay(30); 
}

String CBtext="";

void saveClipboardToSD()
{
    const char *text = lv_textarea_get_text(ui_KeyboardTextArea);
    if (!text || strlen(text) == 0) return;

    File file = SD_MMC.open("/clipboard.txt", FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file");
        return;
    }

    file.println(text);
    file.close();
    lv_textarea_set_text(ui_KeyboardTextArea, "");
}

void typeClipboardFromSD()
{
    File file = SD_MMC.open("/clipboard.txt");
    if (!file) return;

    String options = "";

    while (file.available())
    {
        char c = file.read();

        if (c == '\n')
        {
            options += '\n';  
        }
        else if (c != '\r')
        {
            options += c;
        }
    }

    file.close();

    if (options.length() > 0)
    {
      lv_roller_set_options(ui_Roller6, options.c_str(), LV_ROLLER_MODE_NORMAL);
    }
    else
    {
      lv_roller_set_options(ui_Roller6, "-empty-", LV_ROLLER_MODE_NORMAL);
    }

    int sel = lv_roller_get_selected(ui_Roller6);
    if (sel > 0) sel--;
    lv_roller_set_selected(ui_Roller6, sel, LV_ANIM_OFF);

    char buf[256];
    lv_roller_get_selected_str(ui_Roller6, buf, sizeof(buf));
    CBtext = String(buf);
}

void deleteClipboardEntry(const String& target)
{
    File file = SD_MMC.open("/clipboard.txt");
    if (!file) return;

    String newContent = "";

    while (file.available())
    {
        String line = file.readStringUntil('\n');
        line.trim();

        if (line.length() == 0) continue;

        if (line != target)   // keep everything except selected
        {
            newContent += line + "\n";
        }
    }

    file.close();

    // overwrite file with filtered content
    file = SD_MMC.open("/clipboard.txt", FILE_WRITE);
    if (!file) return;

    file.print(newContent);
    file.close();
}

void CB_roller_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;

    char buf[256];
    lv_roller_get_selected_str(ui_Roller6, buf, sizeof(buf));

    CBtext = String(buf);
}

void CBprint_btn_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;

  if (CBtext.length() == 0 or CBtext =="-empty-") return;

  send_text_as_keyboard(CBtext.c_str());

}

void CBdelete_btn_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;
    
    if (CBtext.length() == 0) return;

    deleteClipboardEntry(CBtext);
    typeClipboardFromSD();

}

void CBopen_btn_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    typeClipboardFromSD();

}

void CBadd_btn_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    saveClipboardToSD();

}

void CBwol_btn_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    while(connectToWifi()){
      delay(20000);
    } 

    WOL.sendMagicPacket(CBtext);
}


lv_timer_t* shortcut_delay_timer = nullptr;
lv_timer_t* shortcut_repeat_timer = nullptr;

bool shortcut_repeating = false;
int shortcut_current = 0;

void shortcut_delay_cb(lv_timer_t* t)
{
    shortcut_repeating = true;

    send_shortcut(shortcut_current);

    shortcut_repeat_timer = lv_timer_create(shortcut_timer_cb, 200, NULL);

    lv_timer_del(t);
    shortcut_delay_timer = nullptr;
}

void shortcut_timer_cb(lv_timer_t* t)
{
    send_shortcut(shortcut_current);
}

void shortcut_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    int shortcut = (int)(intptr_t)lv_event_get_user_data(e);

    if(code == LV_EVENT_PRESSED)
    {
        shortcut_current = shortcut;
        shortcut_repeating = false;

        if(shortcut_delay_timer)
            lv_timer_del(shortcut_delay_timer);

        shortcut_delay_timer = lv_timer_create(shortcut_delay_cb, 600, NULL);
        lv_timer_set_repeat_count(shortcut_delay_timer, 1);
    }

    if(code == LV_EVENT_RELEASED)
    {
        if(shortcut_delay_timer)
        {
            lv_timer_del(shortcut_delay_timer);
            shortcut_delay_timer = nullptr;
        }

        if(shortcut_repeat_timer)
        {
            lv_timer_del(shortcut_repeat_timer);
            shortcut_repeat_timer = nullptr;
        }

        // Only a short press produces the normal single shortcut.
        if(!shortcut_repeating)
            send_shortcut(shortcut);

        shortcut_repeating = false;
    }
}

void dropdown_select_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED)
        return;

    uint16_t * storage = (uint16_t *) lv_event_get_user_data(e);
    lv_obj_t * dd = lv_event_get_target(e);

    *storage = lv_dropdown_get_selected(dd);
}

void send_button_cb(lv_event_t * e)
{
    int base = (int) lv_event_get_user_data(e);
    uint16_t sel = 0;

    if (base == SC_F1)  sel = sel_f;
    else if (base == SC_WIN_D) sel = sel_win;
    else if (base == SC_CTRL_A) sel = sel_ctrl;

    //if (sel == 0) return;  // nothing selected

    send_shortcut(base + sel);
}


void script_roller_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;

    lv_obj_t * roller = lv_event_get_target(e);

    char buf[64];
    lv_roller_get_selected_str(roller, buf, sizeof(buf));

    rollerscript = String(buf);
}

void favourite_roller_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;

    lv_obj_t * roller = lv_event_get_target(e);

    char buf[64];
    lv_roller_get_selected_str(roller, buf, sizeof(buf));

    rollerfavourite = String(buf);
}


int init_ntp_sig=0;
const long  gmtOffset_sec = 3600;     // UTC +1
const int   daylightOffset_sec = 3600; // DST +1

void init_ntp_time(){
    configTime(gmtOffset_sec, daylightOffset_sec,
               "pool.ntp.org", "time.nist.gov");
    Serial.println("Obtaining time...");
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo, 10000)) {
        Serial.println("Failed to get time");
        return;
    }
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    Serial.println(buf);
    init_ntp_sig++;
}

time_t now;
struct tm tm;
bool flipflop=false;
lv_timer_t * flipflop_timer = NULL;

void set_ntp_time(lv_timer_t *timer){

  if(backlightOn == false) return;
  if(!init_ntp_sig)init_ntp_time();

  if(flipflop){
    time(&now);
    localtime_r(&now, &tm);

    char ui_time [25];
    sprintf(ui_time,"%02d:%02d  %02d/%02d/%02d",tm.tm_hour,tm.tm_min, tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900);
    lv_label_set_text(ui_IPlabel, ui_time);
    lv_label_set_text(ui_clocklabel, ui_time);
    flipflop=false;

  }else{
    String ip = WiFi.localIP().toString();
    lv_label_set_text(ui_IPlabel, ip.c_str());
    flipflop=true;
  }
}

int connectToWifi(){

  bool res = false;
  if(enteredPin.isEmpty()){
    _ui_screen_change(&ui_Screen5, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, &ui_Screen5_screen_init);
    return 1;
  }

  if (unlocked || kwconfig.unlock(enteredPin)){
    String tmp = kwconfig.getPasswordString("ap-password");
    res = wm.autoConnect("KeyWizard_AP",tmp.c_str());
    unlocked=true;
  }else{
      Serial.println("Invalid unlock password");
  }

  if (!res) {
    lv_label_set_text(ui_IPlabel, "WIFI OFF");
  } else {
    if(flipflop_timer){lv_timer_del(flipflop_timer); flipflop_timer=NULL;}
    flipflop_timer=lv_timer_create(set_ntp_time,10000,NULL);
    return 0;
  }
  return 1;
}


int start_ota_loop=0;

void ota_bin_upload(lv_timer_t* timer){

  if(connectToWifi()) return;
  wm.setConfigPortalBlocking(false);

  wm.startConfigPortal("KeyWizard_OTA_Portal", kwconfig.getPasswordString("ap-password").c_str());
  lv_timer_del(timer);
 
}

void ota_btn_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED)
        return;

    lv_timer_create(ota_bin_upload,20000,NULL);
    start_ota_loop=1;

}

String readFromSerial;
void config_btn_event_cb(lv_event_t * e)
{
  if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
  
  lv_obj_add_flag(ui_unlockslider, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(ui_configlabel, LV_OBJ_FLAG_HIDDEN);
  _ui_screen_change(&ui_Screen9, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, &ui_Screen9_screen_init);
  ledcWrite(GFX_BL, (1<<10)*0.7);

  Keyboard.end();
  Mouse.end();
  Consumer.end();

  kwcli.setActive(true);

}

void poweroff_btn_event_cb(lv_event_t * e){
  if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
  power.shutdown();
}

int getBatteryPercentSafe() {

  if (!power.isBatteryConnect()) {
    return -1;   // no battery
  }

  return power.getBatteryPercent();
}

void battery_timer_cb(lv_timer_t * timer)
{
    static int last_percent = -2;   // impossible initial value

    int percent = getBatteryPercentSafe();

    if (percent != last_percent)   // Only update if changed
    {
        last_percent = percent;

        if (percent >= 0) {
            lv_label_set_text_fmt(ui_batterystatuslabel, "%d%%", percent);
            lv_label_set_text_fmt(ui_batterystatuslabel2, "%d%%", percent);
        } else {
            lv_label_set_text(ui_batterystatuslabel, "USB-C");
            lv_label_set_text(ui_batterystatuslabel2, "USB-C");
        }
    }
}


void play_selected_track(int override=-1)
{
    uint16_t index = override==-1?lv_roller_get_selected(ui_Roller1):override;

    if(index >= musicPaths.size())
        return;

    Serial.print("Selected: ");
    Serial.println(musicTitles[index]);

    Serial.print("Path: ");
    Serial.println(musicPaths[index]);

    audio.stopSong();

    if(musicIsURL[index])
    {
        Serial.println("Playing URL...");
        if(connectToWifi()) return;
        audio.connecttohost(musicPaths[index].c_str());
    }
    else
    {
        Serial.println("Playing SD file...");

        if(!SD_MMC.exists(musicPaths[index].c_str()))
        {
            Serial.println("ERROR: File does not exist!");
            return;
        }

        Serial.println("File exists.");

        lv_label_set_text(ui_trackname, musicTitles[index].c_str());
        lv_label_set_text(ui_trackname2, musicTitles[index].c_str());
        audio.connecttoFS(SD_MMC, musicPaths[index].c_str());
    }

    playerState = PLAYER_PLAYING;
    update_play_icon(true);
}

void play_button_cb(lv_event_t * e)
{
    if (playerState == PLAYER_PAUSED) {
        audio.pauseResume();
        playerState = PLAYER_PLAYING;
        update_play_icon(true);
        return;
    }

  play_selected_track();
}


void pause_button_cb(lv_event_t * e)
{
    if (playerState == PLAYER_PLAYING) {
        audio.pauseResume();
        playerState = PLAYER_PAUSED;
        update_play_icon(false);
    }
    else if (playerState == PLAYER_PAUSED) {
        audio.pauseResume();
        playerState = PLAYER_PLAYING;
        update_play_icon(true);
    }
}


void stop_button_cb(lv_event_t * e)
{
    audio.stopSong();
    playerState = PLAYER_STOPPED;
    update_play_icon(false);
    update_time_label(0);
    lv_slider_set_value(ui_playposition, 0, LV_ANIM_OFF);
    lastPosSec = 0;
}

lv_timer_t * time_timer;

void time_timer_cb(lv_timer_t * timer)
{
    if (playerState != PLAYER_PLAYING) return;

    uint32_t cur = audio.getAudioCurrentTime();

    if (cur != lastPosSec) {
        lastPosSec = cur;

        update_time_label(cur);
        int max = lv_slider_get_max_value(ui_playposition);
        if (cur >= max - 5) {  // extend range when near end
          lv_slider_set_range(ui_playposition, 0, max + 120);
        }

        if (ui_playposition && lv_obj_is_valid(ui_playposition)) {
            lv_slider_set_value(ui_playposition, cur, LV_ANIM_OFF);
        }
    }
}

void slider_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_RELEASED) return;

    int newPos = lv_slider_get_value(ui_playposition);

    audio.setAudioPlayPosition(newPos);  // jump to second
    lastPosSec = newPos;
    update_time_label(newPos);
}


void vol_up_cb(lv_event_t * e)
{
    if (volume < 21) volume++;
    audio.setVolume(volume);
    update_volume_label();

}

void vol_down_cb(lv_event_t * e)
{
    if (volume > 0) volume--;
    audio.setVolume(volume);
    update_volume_label();
}

void digit_btn_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if (enteredPin.length() >= MAX_PIN_LEN) return;

    int digit = (int) lv_event_get_user_data(e);
    enteredPin += String(digit);

    update_pin_label();
}

void backspace_btn_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if (enteredPin.length() > 0) {
        //enteredPin.remove(enteredPin.length() - 1);
        enteredPin = "";
        update_pin_label();
    }
}

void account_roller_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;

    char buf[32];
    lv_roller_get_selected_str(lv_event_get_target(e), buf, sizeof(buf));
    selectedAccount = String(buf);
}

void enter_btn_cb(lv_event_t * e)
{
  if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

  for (int i = 0; i < accountCount; i++)
  {
      if (selectedAccount == accounts[i])
      {
        if (unlocked || kwconfig.unlock(enteredPin))
        {
            String passkey = kwconfig.getPasswordString(selectedAccount.c_str());
            send_slow_text(passkey.c_str());
            unlocked=true;
        }
        else
        {
            Serial.println("Invalid unlock password");
            break;
        }

        delay(100);
        Keyboard.write(LV_KEY_ENTER); 
          
        break;
      }
  }
}

void fav_enter_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    uint16_t index = lv_roller_get_selected(ui_Roller5);

    if(index < favCount){
      if (unlocked || kwconfig.unlock(enteredPin))
      {
        String tmp = kwconfig.getFavouriteString(favourite_urls[index].c_str());
        open_url(tmp.c_str());
        unlocked=true;
      }
      else
      {
          Serial.println("Invalid unlock password");
      }
    }
}

void script_enter_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    uint16_t index = lv_roller_get_selected(ui_Roller4);
    type_script_file(index);

}

void home_slider_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;

    lv_obj_t * slider = lv_event_get_target(e);
    int value = lv_slider_get_value(slider);   // 0–100

    int startX = 585;
    int endX   = -550;
    int distance = startX - endX; 

    int newX = startX - ((distance * value) / 100);

    lv_obj_set_x(ui_HomePanel, newX);
}

void unlock_slider_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;

    lv_obj_t * slider = lv_event_get_target(e);
    int value = lv_slider_get_value(slider);   // 0–100

    if(value>99){
      _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, &ui_Screen1_screen_init);
      ledcWrite(GFX_BL, (1<<10));
      delay(200);
      lv_slider_set_value(slider,0,LV_ANIM_OFF);
    }
}

void qmi_timer(lv_timer_t* t)
{
  if(backlightOn && (millis() - lastTouchTime > backlightTimeoutMs)) {backlight_off();}
  if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {

    if(gyr.x > 15 or gyr.y > 15 or gyr.z > 15){
      backlight_on();
    }

  }

}



void setup() {

  Serial.begin(115200);
  
  kwcli.begin();
  accounts = kwconfig.getKeys(1);
  accountCount = accounts.size();
  if(accountCount > 0) selectedAccount = accounts[0];

  favourite_urls = kwconfig.getKeys(2);
  favCount = favourite_urls.size();

  gpio_init();
  SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
  if (!SD_MMC.begin("/sdmmc", true, false, 20000))
  {
    esp_rom_printf("Card Mount Failed\n");
    sd_begin_failed=1;
  }

  Wire.begin(I2C_SDA, I2C_SCL);
  TCA.begin();

  USB.begin();
  Keyboard.begin(KeyboardLayout_de_DE);
  Mouse.begin();
  Consumer.begin(); 

  es8311_codec_init();
  TCA.pinMode1(1,OUTPUT);
  lcd_reset();
  randomSeed(esp_random());

  initPMU();

  #if SENSOR_IRQ > 0
  qmi.setPins(SENSOR_IRQ);
  #endif
  qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL);

   qmi.configAccelerometer(
    SensorQMI8658::ACC_RANGE_4G,
    SensorQMI8658::ACC_ODR_1000Hz,
    SensorQMI8658::LPF_MODE_0);

    qmi.configGyroscope(
    SensorQMI8658::GYR_RANGE_64DPS,
    SensorQMI8658::GYR_ODR_896_8Hz,
    SensorQMI8658::LPF_MODE_3);

    qmi.enableGyroscope();

  if (!touch.begin(Wire, FT6X36_SLAVE_ADDRESS)) {
    Serial.println("Failed to find FT6X36 - check your wiring!");
    while (1) {
      delay(1000);
    }
  }
  // Init Display
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }

  gfx->fillScreen(RGB565_BLACK);

#ifdef GFX_BL
  //pinMode(GFX_BL, OUTPUT);
  //digitalWrite(GFX_BL, HIGH);
    //ledcSetup(backlightChannel, backlightFreq, backlightResolution);
    ledcAttach(GFX_BL, 5000, 10);
    ledcWrite(GFX_BL, (1<<10));
#endif

  lv_init();

  screenWidth = gfx->width();
  screenHeight = gfx->height();

  bufSize = screenWidth * 120;

  disp_draw_buf1 = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_DEFAULT | MALLOC_CAP_8BIT);

  disp_draw_buf2 = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_DEFAULT | MALLOC_CAP_8BIT);

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf1, disp_draw_buf2, bufSize);

  /* Initialize the display */
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;

  lv_disp_drv_register(&disp_drv);


  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  /* Initialize the (dummy) input device driver 
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello Arduino! (V" GFX_STR(LVGL_VERSION_MAJOR) "." GFX_STR(LVGL_VERSION_MINOR) "." GFX_STR(LVGL_VERSION_PATCH) ")");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t *sw = lv_switch_create(lv_scr_act());
  lv_obj_align(sw, LV_ALIGN_TOP_MID, 0, 50);

  sw = lv_switch_create(lv_scr_act());
  lv_obj_align(sw, LV_ALIGN_BOTTOM_MID, 0, -50);
  */

  /* Option 3: Or try out a demo. Don't forget to enable the demos in lv_conf.h. E.g. LV_USE_DEMOS_WIDGETS*/
  // lv_demo_widgets();
  // lv_demo_benchmark();
  // lv_demo_keypad_encoder();
  // lv_demo_music();
  // lv_demo_stress();


  ui_init();

  audio.setPinout(I2S_BCLK, I2S_LRCK, I2S_SDOUT, I2S_MCLK);
  audio.setVolume(12);  // 0...21

  time_timer = lv_timer_create(time_timer_cb, 1000, NULL);
  lv_timer_create(battery_timer_cb, 5000, NULL);
  lv_timer_create(qmi_timer, 500, NULL);

  init_rollers();
  update_play_icon(0);

  lv_slider_set_range(ui_startslider, 0, 100);
  lv_obj_add_event_cb(ui_startslider, home_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(ui_OTAbutton, ota_btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(ui_titlebutton, config_btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(ui_poweroffbutton, poweroff_btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(ui_unlockslider, unlock_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);

  lv_obj_add_event_cb(ui_KeyboardTextArea, textarea_event_cb, LV_EVENT_READY, NULL);
  lv_obj_add_event_cb(ui_rightclickbutton, mouseclick_btn_cb, LV_EVENT_CLICKED, (void*)MOUSE_RIGHT);
  lv_obj_add_event_cb(ui_leftclickbutton, mouseclick_btn_cb, LV_EVENT_CLICKED, (void*)MOUSE_LEFT);
  lv_obj_add_event_cb(ui_stickyclickbutton, mouseclick_btn_cb, LV_EVENT_PRESSED, (void*)MOUSE_STICKY);
  lv_obj_add_event_cb(ui_stickyclickbutton, mouseclick_btn_cb, LV_EVENT_CLICKED, (void*)MOUSE_STICKY);

  auto addPressReleaseCallback = [](lv_obj_t* obj, lv_event_cb_t callback, void* user_data)
  {
    lv_obj_add_event_cb(obj, callback, LV_EVENT_PRESSED, user_data);
    lv_obj_add_event_cb(obj, callback, LV_EVENT_RELEASED, user_data);
  };

  addPressReleaseCallback(ui_enterbutton, shortcut_cb, (void*)SC_ENTER);
  addPressReleaseCallback(ui_delbutton, shortcut_cb, (void*)SC_DELETE);
  addPressReleaseCallback(ui_backbutton, shortcut_cb, (void*)SC_BACKSPACE);
  lv_obj_add_event_cb(ui_altf4button, shortcut_cb, LV_EVENT_CLICKED, (void*)SC_ALT_F4);  
  addPressReleaseCallback(ui_tabbutton, shortcut_cb, (void*)SC_TAB);
  addPressReleaseCallback(ui_spacebutton, shortcut_cb, (void*)SC_SPACE);
  addPressReleaseCallback(ui_escbutton, shortcut_cb, (void*)SC_ESCAPE);
  lv_obj_add_event_cb(ui_terminalbutton, shortcut_cb, LV_EVENT_CLICKED, (void*)SC_CTRL_ALT_T);     
  addPressReleaseCallback(ui_upbutton, shortcut_cb, (void*)SC_UP);
  addPressReleaseCallback(ui_downbutton, shortcut_cb, (void*)SC_DOWN);
  addPressReleaseCallback(ui_rightbutton, shortcut_cb, (void*)SC_RIGHT);
  addPressReleaseCallback(ui_leftbutton, shortcut_cb, (void*)SC_LEFT);
  addPressReleaseCallback(ui_volupbutton, shortcut_cb, (void*)SC_VOLUP);
  addPressReleaseCallback(ui_voldownbutton, shortcut_cb, (void*)SC_VOLDOWN);
  lv_obj_add_event_cb(ui_mutebutton, shortcut_cb, LV_EVENT_CLICKED, (void*)SC_MUTE);
  lv_obj_add_event_cb(ui_playpausebutton, shortcut_cb, LV_EVENT_CLICKED, (void*)SC_PLAYPAUSE);


//flist 
lv_obj_add_event_cb(
    ui_flist,
    dropdown_select_cb,
    LV_EVENT_VALUE_CHANGED,
    &sel_f
);
lv_obj_add_event_cb(
    ui_fenter,
    send_button_cb,
    LV_EVENT_CLICKED,
    (void*)SC_F1
);

//winlist
lv_obj_add_event_cb(
    ui_winlist,
    dropdown_select_cb,
    LV_EVENT_VALUE_CHANGED,
    &sel_win
);
lv_obj_add_event_cb(
    ui_winenter,
    send_button_cb,
    LV_EVENT_CLICKED,
    (void*)SC_WIN_D
);

//ctrllist
lv_obj_add_event_cb(
    ui_ctrllist,
    dropdown_select_cb,
    LV_EVENT_VALUE_CHANGED,
    &sel_ctrl
);
lv_obj_add_event_cb(
    ui_ctrlenter,
    send_button_cb,
    LV_EVENT_CLICKED,
    (void*)SC_CTRL_A
);


lv_obj_add_event_cb(ui_playbutton, play_button_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_pausebutton, pause_button_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_stopbutton, stop_button_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_plusbutton, vol_up_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_minusbutton, vol_down_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_playposition, slider_event_cb, LV_EVENT_RELEASED, NULL);


lv_obj_add_event_cb(ui_zerobutton, digit_btn_cb, LV_EVENT_CLICKED, (void*)0);
lv_obj_add_event_cb(ui_onebutton, digit_btn_cb, LV_EVENT_CLICKED, (void*)1);
lv_obj_add_event_cb(ui_twobutton, digit_btn_cb, LV_EVENT_CLICKED, (void*)2);
lv_obj_add_event_cb(ui_threebutton, digit_btn_cb, LV_EVENT_CLICKED, (void*)3);
lv_obj_add_event_cb(ui_fourbutton, digit_btn_cb, LV_EVENT_CLICKED, (void*)4);
lv_obj_add_event_cb(ui_fivebutton, digit_btn_cb, LV_EVENT_CLICKED, (void*)5);
lv_obj_add_event_cb(ui_sixbutton, digit_btn_cb, LV_EVENT_CLICKED, (void*)6);
lv_obj_add_event_cb(ui_sevenbutton, digit_btn_cb, LV_EVENT_CLICKED, (void*)7);
lv_obj_add_event_cb(ui_eightbutton, digit_btn_cb, LV_EVENT_CLICKED, (void*)8);
lv_obj_add_event_cb(ui_ninebutton, digit_btn_cb, LV_EVENT_CLICKED, (void*)9);

lv_obj_add_event_cb(ui_pinbackspacebutton, backspace_btn_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_enterbutton2, enter_btn_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_Roller3, account_roller_cb, LV_EVENT_VALUE_CHANGED, NULL);

lv_obj_add_event_cb(ui_enterbutton4, fav_enter_event_cb, LV_EVENT_CLICKED, NULL);

lv_obj_add_event_cb(ui_enterbutton3, script_enter_event_cb, LV_EVENT_CLICKED, NULL);

lv_obj_add_event_cb(ui_Roller6, CB_roller_cb, LV_EVENT_VALUE_CHANGED, NULL);
lv_obj_add_event_cb(ui_CBprintbutton, CBprint_btn_event_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_CBdeletebutton, CBdelete_btn_event_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_addCBbutton, CBadd_btn_event_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_openCBbutton, CBopen_btn_event_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_CBwolbutton, CBwol_btn_event_cb, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(ui_CBenterbutton, shortcut_cb, LV_EVENT_CLICKED, (void*)SC_ENTER);

  Serial.println("Setup done");

}

void loop() {
  lv_timer_handler(); 
  kwcli.process();
  wm.process();
  //if(start_ota_loop)server.handleClient();
  audio.loop();

  delay(1);
}

// optional
void audio_info(const char *info)
{/*
    // example: "Duration: 245 sec"
    if (strstr(info, "Duration")) {
        trackDurationSec = atoi(info + 10);
        trackRemainingSec = trackDurationSec;
        update_time_label();
    }
    */
}

void audio_id3data(const char *info) {  //id3 metadata
  Serial.print("id3data     ");
  Serial.println(info);
}

void audio_eof_mp3(const char *info)
{
    Serial.println("Track finished");

    if (playerState == PLAYER_PLAYING) {
      play_selected_track(random(0,musicTitles.size()-1));
    }
}

void audio_showstation(const char *info) {
  Serial.print("station     ");
  Serial.println(info);
}
void audio_showstreaminfo(const char *info) {
  Serial.print("streaminfo  ");
  Serial.println(info);
}
void audio_showstreamtitle(const char *info) {
  Serial.print("streamtitle ");
  Serial.println(info);
  lv_label_set_text(ui_trackname, info);
  lv_label_set_text(ui_trackname2, info);
}
void audio_bitrate(const char *info) {
  Serial.print("bitrate     ");
  Serial.println(info);
}
void audio_commercial(const char *info) {  //duration in sec
  Serial.print("commercial  ");
  Serial.println(info);
}
void audio_icyurl(const char *info) {  //homepage
  Serial.print("icyurl      ");
  Serial.println(info);
}
void audio_lasthost(const char *info) {  //stream URL played
  Serial.print("lasthost    ");
  Serial.println(info);
}
void audio_eof_speech(const char *info) {
  Serial.print("eof_speech  ");
  Serial.println(info);
}