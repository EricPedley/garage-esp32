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
            flex-direction: column;
        }
        button {
            background-color: lightgrey;
            border: 2px solid black;
            color: black;
            font-size: 24px;
            border-radius: 8px;
            text-align: center;
            width: calc(50vw - 60px);
            height: calc(50% - 60px);
            margin-bottom: 20px;
            user-select: none; /* supported by Chrome and Opera */
            -webkit-user-select: none; /* Safari */
            -khtml-user-select: none; /* Konqueror HTML */
            -moz-user-select: none; /* Firefox */
            -ms-user-select: none; /* Internet Explorer/Edge */
        }
    </style>
</head>
<body>
    <button>Tap/hold to control door</button>
    <button onclick="window.location.href = '/'">Toggle buttons</button>
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
            const button = document.querySelector('button')
            if(event.data === '1') {
                button.style.backgroundColor = 'green';
            } else if(event.data === '0') {
                button.style.backgroundColor = 'lightgrey';
            } else if (['open', 'close', 'opened', 'closed'].includes(event.data)) {
                // do nothing. These are status messages we don't display on this page.
            } else {
                alert(event.data);
            }
        }
        function initButton() {
            const button = document.querySelector('button')
            button.ontouchstart = ()=>{
                if (ws.readyState !== ws.OPEN) {
                    initWebSocket();
                }
                button.style.backgroundColor = 'yellow';
                ws.send('on');
            }
            button.ontouchend = ()=>{
                button.style.backgroundColor = 'orange';
                ws.send('off');
            }
        }
    </script>
</body>
</html>
)rawliteral";