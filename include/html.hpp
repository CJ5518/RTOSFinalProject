#ifndef HTML_HPP
#define HTML_HPP

//This file contains various files for the web server, not just html but also css and js
//Doing this was so much easier than SPIFFS what can I say

#include <Arduino.h>

const char index_html[] PROGMEM = "<!DOCTYPE html>\n\n<html>\n\t<head>\n\t\t<title>\n\t\t\tESP 32\n\t\t</title>\n\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\t\t<link rel=\"stylesheet\" href=\"styles.css\">\n\t</head>\n\t<body>\n\t\t<h1 class=\"center\">\n\t\t\tStatus: Inactive\n\t\t\t<br>\n\t\t\tMost of this code is copied (all my own work) from my capstone project\n\t\t</h1>\n\t\t<div class=\"center\"> \n\t\t\t<button class=\"mainButton\" onclick=\"location.href='./routineManager.html'\">\n\t\t\t\tRoutine Manager\n\t\t\t</button>\n\t\t</div>\n\t\t<div class=\"center\"> \n\t\t\t<button class=\"mainButton\" onclick=\"location.href='./graphViewer.html'\">\n\t\t\t\tGraph Viewer\n\t\t\t</button>\n\t\t</div>\n\t\t<div class=\"center\" onclick=\"location.href='./networkSettings.html'\"> \n\t\t\t<button class=\"mainButton\">\n\t\t\t\tNetwork Settings\n\t\t\t</button>\n\t\t</div>\n\t\t<div class=\"center\"> \n\t\t\t<button class=\"mainButton\">\n\t\t\t\tGeneral Settings\n\t\t\t</button>\n\t\t</div>\n\t\t<div class=\"center\"> \n\t\t\t<button class=\"mainButton\">\n\t\t\t\tHelp\n\t\t\t</button>\n\t\t</div>\n\t</body>\n</html>\n";
const char styles_css[] PROGMEM = "\n.center {\n\tmargin: auto;\n\twidth: 90%;\n\tpadding: 10px;\n\ttext-align: center;\n}\n\n.mainButton {\n\twidth: 60%;\n\tfont-size: 22px;\n}\n\nul.no-bullets {\n\tlist-style-type: none; /* Remove bullets */\n\tpadding: 0; /* Remove padding */\n\tmargin: 0; /* Remove margins */\n}\n\n.routineManagerInput {\n\twidth: 18%;\n\tfont-size: 22px;\n}\n.routineManagerInputSmaller {\n\twidth: 10%;\n\tfont-size: 22px;\n}\n\n.routineManagerEntry {\n\twidth: 95%;\n\tmargin: auto;\n\tborder: 2px;\n\tmax-width: 800px;\n\tmargin-bottom: 10px;\n}\n";

#endif

