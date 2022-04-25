void draw()
{
  //clearing background and starting to draw
  graphics.setHue(0);
  graphics.begin(0);
  //drawing an image
  luni0.draw(graphics, 200, 10);

  // Cycle the hue used for drawing
  graphics.setHue(millis()/1000);
  
  //drawing a frame
  graphics.fillRect(27, 18, 160, 30, 10);
  graphics.rect(27, 18, 160, 30, 20);

  //setting text color, transparent background
  graphics.setTextColor(50);
  //text starting position
  graphics.setCursor(30, 20);
  //printing some lines of text
  graphics.print("hello!");
  graphics.print(" free memory: ");
  graphics.print((int)heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
  graphics.print("\nrendered frame: ");
  static int frame = 0;
  graphics.print(frame, 10, 4); //base 10, 6 characters
  graphics.print("\n        in hex: ");
  graphics.print(frame, 16, 4);
  frame++;

  //drawing some lines
  for(int i = 0; i <= 100; i++)
  {
    graphics.line(50, i + 60, 50 + i, 160, i / 2);
    graphics.line(150, 160 - i, 50 + i, 60, i / 2);
  }

  //draw single pixel
  graphics.dot(20, 190, 10);

  //finished drawing, swap back and front buffer to display it
  graphics.end();
}

void startPage()
{
  bufferPosition = 0;
  Serial.println("Page starts...");
  graphics.begin(0);
  graphics.setHue(0);
  graphics.setCursor(0, 0);
  graphics.setTextColor(50);
  pageTimer = millis();
}

void endPage()
{
  Serial.println("Page ends, rendering");
  currentRow = 0;
  currentColumn = 0;
  for(uint16_t i = 0; i < bufferPosition; i++)
  {
    if(uint8_t(textToRender[i]) > 31 && uint8_t(textToRender[i]) < 127)
    {
      lineBuffer[lineBufferPosition++] = textToRender[i];
      currentColumn++;
    }
    else
    {
      switch (uint8_t(textToRender[i])) {
        case 0x0a:
          endLine();
        break;
        case 0x1b:
          lineBuffer[lineBufferPosition++] = ' ';
          changeColour(uint8_t(textToRender[++i]));
          currentColumn++;
        break;
        default:
          lineBuffer[lineBufferPosition++] = ' ';
          //Serial.printf("[%02x]",uint8_t(textToRender[i]));
          currentColumn++;
        break;
      }
    }
    if(currentColumn == columns)
    {
      endLine();
    }
  }
  graphics.end();
  currentRow = 0;
  currentColumn = 0;
}

void endLine()
{
  currentColumn = 0;
  lineBuffer[lineBufferPosition] = char(0);
  if(currentRow < rows)
  {
    currentRow++;
    Serial.println(lineBuffer);
  }
  else
  {
    Serial.print(lineBuffer);
  }
  graphics.setCursor(0, currentRow * 8);
  graphics.print(lineBuffer);
  lineBufferPosition = 0;
}

void changeColour(uint8_t colour)
{
}

void clearScreen()
{
  graphics.setHue(0);
  graphics.begin(0);
  graphics.end();
}
