#include "web_pages_status.h"

const char APPLYUSERPASS_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Settings Updated</title>
<style>
body { background: #121212; color: #e0e0e0; font-family: sans-serif; text-align: center; padding: 40px; }
h2 { color: #2ea44f; }
</style>
<script>setTimeout(function(){ window.location.href = 'deviceconfig'; }, 5000);</script>
</head>
<body>
<h2>Credentials Updated!</h2>
<p>Redirecting in 5 seconds...</p>
</body>
</html>
)=====";

const char APPLYMQTTCONFIG_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Settings Updated</title>
<style>
body { background: #121212; color: #e0e0e0; font-family: sans-serif; text-align: center; padding: 40px; }
h2 { color: #2ea44f; }
</style>
<script>setTimeout(function(){ window.location.href = 'mqttconfig'; }, 5000);</script>
</head>
<body>
<h2>MQTT Config Updated!</h2>
<p>Redirecting in 5 seconds...</p>
</body>
</html>
)=====";

const char SUBMITUPDATEFAIL_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Update Failed</title>
<style>
body { background: #121212; color: #e0e0e0; font-family: sans-serif; text-align: center; padding: 40px; }
h2 { color: #da3633; }
</style>
<script>setTimeout(function(){ window.location.href = 'system'; }, 5000);</script>
</head>
<body>
<h2>Update Failed!</h2>
<p>Redirecting in 5 seconds...</p>
</body>
</html>
)=====";

const char SUBMITUPDATEOK_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Update OK</title>
<style>
body { background: #121212; color: #e0e0e0; font-family: sans-serif; text-align: center; padding: 40px; }
h2 { color: #2ea44f; }
</style>
<script>setTimeout(function(){ window.location.href = 'system'; }, 5000);</script>
</head>
<body>
<h2>Update OK!</h2>
<p>Redirecting in 5 seconds...</p>
</body>
</html>
)=====";

const char SUBMITUPDATESUCCESS_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Update Success</title>
<style>
body { background: #121212; color: #e0e0e0; font-family: sans-serif; text-align: center; padding: 40px; }
h2 { color: #2ea44f; }
</style>
<script>setTimeout(function(){ window.location.href = '/'; }, 30000);</script>
</head>
<body>
<h2>Update Successful!</h2>
<p>Device Rebooting Now...</p>
<p>Redirecting in 30 seconds.</p>
</body>
</html>
)=====";

const char REBOOT_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Rebooting</title>
<style>
body { background: #121212; color: #e0e0e0; font-family: sans-serif; text-align: center; padding: 40px; }
h2 { color: #dda41d; }
</style>
<script>setTimeout(function(){ window.location.href = '/'; }, 20000);</script>
</head>
<body>
<h2>Device Rebooting...</h2>
<p>Redirecting in 20 seconds.</p>
</body>
</html>
)=====";

const char FACTORYRESET_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Factory Reset</title>
<style>
body { background: #121212; color: #e0e0e0; font-family: sans-serif; text-align: center; padding: 40px; }
h2 { color: #da3633; }
</style>
</head>
<body>
<h2>Factory Reset Complete</h2>
<p>Please reconnect via the WiFi portal to setup the device.</p>
<p>Device Rebooting...</p>
</body>
</html>
)=====";
