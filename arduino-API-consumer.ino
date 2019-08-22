/*
  A simple Http client which requests current weather data from the Weather Unlocked API,
  and parses the JSON response. In the case that the WiFi connection is lost,
  the program will attempt to re-acquire a connection to the specified network.

  Libraries:
    ArduinoHttpClient         : Http client for interacting with web servers.
    WiFiNINA                  : WiFi library for the Arduino NANO 33 IoT
    ArduinoJson (version 6+)  : Used for pasing JSON response data.

  APIs:
    https://developer.weatherunlocked.com/documentation/localweather

  Written by:
    Ian McLerran
    https://www.imclerran.com
*/

#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

// Wifi globals:
char ssid[] = NETWORK_SSID;      //  your network SSID (name)
char pass[] = NETWORK_PASSWD;       // your network password
int keyIndex = 0;                  // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
WiFiClient wifi;

// HttpCient globals:
char serverAddress[] = "api.weatherunlocked.com";
int port = 80;
HttpClient client = HttpClient(wifi, serverAddress, port);

// Json globals:
StaticJsonDocument<1024> doc;

// Weather data globals:
double temp = 0;
double humid = 0;
double wind = 0;
char desc[64] = "";

void setup() {
  Serial.begin(9600);
  wifiConnect();

  // display network info once connected:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println("\n");
}

void loop() {
  String path = "/api/current/" + String(ZIP_STRING);
  String qParams = "?app_id=" + String(APP_ID) + "&app_key=" + String(APP_KEY);
  String query = path + qParams;
  String headers[] = {"Accept", "application/json"};

  Serial.println("Getting weather data from Weather Unlocked...");
  // make GET request to the server, using the path and query parameters defined above

  
  int statusCode = httpGET(path, qParams, headers, 1); 
  String response = client.responseBody();

  if (statusCode < 200 || statusCode >= 300) { // if status code is not in the 200 range, request failed
    Serial.println("Failed to fetch weather data!");
    Serial.print("  Status code: ");
    Serial.println(statusCode);
    
    if(wifi.status() != WL_CONNECTED) { // check if wifi has been disconnected
      Serial.println("\nWifi connection lost. Attempting to reconnect...");
      wifiConnect(false); // try to connect to wifi, no verbose mode.
      Serial.print("  Successfully reconnected to ");
      Serial.println(WiFi.SSID());
    }
  }
  else { // status code is in 200s, attempt to deserialize response
    DeserializationError error = deserializeJson(doc, response);
    if (error) { // deserialization failed
      Serial.println("Could not interpret API response.");
      Serial.print("  Response: ");
      Serial.println(response); // display http response data for debugging purposes
    }
    else { // successfully deserialized, get desired data from JSON doc.
      temp = doc["temp_f"];
      humid = doc["humid_pct"];
      wind = doc["windspd_mph"];
      strcpy(desc, doc["wx_desc"]);
      printWeather(temp, humid, wind, desc);
    }
  }
  Serial.println();
  delay(5000); // wait 5 seconds before next request
}

/*
 * Print weather conditions to serial console.
 * 
 * @param temp : the temperature in fahrenheit
 * @param humid : the humidity percentage
 * @param wind : the windspeed in Mph
 * @param desc : a text description of the weather conditions
 */
void printWeather(const double temp, const double humid, const double wind, const char* desc) {
  Serial.println("=============================================");
  Serial.print("The weather in " + String(CITY_NAME) + " is: ");
  Serial.println(desc);
  Serial.print("  Temp: ");
  Serial.print(temp);
  Serial.println(" F");
  Serial.print("  Humidity: ");
  Serial.print(humid);
  Serial.println(" %");
  Serial.print("  Wind speed: ");
  Serial.print(wind);
  Serial.println(" Mph");
  Serial.println("=============================================");
}

/*
 * Make an Http GET request, using the specified path, query params, and headers
 * 
 * @param path : the path beginning after the base server URI to the desired endpoint.
 * @param params : the desired query parameters. can be an empty string.
 * @param headers : an array of request headers to pass. Each header should correspond to 2 entries in the array: key, followed by value. Can be empty array.
 * @param n : the number of headers in the array. This number should be half the length of the array.
 */
int httpGET(String path, String params, String headers[], int n) {
  client.beginRequest();
  client.get(path + params);
  for(int i = 0; i < (n*2); i += 2) {
    client.sendHeader(headers[i], headers[i+1]);
  }
  client.endRequest();
  return client.responseStatusCode();
}


/**
 * Connect to the WiFi network using the SSID and password defined in arduino_secrets.h. Uses verbose mode.
 */
void wifiConnect() {
  wifiConnect(true);
}

/**
 * Connect to the WiFi network using the SSID and password defined in arduino_secrets.h. Uses verbose mode.
 * 
 * @param useVerbose : specify weather to print to console while attempting to connect to network.
 */
void wifiConnect(bool useVerbose) {
  status = wifi.status();
  while ( status != WL_CONNECTED) { // loop until successfully connected to network.
    if(useVerbose) {
      Serial.print("Attempting to connect to Network named: ");
      Serial.println(ssid);
    }
    status = WiFi.begin(ssid, pass);
  }
}
