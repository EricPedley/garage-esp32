#pragma once
#include <Arduino.h>
const char override_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <link rel='icon' href='data:,'>
    <style>
        html, body {
            width: 100%;
            height: 100%;
            margin: 0;
            display: flex;
            align-items: center;
            justify-content: center;
            background-color: lightgrey;
        }
        button {
            background-color: lightgrey;
            border: 2px solid black;
            color: black;
            font-size: 24px;
            border-radius: 8px;
            text-align: center;
            width: calc(100vw - 60px);
            height: calc(100% - 60px);
            user-select: none; /* supported by Chrome and Opera */
            -webkit-user-select: none; /* Safari */
            -khtml-user-select: none; /* Konqueror HTML */
            -moz-user-select: none; /* Firefox */
            -ms-user-select: none; /* Internet Explorer/Edge */
        }
    </style>
</head>
<body>
    <button>Tap to control door</button>
    <script>
        var ws;
        window.addEventListener('load', ()=>{
            initWebSocket();
            initButton();
        });
        function initWebSocket() {
            console.log('Trying to open a WebSocket connection...');
            ws = new WebSocket(`ws://${window.location.hostname}/ws`);
            ws.onopen = onOpen;
            ws.onclose = onClose;
            ws.onmessage = onMessage;
        }
        function onOpen(event) {
            console.log('Connection opened');
        }
        function onClose(event) {
            console.log('Connection closed');
            setTimeout(initWebSocket, 2000);
        }
        function onMessage(event) {
            document.body.style.backgroundColor = event.data === '1' ? 'green' : 'lightgrey';
        }
        function initButton() {
            const button = document.querySelector('button')
            button.ontouchstart = ()=>{
                if (ws.readyState !== ws.OPEN) {
                    initWebSocket();
                }
                button.style.backgroundColor = 'green';
                ws.send('on');
            }
            button.ontouchend = ()=>{
                button.style.backgroundColor = 'lightgrey';
                ws.send('off');
            }
        }
    </script>
</body>
</html>
)rawliteral";