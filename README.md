# Ghost Message 2

GhostMessage(2) is an chat-server + LED announcement board. Open the webpage, or connect to the websocket, send a message and it magically appears on your LED Matrix along with a Pacman Ghost for fun. 

## Why did you make this?
A few years ago as a fun side project, I made a project called GhostMessage. It was a NodeJS server that offered chat functionality and sent a message to a board. It could be run on a desktop or laptop computer, or something like a RaspberryPi. It was super fun, but it always kind of bothered me that I had to host the server on a computer. I wanted the project to be self-contained. In the intervening years, the library support for doing cool stuff on the ESP8266 has really expanded. So it was much easier to put the web page server, web socket server, and LED driver components all into the firmware running on the 8266. On top of that, the ESP8266 easily can act as an access point or a Wifi Client, so you can have a captive portal setup to initialize for your wifi network.

## How do I setup my own?
To run the chat server, you can simply install it on an ESP8266 and run it. While that's kind of fun, it's not as though chat is particularly novel. The real fun comes with wiring up an LED matrix that can show the messages as they come in. 

## Is it only for Chat?
No! You don't have to use the chat server at all. You can just connect to the websocket and send messages. They will display on the LED panel just the same. You can use it for chat, some sort of notification, or status indicators. Whatever kind of text data you want to display (so far). Just connect to the WebSocket Server and send a message in the following JSON format:
```
{
    "message":"I am a Message!",
    "color":"cyan" 
}
```
Valid colors are `'red'`,`'pink'`,`'orange'`,`'cyan'`, and `'black'` - with last one just providing a pair of eyes. At some point, I'll add the "blue, scared mode" for the ghost - though I may reserve that for errors with the server.


## Progess
This isn't done yet. I have more features I need to implement to get it to parity with GhostMessage v1. 
 - [x] Messages fetched and displayed by Ghost
 - [x] Ghost Color
 - [x] WebSocket server
 - [ ] Move Chat website from GhostMessage v1
 - [ ] Move Captured Portal to Async Web Server
 - [ ] Message Queue

 Nice to haves: 
 - [ ] User-definable colors
 - [ ] User-definable bitmaps?
 - [ ] Means to tie into Service notifications?

