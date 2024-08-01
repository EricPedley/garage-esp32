#pragma once
#include <Arduino.h>
const char index_html[] PROGMEM = R"rawliteral(
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
            width: calc(33vw - 60px);
            height: calc(33% - 60px);
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
    <button id = 'open'>Open</button>
    <button id = 'close'>Close</button>
    <button onclick="window.location.href = '/override'">Manual Control</button>
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
            const openButton = document.querySelector('#open');
            const closeButton = document.querySelector('#close');
            if(event.data === 'close') {
                closeButton.style.backgroundColor = 'green';
            } else if(event.data === 'open') {
                openButton.style.backgroundColor = 'green';
            } else if(event.data === 'opened') {
                openButton.style.backgroundColor = 'gray'; 
                closeButton.style.backgroundColor = 'lightgrey'; 
            } else if(event.data === 'closed') {
                closeButton.style.backgroundColor = 'gray'; 
                openButton.style.backgroundColor = 'lightgrey'; 
            } else {
                alert(event.data);
            }
        }
        function initButton() {
            const openButton = document.querySelector('#open');
            const closeButton = document.querySelector('#close');
            openButton.onclick = ()=>{
                if (ws.readyState !== ws.OPEN) {
                    initWebSocket();
                }
                openButton.style.backgroundColor = 'yellow';
                ws.send('open');
            }
            closeButton.onclick = ()=>{
                if (ws.readyState !== ws.OPEN) {
                    initWebSocket();
                }
                closeButton.style.backgroundColor = 'yellow';
                ws.send('close');
            }
        }
    </script>
</body>
</html>
)rawliteral";