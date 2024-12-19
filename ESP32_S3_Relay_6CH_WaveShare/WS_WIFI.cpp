#include "WS_WIFI.h"

const char *ssid = STASSID;                   // Name of the WIFI you want to connect to
const char *password = STAPSK;                // The WIFI password to connect to

WebServer server(80);                         // Declare the WebServer object
char ipStr[16];
bool WIFI_Connection = 0;

void handleRoot() {
  String myhtmlPage =
    String("") +
    "<html>"+
    "<head>"+
    "    <meta charset=\"utf-8\">"+
    "    <title>ESP32 S3 Relay 6CH</title>"+
    "    <style>" +
    "        body {" +
    "            font-family: Arial, sans-serif;" +
    "            background-color: #f0f0f0;" +
    "            margin: 0;" +
    "            padding: 0;" +
    "        }" +
    "        .header {" +
    "            text-align: center;" +
    "            padding: 20px 0;" +
    "            background-color: #333;" +
    "            color: #fff;" +
    "            margin-bottom: 20px;" +
    "        }" +
    "        .container {" +
    "            max-width: 600px;" +
    "            margin: 0 auto;" +
    "            padding: 20px;" +
    "            background-color: #fff;" +
    "            border-radius: 5px;" +
    "            box-shadow: 0 0 5px rgba(0, 0, 0, 0.3);" +
    "        }" +
    "        .input-container {//" +
    "            display: flex;" +
    "            align-items: center;" +
    "            margin-bottom: 10px;" +
    "        }" +
    "        .input-container label {" +
    "            width: 80px;" +  
    "            margin-right: 10px;" +
    "        }" +
    "        .input-container input[type=\"text\"] {" +
    "            flex: 1;" +
    "            padding: 5px;" +
    "            border: 1px solid #ccc;" +
    "            border-radius: 3px;" +
    "            margin-right: 10px; "+
    "        }" +
    "        .input-container button {" +
    "            padding: 5px 10px;" +
    "            background-color: #333;" +
    "            color: #fff;" +
    "            font-size: 14px;" +
    "            font-weight: bold;" +
    "            border: none;" +
    "            border-radius: 3px;" +
    "            text-transform: uppercase;" +
    "            cursor: pointer;" +
    "        }" +
    "        .button-container {" +
    "            margin-top: 20px;" +
    "            text-align: center;" +
    "        }" +
    "        .button-container button {" +
    "            margin: 0 5px;" +
    "            padding: 10px 15px;" +
    "            background-color: #333;" +
    "            color: #fff;" +
    "            font-size: 14px;" +
    "            font-weight: bold;" +
    "            border: none;" +
    "            border-radius: 3px;" +
    "            text-transform: uppercase;" +
    "            cursor: pointer;" +
    "        }" +
    "        .button-container button:hover {" +
    "            background-color: #555;" +
    "        }" +
    "    </style>" +
    "</head>"+
    "<body>"+
    "    <script defer=\"defer\">"+
    "        function ledSwitch(ledNumber) {"+
    "            var xhttp = new XMLHttpRequest();" +
    "            xhttp.onreadystatechange = function() {" +
    "                if (this.readyState == 4 && this.status == 200) {" +
    "                    console.log('LED ' + ledNumber + ' state changed');" +
    "                }" +
    "            };" +
    "            if (ledNumber < 7) {" +
    "             xhttp.open('GET', '/Switch' + ledNumber, true);" +
    "            }" +
    "            else if(ledNumber == 7){" +
    "            xhttp.open('GET', '/AllOn', true);" +
    "            }" +
    "            else if(ledNumber == 8){" +
    "            xhttp.open('GET', '/AllOff', true);" +
    "            }" +
    "            xhttp.send();" +
    "        }" +
    "        function updateData() {"+
    "            var xhr = new XMLHttpRequest();"+
    "            xhr.open('GET', '/getData', true);"+
    // "            displayErrorTextBox(false);"+
    "            xhr.onreadystatechange = function() {"+
    "              if (xhr.readyState === 4 && xhr.status === 200) {"+
    "                var dataArray = JSON.parse(xhr.responseText);"+
    "                document.getElementById('ch1').value = dataArray[0];"+
    "                document.getElementById('ch2').value = dataArray[1];"+
    "                document.getElementById('ch3').value = dataArray[2];"+
    "                document.getElementById('ch4').value = dataArray[3];"+
    "                document.getElementById('ch5').value = dataArray[4];"+
    "                document.getElementById('ch6').value = dataArray[5];"+
    // "                // 移除按钮的 disabled 属性，使其变为可点击状态"+
    "                document.getElementById('btn1').removeAttribute(\'disabled\');"+
    "                document.getElementById('btn2').removeAttribute(\'disabled\');"+
    "                document.getElementById('btn3').removeAttribute(\'disabled\');"+
    "                document.getElementById('btn4').removeAttribute(\'disabled\');"+
    "                document.getElementById('btn5').removeAttribute(\'disabled\');"+
    "                document.getElementById('btn6').removeAttribute(\'disabled\');"+
    "                document.getElementById('btn7').removeAttribute(\'disabled\');"+
    "                document.getElementById('btn8').removeAttribute(\'disabled\');"+
    // "                resetErrorTextBox();"+
    // "                displayErrorTextBox(false);"+
    "              }"+
    "              else if (xhr.readyState === 4 && xhr.status !== 200) {"+
    "                displayErrorTextBox(true);"+
    "              }"+
    "            };"+
    "            xhr.send();"+
    "        }"+
    "        function displayErrorTextBox(show) {"+
    "          var errorTextbox = document.getElementById('errorTextbox');"+
    "          errorTextbox.style.display = show ? 'block' : 'none';"+
    "        }"+
    "        function resetErrorTextBox() {"+
    "          document.getElementById(\'errorTextbox\').value = \'\';"+
    "        }"+
    "        var refreshInterval = 10;"+                                      // Define a variable for timing, 100ms
    "        setInterval(updateData, refreshInterval);"+                       // The updateData function is executed periodically every 100ms
    "    </script>" +
    "</head>"+
    "<body>"+
    "    <div class=\"header\">"+
    "        <h1>ESP32-S3-Relay-6CH</h1>"+
    "    </div>"+
    "    <div class=\"container\">"+
    "        <div class=\"input-container\" style=\"margin-left: 140px;\">"+
    "            <label for=\"input1\">CH1</label>"+
    "            <input type=\"text\" id=\"ch1\" />"+
    "            <button value=\"Switch1\" id=\"btn1\" disabled onclick=\"ledSwitch(1)\">Button 1</button>"+
    "        </div>"+
    "        <div class=\"input-container\" style=\"margin-left: 140px;\">"+
    "            <label for=\"input2\">CH2</label>"+
    "            <input type=\"text\" id=\"ch2\" />"+
    "            <button value=\"Switch2\" id=\"btn2\" disabled onclick=\"ledSwitch(2)\">Button 2</button>"+
    "        </div>"+
    "        <div class=\"input-container\" style=\"margin-left: 140px;\">"+
    "            <label for=\"input3\">CH3</label>"+
    "            <input type=\"text\" id=\"ch3\" />"+
    "            <button value=\"Switch3\" id=\"btn3\" disabled onclick=\"ledSwitch(3)\">Button 3</button>"+
    "        </div>"+
    "        <div class=\"input-container\" style=\"margin-left: 140px;\">"+
    "            <label for=\"input4\">CH4</label>"+
    "            <input type=\"text\" id=\"ch4\" />"+
    "            <button value=\"Switch4\" id=\"btn4\" disabled onclick=\"ledSwitch(4)\">Button 4</button>"+
    "        </div>"+
    "        <div class=\"input-container\" style=\"margin-left: 140px;\">"+
    "            <label for=\"input5\">CH5</label>"+
    "            <input type=\"text\" id=\"ch5\" />"+
    "            <button value=\"Switch5\" id=\"btn5\" disabled onclick=\"ledSwitch(5)\">Button 5</button>"+
    "        </div>"+
    "        <div class=\"input-container\" style=\"margin-left: 140px;\">"+
    "            <label for=\"input6\">CH6</label>"+
    "            <input type=\"text\" id=\"ch6\" />"+
    "            <button value=\"Switch6\" id=\"btn6\" disabled onclick=\"ledSwitch(6)\">Button 6</button>"+
    "        </div>"+
    "        <div class=\"button-container\">"+
    "            <button value=\"AllOn\" id=\"btn7\" disabled onclick=\"ledSwitch(7)\">All On</button>"+
    "            <button value=\"AllOff\" id=\"btn8\" disabled onclick=\"ledSwitch(8)\">All Off</button>"+
    "        </div>"+
    "        <div id=\"errorTextbox\" style=\"display: none;\"> "+     
    "            <p>Please refresh the page</p>"+      
    "            <p>Chinese:请刷新页面</p>"+
    "        </div>"+
    "    </div>"+
    "</body>"+
    "</html>";

  server.send(200, "text/html", myhtmlPage); 
  printf("The user visited the home page\r\n");
}
void handleGetData() {
  String json = "[";
  for (int i = 0; i < sizeof(Relay_Flag) / sizeof(Relay_Flag[0]); i++) {
    json += String(Relay_Flag[i]);
    if (i < sizeof(Relay_Flag) / sizeof(Relay_Flag[0]) - 1) {
      json += ",";
    }
  }
  json += "]";
  server.send(200, "application/json", json);
}
void handleSwitch(int ledNumber) {
  uint8_t Data[1]={0};
  Data[0]=ledNumber+48;
  Relay_Analysis(Data,WIFI_Mode);
  server.send(200, "text/plain", "OK");
}
void handleSwitch1() { handleSwitch(1); }
void handleSwitch2() { handleSwitch(2); }
void handleSwitch3() { handleSwitch(3); }
void handleSwitch4() { handleSwitch(4); }
void handleSwitch5() { handleSwitch(5); }
void handleSwitch6() { handleSwitch(6); }
void handleSwitch7() { handleSwitch(7); }
void handleSwitch8() { handleSwitch(8); }


