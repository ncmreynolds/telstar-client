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
  Serial.printf("Data (%03u bytes) received from %s: ", len, client->remoteIP().toString().c_str());
  if(len < (rows * columns) - bufferPosition)
  {
    memcpy(textToRender + bufferPosition, data, len);
    for(uint16_t i = 0; i < len; i++)
    {
      Serial.printf("%02x ",uint8_t(textToRender[bufferPosition + i]));
    }
    bufferPosition += len;
    /*for(uint16_t i = 0; i < len; i++)
    {
      if(uint8_t(data[i]) > 31 && uint8_t(data[i]) < 127)
      {
        textToRender[bufferPosition] = data[i];
        Serial.write(data[i]);
      }
      else
      {
        textToRender[bufferPosition] = char(32);
        Serial.write(' ');
      }
      bufferPosition++;
    }
    */
  }
  else
  {
    Serial.println("Data too long for videotex page");
    //Serial.write((uint8_t *)data, len);
  }
  Serial.println();
}

void onConnect(void *arg, AsyncClient *client)
{
  Serial.printf("Client has connected to %s on port %d \n", TCP_SERVER, TCP_SERVER_PORT);
  tcpConnected = true;
  //requestTelstarFullSpeed(client);
  //httpGet(client);
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
/*
void httpGet(void *arg)
{
  AsyncClient *client = reinterpret_cast<AsyncClient *>(arg);
  //our big json string test
  //String jsonString = "{\"data_from_module_type\":1,\"hub_unique_id\":\"hub-bfd\",\"slave_unique_id\":\"water-sensor-ba9\",\"water_sensor\":{\"unique_id\":\"water-sensor-ba9\",\"firmware\":\"0.0.1\",\"hub_unique_id\":\"hub-bfd\",\"ip_address\":\"192.168.4.2\",\"mdns\":\"water-sensor-ba9.local\",\"pair_status\":254,\"ec\":{\"value\":0,\"calib_launch\":0,\"sensor_k_origin\":1,\"sensor_k_calibration\":1,\"calibration_solution\":1,\"regulation_state\":1,\"max_pumps_durations\":5000,\"set_point\":200},\"ph\":{\"value\":0,\"calib_launch\":0,\"regulation_state\":1,\"max_pumps_durations\":5000,\"set_point\":700},\"water\":{\"temperature\":0,\"pump_enable\":false}}}";
  // send reply
  String jsonString = "GET / HTTP/1.0\r\n\r\n";
  if (client->space() > strlen(jsonString.c_str()) && client->canSend())
  {
    client->add(jsonString.c_str(), strlen(jsonString.c_str()));
    client->send();
  }
}
*/
