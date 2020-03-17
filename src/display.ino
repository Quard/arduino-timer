#include <U8g2lib.h>


U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void displayBegin() {
  u8g2.begin();
}

void displayDuration(bool on, unsigned long duration) {
  int hours = (int) (duration / 60. / 60);
  int minutes = (int) ((duration - hours * 60. * 60) / 60);
  int secs = duration - hours * 60 * 60 - minutes * 60;

  u8g2.firstPage();
  do {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 0, 128, 10);
    u8g2.setDrawColor(0);
    u8g2.setFont(u8g2_font_tenfatguys_tf);
    if (on) {
      u8g2.drawStr(35, 10, "armed");
    } else {
      u8g2.drawStr(20, 10, "disarmed");
    }
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_logisoso26_tn);
    char str[10];
    snprintf(str, 10, "%02d:%02d", hours, minutes);
    u8g2.drawStr(5, 60, str);
    u8g2.setFont(u8g2_font_logisoso18_tr);
    snprintf(str, 10, ".%02d", secs);
    u8g2.drawStr(90, 60, str);
  } while ( u8g2.nextPage() );
}