void WIFI_Init()
{
  uint8_t Count = 0;
  WiFi.mode(WIFI_STA);                                
  WiFi.setSleep(true);                                
  WiFi.begin(ssid, password);                         // Connect to the specified Wi-Fi network
  while (WiFi.status() != WL_CONNECTED)               
  {
    Count++;
    delay(500);
    printf(".\r\n");
    if(Count % 2 == 0 && Count != 0){                 
      RGB_Light(60, 0, 0); 
      delay(1000);
      RGB_Light(0, 0, 0); 
    }
    if(Count % 10 == 0){                               // 10 attempts failed to connect, cancel the connection, try again
      WiFi.disconnect();
      delay(100);
      WiFi.mode(WIFI_OFF);
      delay(100);
      WiFi.mode(WIFI_STA);
      delay(100);
      WiFi.begin(ssid, password);
    }
    if(Count > 22){                                   // connection fail
      break;
    }              
  }
  delay(100);
  if(Count < 23){
    WIFI_Connection = 1;
    RGB_Light(0, 60, 0); 
    delay(1000);
    RGB_Light(0, 0, 0);  
    IPAddress myIP = WiFi.localIP();
    printf("IP Address: ");
    sprintf(ipStr, "%d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
    printf("%s\r\n", ipStr);

    // 注册回调函数
    server.on("/", handleRoot);
    server.on("/getData", handleGetData);
    server.on("/Switch1", handleSwitch1);
    server.on("/Switch2", handleSwitch2);
    server.on("/Switch3", handleSwitch3);
    server.on("/Switch4", handleSwitch4);
    server.on("/Switch5", handleSwitch5);
    server.on("/Switch6", handleSwitch6);
    server.on("/AllOn"  , handleSwitch7);
    server.on("/AllOff" , handleSwitch8);

    server.begin(); //启动服务器
    printf("Web server started\r\n");
  }
  else{
    WIFI_Connection = 0;
    printf("WIFI connection fails, you can use the Bluetooth debugging Assistant to control the device.\r\n");
    RGB_Light(60, 0, 0); 
  }
}

void WIFI_Loop()
{
  if(WIFI_Connection == 1)
    server.handleClient();                        // Processing requests from clients
}
