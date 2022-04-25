void onData(void *arg, AsyncClient *client, void *data, size_t len)
{
  if(pageTimer == 0)
  {
    startPage();
  }
  else
  {
    pageTimer = millis();
  }
  if(fast == false)
  {
    Serial.printf("Data (%03u bytes) received from %s: ", len, client->remoteIP().toString().c_str());
  }
  if(len < (bufferSize) - bufferPosition)
  {
    memcpy(tcpBuffer + bufferPosition, data, len);
    if(fast == false)
    {
      for(uint16_t i = 0; i < len; i++)
      {
        Serial.printf("%02x ",uint8_t(tcpBuffer[bufferPosition + i]));
      }
    }
    bufferPosition += len;
    if(tcpBuffer[bufferPosition - 1] == 0x47 && tcpBuffer[bufferPosition - 2] == 0x1b)  //ESC 0x47 signifies end of page
    {
      bufferPosition-=2;  //Strip the page end marker
      pageEnded = true;
    }
  }
  else
  {
    Serial.println("Data too long for TCP buffer page");
  }
  if(fast == false)
  {
    Serial.println();
  }
}

void onConnect(void *arg, AsyncClient *client)
{
  Serial.printf("Client has connected to %s on port %d\n", TCP_SERVER, TCP_SERVER_PORT);
  tcpConnected = true;
  if(fast)
  {
    requestTelstarFullSpeed(client);
  }
}

void onDisconnect(void *arg, AsyncClient *client)
{
  Serial.printf("Client has been disconnected from %s on port %d \n", TCP_SERVER, TCP_SERVER_PORT);
  tcpConnected = false;
}

void onError(void *arg, AsyncClient *client)
{
  Serial.printf("Client has been disconnected from %s on port %d \n", TCP_SERVER, TCP_SERVER_PORT);
  tcpConnected = false;
}

void onTimeout(void *arg, AsyncClient *client)
{
  Serial.printf("Client has timed out %s on port %d \n", TCP_SERVER, TCP_SERVER_PORT);
  tcpConnected = false;
}

void sendSerialToServer(void *arg)
{
  AsyncClient *client = reinterpret_cast<AsyncClient *>(arg);
  uint8_t bufferLength = Serial.available();
  char serialBuffer[bufferLength];
  for(uint8_t i = 0; i < bufferLength; i++)
  {
    serialBuffer[i] = Serial.read();
  }
  Serial.print("Serial buffer : \"");
  for(uint8_t i = 0; i < bufferLength; i++)
  {
    if(uint8_t(serialBuffer[i]) < 32)
    {
      Serial.printf("[%02x]", uint8_t(serialBuffer[i]));
    }
    else
    {
      Serial.write(serialBuffer[i]);
    }
  }
  Serial.println("\"");
  if(tcpConnected == true)
  {
    if (client->space() > bufferLength && client->canSend())
    {
      if(bufferLength > 1 && serialBuffer[bufferLength - 1] == 0x0d)
      {
        client->add(serialBuffer, bufferLength - 1);
        Serial.printf("Sent serialBuffer[%i] to server\r\n", bufferLength - 1);
      }
      else
      {
        client->add(serialBuffer, bufferLength);
        Serial.printf("Sent serialBuffer[%i] to server\r\n", bufferLength);
      }
      client->send();
    }
  }
  else
  {
    Serial.printf("Unable to send serialBuffer[%i] to server, disconnected\r\n", bufferLength);
  }
}
void requestTelstarFullSpeed(void *arg)
{
  AsyncClient *client = reinterpret_cast<AsyncClient *>(arg);
  char fullSpeedSequence[3] = {char(0xFF), char(0xFD), char(0x03)};
  if (client->space() > 3 && client->canSend())
  {
    client->add(fullSpeedSequence, 3);
    client->send();
  }
}
