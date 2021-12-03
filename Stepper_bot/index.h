const char MAIN_page[] PROGMEM = R"=====(

<!DOCTYPE html>
<html>
<head>
<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />
<meta name='viewport' content='width=device-width, initial-scale=1.0' />
<title>Stepper Bot</title>
<style>
body, html {
  background: #ECF0F1;
  text-align: center;
  position: fixed;
  top: 0px;
  left: 0px;
  width: 100%;
  height: 100%;
}
.buttons {
  text-align: center;
  margin-left: auto;
  margin-right: auto;
}

button {
  text-decoration: none;
  color: white;
  width: 180px;
  height: 140px;
  background: #949494;
  position: relative;
  top: 20px;
  margin: 60px;
  font-size: 80px;
  border-radius: 60px;
  box-shadow: 0px 15px 0px 0px #696969, 0px 0px 20px 0px #bbb;
  transition: all 0.2s;
}

button:active {
  top: 30px;
  box-shadow: 0px 7px 0px 0px #696969;
}
.container{
  margin: 30px;
  border: 1px solid black;
}
.chart div {
  font: 30px sans-serif;
  text-align: right;
  padding: 5px;
  margin: 10px;
  margin-left: 30px;
  color: white;
}
</style>
</head>
<body onload="myFunction()">
  <div class="container">
    <h1>VL53L0X Distance Values<h1>
    <div class="chart">
      <div id="v1">0</div>
      <div id="v2">0</div>
      <div id="v3">0</div>
      <div id="v4">0</div>
    </div>
  </div>
  <div style="height: 200px;"></div>
  <div class="buttons">
    <button onclick="front()">&#9650</button>
    <br>
    <button onclick="left()">&#9664</button>
    <button onclick="stop()">&#9673</button>
    <button onclick="right()">&#9654</button>
    <br>
    <button onclick="back()">&#9660</button>
  </div>
</body>
<script>
function front(){
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "stepper?state=front", true);
    xhttp.send();
}
function left(){
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "stepper?state=left", true);
    xhttp.send();
}
function right(){
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "stepper?state=right", true);
    xhttp.send();
}
function back(){
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "stepper?state=back", true);
    xhttp.send();
}
function stop(){
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "stepper?state=stop", true);
    xhttp.send();
}
function myFunction(){
  url = "http://"+window.location.hostname+"/distance";
  fetch(url, {mode: "no-cors"})
    .then(
      function(response) {
        if (response.status !== 200) {
          return;
        }
        response.json().then(function(data) {
          //console.log(data); 
          var minDistance = 100;
          var maxDistance = 1200;
          if(data.v1 > maxDistance) data.v1 = maxDistance;
            document.getElementById("v1").innerHTML = data.v1;
            document.getElementById("v1").style.width = data.v1 + "px";
            if(data.v1 < minDistance) document.getElementById("v1").style.backgroundColor = "red";
            else document.getElementById("v1").style.backgroundColor = "green";

          if(data.v2 > maxDistance) data.v2 = maxDistance;
            document.getElementById("v2").innerHTML = data.v2;
            document.getElementById("v2").style.width = data.v2 + "px";
            if(data.v2 < minDistance) document.getElementById("v2").style.backgroundColor = "red";
            else document.getElementById("v2").style.backgroundColor = "green";

          if(data.v3 > maxDistance) data.v3 = maxDistance;
            document.getElementById("v3").innerHTML = data.v3;
            document.getElementById("v3").style.width = data.v3 + "px";
            if(data.v3 < minDistance) document.getElementById("v3").style.backgroundColor = "red";
            else document.getElementById("v3").style.backgroundColor = "green";

          if(data.v4 > maxDistance) data.v4 = maxDistance;
            document.getElementById("v4").innerHTML = data.v4;
            document.getElementById("v4").style.width = data.v4 + "px";
            if(data.v4 < minDistance) document.getElementById("v4").style.backgroundColor = "red";
            else document.getElementById("v4").style.backgroundColor = "green";

        });
      }
    )
  setTimeout(myFunction, 1000);
}
</script>
</html> 

)=====";
