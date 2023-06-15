const char configureSite[] PROGMEM = R"=====(
<!DOCTYPE html>
<html                      >
    <head>
        <meta name="vieport" content="width=device-width, initial-scale=1.0" />
        <meta charset="utf-8" />
    
        <title>BLG meter</title>
<!--background-color: cyan;-->
        <style>
            .smallbox{
                
                text-align: center;
                padding: 20px;
                margin: 5px;
                flex-basis: content;
            }
            .container{
                display: flex;
            }
        </style>

        <script>
            function formatParams( params ){
                if(params == null)
                    return ""
                return "?" + Object
                .keys(params)
                .map(function(key){
                return key+"="+encodeURIComponent(params[key])
                })
                .join("&")
            }

            function sendRequest(url, responseId, params){
                const request = new XMLHttpRequest();
                request.onreadystatechange = function() {
                      if(this.status == 200) {
                        document.getElementById(responseId).innerHTML = this.responseText;
                   }
                };
                request.open("GET", "/" + url + formatParams(params), true);
                request.send();
            }

            function saveWiFiConfig() {
                let params = {
                    ssid:"",
                    pass:""
                }
                params.ssid = document.getElementById("WiFi_SSID").value
                params.pass = document.getElementById("WiFi_PASS").value
                console.log(params);

                sendRequest("saveWiFiConfig", "responseWiFiConfig", params)
            }
            function readWiFiConfig(){
                sendRequest("readWiFiConfig","responseWiFiConfig");
            }

            function saveServerConfig() {
                let params = {
                    host:"",
                    url:""
                }
                params.host = document.getElementById("HOST").value
                params.url = document.getElementById("URL").value
                console.log(params);

                sendRequest("saveServerConfig", "responseServerConfig", params)
            }

            function readServerConfig() {
                sendRequest("readServerConfig","responseServerConfig");
            }

            function saveSleepConfig(){
                sendRequest("saveSleepConfig", "responseSleepConfig")
            }
            function startWork(){
                let params = {
                    sleepTime:"6"
                }
                sendRequest("startWork", "startWorkResponse", params)
            }

            /*Testing*/
            function testWriteRTC(){
                sendRequest("testWriteRTC", "testRTCResponse")
            }
            function testReadRTC(){
                sendRequest("testReadRTC", "testRTCResponse")
            }
            function testCalcVoltage(){
                sendRequest("testCalcVoltage", "testCalculationsResponse")
            }
            function testCalcBLG(){
                sendRequest("testCalcBLG", "testCalculationsResponse")
            }
            function testCalcAll(){
                sendRequest("testCalcAll", "testCalculationsResponse")
            }
            function testServerSend(){
                sendRequest("testServerSend", "testServerResponse")
            }
            function testSleeping(){
                sendRequest("testSleeping", "testSleepingResponse")
            }

        </script>
    </head>
    <body>
    <div class="container">
       <div class="smallbox">
        <p>Configure WiFi</p>
        <input type="text" value="WiFi_SSID" id="WiFi_SSID"/>
        <br>
        <input type="text" value="WiFi_PASS" id="WiFi_PASS"/>
        <br>
        
        <input type="button" value="save" onclick="saveWiFiConfig()"/>
        <input type="button" value="read" onclick="readWiFiConfig()"/>
        <div id="responseWiFiConfig"></div>
       </div>

       <div class="smallbox">
        <p>Configure Google Sheets (Server)</p>
        <input type="text" value="example.com" id="HOST">
        <br>
        <input type="text" value="/something" id="URL">
        <br>
        <input type="button" value="save" onclick="saveServerConfig()"/>
        <input type="button" value="read" onclick="readServerConfig()"/>
        <br>
        <div id="responseServerConfig"></div>
       </div>

        <div  class="smallbox">
            <p>Configure <br> sleep interval</p>
            <input style="width: 35px; text-align: center;" type="number" value="6" id="SLEEP_INTERVAL">
            <br>
            <input type="button" value="save" onclick="saveSleepConfig()"/>
            <div id="responseSleepConfig"></div>
        </div>

        <div  class="smallbox">
            <input type="button" value="start work" onclick="startWork()">
            <div id="startWorkResponse"></div>
        </div>
    </div>

    <div class="container">
        <div class="smallbox">
            <p>Test RTC</p>
            <input type="button" value="write" onclick="testWriteRTC()">
            <input type="button" value="read" onclick="testReadRTC()">
            <div id="readedRTC">readed RTC</div>
            <div id="testRTCResponse"></div>
        </div>

        <div class="smallbox">
            <p>Test calculations</p>
            <input type="button" value="calc voltage" onclick="testCalcVoltage()">
            <input type="button" value="calc BLG" onclick="testCalcBLG()">
            <input type="button" value="calc all" onclick="testCalcAll()">
            <div id="calculatedBLG">calculated BLG</div>
            <div id="calculatedVoltage">calculated voltage</div>
            <div id="testCalculationsResponse"></div>
        </div>

        <div class="smallbox">
            <p>Test server</p>
            <input type="button" value="send" onclick="testServerSend()">
            <div id="testServerResponse"></div>
        </div>

        <div class="smallbox">
            <p>Test sleeping</p>
            <input type="number" value="1">
            <input type="button" value="sleep" onclick="testSleeping()">
            <div id="testSleepingResponse"></div>
        </div>
    </div>

    </body>
</html>
)=====";