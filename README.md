html5-browser-editor
====================

HTML5 is cool, right? Websocket is cool? Linux is super cool? What about combining all of them together then? Yes, this is a personal "toy" project for fun by myself trying to combine these super cool and fun stuff together!

## Overview
I design this project as a B/S one which even involves inter-process communication (GTK part). The intension here is actually to play around HTML5, Websockets, Linux processes (e.g., IPC). The inital skeleton version contains these components: 

1. A light-weighted server (libwebsockets) running in the backgrould waiting for the commands from HTML5
2. HTML5 page as the UI and JavaScript sends commands (open file, save file) to the service
3. Another process which interacts with the user (GTK select file), reading/writing data from/into local file

## Techniques/Programming Languages involved 
HTML5, Websockets, Linux Sockets, IPC, GTK, JavaScript, C/C++

## How to build and run the test

1. go to the /src folder and run make
2. run the lws-server
3. open the index.html test page and click open file button
4. edit the content in the txtarea of the webpage and clisk save file button to save file

## Screenshots

some of the screenshots are:

The front end web page looks like the following

![alt tag](https://github.com/yongminyan/html5-browser-editor/blob/master/screenshots.d/mainPageUI.png)

After you open the testcase1.txt file, the content of that file will be loaded into the textarea as follow

![alt tag](https://github.com/yongminyan/html5-browser-editor/blob/master/screenshots.d/openFile.png)


## Future Work

Some future work in my mind:

1. Add threads into the backend
2. remove the sockets, try other IPC (pipes, shared memeory, .etc) 
3. refactoring the ugly and hard coded parts of the code
