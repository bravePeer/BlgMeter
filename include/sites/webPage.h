const char webPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <meta name="vieport" content="width=device-width, initial-scale=1.0" />
        <meta charset="utf-8" />
    
        <title>BLG meter</title>

        <script>
            function buttonSleep(){
                const request = new XMLHttpRequest();
                request.onreadystatechange = function() {
                      if(this.status == 200) {
                        document.getElementById("data").innerHTML = this.responseText;
                   }
                };
                request.open("GET", "/sleep", true);
                request.send();
            }

            function buttonAdd() {
                const request = new XMLHttpRequest();
                request.onreadystatechange = function() {
                      if(this.status == 200) {
                        document.getElementById("data").innerHTML = this.responseText;
                   }
                };
                request.open("GET", "/add", true);
                request.send();
            }
            function buttonRead() {
                const request = new XMLHttpRequest();
                request.onreadystatechange = function() {
                      if(this.status == 200) {
                        document.getElementById("data").innerHTML = this.responseText;
                   }
                };
                request.open("GET", "/read", true);
                request.send();
            }
            
            function buttonWrite() {
                const request = new XMLHttpRequest();
                request.onreadystatechange = function() {
                      if(this.status == 200) {
                        document.getElementById("data").innerHTML = this.responseText;
                   }
                };
                request.open("GET", "/write", true);
                request.send();

            }

            function buttonShow() {
                const request = new XMLHttpRequest();
                request.onreadystatechange = function() {
                      if(this.status == 200) {
                        document.getElementById("data").innerHTML = this.responseText;
                   }
                };
                request.open("GET", "/show", true);
                request.send();
            }

            function buttonInit() {
                const request = new XMLHttpRequest();
                request.onreadystatechange = function() {
                      if(this.status == 200) {
                        document.getElementById("date").innerHTML = this.responseText;
                   }
                };
                request.open("GET", "/init", true);
                request.send();
            }

            function buttonMeasure() {
                const request = new XMLHttpRequest();
                request.onreadystatechange = function() {
                      if(this.status == 200) {
                        document.getElementById("data").innerHTML = this.responseText;
                   }
                };
                request.open("GET", "/measure", true);
                request.send();
            }

            function buttonSendToCloud() {
                const request = new XMLHttpRequest();
                request.onreadystatechange = function() {
                      if(this.status == 200) {
                        document.getElementById("data").innerHTML = this.responseText;
                   }
                };
                request.open("GET", "/buttonSendToCloud", true);
                request.send();
            }

            function buttonTest() {
                const request = new XMLHttpRequest();
                request.onreadystatechange = function() {
                      if(this.status == 200) {
                        document.getElementById("data").innerHTML = this.responseText;
                   }
                };
                let minutes = Number(document.getElementById("minuteWaitTime").value);
                let hours = Number(document.getElementById("hourWaitTime").value);
                minutes += hours*60
                
                console.log(minutes);
                request.open("GET", "/test?sleepTime="+minutes, true);
                request.send();
            }

        </script>
    </head>
    <body>
        <p>test</p>
        <input type="button" value="sleep" onclick="buttonSleep()"/>
        <input type="button" value="add" onclick="buttonAdd()"/>
        <input type="button" value="read" onclick="buttonRead()"/>
        <input type="button" value="write" onclick="buttonWrite()"/>
        <input type="button" value="show" onclick="buttonShow()"/>
        <input type="button" value="init" onclick="buttonInit()"/>
        <input type="button" value="measure" onclick="buttonMeasure()"/>
        <br>
        <input type="button" value="send to Google Sheet" onclick="buttonSendToCloud()"/>
        
        <br><br><br>
        
        <div>
            <p>hours:</p>
            <input type="number" id="hourWaitTime" min="0">
            <br>
            <p>minutes: </p>
            <input type="number" id="minuteWaitTime" min="0">
            <input type="button" value="sleep" onclick="buttonSleep()"/>
        </div>
        <br>
        <br>
        <div id="data">yo</div>
    </body>
</html>
)=====";