# arduino-API-consumer
An simple Arduino sketch written to practice consuming API's using the ArduinoHttpClient library.

## Functionality
This sketch connects to a wifi network, and then proceeds to make an Http GET request for the current weather data to the API endpoint. If the request fails, the sketch will print the Http status code, and in the case that the request failed due to loss of WiFi connection, will attempt to reconnect to WiFi.

If the request succeeds, the sketch parses the JSON response, and prints a few select items of the response to the serial console. If parsing fails, the sketch will notify the user with an error message.

## Sketch Highlights:
```C++
int HttpGET(String path, String params, String headers[], int n)
```
* An abstracted method to simplify Http GET requests. This method makes a GET request to the HttpClient's target server, using the parameters specified below, and returns the Http response code.
* Parameters:
  - path : the path from the base server URI to the request endpoint. eg: `"/path/to/api/endpoint"`
  - params : a string containing the desired query parameters: eg: `"?param1=arg1&param2=arg2"`
  - headers : an array of string pairs, where the first string is the header key, and the second string is the header value.
  - n : the number of headers in the array. This number is half the number of elements in the array.

## 3rd Party Library and API Documentation
### Libraries:
- [ArduinoHttpClient](https://github.com/arduino-libraries/ArduinoHttpClient)
- [ArduinoJson (v6)](https://arduinojson.org/v6/doc/)
- [WiFiNINA](https://www.arduino.cc/en/Reference/WiFiNINA)
### APIs:
- [Weather Unlocked](https://developer.weatherunlocked.com/documentation/localweather)

