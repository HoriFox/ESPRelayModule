#pragma once
#ifndef INDEX_H
#define INDEX_H

const char MAIN_page[] PROGMEM = R"=====(
<html>
    <head>
    <title>Control page</title>
    </head>
<body>
    <style>
      .ibutton {
        display: table;
        background-color: #afafaf;
        outline: none;
        text-decoration: none;
        border-radius: 5px;
        padding: 5px;
        cursor: pointer;
        color: white;
      }
      .ibutton-off {
        background-color: #e25d5d;
      }
      .ibutton-on {
        background-color: #67d06d;
      }
    </style>
    <script>
      function SendRequest(requestValue) {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', requestValue, false);
        xhr.send();
      }
    </script>
    <h2>Relay page</h2>
    <h4>Control</h4>
    <div class="ibutton ibutton-off" style="display: inline-block;" onclick="SendRequest('/relay?value=0')">Switch off</div>
    <div class="ibutton ibutton-on" style="display: inline-block;" onclick="SendRequest('/relay?value=1')">Switch on</div>
    <h4>Service</h4>
    <div class="ibutton" onclick="SendRequest('/restart')">Restart</div>
    <h4>Links</h4>
    <div><a href="/metrics">Metrics page</a></div>
</body>
</html>
)=====";

#endif // INDEX_H
