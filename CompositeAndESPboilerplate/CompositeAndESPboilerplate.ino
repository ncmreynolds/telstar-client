/*
 * Configuration
 */
#define ESP_BOILERPLATE
#include "credentials.h"
/*
 * WiFi
 */
#ifdef ESP_BOILERPLATE
  #include <espBoilerplate.h>
#else
  #include "WiFi.h"
#endif
bool connectedOk = false;
/*
 * Composite video essentials
 */
#include "esp_pm.h"
#include "CompositeGraphics.h"
#include "Image.h"
#include "CompositeColorOutput.h"
#include "luni.h"
#include "font6x8.h"
//Graphics using the fixed resolution for the color graphics
CompositeGraphics graphics(CompositeColorOutput::XRES, CompositeColorOutput::YRES);
//Composite output using the desired mode (PAL/NTSC) and a fixed resolution
CompositeColorOutput composite(CompositeColorOutput::NTSC);
//CompositeColorOutput composite(CompositeColorOutput::PAL);
//image and font from the included headers created by the converter. Each iamge uses its own namespace.
Image<CompositeGraphics> luni0(luni::xres, luni::yres, luni::pixels);
//font is based on ASCII starting from char 32 (space), width end height of the monospace characters.
//All characters are staored in an image vertically. Value 0 is background.
Font<CompositeGraphics> font(6, 8, font6x8::pixels);
#include <soc/rtc.h>
/*
 * Async TCP essentials
 */
#include <AsyncTCP.h>
AsyncClient *client_tcp = new AsyncClient;
bool tcpConnected = false;
/*
 * Videotex rendering
 */
const uint8_t rows = 25;
const uint8_t columns = 40;
uint16_t bufferPosition = 0;
char textToRender[rows * columns];
uint8_t currentRow = 0;
uint8_t currentColumn = 0;
const uint8_t lineBufferSize = 64;
char lineBuffer[lineBufferSize];
uint8_t lineBufferPosition = 0;
uint32_t pageTimer = 0;
const uint32_t pageTimeout = 5000;
/*
 * Setup
 */
void setup()
{
  Serial.begin(115200);
  /*
   * Video setup
   */
  Serial.println("Intialising video...");
  //highest clockspeed needed
  esp_pm_lock_handle_t powerManagementLock;
  esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "compositeCorePerformanceLock", &powerManagementLock);
  esp_pm_lock_acquire(powerManagementLock);
  //initializing DMA buffers and I2S
  composite.init();
  //initializing graphics double buffer
  graphics.init();
  //select font
  graphics.setFont(font);
  Serial.printf("Graphics resolution(x/y): %u/%u\r\n", graphics.xres, graphics.yres);
  #ifdef ESP_BOILERPLATE
    espBoilerplate.setRetries(60);                          //Optionally, increase retries on the connection to 60s. Default is 30s.
    espBoilerplate.setOutputStream(Serial);
    connectedOk = espBoilerplate.begin(WIFI_SSID,WIFI_PSK); //Connect to the Wi-Fi SSID WIFI_SSID with pre-shared key WIFI_PSK, return is true if succesful
    if(connectedOk == false)
    {
      Serial.println("Restarting");
      ESP.restart();
    }
  #else
    Serial.println("Connecting WiFi...");
    WiFi.begin(WIFI_SSID,WIFI_PSK);
    uint8_t retries = 30;
    while(WiFi.status() != WL_CONNECTED && retries > 0)
    {
      Serial.print('.');
      delay(1000);
      retries--;
    }
    if(WiFi.status() != WL_CONNECTED)
    {
      Serial.println("WiFi connection failed, restarting");
      ESP.restart();
    }
    else
    {
      Serial.println("Connected");
    }
  #endif
  /*
   * Set up TCP connection to server
   */
  client_tcp->onData(onData, client_tcp);
  client_tcp->onConnect(onConnect, client_tcp);
  client_tcp->onDisconnect(onDisconnect, client_tcp);
  //client_tcp->onError(onError, client_tcp);
  //client_tcp->onTimeout(onTimeout, client_tcp);
  /*
   * Make the TCP connection
   */
  client_tcp->connect(TCP_SERVER, TCP_SERVER_PORT);
  /*
   * Put something on the screen to start
   */
  clearScreen();
  //draw();
}



void loop()
{
  //draw();
  composite.sendFrameHalfResolution(&graphics.frame);
  if (Serial.available() > 0)
  {
    sendSerialToServer(client_tcp);
  }
  if(pageTimer > 0 && millis() - pageTimer > pageTimeout)
  {
    pageTimer = 0;
    endPage();  //Display the page
  }
}
