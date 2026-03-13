#include "web_pages_config.h"

const char DEVICE_CONFIG_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Device Config</title>
<style>
:root { --bg: #121212; --card: #1e1e1e; --text: #e0e0e0; --accent: #2ea44f; --input: #2d2d2d; --border: #404040; --subtext: #888; --warning: #d29922; }
body { background: var(--bg); color: var(--text); font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; margin: 0; padding: 20px; text-align: center; height: 100vh; box-sizing: border-box; display: flex; flex-direction: column; overflow: hidden; }
.container { max-width: 650px; margin: 0 auto; background: var(--card); padding: 25px 25px 10px 25px; border-radius: 12px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); display: flex; flex-direction: column; max-height: 100%; box-sizing: border-box; width: 100%; }
.main-scroll-content { flex: 1; overflow-y: auto; padding: 5px 15px 5px 5px; box-sizing: border-box; }
.main-scroll-content::-webkit-scrollbar { width: 6px; }
.main-scroll-content::-webkit-scrollbar-track { background: rgba(255, 255, 255, 0.05); border-radius: 3px; }
.main-scroll-content::-webkit-scrollbar-thumb { background: rgba(255, 255, 255, 0.2); border-radius: 3px; }
.app-header { margin-bottom: 25px; border-bottom: 1px solid var(--border); padding-bottom: 15px; }
h1 { font-size: 1.8rem; margin: 5px 0; color: #fff; }
.hostname-display { font-size: 0.9rem; color: var(--subtext); background: rgba(255,255,255,0.05); display: inline-block; padding: 4px 12px; border-radius: 12px; margin-top: 5px; }
.hostname-display span { color: #fff; font-weight: 500; }
label { display: block; text-align: left; margin: 10px 0 5px; font-weight: 500; font-size: 0.9rem; }
input[type=text], input[type=password], input[type=number], input[type=time] { width: 100%; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; box-sizing: border-box; font-size: 1rem; color-scheme: dark; }
input[type=submit] { background: var(--accent); color: white; border: none; padding: 12px; border-radius: 6px; font-size: 1rem; font-weight: 600; cursor: pointer; width: 100%; margin-top: 15px; transition: opacity 0.2s; }
input[type=submit]:hover { opacity: 0.9; }
.section-card { background: #252525; border: 1px solid var(--border); border-radius: 8px; padding: 15px; margin: 15px 0; }
.section-title { font-size: 0.85rem; color: var(--subtext); text-transform: uppercase; letter-spacing: 1px; margin: 0 0 10px 0; text-align: left; font-weight: 600; }
.section-card label:first-of-type { margin-top: 0; }
.toggle-card { display: flex; align-items: center; justify-content: space-between; background: #252525; padding: 15px; border-radius: 8px; margin-bottom: 15px; border: 1px solid var(--border); }
.toggle-card .label-text { font-weight: 500; font-size: 1rem; }
.switch { position: relative; display: inline-block; width: 50px; height: 26px; flex-shrink: 0; }
.switch input[type=checkbox] { opacity: 0; width: 100%; height: 100%; position: absolute; z-index: 999; cursor: pointer; left: 0; top: 0; margin: 0; }
.slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #404040; transition: .4s; border-radius: 34px; z-index: 1; }
.slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 3px; bottom: 3px; background-color: white; transition: .4s; border-radius: 50%; box-shadow: 0 2px 4px rgba(0,0,0,0.2); }
input:checked ~ .slider { background-color: var(--accent); }
input:focus ~ .slider { box-shadow: 0 0 1px var(--accent); }
input:checked ~ .slider:before { transform: translateX(24px); }
.slider-row { display: flex; align-items: center; gap: 15px; margin-bottom: 15px; }
.seg-control { display: flex; background: #1a1a1a; border-radius: 8px; padding: 3px; gap: 2px; }
.seg-btn { border: none; background: transparent; color: var(--subtext); padding: 6px 13px; border-radius: 6px; cursor: pointer; font-size: 0.9rem; font-weight: 500; transition: background 0.2s, color 0.2s; white-space: nowrap; }
.seg-btn.active { background: var(--accent); color: #fff; }
.seg-btn:disabled { cursor: not-allowed; }
input[type=range] { accent-color: var(--accent); cursor: pointer; }

.toast { visibility: hidden; min-width: 200px; margin-left: -100px; background-color: #333; color: #fff; text-align: center; border-radius: 6px; padding: 12px 16px; position: fixed; z-index: 20000; left: 50%; bottom: 30px; font-size: 0.9rem; }
.toast.show { visibility: visible; animation: fadein 0.3s, fadeout 0.3s 2.7s; }
.toast.success { background-color: var(--accent); }
.toast.error { background-color: #da3633; }
@keyframes fadein { from { bottom: 0; opacity: 0; } to { bottom: 30px; opacity: 1; } }
@keyframes fadeout { from { bottom: 30px; opacity: 1; } to { bottom: 0; opacity: 0; } }
/* Modal */
.modal-overlay { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.8); z-index: 1000; align-items: center; justify-content: center; }
.modal-overlay.show { display: flex; }
.modal { background: var(--card); border: 1px solid var(--border); border-radius: 12px; padding: 25px; max-width: 400px; width: 90%; text-align: center; display: flex; flex-direction: column; max-height: 90vh; box-sizing: border-box; }
.modal-scroll-content { flex: 1; overflow-y: auto; margin-bottom: 20px; padding: 5px 15px 5px 5px; text-align: left; box-sizing: border-box; width: 100%; }
.modal-scroll-content::-webkit-scrollbar { width: 6px; }
.modal-scroll-content::-webkit-scrollbar-track { background: rgba(255, 255, 255, 0.05); border-radius: 3px; }
.modal-scroll-content::-webkit-scrollbar-thumb { background: rgba(255, 255, 255, 0.2); border-radius: 3px; }
.modal h3 { margin: 0 0 10px 0; color: #fff; }
.modal p { color: var(--subtext); margin: 0 0 20px 0; font-size: 0.9rem; }
.modal-actions { display: grid; gap: 10px; }
.btn { background: #333; border: 1px solid var(--border); color: #aaa; padding: 10px 16px; border-radius: 6px; cursor: pointer; font-size: 0.9rem; transition: all 0.2s; }
.btn:hover { background: #3d3d3d; color: #fff; }
.btn-confirm { background: var(--accent); border: 1px solid var(--accent); color: white; }
.btn-confirm:hover { background: #279b42; }

/* Config Button Styles */
.config-btn { width: 100%; margin: 15px 0; padding: 15px; background: #2c2c2c; border: 1px solid var(--border); color: #fff; border-radius: 8px; cursor: pointer; display: flex; align-items: center; justify-content: center; gap: 10px; font-size: 1rem; transition: all 0.2s; }
.config-btn:hover { background: #353535; border-color: #505050; transform: translateY(-1px); box-shadow: 0 2px 8px rgba(0,0,0,0.3); }

/* Navigation Tabs */
.nav-tabs { display: flex; justify-content: center; gap: 4px; margin: 0 0 20px; flex-shrink: 0; }
.nav-tabs a { flex: 1; display: flex; align-items: center; justify-content: center; color: #888; text-decoration: none; padding: 12px 20px; background: #252525; border-radius: 8px; font-size: 0.85rem; font-weight: 600; transition: all 0.3s ease; text-transform: uppercase; letter-spacing: 0.5px; }
.nav-tabs a:hover { color: #fff; background: #2d2d2d; }
.nav-tabs a.active { color: #fff; background: var(--accent); box-shadow: 0 2px 8px rgba(46, 164, 79, 0.3); }
.footer-separator { height: 1px; background: #fff; opacity: 0.1; margin: 20px 0 10px; }
.footer-app-title { color: var(--accent); font-size: 0.8rem; letter-spacing: 1px; text-transform: uppercase; font-weight: 700; margin-bottom: 5px; }
.info { font-size: 0.8rem; color: #666; width: 100%; text-align: center; }
.info span { color: #888; }
.not-supported-text { color: var(--warning); font-size: 0.85rem; font-weight: 500; }
</style>
<script>
function showToast(msg, type) {
  var t = document.getElementById('toast');
  t.innerText = msg;
  t.className = 'toast ' + type + ' show';
  setTimeout(function(){ t.className = 'toast'; }, 3000);
}

// Global variable for max alarms
var maxAlarms = 10; // Default fallback

function getData() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var parser = new DOMParser();
      var xmlDoc = parser.parseFromString(this.responseText, 'text/xml');
      var currentHostname = xmlDoc.getElementsByTagName('clientid')[0].childNodes[0].nodeValue;
      document.getElementById('clientid_val').innerText = currentHostname;
      document.getElementById('version_val').innerText = xmlDoc.getElementsByTagName('version')[0].childNodes[0].nodeValue;
      if(xmlDoc.getElementsByTagName('maxdevices').length > 0) {
        document.getElementById('maxdevices_val').innerText = xmlDoc.getElementsByTagName('maxdevices')[0].childNodes[0].nodeValue;
      }
      // Parse Max Alarms
      if(xmlDoc.getElementsByTagName('maxalarms').length > 0) {
        maxAlarms = parseInt(xmlDoc.getElementsByTagName('maxalarms')[0].childNodes[0].nodeValue) || 10;
        console.log("Max Alarms set to: " + maxAlarms);
      }
    }
  };
  request.open('GET', 'changecredvars', true);
  request.send();
}

// Platform detection and feature availability
function checkPlatformFeatures() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var parser = new DOMParser();
      var xmlDoc = parser.parseFromString(this.responseText, 'text/xml');
      var timerFeature = xmlDoc.getElementsByTagName('timerfeature').length > 0 ? 
                     xmlDoc.getElementsByTagName('timerfeature')[0].childNodes[0].nodeValue : 'false';
      
      // Handle Timer Settings button
      var timerBtn = document.getElementById('TIMER_SETTINGS_BTN');
      var timerIcon = document.getElementById('TIMER_NOT_SUPPORTED_ICON');
      
      if (timerFeature !== 'true') {
        timerBtn.style.opacity = '0.5';
        timerBtn.style.cursor = 'not-allowed';
        // Allow pointer events so tooltip works, but disable click
        timerBtn.style.pointerEvents = 'auto';
        timerBtn.title = 'Feature Disabled via build flag (DISABLE_TIMER_FEATURE)';
        timerBtn.onclick = function(e) { e.preventDefault(); return false; };

        // Center content for disabled state
        timerBtn.style.justifyContent = 'center';
        if (timerIcon) {
          timerIcon.style.display = 'block';
          timerIcon.style.marginLeft = '8px'; // Add small gap instead of auto margin
        }
      } else {
        // Ensure enabled state
        timerBtn.style.opacity = '1';
        timerBtn.style.cursor = 'pointer';
        timerBtn.style.pointerEvents = 'auto';
        timerBtn.title = '';
        timerBtn.onclick = function() { openTimerModal(); }; // Restore original click handler

        if (timerIcon) timerIcon.style.display = 'none';
      }

      // Handle Sleep Mode Settings button
      var sleepModeFeature = xmlDoc.getElementsByTagName('sleepmodefeature').length > 0 ?
                             xmlDoc.getElementsByTagName('sleepmodefeature')[0].childNodes[0].nodeValue : 'false';

      var sleepModeBtn = document.getElementById('SLEEP_MODE_SETTINGS_BTN');
      var sleepModeIcon = document.getElementById('SLEEP_MODE_NOT_SUPPORTED_ICON');

      if (sleepModeFeature === 'false') {
        sleepModeBtn.style.opacity = '0.5';
        sleepModeBtn.style.cursor = 'not-allowed';
        // Allow pointer events so tooltip works, but disable click
        sleepModeBtn.style.pointerEvents = 'auto';
        sleepModeBtn.title = 'Feature Disabled via build flag (DISABLE_SLEEP_MODE_FEATURE)';
        sleepModeBtn.onclick = function(e) { e.preventDefault(); return false; };
        // Center content for disabled state
        sleepModeBtn.style.justifyContent = 'center';
        if (sleepModeIcon) {
          sleepModeIcon.style.display = 'block';
          sleepModeIcon.style.marginLeft = '8px';
        }
      } else {
        sleepModeBtn.style.cursor = 'pointer';
        sleepModeBtn.style.pointerEvents = 'auto';
        sleepModeBtn.title = '';
        sleepModeBtn.onclick = function() { openSleepModeModal(); };
        if (sleepModeIcon) sleepModeIcon.style.display = 'none';
      }

      // Handle Weather Settings button
      var weatherFeature = xmlDoc.getElementsByTagName('weatherfeature').length > 0 ?
                           xmlDoc.getElementsByTagName('weatherfeature')[0].childNodes[0].nodeValue : 'false';

      var weatherBtn = document.getElementById('WEATHER_SETTINGS_BTN');
      var weatherIcon = document.getElementById('WEATHER_NOT_SUPPORTED_ICON');

      if (weatherFeature === 'false') {
        weatherBtn.style.opacity = '0.5';
        weatherBtn.style.cursor = 'not-allowed';
        // Allow pointer events so tooltip works, but disable click
        weatherBtn.style.pointerEvents = 'auto';
        weatherBtn.title = 'ESP32 only — not supported on ESP8266 due to heap constraints and instability. Disabled via -DDISABLE_WEATHER_FEATURE build flag.';
        weatherBtn.onclick = function(e) { e.preventDefault(); return false; };
        // Center content for disabled state
        weatherBtn.style.justifyContent = 'center';
        if (weatherIcon) {
          weatherIcon.style.display = 'block';
          weatherIcon.style.marginLeft = '8px';
        }
      } else {
        weatherBtn.style.cursor = 'pointer';
        weatherBtn.style.pointerEvents = 'auto';
        weatherBtn.title = '';
        weatherBtn.onclick = function() { openWeatherModal(); };
        if (weatherIcon) weatherIcon.style.display = 'none';
      }

      // Handle Crypto Settings button
      var cryptoFeature = xmlDoc.getElementsByTagName('cryptofeature').length > 0 ?
                          xmlDoc.getElementsByTagName('cryptofeature')[0].childNodes[0].nodeValue : 'false';

      var cryptoBtn = document.getElementById('CRYPTO_SETTINGS_BTN');
      var cryptoIcon = document.getElementById('CRYPTO_NOT_SUPPORTED_ICON');

      if (cryptoFeature === 'false') {
        cryptoBtn.style.opacity = '0.5';
        cryptoBtn.style.cursor = 'not-allowed';
        cryptoBtn.title = 'ESP32 only — not supported on ESP8266 due to heap constraints and instability. Disabled via -DDISABLE_CRYPTO_FEATURE build flag.';
        cryptoBtn.onclick = function(e) { e.preventDefault(); return false; };
        cryptoBtn.style.justifyContent = 'center';
        if (cryptoIcon) {
          cryptoIcon.style.display = 'block';
          cryptoIcon.style.marginLeft = '8px';
        }
      } else {
        cryptoBtn.style.cursor = 'pointer';
        cryptoBtn.style.pointerEvents = 'auto';
        cryptoBtn.title = '';
        cryptoBtn.onclick = function() { openCryptoModal(); };
        if (cryptoIcon) cryptoIcon.style.display = 'none';
      }
    }
  };
  request.open('GET', 'changecredvars', true);
  request.send();
}

// Device Hostname Modal Functions
// General Settings Functions
function openGeneralModal() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      if (this.responseText.indexOf('buzzerEnable') > -1) {
        var parser = new DOMParser();
        var xmlDoc = parser.parseFromString(this.responseText, "text/xml");
        var beNode = xmlDoc.getElementsByTagName("buzzerEnable")[0];
        if (beNode && beNode.childNodes.length > 0) {
           document.getElementById('BUZZERENABLE').checked = (beNode.childNodes[0].nodeValue == "on");
        }
        var boeNode = xmlDoc.getElementsByTagName("brightnessOverrideEnable")[0];
        if (boeNode && boeNode.childNodes.length > 0) {
          document.getElementById('BRIGHTNESSOVERRIDEENABLE').checked = (boeNode.childNodes[0].nodeValue == "on");
        }
        var bovNode = xmlDoc.getElementsByTagName("brightnessOverrideValue")[0];
        if (bovNode && bovNode.childNodes.length > 0) {
          var value = bovNode.childNodes[0].nodeValue;
          document.getElementById('BRIGHTNESSOVERRIDE').value = value;
          document.getElementById('BRIGHTNESSOVERRIDE_LABEL').innerText = value;
        }
        toggleBrightnessOverride();
      }
      document.getElementById('generalModal').classList.add('show');
    }
  };
  // Add timestamp to prevent caching
  request.open('GET', 'generalvars?_=' + new Date().getTime(), true);
  request.send();
}

function closeGeneralModal() {
  document.getElementById('generalModal').classList.remove('show');
}

function saveGeneralConfig(event, shouldClose) {
  if (event) event.preventDefault();
  if (shouldClose === undefined) shouldClose = true;
  
  var params = "BuzzerEnable=" + (document.getElementById('BUZZERENABLE').checked ? "on" : "off") +
               "&BrightnessOverrideEnable=" + (document.getElementById('BRIGHTNESSOVERRIDEENABLE').checked ? "on" : "off") +
               "&BrightnessOverrideValue=" + document.getElementById('BRIGHTNESSOVERRIDE').value;
  
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (shouldClose) closeGeneralModal();
        showToast('Global settings saved!', 'success');
        
        // Update UI state if staying open
        if (!shouldClose) {
          toggleBrightnessOverride();
        }
      } else {
        showToast('Failed to save settings!', 'error');
      }
    }
  };
  request.open('POST', 'savegeneral', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.send(params);
}

function toggleBrightnessOverride() {
  var checkbox = document.getElementById('BRIGHTNESSOVERRIDEENABLE');
  var container = document.getElementById('BRIGHTNESSOVERRIDECONTAINER');
  if (checkbox.checked) {
    container.style.display = 'block';
  } else {
    container.style.display = 'none';
  }
}

function openDeviceModal() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      try {
        var parser = new DOMParser();
        var xmlDoc = parser.parseFromString(this.responseText, 'text/xml');
        document.getElementById('DEVICEHOSTNAME').value = xmlDoc.getElementsByTagName('clientid')[0].childNodes[0].nodeValue;
        document.getElementById('deviceModal').classList.add('show');
      } catch(e) {
        console.error('Error loading device settings:', e);
        showToast('Error loading device settings', 'error');
      }
    }
  };
  request.open('GET', 'changecredvars', true);
  request.send();
}

function closeDeviceModal() {
  document.getElementById('deviceModal').classList.remove('show');
}

function saveDeviceConfig(event) {
  event.preventDefault();
  var form = document.getElementById('device_config_form');
  var formData = new FormData(form);
  var params = new URLSearchParams(formData).toString();

  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        closeDeviceModal();
        showToast('Device settings saved successfully!', 'success');
        // Update displayed hostname
        var newHostname = document.getElementById('DEVICEHOSTNAME').value;
        if (newHostname) {
          document.getElementById('clientid_val').innerText = newHostname;
        }
      } else {
        showToast('Failed to save device settings!', 'error');
      }
    }
  };
  request.open('POST', 'changecredentials', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.send(params);
}

// Login Credentials Modal Functions
function openLoginModal() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      try {
        var parser = new DOMParser();
        var xmlDoc = parser.parseFromString(this.responseText, 'text/xml');
        document.getElementById('LOGINUSERNAME').value = xmlDoc.getElementsByTagName('username')[0].childNodes[0].nodeValue;
        document.getElementById('LOGINPASSWORD').value = '';
        document.getElementById('loginModal').classList.add('show');
      } catch(e) {
        console.error('Error loading login settings:', e);
        showToast('Error loading login settings', 'error');
      }
    }
  };
  request.open('GET', 'changecredvars', true);
  request.send();
}

function closeLoginModal() {
  document.getElementById('loginModal').classList.remove('show');
}

function saveLoginConfig(event) {
  event.preventDefault();
  var form = document.getElementById('login_config_form');
  var formData = new FormData(form);
  var params = new URLSearchParams(formData).toString();

  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        closeLoginModal();
        showToast('Login credentials saved successfully!', 'success');
      } else {
        showToast('Failed to save login credentials!', 'error');
      }
    }
  };
  request.open('POST', 'changecredentials', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.send(params);
}

// Timezone Mapping Functions (UI has 33 zones, MQTT has 16)
function posixToFriendly(posixStr) {
  var map = {
    'UTC0': 'UTC',
    'GMT0BST,M3.5.0/1,M10.5.0': 'Europe/London',
    'CET-1CEST,M3.5.0,M10.5.0/3': 'Europe/Paris',
    'EET-2EEST,M3.5.0/3,M10.5.0/4': 'Europe/Athens',
    'MSK-3': 'Europe/Moscow',
    'EST5EDT,M3.2.0,M11.1.0': 'US/Eastern',
    'CST6CDT,M3.2.0,M11.1.0': 'US/Central',
    'MST7MDT,M3.2.0,M11.1.0': 'US/Mountain',
    'PST8PDT,M3.2.0,M11.1.0': 'US/Pacific',
    'AKST9AKDT,M3.2.0,M11.1.0': 'US/Alaska',
    'HST10': 'US/Hawaii',
    'CST6CDT,M4.1.0,M10.5.0': 'America/Mexico_City',
    '<-03>3': 'America/Sao_Paulo',
    '<+04>-4': 'Asia/Dubai',
    'PKT-5': 'Asia/Karachi',
    'IST-5:30': 'Asia/Kolkata',
    '<+07>-7': 'Asia/Bangkok',
    '<+08>-8': 'Asia/Singapore',
    'CST-8': 'Asia/Shanghai',
    'JST-9': 'Asia/Tokyo',
    'KST-9': 'Asia/Seoul',
    'AWST-8': 'Australia/Perth',
    'ACST-9:30ACDT,M10.1.0,M4.1.0/3': 'Australia/Adelaide',
    'AEST-10': 'Australia/Brisbane',
    'AEST-10AEDT,M10.1.0,M4.1.0/3': 'Australia/Sydney',
    'NZST-12NZDT,M9.5.0,M4.1.0/3': 'Pacific/Auckland'
  };
  return map[posixStr] || 'Custom';
}

function friendlyToPosix(friendlyName) {
  var map = {
    'UTC': 'UTC0',
    'Europe/London': 'GMT0BST,M3.5.0/1,M10.5.0',
    'Europe/Paris': 'CET-1CEST,M3.5.0,M10.5.0/3',
    'Europe/Berlin': 'CET-1CEST,M3.5.0,M10.5.0/3',
    'Europe/Athens': 'EET-2EEST,M3.5.0/3,M10.5.0/4',
    'Europe/Moscow': 'MSK-3',
    'US/Eastern': 'EST5EDT,M3.2.0,M11.1.0',
    'US/Central': 'CST6CDT,M3.2.0,M11.1.0',
    'US/Mountain': 'MST7MDT,M3.2.0,M11.1.0',
    'US/Pacific': 'PST8PDT,M3.2.0,M11.1.0',
    'US/Alaska': 'AKST9AKDT,M3.2.0,M11.1.0',
    'US/Hawaii': 'HST10',
    'Canada/Pacific': 'PST8PDT,M3.2.0,M11.1.0',
    'Canada/Mountain': 'MST7MDT,M3.2.0,M11.1.0',
    'Canada/Central': 'CST6CDT,M3.2.0,M11.1.0',
    'Canada/Eastern': 'EST5EDT,M3.2.0,M11.1.0',
    'America/Mexico_City': 'CST6CDT,M4.1.0,M10.5.0',
    'America/Sao_Paulo': '<-03>3',
    'America/Argentina/Buenos_Aires': '<-03>3',
    'Asia/Dubai': '<+04>-4',
    'Asia/Karachi': 'PKT-5',
    'Asia/Kolkata': 'IST-5:30',
    'Asia/Bangkok': '<+07>-7',
    'Asia/Singapore': '<+08>-8',
    'Asia/Hong_Kong': '<+08>-8',
    'Asia/Shanghai': 'CST-8',
    'Asia/Tokyo': 'JST-9',
    'Asia/Seoul': 'KST-9',
    'Australia/Perth': 'AWST-8',
    'Australia/Adelaide': 'ACST-9:30ACDT,M10.1.0,M4.1.0/3',
    'Australia/Brisbane': 'AEST-10',
    'Australia/Sydney': 'AEST-10AEDT,M10.1.0,M4.1.0/3',
    'Pacific/Auckland': 'NZST-12NZDT,M9.5.0,M4.1.0/3'
  };
  return map[friendlyName] || friendlyName;
}

function onTimezoneChange() {
  var dropdown = document.getElementById('TIMEZONE');
  var hiddenInput = document.getElementById('TZSTRING');
  var visibleInput = document.getElementById('TZSTRING_VISIBLE');
  
  if (dropdown.value === 'Custom') {
    // Show custom input, copy from hidden to visible
    visibleInput.value = hiddenInput.value;
    toggleCustomTzInput();
  } else {
    // Update both hidden and visible with POSIX value
    var posixValue = friendlyToPosix(dropdown.value);
    hiddenInput.value = posixValue;
    visibleInput.value = posixValue;
    toggleCustomTzInput();
  }
}

function onCustomTzChange() {
  var visibleInput = document.getElementById('TZSTRING_VISIBLE');
  var hiddenInput = document.getElementById('TZSTRING');
  var dropdown = document.getElementById('TIMEZONE');
  
  // Sync visible to hidden
  hiddenInput.value = visibleInput.value;
  
  // Update dropdown to show friendly name if match found
  var friendlyName = posixToFriendly(visibleInput.value);
  dropdown.value = friendlyName;
}

function toggleCustomTzInput() {
  var dropdown = document.getElementById('TIMEZONE');
  var customContainer = document.getElementById('CUSTOMTZCONTAINER');
  if (dropdown.value === 'Custom') {
    customContainer.style.display = 'block';
  } else {
    customContainer.style.display = 'none';
  }
}

// Clock options helpers
function isMatrixLight() {
  var face = document.getElementById('CLOCKFACE').value;
  return face !== 'DEFAULT';
}

function populateDateFormatOptions() {
  var maxDevices = parseInt(document.getElementById('maxdevices_val').innerText || '4');
  var dropdown = document.getElementById('DATEFORMAT');
  dropdown.innerHTML = '';
  if (maxDevices == 4) {
    dropdown.innerHTML = `
      <option value="TIME_ONLY">Time Only (HH:MM)</option>
      <option value="TIME_SECONDS">Time with Seconds (HH:MM.SS)</option>
    `;
  } else if (maxDevices == 8) {
    dropdown.innerHTML = `
      <option value="TIME_ONLY">Time Only (HH:MM)</option>
      <option value="TIME_DATE">Time + Date (HH:MM Mon DD)</option>
      <option value="FULL_DATE">Full Date (Day, Mon DD)</option>
      <option value="TIME_FULL_DATE">Time + Full Date</option>
      <option value="CUSTOM">Custom Format</option>
      <option value="TIME_SECONDS">Time with Seconds (HH:MM.SS)</option>
    `;
  } else {
    dropdown.innerHTML = '<option value="TIME_ONLY">Time Only</option>';
  }
}

function toggleClockOptions() {
  var maxDevices = parseInt(document.getElementById('maxdevices_val').innerText || '4');
  var clockFaceSelect = document.getElementById('CLOCKFACE');
  var dateFormatSelect = document.getElementById('DATEFORMAT');
  var ampmToggle = document.getElementById('CLOCKAMPM');

  var dateFormat = dateFormatSelect.value;
  var ampmOn = ampmToggle.value === 'on';

  // Step 1: TIME_SECONDS on 4m requires a bitmap font (DEFAULT built-in is too wide)
  var defaultOption = clockFaceSelect.querySelector('option[value="DEFAULT"]');
  if (maxDevices == 4 && dateFormat === 'TIME_SECONDS') {
    if (clockFaceSelect.value === 'DEFAULT') clockFaceSelect.value = 'MATRIX_LIGHT_6';
    if (defaultOption) defaultOption.disabled = true;
  } else {
    if (defaultOption) defaultOption.disabled = false;
  }

  var matrixLight = isMatrixLight(); // re-read after possible face change

  // Step 2: On 4m, TIME_SECONDS + AM/PM exceed the 32-pixel display budget.
  //   Rule: TIME_SECONDS selected  → disable AM/PM (force off).
  //         AM/PM on               → disable TIME_SECONDS option (revert if selected).
  //         DEFAULT font on 4m     → disable both AM/PM and TIME_SECONDS.
  var timeSecondsOption = dateFormatSelect.querySelector('option[value="TIME_SECONDS"]');
  var ampmNote = document.getElementById('ampm_constraint_note');
  var tsNote = document.getElementById('ts_constraint_note');
  var ampmCard = document.getElementById('ampm_toggle_card');
  var ampmSeg  = document.getElementById('ampm_seg');

  function _setAmPmDisabled(disabled, forcedOff) {
    if (ampmCard) ampmCard.style.opacity = disabled ? '0.4' : '1';
    if (ampmSeg)  ampmSeg.style.pointerEvents = disabled ? 'none' : '';
    if (disabled && forcedOff) {
      ampmToggle.value = 'off';
      ampmSeg && ampmSeg.querySelectorAll('.seg-btn').forEach(function(b) {
        b.classList.toggle('active', b.dataset.val === 'off');
      });
    }
  }

  if (maxDevices == 4) {
    if (dateFormat === 'TIME_SECONDS') {
      // TIME_SECONDS active: disable AM/PM (HH:MM.SS AM/PM would exceed 32px)
      _setAmPmDisabled(true, true);
      if (timeSecondsOption) timeSecondsOption.disabled = false;
      if (ampmNote) { ampmNote.textContent = 'Not available with seconds display on 4-module builds.'; ampmNote.style.display = 'block'; }
      if (tsNote) tsNote.style.display = 'none';
    } else if (!matrixLight) {
      // DEFAULT font on 4m: disable AM/PM and TIME_SECONDS (both need Matrix Light)
      _setAmPmDisabled(true, true);
      if (timeSecondsOption) timeSecondsOption.disabled = true;
      if (ampmNote) { ampmNote.textContent = 'Requires Matrix Light font.'; ampmNote.style.display = 'block'; }
      if (tsNote) tsNote.style.display = 'none';
    } else if (ampmOn) {
      // AM/PM on + Matrix Light on 4m: disable TIME_SECONDS (12:34.56 PM too wide for 32px)
      if (timeSecondsOption) timeSecondsOption.disabled = true;
      if (dateFormat === 'TIME_SECONDS') { dateFormatSelect.value = 'TIME_ONLY'; dateFormat = 'TIME_ONLY'; }
      _setAmPmDisabled(false, false);
      if (ampmNote) ampmNote.style.display = 'none';
      if (tsNote) tsNote.style.display = 'block';
    } else {
      // Matrix Light, no AM/PM conflict — all clear
      _setAmPmDisabled(false, false);
      if (timeSecondsOption) timeSecondsOption.disabled = false;
      if (ampmNote) ampmNote.style.display = 'none';
      if (tsNote) tsNote.style.display = 'none';
    }
  } else {
    // 8m: all combinations allowed
    _setAmPmDisabled(false, false);
    if (timeSecondsOption) timeSecondsOption.disabled = false;
    if (ampmNote) ampmNote.style.display = 'none';
    if (tsNote) tsNote.style.display = 'none';
  }

  // Custom format container (8-module only)
  var customContainer = document.getElementById('CUSTOMDATEFORMATCONTAINER');
  customContainer.style.display = (maxDevices == 8 && dateFormatSelect.value === 'CUSTOM') ? 'block' : 'none';

  // Date alternate interval — shown whenever alternate is on
  var altOn = document.getElementById('DATEALTERNATE').checked;
  document.getElementById('DATEALTERNATECONTAINER').style.display = altOn ? 'block' : 'none';
}

function setAmPm(val) {
  document.getElementById('CLOCKAMPM').value = val;
  document.querySelectorAll('#ampm_seg .seg-btn').forEach(function(b) {
    b.classList.toggle('active', b.dataset.val === val);
  });
  toggleClockOptions();
}

function testMatrixFont() {
  var xhr = new XMLHttpRequest();
  xhr.open('GET', '/matrixfonttest', true);
  xhr.send();
}

// Clock Modal Functions
function openClockModal() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      try {
        var parser = new DOMParser();
        var xmlDoc = parser.parseFromString(this.responseText, 'text/xml');

        var getVal = function(tag, def) {
          var elem = xmlDoc.getElementsByTagName(tag)[0];
          return (elem && elem.childNodes[0]) ? elem.childNodes[0].nodeValue : def;
        };

        document.getElementById('CLOCKENABLED').checked = (getVal('enabled', 'off') == 'on');
        document.getElementById('NTPSERVER').value = getVal('ntpserver', 'pool.ntp.org');

        // Load timezone settings
        var tzString = getVal('tzstring', 'UTC0');
        document.getElementById('TZSTRING').value = tzString;
        document.getElementById('TZSTRING_VISIBLE').value = tzString;
        var friendlyTz = posixToFriendly(tzString);
        document.getElementById('TIMEZONE').value = friendlyTz;
        toggleCustomTzInput();

        // Load time display / alternation settings
        populateDateFormatOptions();
        document.getElementById('DATEFORMAT').value = getVal('dateformat', 'TIME_ONLY');
        document.getElementById('CLOCKFACE').value = getVal('clockface', 'DEFAULT');
        document.getElementById('DATEALTERNATE').checked = (getVal('datealternate', 'off') === 'on');
        document.getElementById('CLOCKDISPLAYSECONDS').value = getVal('clockdisplayseconds', '30');
        document.getElementById('clockDispVal').innerText = getVal('clockdisplayseconds', '30');
        document.getElementById('DATEALTERNATESECONDS').value = getVal('datealternateseconds', '5');
        document.getElementById('dateAltVal').innerText = getVal('datealternateseconds', '5');
        document.getElementById('CUSTOMDATEFORMAT').value = getVal('customdateformat', '');
        var ampmVal = getVal('clockampm', 'off');
        document.getElementById('CLOCKAMPM').value = ampmVal;
        document.querySelectorAll('#ampm_seg .seg-btn').forEach(function(b) {
          b.classList.toggle('active', b.dataset.val === ampmVal);
        });
        toggleClockOptions();

        var bri = getVal('brightness', '5');
        document.getElementById('CLOCKBRIGHTNESS').value = bri;
        document.getElementById('clockBriVal').innerText = bri;
        document.getElementById('TRANSITIONDELAY').value = parseInt(getVal('transitiondelay', '2000')) / 1000;
        document.getElementById('TRANSITIONSPEED').value = getVal('transitionspeed', '40');
        document.getElementById('TRANSITIONEFFECT').value = getVal('transitioneffect', 'FADE');
        document.getElementById('RANDOMIZETRANSITION').checked = (getVal('randomizetransition', 'off') == 'on');
        toggleTransitionDropdown();

        document.getElementById('clockModal').classList.add('show');
      } catch(e) {
        console.error('Error loading clock settings:', e);
        showToast('Error loading clock settings', 'error');
      }
    }
  };
  request.open('GET', 'clockpagevars', true);
  request.send();
}

// Toggle transition dropdown enabled state based on randomize checkbox
function toggleTransitionDropdown() {
  var randomize = document.getElementById('RANDOMIZETRANSITION').checked;
  var dropdown = document.getElementById('TRANSITIONEFFECT');
  dropdown.disabled = randomize;
  dropdown.style.opacity = randomize ? '0.5' : '1';
}

function closeClockModal() {
  document.getElementById('clockModal').classList.remove('show');
}

function saveClockConfig(event, shouldClose) {
  if (event) event.preventDefault();
  if (shouldClose === undefined) shouldClose = true;

  // Handle toggle switches
  if(document.getElementById("CLOCKENABLED").checked)
    document.getElementById("CLOCKENABLEDHIDDEN").disabled = true;
  if(document.getElementById("RANDOMIZETRANSITION").checked)
    document.getElementById("RANDOMIZETRANSITIONHIDDEN").disabled = true;
  if(document.getElementById("DATEALTERNATE").checked)
    document.getElementById("DATEALTERNATEHIDDEN").disabled = true;

  var form = document.getElementById('clock_config_form');
  var formData = new FormData(form);

  // Convert transition delay from seconds to milliseconds
  var delaySeconds = formData.get('TransitionDelay');
  formData.set('TransitionDelay', (parseFloat(delaySeconds) * 1000).toString());

  var params = new URLSearchParams(formData).toString();

  // Re-enable hidden fields immediately for next save
  document.getElementById("CLOCKENABLEDHIDDEN").disabled = false;
  document.getElementById("RANDOMIZETRANSITIONHIDDEN").disabled = false;
  document.getElementById("DATEALTERNATEHIDDEN").disabled = false;

  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (shouldClose) closeClockModal();
        showToast('Clock settings saved successfully!', 'success');
      } else {
        showToast('Failed to save clock settings!', 'error');
      }
    }
  };
  request.open('POST', 'saveclocksettings', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.send(params);
}

// Timer Modal Functions
var timerStatusPollInterval = null;

function openTimerModal() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      try {
        var parser = new DOMParser();
        var xmlDoc = parser.parseFromString(this.responseText, 'text/xml');

        var getVal = function(tag, def) {
          var elem = xmlDoc.getElementsByTagName(tag)[0];
          return (elem && elem.childNodes[0]) ? elem.childNodes[0].nodeValue : def;
        };

        document.getElementById('TIMERENABLED').checked = (getVal('enabled', 'off') == 'on');
        document.getElementById('TIMERMODE').value = getVal('mode', 'countdown');
        
        var durationSeconds = parseInt(getVal('durationseconds', '300'));
        var hours = Math.floor(durationSeconds / 3600);
        var minutes = Math.floor((durationSeconds % 3600) / 60);
        var seconds = durationSeconds % 60;
        document.getElementById('TIMER_HOURS').value = hours;
        document.getElementById('TIMER_MINUTES').value = minutes;
        document.getElementById('TIMER_SECONDS').value = seconds;
        document.getElementById('DURATIONSECONDS').value = durationSeconds;

        var bri = getVal('brightness', '7');
        document.getElementById('TIMERBRIGHTNESS').value = bri;
        document.getElementById('timerBriVal').innerText = bri;

        document.getElementById('ALERTBUZZER').checked = (getVal('alertbuzzer', 'off') == 'on');
        
        // Load and sync beep count
        var beepCount = getVal('alertbuzzercount', '5');
        document.getElementById('ALERTBUZZERCOUNT').value = beepCount;
        document.getElementById('ALERTBUZZERCOUNT_VISIBLE').value = beepCount;
        document.getElementById('beepCountVal').innerText = beepCount;
        
        document.getElementById('AUTOREPEAT').checked = (getVal('autorepeat', 'off') == 'on');

        // Load chirp list and selection
        var alertChirp = getVal('alertchirp', 'Gentle Dawn');
        var chirpSelect = document.getElementById('ALERTCHIRP');
        var chirpListElem = xmlDoc.getElementsByTagName('chirplist')[0];
        if (chirpListElem && chirpSelect) {
          chirpSelect.innerHTML = ''; // Clear existing options
          var chirps = chirpListElem.getElementsByTagName('chirp');
          for (var i = 0; i < chirps.length; i++) {
            var option = document.createElement('option');
            option.value = chirps[i].childNodes[0].nodeValue;
            option.text = chirps[i].childNodes[0].nodeValue;
            chirpSelect.appendChild(option);
          }
          chirpSelect.value = alertChirp;
        }

        var globalBuzzer = (getVal('globalbuzzer', 'on') == 'on');
        var alertBuzzerToggle = document.getElementById('ALERTBUZZER');
        var alertBuzzerCard = alertBuzzerToggle.closest('.toggle-card');
        
        if (!globalBuzzer) {
            alertBuzzerToggle.checked = false;
            alertBuzzerToggle.disabled = true;
            if (alertBuzzerCard) {
                alertBuzzerCard.style.opacity = '0.5';
                alertBuzzerCard.style.pointerEvents = 'none';
                alertBuzzerCard.title = "Enable Global Buzzer in General Settings first";
            }
        } else {
             alertBuzzerToggle.disabled = false;
             if (alertBuzzerCard) {
                alertBuzzerCard.style.opacity = '1';
                alertBuzzerCard.style.pointerEvents = 'auto';
                alertBuzzerCard.title = "";
             }
        }

        document.getElementById('timerModal').classList.add('show');
        toggleTimerControls();
        
        // Start status polling
        updateTimerStatus();
        timerStatusPollInterval = setInterval(updateTimerStatus, 500);
      } catch(e) {
        console.error('Error loading timer settings:', e);
        showToast('Error loading timer settings', 'error');
      }
    }
  };
  request.open('GET', 'timerpagevars', true);
  request.send();
}

function toggleTimerControls() {
  var enabled = document.getElementById('TIMERENABLED').checked;
  var block = document.getElementById('TIMER_CONTROLS_BLOCK');
  if (block) {
    block.style.opacity = enabled ? '1' : '0.4';
    block.style.pointerEvents = enabled ? 'auto' : 'none';
    block.style.filter = enabled ? 'none' : 'grayscale(100%)';
  }
}

function closeTimerModal() {
  document.getElementById('timerModal').classList.remove('show');
  // Stop status polling
  if (timerStatusPollInterval) {
    clearInterval(timerStatusPollInterval);
    timerStatusPollInterval = null;
  }
}

function toggleBeepCountInput() {
  var chirpSelect = document.getElementById('ALERTCHIRP');
  var beepCountContainer = document.getElementById('BEEPCOUNTCONTAINER');
  var beepCountVisible = document.getElementById('ALERTBUZZERCOUNT_VISIBLE');
  var beepCountHidden = document.getElementById('ALERTBUZZERCOUNT');
  
  if (chirpSelect && beepCountContainer) {
    var isLegacyBeeps = (chirpSelect.value === 'Simple Beep');
    beepCountContainer.style.display = isLegacyBeeps ? 'block' : 'none';
    
    // Sync visible slider with hidden input
    if (beepCountVisible && beepCountHidden) {
      if (isLegacyBeeps) {
        beepCountVisible.value = beepCountHidden.value;
        document.getElementById('beepCountVal').innerText = beepCountHidden.value;
      }
    }
  }
}

function previewChirp() {
  var chirpName = document.getElementById('ALERTCHIRP').value;
  var beepCount = document.getElementById('ALERTBUZZERCOUNT').value;
  
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        showToast('Playing: ' + chirpName, 'success');
      } else if (this.status == 403) {
        showToast('Preview blocked: Sleep mode is active', 'error');
      } else {
        showToast('Failed to preview sound', 'error');
      }
    }
  };
  request.open('POST', '/previewchirp', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.send('chirp=' + encodeURIComponent(chirpName) + '&count=' + encodeURIComponent(beepCount));
}

function saveTimerConfig(event, shouldClose) {
  if (event) event.preventDefault();
  if (shouldClose === undefined) shouldClose = true;

  // Update duration seconds from HH:MM:SS inputs
  var hours = parseInt(document.getElementById('TIMER_HOURS').value) || 0;
  var minutes = parseInt(document.getElementById('TIMER_MINUTES').value) || 0;
  var seconds = parseInt(document.getElementById('TIMER_SECONDS').value) || 0;
  var totalSeconds = (hours * 3600) + (minutes * 60) + seconds;
  document.getElementById('DURATIONSECONDS').value = totalSeconds;

  // Handle toggle switches
  if(document.getElementById("TIMERENABLED").checked)
    document.getElementById("TIMERENABLEDHIDDEN").disabled = true;
  if(document.getElementById("ALERTBUZZER").checked)
    document.getElementById("ALERTBUZZERHIDDEN").disabled = true;
  if(document.getElementById("AUTOREPEAT").checked)
    document.getElementById("AUTOREPEATHIDDEN").disabled = true;

  var form = document.getElementById('timer_config_form');
  var formData = new FormData(form);
  var params = new URLSearchParams(formData).toString();

  // Re-enable hidden fields immediately for next save
  document.getElementById("TIMERENABLEDHIDDEN").disabled = false;
  document.getElementById("ALERTBUZZERHIDDEN").disabled = false;
  document.getElementById("AUTOREPEATHIDDEN").disabled = false;

  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (shouldClose) closeTimerModal();
        showToast('Timer settings saved successfully!', 'success');
        
        // Update UI state if staying open
        if (!shouldClose) {
             toggleTimerControls();
             updateTimerStatus();
        }
      } else {
        showToast('Failed to save timer settings!', 'error');
      }
    }
  };
  request.open('POST', 'savetimersettings', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.send(params);
}

function startTimerControl() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        showToast('Timer started', 'success');
        updateTimerStatus();
      } else {
        showToast('Failed to start timer', 'error');
      }
    }
  };
  request.open('POST', '/api/timer/start', true);
  request.send();
}

function pauseTimerControl() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        showToast('Timer paused', 'success');
        updateTimerStatus();
      } else {
        showToast('Failed to pause timer', 'error');
      }
    }
  };
  request.open('POST', '/api/timer/pause', true);
  request.send();
}

function stopTimerControl() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        showToast('Timer stopped', 'success');
        updateTimerStatus();
      } else {
        showToast('Failed to stop timer', 'error');
      }
    }
  };
  request.open('POST', '/api/timer/stop', true);
  request.send();
}

function resetTimerControl() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        showToast('Timer reset', 'success');
        updateTimerStatus();
      } else {
        showToast('Failed to reset timer', 'error');
      }
    }
  };
  request.open('POST', '/api/timer/reset', true);
  request.send();
}


function updateTimerStatus() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      try {
        var status = JSON.parse(this.responseText);
        
        // Update status text
        // Update status text
        var statusText = 'Stopped';
        if (status.state !== undefined) {
          if (status.state === 1) statusText = 'Running';
          else if (status.state === 2) statusText = 'Paused';
          else if (status.state === 3) statusText = 'ALARMING!';
          else if (status.state === 4) {
             statusText = status.alertBuzzer === 'on' ? 'Timer Complete' : 'Timer Complete (Silent)';
          }
        } else {
           if (status.running && status.paused) statusText = 'Paused';
           else if (status.running) statusText = 'Running';
        }
        document.getElementById('TIMERSTATUS').innerText = statusText;
        
        // Update time display
        var displaySeconds;
        if (status.state >= 3 && status.mode === 'countdown') {
          displaySeconds = 0;
        } else if (status.mode === 'countdown') {
          displaySeconds = Math.max(0, status.targetSeconds - status.elapsedSeconds);
        } else {
          displaySeconds = status.elapsedSeconds;
        }
        document.getElementById('TIMERDISPLAY').innerText = formatTimerDuration(displaySeconds);
      } catch(e) {
        console.error('Error updating timer status:', e);
      }
    }
  };
  request.open('GET', '/api/timer', true);
  request.send();
}

function formatTimerDuration(seconds) {
  var h = Math.floor(seconds / 3600);
  var m = Math.floor((seconds % 3600) / 60);
  var s = seconds % 60;
  return ('0' + h).slice(-2) + ':' + ('0' + m).slice(-2) + ':' + ('0' + s).slice(-2);
}

// Sleep Mode Modal Functions
function toggleMuteOnly() {
  var enabled = document.getElementById('SLEEPMODEENABLED').checked;
  var muteOnly = document.getElementById('SLEEPMODEMUTEONLY');
  var muteOnlyCard = document.getElementById('MUTE_ONLY_CARD');
  var weekendEnabled = document.getElementById('WEEKENDENABLED');
  var weekendCard = document.getElementById('WEEKEND_TOGGLE_CARD');
  
  if (muteOnly && muteOnlyCard) {
    muteOnly.disabled = !enabled;
    muteOnlyCard.style.opacity = enabled ? '1' : '0.5';
  }
  
  if (weekendEnabled && weekendCard) {
    weekendEnabled.disabled = !enabled;
    weekendCard.style.opacity = enabled ? '1' : '0.5';
    // If we're disabling sleep mode overall, make sure weekend times hide
    if (!enabled) {
      document.getElementById('weekendTimesContainer').style.display = 'none';
    } else {
      toggleWeekendTimes(); // re-evaluate if they should be shown
    }
  }
}

function toggleWeekendTimes() {
  var weekendEnabled = document.getElementById('WEEKENDENABLED').checked;
  var container = document.getElementById('weekendTimesContainer');
  if (weekendEnabled) {
    container.style.display = 'flex';
    document.getElementById('WEEKEND_ON_TIME').required = true;
    document.getElementById('WEEKEND_OFF_TIME').required = true;
  } else {
    container.style.display = 'none';
    document.getElementById('WEEKEND_ON_TIME').required = false;
    document.getElementById('WEEKEND_OFF_TIME').required = false;
  }
}

function openSleepModeModal() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var parser = new DOMParser();
      var xmlDoc = parser.parseFromString(this.responseText, 'text/xml');

      // Parse configuration values
      var enabled = xmlDoc.getElementsByTagName('enabled')[0].childNodes[0] ? xmlDoc.getElementsByTagName('enabled')[0].childNodes[0].nodeValue : 'off';
      var onTime = xmlDoc.getElementsByTagName('ontime')[0].childNodes[0] ? xmlDoc.getElementsByTagName('ontime')[0].childNodes[0].nodeValue : '22:00';
      var offTime = xmlDoc.getElementsByTagName('offtime')[0].childNodes[0] ? xmlDoc.getElementsByTagName('offtime')[0].childNodes[0].nodeValue : '06:00';
      var active = xmlDoc.getElementsByTagName('active')[0].childNodes[0] ? xmlDoc.getElementsByTagName('active')[0].childNodes[0].nodeValue : 'false';
      var clockEnabled = xmlDoc.getElementsByTagName('clockenabled')[0].childNodes[0] ? xmlDoc.getElementsByTagName('clockenabled')[0].childNodes[0].nodeValue : 'false';
      var muteOnly = xmlDoc.getElementsByTagName('muteonly')[0].childNodes[0] ? xmlDoc.getElementsByTagName('muteonly')[0].childNodes[0].nodeValue : 'off';
      var weekendEnabled = xmlDoc.getElementsByTagName('weekendenabled')[0] && xmlDoc.getElementsByTagName('weekendenabled')[0].childNodes[0] ? xmlDoc.getElementsByTagName('weekendenabled')[0].childNodes[0].nodeValue : 'off';
      var weekendOnTime = xmlDoc.getElementsByTagName('weekendontime')[0] && xmlDoc.getElementsByTagName('weekendontime')[0].childNodes[0] ? xmlDoc.getElementsByTagName('weekendontime')[0].childNodes[0].nodeValue : '22:00';
      var weekendOffTime = xmlDoc.getElementsByTagName('weekendofftime')[0] && xmlDoc.getElementsByTagName('weekendofftime')[0].childNodes[0] ? xmlDoc.getElementsByTagName('weekendofftime')[0].childNodes[0].nodeValue : '08:00';

      // Populate form fields
      document.getElementById('SLEEPMODEENABLED').checked = (enabled === 'on');
      document.getElementById('SLEEPMODEMUTEONLY').checked = (muteOnly === 'on');
      document.getElementById('SLEEPMODE_ON_TIME').value = onTime;
      document.getElementById('SLEEPMODE_OFF_TIME').value = offTime;
      document.getElementById('WEEKENDENABLED').checked = (weekendEnabled === 'on');
      document.getElementById('WEEKEND_ON_TIME').value = weekendOnTime;
      document.getElementById('WEEKEND_OFF_TIME').value = weekendOffTime;

      // Update toggle states
      toggleMuteOnly();
      toggleWeekendTimes();

      // Update status display
      var statusElement = document.getElementById('SLEEPMODESTATUS');
      if (active === 'true') {
        statusElement.textContent = 'Active';
        statusElement.style.color = '#2ea44f';
      } else {
        statusElement.textContent = 'Inactive';
        statusElement.style.color = '#888';
      }

      // Show modal
      document.getElementById('sleepModeModal').classList.add('show');
    }
  };
  request.open('GET', '/sleepmodeparevars', true);
  request.send();
}

function closeSleepModeModal() {
  document.getElementById('sleepModeModal').classList.remove('show');
}

function saveSleepModeConfig(event, shouldClose) {
  if (event) event.preventDefault();
  if (shouldClose === undefined) shouldClose = true;

  // Handle toggle switches - disable hidden fields if checked
  var enabledCheckbox = document.getElementById('SLEEPMODEENABLED');
  var enabledHidden = document.getElementById('SLEEPMODEENABLEDHIDDEN');
  enabledHidden.disabled = enabledCheckbox.checked;

  var muteOnlyCheckbox = document.getElementById('SLEEPMODEMUTEONLY');
  var muteOnlyHidden = document.getElementById('SLEEPMODEMUTEONLYHIDDEN');
  muteOnlyHidden.disabled = muteOnlyCheckbox.checked;

  var weekendEnabledCheckbox = document.getElementById('WEEKENDENABLED');
  var weekendEnabledHidden = document.getElementById('WEEKENDENABLEDHIDDEN');
  if (weekendEnabledHidden && weekendEnabledCheckbox) {
      weekendEnabledHidden.disabled = weekendEnabledCheckbox.checked;
  }

  // Temporarily enable everything to ensure all values are captured
  var muteOnlyWasDisabled = muteOnlyCheckbox.disabled;
  muteOnlyCheckbox.disabled = false;
  
  var weekendEnabledWasDisabled = false;
  if (weekendEnabledCheckbox) {
      weekendEnabledWasDisabled = weekendEnabledCheckbox.disabled;
      weekendEnabledCheckbox.disabled = false;
  }

  // Collect form data
  var form = document.getElementById('sleep_mode_config_form');
  var formData = new FormData(form);
  
  // Restore disabled state
  muteOnlyCheckbox.disabled = muteOnlyWasDisabled;
  if (weekendEnabledCheckbox) {
      weekendEnabledCheckbox.disabled = weekendEnabledWasDisabled;
  }

  var params = new URLSearchParams(formData).toString();

  // Re-enable hidden fields
  enabledHidden.disabled = false;
  muteOnlyHidden.disabled = false;
  if (weekendEnabledHidden) {
      weekendEnabledHidden.disabled = false;
  }

  // Send POST request
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (shouldClose) closeSleepModeModal();
        showToast('Sleep Mode settings saved successfully!', 'success');
      } else {
        showToast('Failed to save Sleep Mode settings', 'error');
      }
    }
  };
  request.open('POST', '/savesleepmodesettings', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.send(params);
}

// Progressive slider helpers — slider position (index) maps to a non-linear value array
var WEATHER_FETCH_STEPS    = [5, 10, 15, 20, 30, 45, 60, 90, 120];
var CRYPTO_FETCH_STEPS     = [15, 20, 30, 45, 60, 90, 120, 180, 240];
var DISPLAY_INTERVAL_STEPS = [1, 2, 5, 10, 15, 20, 30, 45, 60, 90, 120, 180, 240];

function stepsVal(steps, idx) {
  return steps[Math.max(0, Math.min(Math.round(idx), steps.length - 1))];
}
function stepsIdx(steps, val) {
  var best = 0, bestDiff = Math.abs(steps[0] - val);
  for (var i = 1; i < steps.length; i++) {
    var d = Math.abs(steps[i] - val);
    if (d < bestDiff) { bestDiff = d; best = i; }
  }
  return best;
}
function stepsLabel(val) {
  return val >= 60 ? (val / 60) + 'h' : val + ' min';
}

// Weather Modal Functions
function openWeatherModal() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var data = JSON.parse(this.responseText);

      // Populate form fields
      document.getElementById('WEATHERENABLED').checked = (data.enabled === true || data.enabled === 'on');
      document.getElementById('WEATHER_LOCATION').value = data.location || '';
      document.getElementById('WEATHER_LATITUDE').value = data.latitude || '';
      document.getElementById('WEATHER_LONGITUDE').value = data.longitude || '';

      // Temperature unit
      if (data.temperatureUnit === 'F') {
        document.getElementById('WEATHER_UNIT_F').checked = true;
      } else {
        document.getElementById('WEATHER_UNIT_C').checked = true;
      }

      // Progressive sliders
      var fetchIdx = stepsIdx(WEATHER_FETCH_STEPS, parseInt(data.updateInterval) || 30);
      document.getElementById('WEATHER_UPDATE_INTERVAL').value = fetchIdx;
      document.getElementById('WEATHER_INTERVAL_VAL').textContent = stepsLabel(WEATHER_FETCH_STEPS[fetchIdx]);

      var dispIdx = stepsIdx(DISPLAY_INTERVAL_STEPS, parseInt(data.displayInterval) || 5);
      document.getElementById('WEATHER_DISPLAY_INTERVAL').value = dispIdx;
      document.getElementById('WEATHER_DISPLAY_INTERVAL_VAL').textContent = stepsLabel(DISPLAY_INTERVAL_STEPS[dispIdx]);

      var repeatCount = parseInt(data.displayRepeat) || 2;
      document.getElementById('WEATHER_DISPLAY_REPEAT').value = repeatCount;
      document.getElementById('WEATHER_REPEAT_VAL').textContent = repeatCount;

      var brightness = parseInt(data.brightness) || 5;
      document.getElementById('WEATHER_BRIGHTNESS').value = brightness;
      document.getElementById('WEATHER_BRI_VAL').textContent = brightness;

      // Update weather status display
      updateWeatherStatus(data);

      // Show modal
      document.getElementById('weatherModal').classList.add('show');
    }
  };
  request.open('GET', '/api/weather/status', true);
  request.send();
}

function updateWeatherStatus(data) {
  var statusEl = document.getElementById('WEATHER_STATUS');
  var forecastEl = document.getElementById('WEATHER_FORECAST');

  if (data.dataValid) {
    statusEl.textContent = data.temperature + ' ' + data.condition;
    statusEl.style.color = 'var(--text)';
    forecastEl.textContent = data.forecast || '';
  } else if (!data.latitude || !data.longitude) {
    statusEl.textContent = 'Not configured';
    statusEl.style.color = '#888';
    forecastEl.textContent = 'Enter coordinates to enable weather';
  } else {
    statusEl.textContent = 'Waiting for data...';
    statusEl.style.color = '#888';
    forecastEl.textContent = '';
  }
}

function closeWeatherModal() {
  document.getElementById('weatherModal').classList.remove('show');
}

function saveWeatherConfig(event, shouldClose) {
  if (event) event.preventDefault();
  if (shouldClose === undefined) shouldClose = true;

  // Handle toggle switch
  var enabledCheckbox = document.getElementById('WEATHERENABLED');
  var enabledHidden = document.getElementById('WEATHERENABLEDHIDDEN');
  if (enabledCheckbox.checked) {
    enabledHidden.disabled = true;
  }

  // Build form data manually for JSON submission
  var formData = {
    enabled: enabledCheckbox.checked ? 'on' : 'off',
    location: document.getElementById('WEATHER_LOCATION').value,
    latitude: document.getElementById('WEATHER_LATITUDE').value,
    longitude: document.getElementById('WEATHER_LONGITUDE').value,
    temperatureUnit: document.querySelector('input[name="WeatherTempUnit"]:checked').value,
    updateIntervalMinutes: String(stepsVal(WEATHER_FETCH_STEPS, document.getElementById('WEATHER_UPDATE_INTERVAL').value)),
    displayIntervalMinutes: String(stepsVal(DISPLAY_INTERVAL_STEPS, document.getElementById('WEATHER_DISPLAY_INTERVAL').value)),
    displayRepeatCount: document.getElementById('WEATHER_DISPLAY_REPEAT').value,
    brightness: document.getElementById('WEATHER_BRIGHTNESS').value
  };

  // Re-enable hidden field
  enabledHidden.disabled = false;

  // Send POST request
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (shouldClose) closeWeatherModal();
        showToast('Weather settings saved successfully!', 'success');
      } else {
        showToast('Failed to save Weather settings', 'error');
      }
    }
  };
  request.open('POST', '/api/weather/save', true);
  request.setRequestHeader('Content-Type', 'application/json');
  request.send(JSON.stringify(formData));
}

function refreshWeather() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        var data = JSON.parse(this.responseText);
        updateWeatherStatus(data);
        showToast('Weather data refreshed!', 'success');
      } else {
        showToast('Failed to refresh weather', 'error');
      }
    }
  };
  request.open('POST', '/api/weather/refresh', true);
  request.send();
}

// Crypto Price Ticker Modal Functions
function openCryptoModal() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var data = JSON.parse(this.responseText);

      // Populate form fields
      document.getElementById('CRYPTOENABLED').checked = (data.enabled === true || data.enabled === 'on');
      document.getElementById('CRYPTO_API_KEY').value = data.apiKey || '';
      document.getElementById('CRYPTO_COINS').value = data.coins || 'btc-bitcoin,eth-ethereum';

      // Currency radio buttons
      var currency = data.currency || 'USD';
      var currencyRadios = document.querySelectorAll('input[name="CryptoCurrency"]');
      currencyRadios.forEach(function(r) { r.checked = (r.value === currency); });

      // Progressive sliders
      var fetchIdx = stepsIdx(CRYPTO_FETCH_STEPS, parseInt(data.updateInterval) || 30);
      document.getElementById('CRYPTO_FETCH_INTERVAL').value = fetchIdx;
      document.getElementById('CRYPTO_FETCH_INTERVAL_VAL').textContent = stepsLabel(CRYPTO_FETCH_STEPS[fetchIdx]);

      var dispIdx = stepsIdx(DISPLAY_INTERVAL_STEPS, parseInt(data.displayInterval) || 5);
      document.getElementById('CRYPTO_DISPLAY_INTERVAL').value = dispIdx;
      document.getElementById('CRYPTO_DISPLAY_INTERVAL_VAL').textContent = stepsLabel(DISPLAY_INTERVAL_STEPS[dispIdx]);

      // Sliders
      var repeatCount = parseInt(data.displayRepeat) || 2;
      document.getElementById('CRYPTO_DISPLAY_REPEAT').value = repeatCount;
      document.getElementById('CRYPTO_REPEAT_VAL').textContent = repeatCount;

      var brightness = parseInt(data.brightness) || 5;
      document.getElementById('CRYPTO_BRIGHTNESS').value = brightness;
      document.getElementById('CRYPTO_BRI_VAL').textContent = brightness;

      // Coin counter
      updateCoinCount();

      // Status section
      updateCryptoStatus(data);

      document.getElementById('cryptoModal').classList.add('show');
    }
  };
  request.open('GET', '/api/crypto/status', true);
  request.send();
}

function updateCryptoStatus(data) {
  var statusEl = document.getElementById('CRYPTO_STATUS');
  if (data.dataValid && data.priceBuffer && data.priceBuffer.length > 0) {
    statusEl.textContent = data.priceBuffer;
    statusEl.style.color = 'var(--text)';
  } else if (!data.coins || data.coins.length === 0) {
    statusEl.textContent = 'Not configured — enter coin IDs to enable';
    statusEl.style.color = '#888';
  } else {
    statusEl.textContent = 'Waiting for data...';
    statusEl.style.color = '#888';
  }

  var lastEl = document.getElementById('CRYPTO_LAST_UPDATE');
  if (lastEl) {
    var ago = data.lastUpdateAgo;
    if (typeof ago === 'number' && ago >= 0) {
      if (ago < 60) lastEl.textContent = 'Updated ' + ago + 's ago';
      else if (ago < 3600) lastEl.textContent = 'Updated ' + Math.round(ago / 60) + 'min ago';
      else lastEl.textContent = 'Updated ' + Math.round(ago / 3600) + 'h ago';
    } else {
      lastEl.textContent = 'No data fetched yet';
    }
  }
}

function closeCryptoModal() {
  document.getElementById('cryptoModal').classList.remove('show');
}

function updateCoinCount() {
  var coinsVal = document.getElementById('CRYPTO_COINS').value.trim();
  var count = coinsVal.length === 0 ? 0 : coinsVal.split(',').filter(function(s) { return s.trim().length > 0; }).length;
  var counterEl = document.getElementById('CRYPTO_COIN_COUNT');
  if (counterEl) {
    counterEl.textContent = count + '/10 coins';
    counterEl.style.color = count > 10 ? 'var(--warning)' : 'var(--subtext)';
  }
}

function saveCryptoConfig(event, shouldClose) {
  if (event) event.preventDefault();
  if (shouldClose === undefined) shouldClose = true;

  var enabledCheckbox = document.getElementById('CRYPTOENABLED');
  var coinsInput = document.getElementById('CRYPTO_COINS');

  // Validate coin count
  var coinsVal = coinsInput.value.trim();
  var coinList = coinsVal.length === 0 ? [] : coinsVal.split(',').filter(function(s) { return s.trim().length > 0; });
  if (coinList.length > 10) {
    showToast('Too many coins — maximum 10 allowed', 'error');
    return;
  }

  var selectedCurrency = document.querySelector('input[name="CryptoCurrency"]:checked');

  var formData = {
    enabled: enabledCheckbox.checked ? 'on' : 'off',
    apiKey: document.getElementById('CRYPTO_API_KEY').value.trim(),
    coins: coinsInput.value.trim(),
    currency: selectedCurrency ? selectedCurrency.value : 'USD',
    updateIntervalMinutes: String(stepsVal(CRYPTO_FETCH_STEPS, document.getElementById('CRYPTO_FETCH_INTERVAL').value)),
    displayIntervalMinutes: String(stepsVal(DISPLAY_INTERVAL_STEPS, document.getElementById('CRYPTO_DISPLAY_INTERVAL').value)),
    displayRepeatCount: document.getElementById('CRYPTO_DISPLAY_REPEAT').value,
    brightness: document.getElementById('CRYPTO_BRIGHTNESS').value
  };

  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (shouldClose) closeCryptoModal();
        showToast('Crypto settings saved successfully!', 'success');
      } else {
        var errMsg = 'Failed to save Crypto settings';
        try { var resp = JSON.parse(this.responseText); if (resp.error) errMsg = resp.error; } catch(e) {}
        showToast(errMsg, 'error');
      }
    }
  };
  request.open('POST', '/api/crypto/save', true);
  request.setRequestHeader('Content-Type', 'application/json');
  request.send(JSON.stringify(formData));
}

function refreshCrypto() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        var data = JSON.parse(this.responseText);
        updateCryptoStatus(data);
        showToast('Crypto data refresh requested!', 'success');
      } else {
        showToast('Failed to refresh crypto', 'error');
      }
    }
  };
  request.open('POST', '/api/crypto/refresh', true);
  request.send();
}

// Alarm Clock Modal Functions
var alarmChirpList = [];

function openAlarmModal() {
  // Update subtitle based on maxAlarms
  var subtitle = document.getElementById('alarmModalSubtitle');
  if (subtitle) {
    if (maxAlarms === 1) {
      subtitle.innerText = 'Configure 1 alarm clock';
    } else {
      subtitle.innerText = 'Configure up to ' + maxAlarms + ' alarm clocks';
    }
  }
  
  document.getElementById('alarmModal').classList.add('show');
  
  // First fetch chirp list, then config
  var chirpRequest = new XMLHttpRequest();
  chirpRequest.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      alarmChirpList = JSON.parse(this.responseText);
      loadAlarmConfig();
    }
  };
  chirpRequest.open('GET', '/api/chirps', true);
  chirpRequest.send();
}

function loadAlarmConfig() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var data = JSON.parse(this.responseText);

      // Populate master enable toggle
      document.getElementById('ALARM_MASTER_ENABLED').checked = (data.masterEnabled === 'on');

      // Populate alarm items
      populateAlarmItems(data.alarms || []);

      // Show modal
      document.getElementById('alarmModal').classList.add('show');
    }
  };
  request.open('GET', '/api/alarm/config', true);
  request.send();
}

function closeAlarmModal() {
  document.getElementById('alarmModal').classList.remove('show');
}

function populateAlarmItems(alarms) {
  var container = document.getElementById('ALARM_ITEMS_CONTAINER');
  container.innerHTML = '';

  for (var i = 0; i < maxAlarms; i++) {
    var alarm = alarms[i] || getDefaultAlarm();
    var alarmHtml = createAlarmItemHtml(i, alarm);
    container.innerHTML += alarmHtml;
  }
}

function getDefaultAlarm() {
  return {
    enabled: 'off',
    time: '07:00',
    daysOfWeek: '',
    message: 'Alarm!',
    chirpName: 'Gentle Dawn',
    repeatCount: '3'
  };
}

function createAlarmItemHtml(index, alarm) {
  var num = index + 1;
  var isEnabled = (alarm.enabled === 'on');
  var headerColor = isEnabled ? 'var(--accent)' : 'var(--subtext)';

  // Build chirp dropdown options
  var chirpOptions = '';
  for (var c = 0; c < alarmChirpList.length; c++) {
    var chirp = alarmChirpList[c];
    var selected = (chirp.name === alarm.chirpName) ? 'selected' : '';
    chirpOptions += '<option value="' + chirp.name + '" ' + selected + '>' + chirp.name + ' - ' + chirp.description + '</option>';
  }

  // Build day checkboxes (1=Mon, 7=Sun)
  var days = ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun'];
  var dayCheckboxes = '';
  for (var d = 1; d <= 7; d++) {
    var checked = (alarm.daysOfWeek.indexOf(d.toString()) !== -1) ? 'checked' : '';
    dayCheckboxes += '<label style="display: inline-flex; align-items: center; gap: 4px; font-size: 0.8rem; margin-right: 8px;">';
    dayCheckboxes += '<input type="checkbox" id="ALARM_' + index + '_DAY_' + d + '" ' + checked + '>';
    dayCheckboxes += days[d-1] + '</label>';
  }

  var html = '<div style="background: #252525; border: 1px solid var(--border); border-radius: 8px; padding: 15px; margin-bottom: 15px;">';
  html += '<div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px;">';
  html += '<span style="font-weight: bold; color: ' + headerColor + ';">Alarm #' + num + '</span>';
  html += '<label class="switch" style="margin: 0;"><input type="checkbox" id="ALARM_' + index + '_ENABLED" ' + (isEnabled ? 'checked' : '') + ' onchange="updateAlarmHeader(' + index + ')"><span class="slider"></span></label>';
  html += '</div>';

  html += '<div style="display: grid; gap: 10px;">';

  // Time input (styled to match Sleep Mode time inputs)
  html += '<div style="display: flex; flex-direction: column; align-items: center;">';
  html += '<label style="font-size: 0.85rem; text-align: center; display: block; margin-bottom: 5px;">Alarm Time (HH:MM)</label>';
  html += '<input type="time" id="ALARM_' + index + '_TIME" value="' + alarm.time + '" style="width: 100%; max-width: 200px; text-align: center; font-size: 1.1rem; padding: 14px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px;"></div>';

  // Days of week
  html += '<div><label style="font-size: 0.85rem;">Days (empty = daily)</label>';
  html += '<div style="padding: 10px 0;">' + dayCheckboxes + '</div></div>';

  // Message
  html += '<div><label style="font-size: 0.85rem;">Message</label>';
  html += '<input type="text" id="ALARM_' + index + '_MESSAGE" value="' + escapeHtml(alarm.message) + '" maxlength="128" placeholder="Alarm message" style="width: 100%; padding: 10px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px;"></div>';

  // Chirp sound
  html += '<div><label style="font-size: 0.85rem;">Sound</label>';
  html += '<div style="display: flex; gap: 8px;">';
  html += '<select id="ALARM_' + index + '_CHIRP" style="flex: 1; padding: 10px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px;">' + chirpOptions + '</select>';
  html += '<button type="button" onclick="previewAlarmChirp(' + index + ')" class="btn" style="padding: 10px 14px; background: var(--input); border: 1px solid var(--border);" title="Preview Sound">';
  html += '<svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polygon points="5 3 19 12 5 21 5 3"></polygon></svg>';
  html += '</button></div></div>';

  // Repeat count
  var repeatVal = parseInt(alarm.repeatCount) || 3;
  html += '<div><label style="font-size: 0.85rem;">Repeat: <span id="alarm_' + index + '_repeat_val">' + repeatVal + '</span></label>';
  html += '<input type="range" id="ALARM_' + index + '_REPEAT" min="1" max="20" value="' + repeatVal + '" oninput="document.getElementById(\'alarm_' + index + '_repeat_val\').innerText=this.value" style="width: 100%;"></div>';

  html += '</div></div>';

  return html;
}

function escapeHtml(text) {
  var div = document.createElement('div');
  div.appendChild(document.createTextNode(text || ''));
  return div.innerHTML.replace(/"/g, '&quot;');
}

function previewAlarmChirp(index) {
  var chirpName = document.getElementById('ALARM_' + index + '_CHIRP').value;
  var beepCount = document.getElementById('ALARM_' + index + '_REPEAT').value;

  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        showToast('Playing: ' + chirpName, 'success');
      } else if (this.status == 403) {
        showToast('Preview blocked: Sleep mode is active', 'error');
      } else {
        showToast('Failed to preview sound', 'error');
      }
    }
  };
  request.open('POST', '/previewchirp', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.send('chirp=' + encodeURIComponent(chirpName) + '&count=' + encodeURIComponent(beepCount));
}

function updateAlarmHeader(index) {
  // Visual feedback when alarm enabled/disabled
  var checkbox = document.getElementById('ALARM_' + index + '_ENABLED');
  var header = checkbox.parentNode.parentNode.querySelector('span');
  header.style.color = checkbox.checked ? 'var(--accent)' : 'var(--subtext)';
}

function getSelectedDays(alarmIndex) {
  var days = '';
  for (var d = 1; d <= 7; d++) {
    var checkbox = document.getElementById('ALARM_' + alarmIndex + '_DAY_' + d);
    if (checkbox && checkbox.checked) {
      days += d;
    }
  }
  return days;
}

function saveAlarmConfig(event, shouldClose) {
  if (event) event.preventDefault();
  if (shouldClose === undefined) shouldClose = true;

  // Collect all alarm data
  var alarmData = {
    masterEnabled: document.getElementById('ALARM_MASTER_ENABLED').checked ? 'on' : 'off',
    alarms: []
  };

  for (var i = 0; i < maxAlarms; i++) {
    alarmData.alarms.push({
      enabled: document.getElementById('ALARM_' + i + '_ENABLED').checked ? 'on' : 'off',
      time: document.getElementById('ALARM_' + i + '_TIME').value,
      daysOfWeek: getSelectedDays(i),
      message: document.getElementById('ALARM_' + i + '_MESSAGE').value,
      chirpName: document.getElementById('ALARM_' + i + '_CHIRP').value,
      repeatCount: document.getElementById('ALARM_' + i + '_REPEAT').value
    });
  }

  // POST to server
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (shouldClose) closeAlarmModal();
        showToast('Alarm settings saved successfully!', 'success');
      } else {
        showToast('Failed to save alarm settings', 'error');
      }
    }
  };
  request.open('POST', '/api/alarm/config', true);
  request.setRequestHeader('Content-Type', 'application/json');
  request.send(JSON.stringify(alarmData));
}

function stopAllAlerts() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        showToast('All alarms stopped', 'success');
      } else {
        showToast('Failed to stop alarms', 'error');
      }
    }
  };
  request.open('POST', '/api/alarm/stop', true);
  request.send();
}

// Recurrent Alarm Modal Functions
var recurrentAlarmChirpList = [];

function openRecurrentAlarmModal() {
  document.getElementById('recurrentAlarmModal').classList.add('show');
  
  // First fetch chirp list, then config
  var chirpRequest = new XMLHttpRequest();
  chirpRequest.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      recurrentAlarmChirpList = JSON.parse(this.responseText);
      loadRecurrentAlarmConfig();
    }
  };
  chirpRequest.open('GET', '/api/chirps', true);
  chirpRequest.send();
}

function loadRecurrentAlarmConfig() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var data = JSON.parse(this.responseText);

      // Populate enable toggle
      document.getElementById('RECURRENT_ALARM_ENABLED').checked = (data.enabled === 'on');

      // Populate disable weekends toggle
      document.getElementById('RECURRENT_ALARM_DISABLE_WEEKENDS').checked = (data.disableWeekends === 'on');

      // Populate interval dropdown
      document.getElementById('RECURRENT_ALARM_INTERVAL').value = data.interval || '15min';

      // Populate chirp dropdown
      var chirpSelect = document.getElementById('RECURRENT_ALARM_CHIRP');
      chirpSelect.innerHTML = '';
      for (var i = 0; i < recurrentAlarmChirpList.length; i++) {
        var option = document.createElement('option');
        option.value = recurrentAlarmChirpList[i].name;
        option.text = recurrentAlarmChirpList[i].name + ' - ' + recurrentAlarmChirpList[i].description;
        chirpSelect.appendChild(option);
      }
      chirpSelect.value = data.chirpName || 'Gentle Dawn';
    }
  };
  request.open('GET', '/api/recurrent_alarm/config', true);
  request.send();
}

function closeRecurrentAlarmModal() {
  document.getElementById('recurrentAlarmModal').classList.remove('show');
}

function saveRecurrentAlarmConfig(event, shouldClose) {
  if (event) event.preventDefault();
  if (shouldClose === undefined) shouldClose = true;

  // Collect form data
  var configData = {
    enabled: document.getElementById('RECURRENT_ALARM_ENABLED').checked ? 'on' : 'off',
    disableWeekends: document.getElementById('RECURRENT_ALARM_DISABLE_WEEKENDS').checked ? 'on' : 'off',
    interval: document.getElementById('RECURRENT_ALARM_INTERVAL').value,
    chirpName: document.getElementById('RECURRENT_ALARM_CHIRP').value
  };

  // POST to server
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (shouldClose) closeRecurrentAlarmModal();
        showToast('Recurrent alarm settings saved successfully!', 'success');
      } else {
        showToast('Failed to save recurrent alarm settings', 'error');
      }
    }
  };
  request.open('POST', '/api/recurrent_alarm/config', true);
  request.setRequestHeader('Content-Type', 'application/json');
  request.send(JSON.stringify(configData));
}

function previewRecurrentChirp() {
  var chirpName = document.getElementById('RECURRENT_ALARM_CHIRP').value;

  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        showToast('Playing: ' + chirpName, 'success');
      } else if (this.status == 403) {
        showToast('Preview blocked: Sleep mode is active', 'error');
      } else {
        showToast('Failed to preview sound', 'error');
      }
    }
  };
  request.open('POST', '/previewchirp', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.send('chirp=' + encodeURIComponent(chirpName) + '&count=1');
}

window.onload = function() {
  getData();
  checkPlatformFeatures();
};
</script>
</head>
<body>
<div class="container">
  <div class="app-header">
    <h1>Device Config</h1>
    <div class="hostname-display">Hostname: <span id="clientid_val">...</span></div>
  </div>

  <nav class="nav-tabs">
    <a href="/">Home</a>
    <a class="active" href="/deviceconfig">Config</a>
    <a href="/mqttconfig">MQTT</a>
    <a href="/system">System</a>
  </nav>

  <div class="main-scroll-content">
    <!-- Device Hostname Button -->
    <button type="button" onclick="openDeviceModal()" class="config-btn">
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <rect x="2" y="2" width="20" height="8" rx="2" ry="2"/>
        <rect x="2" y="14" width="20" height="8" rx="2" ry="2"/>
        <line x1="6" y1="6" x2="6.01" y2="6"/>
        <line x1="6" y1="18" x2="6.01" y2="18"/>
      </svg>
      <span>Device Hostname</span>
    </button>

    <!-- Login Credentials Button -->
    <button type="button" onclick="openLoginModal()" class="config-btn">
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <circle cx="12" cy="12" r="3"/>
        <path d="M12 1v6m0 6v6"/>
        <path d="m4.93 4.93 4.24 4.24m5.66 5.66 4.24 4.24"/>
        <path d="m1 12 6 0m6 0 6 0"/>
        <path d="m4.93 19.07 4.24-4.24m5.66-5.66 4.24-4.24"/>
      </svg>
      <span>Login Credentials</span>
    </button>

    <!-- General Settings Button -->
    <button type="button" onclick="openGeneralModal()" class="config-btn">
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <path d="M12.22 2h-.44a2 2 0 0 0-2 2v.18a2 2 0 0 1-1 1.73l-.43.25a2 2 0 0 1-2 0l-.15-.08a2 2 0 0 0-2.73.73l-.22.38a2 2 0 0 0 .73 2.73l.15.1a2 2 0 0 1 1 1.72v.51a2 2 0 0 1-1 1.74l-.15.09a2 2 0 0 0-.73 2.73l.22.38a2 2 0 0 0 2.73.73l.15-.08a2 2 0 0 1 2 0l.43.25a2 2 0 0 1 1 1.73V20a2 2 0 0 0 2 2h.44a2 2 0 0 0 2-2v-.18a2 2 0 0 1 1-1.73l.43-.25a2 2 0 0 1 2 0l.15.08a2 2 0 0 0 2.73-.73l.22-.38a2 2 0 0 0-.73-2.73l-.15-.1a2 2 0 0 1-1-1.72v-.51a2 2 0 0 1 1-1.74l.15-.09a2 2 0 0 0 .73-2.73l-.22-.38a2 2 0 0 0-2.73-.73l-.15.08a2 2 0 0 1-2 0l-.43-.25a2 2 0 0 1-1-1.73V4a2 2 0 0 0-2-2z"/>
        <circle cx="12" cy="12" r="3"/>
      </svg>
      <span>General</span>
    </button>

    <!-- Clock Settings Button -->
    <button type="button" onclick="openClockModal()" class="config-btn">
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <circle cx="12" cy="12" r="10"/>
        <polyline points="12 6 12 12 16 14"/>
      </svg>
      <span>Clock</span>
    </button>

    <!-- Sleep Mode Settings Button -->
    <button type="button" id="SLEEP_MODE_SETTINGS_BTN" onclick="openSleepModeModal()" class="config-btn">
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"/>
      </svg>
      <span>Sleep Mode</span>
      <svg id="SLEEP_MODE_NOT_SUPPORTED_ICON" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--warning)" stroke-width="2" style="display:none; margin-left: auto;">
        <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/>
        <line x1="12" y1="9" x2="12" y2="13"/>
        <line x1="12" y1="17" x2="12.01" y2="17"/>
      </svg>
    </button>

    <!-- Timer Settings Button (ESP32 only) -->
    <button type="button" id="TIMER_SETTINGS_BTN" onclick="openTimerModal()" class="config-btn">
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <circle cx="12" cy="12" r="10"/>
        <polyline points="12 6 12 12 16 14"/>
        <line x1="12" y1="2" x2="12" y2="4"/>
        <line x1="12" y1="20" x2="12" y2="22"/>
      </svg>
      <span>Timer</span>
      <svg id="TIMER_NOT_SUPPORTED_ICON" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--warning)" stroke-width="2" style="display:none; margin-left: auto;">
        <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/>
        <line x1="12" y1="9" x2="12" y2="13"/>
        <line x1="12" y1="17" x2="12.01" y2="17"/>
      </svg>
    </button>

    <!-- Weather Settings Button -->
    <button type="button" id="WEATHER_SETTINGS_BTN" onclick="openWeatherModal()" class="config-btn">
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <path d="M17.5 19H9a7 7 0 1 1 6.71-9h1.79a4.5 4.5 0 1 1 0 9z"/>
      </svg>
      <span>Weather</span>
      <svg id="WEATHER_NOT_SUPPORTED_ICON" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--warning)" stroke-width="2" style="display:none; margin-left: auto;">
        <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/>
        <line x1="12" y1="9" x2="12" y2="13"/>
        <line x1="12" y1="17" x2="12.01" y2="17"/>
      </svg>
    </button>

    <!-- Crypto Price Ticker Button -->
    <button type="button" id="CRYPTO_SETTINGS_BTN" onclick="openCryptoModal()" class="config-btn">
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <circle cx="12" cy="12" r="10"/>
        <path d="M9 8h5a2 2 0 0 1 0 4H9v4h5a2 2 0 0 0 0-4"/>
        <line x1="9" y1="12" x2="14" y2="12"/>
      </svg>
      <span>Crypto Prices</span>
      <svg id="CRYPTO_NOT_SUPPORTED_ICON" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="var(--warning)" stroke-width="2" style="display:none; margin-left: auto;">
        <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/>
        <line x1="12" y1="9" x2="12" y2="13"/>
        <line x1="12" y1="17" x2="12.01" y2="17"/>
      </svg>
    </button>
  </div>

  <div class="footer-separator"></div>
  <div class="footer-app-title">RDA Msg Board</div>
  <div class="info">
    <div>Version: <span id="version_val">...</span> | Modules: <span id="maxdevices_val">...</span></div>
  </div>
</div>
<div id="toast" class="toast"></div>

<!-- Device Hostname Modal -->
<div id="deviceModal" class="modal-overlay">
  <div class="modal" style="max-width: 450px;">
    <h3 style="color: var(--accent); display: flex; align-items: center; justify-content: center; gap: 10px;">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <rect x="2" y="2" width="20" height="8" rx="2" ry="2"/>
        <rect x="2" y="14" width="20" height="8" rx="2" ry="2"/>
        <line x1="6" y1="6" x2="6.01" y2="6"/>
        <line x1="6" y1="18" x2="6.01" y2="18"/>
      </svg>
      Device Hostname
    </h3>
    <p style="color: var(--subtext); margin-bottom: 20px;">Configure device hostname and identification</p>

    <form id="device_config_form" onsubmit="saveDeviceConfig(event)" style="display: flex; flex-direction: column; overflow: hidden; height: 100%;">
      <div class="modal-scroll-content">
        <label for="DEVICEHOSTNAME">Hostname</label>
        <input type="text" id="DEVICEHOSTNAME" name="Hostname" placeholder="Enter Hostname" required>
      </div>

      <div class="modal-actions" style="grid-template-columns: 1fr 1fr; margin-top: auto; padding-top: 15px; border-top: 1px solid #333;">
        <button type="button" class="btn" onclick="closeDeviceModal()">Cancel</button>
        <button type="submit" class="btn btn-confirm">Save</button>
      </div>
    </form>
  </div>
</div>

<!-- Login Credentials Modal -->
<div id="loginModal" class="modal-overlay">
  <div class="modal" style="max-width: 450px;">
    <h3 style="color: var(--accent); display: flex; align-items: center; justify-content: center; gap: 10px;">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <circle cx="12" cy="12" r="3"/>
        <path d="M12 1v6m0 6v6"/>
        <path d="m4.93 4.93 4.24 4.24m5.66 5.66 4.24 4.24"/>
        <path d="m1 12 6 0m6 0 6 0"/>
        <path d="m4.93 19.07 4.24-4.24m5.66-5.66 4.24-4.24"/>
      </svg>
      Login Credentials
    </h3>
    <p style="color: var(--subtext); margin-bottom: 20px;">Update web interface login credentials</p>

    <form id="login_config_form" onsubmit="saveLoginConfig(event)" style="display: flex; flex-direction: column; overflow: hidden; height: 100%;">
      <div class="modal-scroll-content">
        <label for="LOGINUSERNAME">Username</label>
        <input type="text" id="LOGINUSERNAME" name="Username" placeholder="Enter Username" required>

        <label for="LOGINPASSWORD">Password</label>
        <input type="password" id="LOGINPASSWORD" name="Password" placeholder="Leave empty to keep unchanged">
      </div>

      <div class="modal-actions" style="grid-template-columns: 1fr 1fr; margin-top: auto; padding-top: 15px; border-top: 1px solid #333;">
        <button type="button" class="btn" onclick="closeLoginModal()">Cancel</button>
        <button type="submit" class="btn btn-confirm">Save</button>
      </div>
    </form>
  </div>
</div>

<!-- General Settings Modal -->
<div id="generalModal" class="modal-overlay">
  <div class="modal" style="max-width: 550px;">
    <h3 style="color: var(--accent); display: flex; align-items: center; justify-content: center; gap: 10px;">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <path d="M12.22 2h-.44a2 2 0 0 0-2 2v.18a2 2 0 0 1-1 1.73l-.43.25a2 2 0 0 1-2 0l-.15-.08a2 2 0 0 0-2.73.73l-.22.38a2 2 0 0 0 .73 2.73l.15.1a2 2 0 0 1 1 1.72v.51a2 2 0 0 1-1 1.74l-.15.09a2 2 0 0 0-.73 2.73l.22.38a2 2 0 0 0 2.73.73l.15-.08a2 2 0 0 1 2 0l.43.25a2 2 0 0 1 1 1.73V20a2 2 0 0 0 2 2h.44a2 2 0 0 0 2-2v-.18a2 2 0 0 1 1-1.73l.43-.25a2 2 0 0 1 2 0l.15.08a2 2 0 0 0 2.73-.73l.22-.38a2 2 0 0 0-.73-2.73l-.15-.1a2 2 0 0 1-1-1.72v-.51a2 2 0 0 1 1-1.74l.15-.09a2 2 0 0 0 .73-2.73l-.22-.38a2 2 0 0 0-2.73-.73l-.15.08a2 2 0 0 1-2 0l-.43-.25a2 2 0 0 1-1-1.73V4a2 2 0 0 0-2-2z"/>
        <circle cx="12" cy="12" r="3"/>
      </svg>
      General Settings
    </h3>
    <p style="color: var(--subtext); margin-bottom: 20px;">Configure global device settings</p>

    <form id="general_config_form" onsubmit="saveGeneralConfig(event)" style="display: flex; flex-direction: column; overflow: hidden; height: 100%;">
      <div class="modal-scroll-content">
        <label for="BUZZERENABLE">Global Buzzer</label>
        <div class="toggle-card">
          <span class="label-text">Enable Buzzer</span>
          <label class="switch">
            <input type="checkbox" id="BUZZERENABLE" name="BuzzerEnable" value="on">
            <input type="hidden" id="BUZZERENABLEHIDDEN" name="BuzzerEnable" value="off">
            <span class="slider"></span>
          </label>
        </div>
        <p style="font-size: 0.85rem; color: var(--subtext); margin: -5px 0 15px 0;">
          Master switch for all buzzer sounds (boot, message, alerts)
        </p>

        <!-- Section Separator -->
        <div style="height: 1px; background: rgba(255,255,255,0.1); margin: 20px 0;"></div>

        <label for="BRIGHTNESSOVERRIDEENABLE">Brightness Override</label>
        <div class="toggle-card">
          <span class="label-text">Override All Brightness</span>
          <label class="switch">
            <input type="checkbox" id="BRIGHTNESSOVERRIDEENABLE" name="BrightnessOverrideEnable" value="on" onchange="toggleBrightnessOverride()">
            <input type="hidden" id="BRIGHTNESSOVERRIDEENABLEHIDDEN" name="BrightnessOverrideEnable" value="off">
            <span class="slider"></span>
          </label>
        </div>
        <p style="font-size: 0.85rem; color: var(--subtext); margin: -5px 0 15px 0;">
          Override all brightness settings (messages, clock, timer) with a single value
        </p>
        <div id="BRIGHTNESSOVERRIDECONTAINER" style="display: none;">
          <label for="BRIGHTNESSOVERRIDE">Override Brightness: <span id="BRIGHTNESSOVERRIDE_LABEL" class="val-label">8</span></label>
          <div class="slider-row">
            <input type="range" id="BRIGHTNESSOVERRIDE" name="BrightnessOverrideValue" min="0" max="15" value="8"
                   oninput="document.getElementById('BRIGHTNESSOVERRIDE_LABEL').innerText=this.value" style="flex: 1;">
            <span class="val-label" id="BRIGHTNESSOVERRIDE_LABEL" style="min-width: 30px; text-align: right;">8</span>
          </div>
        </div>
      </div>

      <div class="modal-actions" style="grid-template-columns: 1fr 1fr 1fr; margin-top: auto; padding-top: 15px; border-top: 1px solid #333;">
        <button type="button" class="btn" onclick="closeGeneralModal()">Close</button>
        <button type="button" class="btn" onclick="saveGeneralConfig(event, false)" style="background: #238636; border-color: #238636; color: white;">Apply</button>
        <button type="submit" class="btn btn-confirm">Save & Close</button>
      </div>
    </form>
  </div>
</div>

<!-- Clock Configuration Modal -->
<div id="clockModal" class="modal-overlay">
  <div class="modal" style="max-width: 550px;">
    <h3 style="color: var(--accent); display: flex; align-items: center; justify-content: center; gap: 10px;">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <circle cx="12" cy="12" r="10"/>
        <polyline points="12 6 12 12 16 14"/>
      </svg>
      Clock Configuration
    </h3>
    <p style="color: var(--subtext); margin-bottom: 20px;">Configure the LED clock screensaver</p>

    <form id="clock_config_form" onsubmit="saveClockConfig(event)" style="display: flex; flex-direction: column; overflow: hidden; height: 100%;">
      <div class="modal-scroll-content">
      <!-- Enable Clock Toggle -->
      <div class="toggle-card">
        <span class="label-text">Enable Clock</span>
        <label class="switch">
          <input type="checkbox" id="CLOCKENABLED" name="ClockEnabled" value="on">
          <input type="hidden" id="CLOCKENABLEDHIDDEN" name="ClockEnabled" value="off">
          <span class="slider"></span>
        </label>
      </div>

      <!-- NTP Server -->
      <label for="NTPSERVER">NTP Server</label>
      <input type="text" id="NTPSERVER" name="NtpServer" placeholder="pool.ntp.org" required>

      <!-- Timezone Dropdown (33 zones - more than MQTT's 16) -->
      <label for="TIMEZONE">Timezone</label>
      <select id="TIMEZONE" name="Timezone" onchange="onTimezoneChange()" style="width: 100%; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; font-size: 1rem; margin-bottom: 15px;">
        <option value="UTC">UTC</option>
        <option value="Europe/London">Europe/London (GMT/BST)</option>
        <option value="Europe/Paris">Europe/Paris (CET/CEST)</option>
        <option value="Europe/Berlin">Europe/Berlin (CET/CEST)</option>
        <option value="Europe/Athens">Europe/Athens (EET/EEST)</option>
        <option value="Europe/Moscow">Europe/Moscow (MSK)</option>
        <option value="US/Eastern">US/Eastern (EST/EDT)</option>
        <option value="US/Central">US/Central (CST/CDT)</option>
        <option value="US/Mountain">US/Mountain (MST/MDT)</option>
        <option value="US/Pacific">US/Pacific (PST/PDT)</option>
        <option value="US/Alaska">US/Alaska (AKST/AKDT)</option>
        <option value="US/Hawaii">US/Hawaii (HST)</option>
        <option value="Canada/Pacific">Canada/Pacific (PST/PDT)</option>
        <option value="Canada/Mountain">Canada/Mountain (MST/MDT)</option>
        <option value="Canada/Central">Canada/Central (CST/CDT)</option>
        <option value="Canada/Eastern">Canada/Eastern (EST/EDT)</option>
        <option value="America/Mexico_City">America/Mexico City (CST/CDT)</option>
        <option value="America/Sao_Paulo">America/Sao Paulo (BRT)</option>
        <option value="America/Argentina/Buenos_Aires">America/Buenos Aires (ART)</option>
        <option value="Asia/Dubai">Asia/Dubai (GST)</option>
        <option value="Asia/Karachi">Asia/Karachi (PKT)</option>
        <option value="Asia/Kolkata">Asia/Kolkata (IST)</option>
        <option value="Asia/Bangkok">Asia/Bangkok (ICT)</option>
        <option value="Asia/Singapore">Asia/Singapore (SGT)</option>
        <option value="Asia/Hong_Kong">Asia/Hong Kong (HKT)</option>
        <option value="Asia/Shanghai">Asia/Shanghai (CST)</option>
        <option value="Asia/Tokyo">Asia/Tokyo (JST)</option>
        <option value="Asia/Seoul">Asia/Seoul (KST)</option>
        <option value="Australia/Perth">Australia/Perth (AWST)</option>
        <option value="Australia/Adelaide">Australia/Adelaide (ACST/ACDT)</option>
        <option value="Australia/Brisbane">Australia/Brisbane (AEST)</option>
        <option value="Australia/Sydney">Australia/Sydney (AEST/AEDT)</option>
        <option value="Pacific/Auckland">Pacific/Auckland (NZST/NZDT)</option>
        <option value="Custom">Custom POSIX String</option>
      </select>

      <!-- Hidden input for TzString - always submitted with form -->
      <input type="hidden" id="TZSTRING" name="TzString" value="">

      <!-- Custom POSIX Timezone String (visible when Custom selected) -->
      <div id="CUSTOMTZCONTAINER" style="display: none;">
        <label for="TZSTRING_VISIBLE">Custom Timezone (POSIX TZ String)</label>
        <input type="text" id="TZSTRING_VISIBLE" placeholder="EST5EDT,M3.2.0,M11.1.0" oninput="onCustomTzChange()" style="font-family: monospace; margin-bottom: 15px;">
        <p style="font-size: 0.85rem; color: var(--subtext); margin: -10px 0 5px 0;">
          Examples: <code style="color: var(--accent);">UTC0</code>, <code style="color: var(--accent);">IST-5:30</code>, <code style="color: var(--accent);">CET-1CEST,M3.5.0,M10.5.0/3</code><br>
          <a href="https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv" target="_blank" rel="noopener" style="color: var(--accent); text-decoration: underline;">Find your timezone POSIX string</a>
        </p>
      </div>

      <!-- Brightness Slider -->
      <label for="CLOCKBRIGHTNESS">Clock Brightness: <span id="clockBriVal" class="val-label">5</span></label>
      <div class="slider-row">
        <input type="range" id="CLOCKBRIGHTNESS" name="ClockBrightness" min="0" max="15" value="5"
               oninput="document.getElementById('clockBriVal').innerText=this.value" style="flex: 1;">
        <span class="val-label" id="clockBriVal" style="min-width: 30px; text-align: right;">5</span>
      </div>

      <!-- Clock Face / Font Selection -->
      <label for="CLOCKFACE">Clock Face</label>
      <select id="CLOCKFACE" name="ClockFace" onchange="toggleClockOptions()" style="width: 100%; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; font-size: 1rem; margin-bottom: 5px;">
        <option value="DEFAULT">Default (built-in Parola font)</option>
        <optgroup label="Height 8px — full height">
          <option value="MATRIX_LIGHT">Matrix Light 8px</option>
          <option value="MATRIX_LIGHT_8X">Matrix Light 8px X</option>
        </optgroup>
        <optgroup label="Height 6px — compact, centred">
          <option value="MATRIX_LIGHT_6">Matrix Light 6px</option>
          <option value="MATRIX_LIGHT_6X">Matrix Light 6px X</option>
          <option value="MATRIX_CHUNKY_6">Matrix Chunky 6px</option>
          <option value="MATRIX_CHUNKY_6X">Matrix Chunky 6px X</option>
        </optgroup>
      </select>
      <p style="font-size: 0.85rem; color: var(--subtext); margin: -5px 0 8px 0;">
        Bitmap fonts use narrow 3px-wide digits. Chunky = bold strokes (open-top 4). X = extended variant. Pair 6px with &ldquo;Time with Seconds&rdquo; for 4-module.
      </p>
      <button type="button" onclick="testMatrixFont()" style="padding: 6px 14px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; cursor: pointer; font-size: 0.85rem; margin-bottom: 15px;">Preview Clock Face Font</button>

      <!-- Time Display Format Dropdown (module-aware) -->
      <label for="DATEFORMAT">Time Display Format</label>
      <select id="DATEFORMAT" name="DateFormat" onchange="toggleClockOptions()" style="width: 100%; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; font-size: 1rem; margin-bottom: 5px;">
        <!-- Options populated by JavaScript based on maxdevices -->
      </select>
      <p id="ts_constraint_note" style="display:none; font-size: 0.85rem; color: #f0a040; margin: 0 0 12px 0;">Not available with AM/PM on 4-module builds (exceeds display width).</p>
      <div style="margin-bottom: 10px;"></div>

      <!-- AM/PM mode — segmented pill -->
      <div class="toggle-card" id="ampm_toggle_card">
        <span class="label-text">Time Format</span>
        <div class="seg-control" id="ampm_seg">
          <button type="button" class="seg-btn active" data-val="off" onclick="setAmPm('off')">24h</button>
          <button type="button" class="seg-btn" data-val="on" onclick="setAmPm('on')">12h AM/PM</button>
        </div>
        <input type="hidden" id="CLOCKAMPM" name="ClockAmPm" value="off">
      </div>
      <p id="ampm_constraint_note" style="display:none; font-size: 0.85rem; color: #f0a040; margin: -8px 0 12px 0;"></p>

      <!-- Date Alternation toggle — standard switch -->
      <div class="toggle-card">
        <span class="label-text">Alternate Date Display</span>
        <label class="switch">
          <input type="checkbox" id="DATEALTERNATE" name="DateAlternate" value="on" onchange="toggleClockOptions()">
          <span class="slider round"></span>
        </label>
        <input type="hidden" id="DATEALTERNATEHIDDEN" name="DateAlternate" value="off">
      </div>

      <!-- Alternation durations (shown when dateAlternate=on) -->
      <div id="DATEALTERNATECONTAINER" style="display: none;">
        <label for="CLOCKDISPLAYSECONDS">Clock Duration: <span id="clockDispVal" class="val-label">30</span>s</label>
        <div class="slider-row">
          <input type="range" id="CLOCKDISPLAYSECONDS" name="ClockDisplaySeconds" min="5" max="300" step="5" value="30"
                 oninput="document.getElementById('clockDispVal').innerText=this.value" style="flex: 1;">
          <span style="min-width: 40px; text-align: right; font-size: 0.9rem; color: var(--subtext);">s</span>
        </div>
        <label for="DATEALTERNATESECONDS" style="margin-top: 10px;">Date/Weekday Duration: <span id="dateAltVal" class="val-label">5</span>s</label>
        <div class="slider-row">
          <input type="range" id="DATEALTERNATESECONDS" name="DateAlternateSeconds" min="1" max="60" step="1" value="5"
                 oninput="document.getElementById('dateAltVal').innerText=this.value" style="flex: 1;">
          <span style="min-width: 40px; text-align: right; font-size: 0.9rem; color: var(--subtext);">s</span>
        </div>
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 4px 0 15px 0;">
          Rotates: time &rarr; day of week (Matrix Light only) &rarr; date &rarr; repeat
        </p>
      </div>

      <!-- Custom Date Format (8-module only) -->
      <div id="CUSTOMDATEFORMATCONTAINER" style="display: none;">
        <label for="CUSTOMDATEFORMAT">Custom Date Format (strftime)</label>
        <input type="text" id="CUSTOMDATEFORMAT" name="CustomDateFormat" placeholder="%H:%M %a %b %d" style="font-family: monospace; margin-bottom: 15px;">
        <p style="font-size: 0.85rem; color: var(--subtext); margin: -10px 0 15px 0;">
          Examples: <code style="color: var(--accent);">%H:%M %m/%d</code>, <code style="color: var(--accent);">%a, %b %e</code>
        </p>
      </div>

      <!-- Transition Delay -->
      <label for="TRANSITIONDELAY">Transition Delay (seconds)</label>
      <input type="number" id="TRANSITIONDELAY" name="TransitionDelay" min="0" max="10" step="0.5" value="2">

      <!-- Transition Speed -->
      <label for="TRANSITIONSPEED">Animation Speed (ms)</label>
      <input type="number" id="TRANSITIONSPEED" name="TransitionSpeed" min="10" max="200" step="5" value="40" style="margin-bottom: 15px;">

      <!-- Randomize Transitions Toggle -->
      <div class="toggle-card">
        <span class="label-text">Randomize Animations</span>
        <label class="switch">
          <input type="checkbox" id="RANDOMIZETRANSITION" name="RandomizeTransition" value="on" onchange="toggleTransitionDropdown()">
          <input type="hidden" id="RANDOMIZETRANSITIONHIDDEN" name="RandomizeTransition" value="off">
          <span class="slider"></span>
        </label>
      </div>

      <!-- Transition Effect -->
      <label for="TRANSITIONEFFECT">Transition Animation</label>
      <select id="TRANSITIONEFFECT" name="TransitionEffect" style="width: 100%; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; font-size: 1rem; margin-bottom: 15px;">
        <option value="FADE" selected>Fade</option>
        <option value="DISSOLVE">Dissolve</option>
        <option value="BLINDS">Blinds</option>
        <option value="WIPE">Wipe</option>
        <option value="GROW">Grow Up</option>
        <option value="SCROLL">Scroll Up</option>
        <option value="SCAN">Scan Horizontal</option>
        <option value="OPENING">Opening</option>
        <option value="CLOSING">Closing</option>
        <option value="SCROLL_DOWN">Scroll Down</option>
        <option value="SCROLL_LEFT">Scroll Left</option>
        <option value="SCROLL_RIGHT">Scroll Right</option>
        <option value="SLICE">Slice</option>
        <option value="MESH">Mesh</option>
        <option value="GROW_DOWN">Grow Down</option>
        <option value="SCAN_VERTX">Scan Vertical X</option>
        <option value="SCROLL_UP_LEFT">Scroll Up Left</option>
        <option value="SCROLL_UP_RIGHT">Scroll Up Right</option>
        <option value="OPENING_CURSOR">Opening Cursor</option>
        <option value="SCAN_VERT">Scan Vertical</option>
        <option value="SCROLL_DOWN_LEFT">Scroll Down Left</option>
        <option value="WIPE_CURSOR">Wipe Cursor</option>
        <option value="SCAN_HORIZX">Scan Horizontal X</option>
        <option value="CLOSING_CURSOR">Closing Cursor</option>
        <option value="SCROLL_DOWN_RIGHT">Scroll Down Right</option>
        <option value="PRINT">Instant (No Animation)</option>
      </select>

      <!-- Recurrent Alarm Button (opens second-level modal) -->
      <div style="margin-top: 20px; padding-top: 15px; border-top: 1px solid var(--border);">
        <button type="button" onclick="openRecurrentAlarmModal()" class="config-btn" style="width: 100%; justify-content: center;">
          <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <path d="M18 8A6 6 0 0 0 6 8c0 7-3 9-3 9h18s-3-2-3-9"/>
            <path d="M13.73 21a2 2 0 0 1-3.46 0"/>
            <path d="M12 2v4"/>
          </svg>
          <span>Recurrent Alarm</span>
        </button>
        <p style="font-size: 0.8rem; color: var(--subtext); text-align: center; margin-top: 8px;">Configure interval-based sound alerts</p>
      </div>

      <!-- Alarm Clock Button (opens second-level modal) -->
      <div style="margin-top: 20px; padding-top: 15px; border-top: 1px solid var(--border);">
        <button type="button" onclick="openAlarmModal()" class="config-btn" style="width: 100%; justify-content: center;">
          <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <circle cx="12" cy="13" r="7"/>
            <polyline points="12 9 12 13 15 15"/>
            <path d="M16.51 17.35l-.35 3.83a2 2 0 0 1-2 1.82H9.83a2 2 0 0 1-2-1.82l-.35-3.83m.01-10.7l1.2-1.2a3 3 0 0 1 4.24 0l1.2 1.2"/>
          </svg>
          <span>Alarm Clock</span>
        </button>
        <p style="font-size: 0.8rem; color: var(--subtext); text-align: center; margin-top: 8px;">Configure scheduled alarms (requires clock enabled)</p>
      </div>

      <!-- Action Buttons -->
      </div> <!-- Close modal-scroll-content -->
      <div class="modal-actions" style="grid-template-columns: 1fr 1fr 1fr; margin-top: auto; padding-top: 15px; border-top: 1px solid #333;">
        <button type="button" class="btn" onclick="closeClockModal()">Close</button>
        <button type="button" class="btn" onclick="saveClockConfig(event, false)" style="background: #238636; border-color: #238636; color: white;">Apply</button>
        <button type="submit" class="btn btn-confirm">Save & Close</button>
      </div>
    </form>
  </div>
</div>

<!-- Timer Configuration Modal -->
<div id="timerModal" class="modal-overlay">
  <div class="modal" style="max-width: 550px;">
    <h3 style="color: var(--accent); display: flex; align-items: center; justify-content: center; gap: 10px;">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <circle cx="12" cy="12" r="10"/>
        <polyline points="12 6 12 12 16 14"/>
        <line x1="12" y1="2" x2="12" y2="4"/>
        <line x1="12" y1="20" x2="12" y2="22"/>
      </svg>
      Timer Configuration
    </h3>
    <p style="color: var(--subtext); margin-bottom: 20px;">Configure countdown timer or stopwatch</p>

    <form id="timer_config_form" onsubmit="saveTimerConfig(event)" style="display: flex; flex-direction: column; overflow: hidden; height: 100%;">
      <div class="modal-scroll-content">
      <!-- Enable Timer Toggle -->
      <div class="toggle-card">
        <span class="label-text">Enable Timer</span>
        <label class="switch">
          <input type="checkbox" id="TIMERENABLED" name="TimerEnabled" value="on" onchange="toggleTimerControls()">
          <input type="hidden" id="TIMERENABLEDHIDDEN" name="TimerEnabled" value="off">
          <span class="slider"></span>
        </label>
      </div>

      <!-- Timer Mode -->
      <label for="TIMERMODE">Timer Mode</label>
      <select id="TIMERMODE" name="TimerMode" style="width: 100%; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; font-size: 1rem; margin-bottom: 15px;">
        <option value="countdown">Countdown</option>
        <option value="stopwatch">Stopwatch</option>
      </select>

      <!-- Duration Input (HH:MM:SS) -->
      <label for="TIMERDURATION">Duration (HH:MM:SS)</label>
      <div style="display: flex; gap: 10px; margin-bottom: 15px;">
        <input type="number" id="TIMER_HOURS" min="0" max="23" value="0" placeholder="HH" style="flex: 1; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; text-align: center;">
        <span style="color: var(--text); font-size: 1.5rem; line-height: 48px;">:</span>
        <input type="number" id="TIMER_MINUTES" min="0" max="59" value="5" placeholder="MM" style="flex: 1; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; text-align: center;">
        <span style="color: var(--text); font-size: 1.5rem; line-height: 48px;">:</span>
        <input type="number" id="TIMER_SECONDS" min="0" max="59" value="0" placeholder="SS" style="flex: 1; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; text-align: center;">
      </div>
      <input type="hidden" id="DURATIONSECONDS" name="DurationSeconds" value="300">
      

      <div id="TIMER_CONTROLS_BLOCK">

      <!-- Status Display -->
      <div style="background: #252525; border: 1px solid var(--border); border-radius: 8px; padding: 15px; margin: 15px 0;">
        <div style="font-size: 0.85rem; color: var(--subtext); text-transform: uppercase; letter-spacing: 1px; margin-bottom: 10px;">Timer Status</div>
        <div id="TIMERSTATUS" style="font-size: 1.2rem; color: var(--text); margin-bottom: 5px;">Stopped</div>
        <div id="TIMERDISPLAY" style="font-size: 2rem; color: var(--accent); font-family: monospace; font-weight: bold;">00:00:00</div>
      </div>

      <!-- Control Buttons -->
      <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-bottom: 15px;">
        <button type="button" class="btn" onclick="startTimerControl()" style="background: var(--accent); color: white; border-color: var(--accent);">
          Start
        </button>
        <button type="button" class="btn" onclick="pauseTimerControl()" style="background: #d29922; color: white; border-color: #d29922;">
          Pause
        </button>
        <button type="button" class="btn" onclick="stopTimerControl()" style="background: #da3633; color: white; border-color: #da3633;">
          Stop
        </button>
        <button type="button" class="btn" onclick="resetTimerControl()">
          Reset
        </button>
      </div>
      </div> <!-- Close TIMER_CONTROLS_BLOCK -->

      <!-- Brightness Slider -->
      <label for="TIMERBRIGHTNESS">Timer Brightness: <span id="timerBriVal" class="val-label">7</span></label>
      <div class="slider-row">
        <input type="range" id="TIMERBRIGHTNESS" name="TimerBrightness" min="0" max="15" value="7"
               oninput="document.getElementById('timerBriVal').innerText=this.value" style="flex: 1;">
        <span class="val-label" id="timerBriVal" style="min-width: 30px; text-align: right;">7</span>
      </div>

      <!-- Alert Buzzer Toggle -->
      <div class="toggle-card">
        <span class="label-text">Alert Buzzer</span>
        <label class="switch">
          <input type="checkbox" id="ALERTBUZZER" name="AlertBuzzer" value="on">
          <input type="hidden" id="ALERTBUZZERHIDDEN" name="AlertBuzzer" value="off">
          <span class="slider"></span>
        </label>
      </div>

      <!-- Alert Sound Selection -->
      <label for="ALERTCHIRP">Alert Sound</label>
      <div style="display: flex; gap: 10px; margin-bottom: 15px;">
        <select id="ALERTCHIRP" name="AlertChirp" style="flex: 1; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; font-size: 1rem;">
          <option value="Gentle Dawn">Gentle Dawn</option>
        </select>
        <button type="button" onclick="previewChirp()" class="btn" style="padding: 12px 16px; background: var(--input); border: 1px solid var(--border);" title="Preview Sound">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <polygon points="5 3 19 12 5 21 5 3"></polygon>
          </svg>
        </button>
      </div>

      <!-- Alert Sound Repeat (always visible for all chirp types) -->
      <div style="margin-bottom: 15px;">
        <label for="ALERTBUZZERCOUNT_VISIBLE">Alert Sound Repeat: <span id="beepCountVal" class="val-label">5</span></label>
        <div class="slider-row">
          <input type="range" id="ALERTBUZZERCOUNT_VISIBLE" min="1" max="20" value="5"
                 oninput="document.getElementById('beepCountVal').innerText=this.value; document.getElementById('ALERTBUZZERCOUNT').value=this.value;" style="flex: 1;">
        </div>
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 5px 0 0 0;">
          Number of times to repeat the alert sound (500ms gap between repetitions)
        </p>
      </div>

      <!-- Hidden input for form submission (always present) -->
      <input type="hidden" id="ALERTBUZZERCOUNT" name="AlertBuzzerCount" value="5">

      <!-- Auto-Repeat Toggle -->
      <div class="toggle-card">
        <span class="label-text">Auto-Repeat</span>
        <label class="switch">
          <input type="checkbox" id="AUTOREPEAT" name="AutoRepeat" value="on">
          <input type="hidden" id="AUTOREPEATHIDDEN" name="AutoRepeat" value="off">
          <span class="slider"></span>
        </label>
      </div>
      <p style="font-size: 0.85rem; color: var(--subtext); margin: -5px 0 15px 0;">
        Automatically restart timer after completion
      </p>



      </div> <!-- Close modal-scroll-content -->
      <div class="modal-actions" style="grid-template-columns: 1fr 1fr 1fr; margin-top: auto; padding-top: 15px; border-top: 1px solid #333;">
        <button type="button" class="btn" onclick="closeTimerModal()">Close</button>
        <button type="button" class="btn" onclick="saveTimerConfig(event, false)" style="background: #238636; border-color: #238636; color: white;">Apply</button>
        <button type="submit" class="btn btn-confirm">Save & Close</button>
      </div>
    </form>
  </div>
</div>

<!-- Sleep Mode Configuration Modal -->
<div id="sleepModeModal" class="modal-overlay">
  <div class="modal" style="max-width: 550px;">
    <h3>Sleep Mode Configuration</h3>
    <p>Scheduled blackout or sound muting during configured hours</p>

    <form id="sleep_mode_config_form" onsubmit="saveSleepModeConfig(event)" style="display: flex; flex-direction: column; overflow: hidden; height: 100%;">
      <div class="modal-scroll-content">

        <!-- Enable Sleep Mode Toggle -->
        <div class="toggle-card">
          <span class="label-text">Enable Sleep Mode</span>
          <label class="switch">
            <input type="checkbox" id="SLEEPMODEENABLED" name="SleepModeEnabled" value="on" onchange="toggleMuteOnly()">
            <input type="hidden" id="SLEEPMODEENABLEDHIDDEN" name="SleepModeEnabled" value="off">
            <span class="slider"></span>
          </label>
        </div>

        <!-- Alternate Weekend Timer Toggle -->
        <div class="toggle-card" id="WEEKEND_TOGGLE_CARD">
          <span class="label-text" style="line-height: 1.2;">Use Alternate Weekend Timer<br><span style="font-size: 0.8rem; opacity: 0.7;">Different times for Sat/Sun</span></span>
          <label class="switch">
            <input type="checkbox" id="WEEKENDENABLED" name="WeekendEnabled" value="on" onchange="toggleWeekendTimes()">
            <input type="hidden" id="WEEKENDENABLEDHIDDEN" name="WeekendEnabled" value="off">
            <span class="slider"></span>
          </label>
        </div>

        <!-- Mute Only Toggle -->
        <div class="toggle-card" id="MUTE_ONLY_CARD">
          <span class="label-text">Mute Only (Keep LEDs Active)</span>
          <label class="switch">
            <input type="checkbox" id="SLEEPMODEMUTEONLY" name="SleepModeMuteOnly" value="on">
            <input type="hidden" id="SLEEPMODEMUTEONLYHIDDEN" name="SleepModeMuteOnly" value="off">
            <span class="slider"></span>
          </label>
        </div>
        <p style="font-size: 0.85rem; color: var(--subtext); margin: -10px 0 15px 0; padding: 0 5px;">
          When active, only mute sounds and keep the LED display active.
        </p>

        <!-- Time Inputs Container (Centered) -->
        <div style="display: flex; flex-direction: column; align-items: center; gap: 15px; margin: 20px 0;">
          <!-- On Time Input -->
          <div style="width: 100%; max-width: 200px;">
            <label for="SLEEPMODE_ON_TIME" style="text-align: center; display: block;">Sleep Mode On Time (HH:MM)</label>
            <input type="time" id="SLEEPMODE_ON_TIME" name="SleepModeOnTime" required style="text-align: center; font-size: 1.1rem; padding: 14px; width: 100%;">
          </div>

          <!-- Off Time Input -->
          <div style="width: 100%; max-width: 200px;">
            <label for="SLEEPMODE_OFF_TIME" style="text-align: center; display: block;">Sleep Mode Off Time (HH:MM)</label>
            <input type="time" id="SLEEPMODE_OFF_TIME" name="SleepModeOffTime" required style="text-align: center; font-size: 1.1rem; padding: 14px; width: 100%;">
          </div>
        </div>
        
        <!-- Weekend Time Inputs Container (Centered and hidden by default) -->
        <div id="weekendTimesContainer" style="display: none; flex-direction: column; align-items: center; gap: 15px; margin: 20px 0; padding: 15px; background: rgba(0,0,0,0.05); border-radius: 8px;">
          <!-- Weekend On Time Input -->
          <div style="width: 100%; max-width: 200px;">
            <label for="WEEKEND_ON_TIME" style="text-align: center; display: block; color: var(--accent-color);">Weekend On Time (HH:MM)</label>
            <input type="time" id="WEEKEND_ON_TIME" name="WeekendOnTime" required style="text-align: center; font-size: 1.1rem; padding: 14px; width: 100%;">
          </div>

          <!-- Weekend Off Time Input -->
          <div style="width: 100%; max-width: 200px;">
            <label for="WEEKEND_OFF_TIME" style="text-align: center; display: block; color: var(--accent-color);">Weekend Off Time (HH:MM)</label>
            <input type="time" id="WEEKEND_OFF_TIME" name="WeekendOffTime" required style="text-align: center; font-size: 1.1rem; padding: 14px; width: 100%;">
          </div>
        </div>

        <!-- Status Display -->
        <div style="background: #252525; border: 1px solid var(--border); border-radius: 8px; padding: 15px; margin: 15px 0;">
          <div style="font-size: 0.85rem; color: var(--subtext);">Current Status</div>
          <div id="SLEEPMODESTATUS" style="font-size: 1.2rem; color: var(--text);">Inactive</div>
        </div>

        <!-- Clock Dependency Warning -->
        <div style="background: rgba(255, 152, 0, 0.1); border: 1px solid #ff9800; border-radius: 8px; padding: 12px; margin: 15px 0;">
          <p style="font-size: 0.85rem; color: #ff9800; margin: 0;">
            ⚠️ <strong>Clock Dependency:</strong> Sleep Mode requires Clock to be enabled for time-based scheduling.
            If Clock is <strong>disabled</strong>, Sleep Mode will be <strong>deactivated</strong>.
            If Clock is enabled but <strong>NTP not synced</strong>, Sleep Mode will be <strong>enforced</strong> (fail-safe).
          </p>
        </div>

      </div>

      <div class="modal-actions" style="grid-template-columns: 1fr 1fr 1fr; margin-top: auto; padding-top: 15px; border-top: 1px solid #333;">
        <button type="button" class="btn" onclick="closeSleepModeModal()">Close</button>
        <button type="button" class="btn" onclick="saveSleepModeConfig(event, false)" style="background: #238636; border-color: #238636; color: white;">Apply</button>
        <button type="submit" class="btn btn-confirm">Save & Close</button>
      </div>
    </form>
  </div>
</div>

<!-- Recurrent Alarm Configuration Modal (Second-level modal - higher z-index) -->
<div id="recurrentAlarmModal" class="modal-overlay" style="z-index: 10001;">
  <div class="modal" style="max-width: 550px; max-height: 90vh;">
    <h3 style="color: var(--accent); display: flex; align-items: center; justify-content: center; gap: 10px;">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <path d="M18 8A6 6 0 0 0 6 8c0 7-3 9-3 9h18s-3-2-3-9"/>
        <path d="M13.73 21a2 2 0 0 1-3.46 0"/>
        <path d="M12 2v4"/>
      </svg>
      Recurrent Alarm
    </h3>
    <p style="color: var(--subtext); margin-bottom: 20px;">Configure interval-based sound alerts</p>

    <form id="recurrent_alarm_config_form" onsubmit="saveRecurrentAlarmConfig(event)" style="display: flex; flex-direction: column; overflow: hidden; height: 100%;">
      <div class="modal-scroll-content">

        <!-- Master Enable Toggle -->
        <div class="toggle-card">
          <span class="label-text">Enable Recurrent Alarm</span>
          <label class="switch">
            <input type="checkbox" id="RECURRENT_ALARM_ENABLED" name="RecurrentAlarmEnabled" value="on">
            <input type="hidden" id="RECURRENT_ALARM_ENABLED_HIDDEN" name="RecurrentAlarmEnabled" value="off">
            <span class="slider"></span>
          </label>
        </div>

        <!-- Disable Weekends Toggle -->
        <div class="toggle-card">
          <span class="label-text">Disable on Weekends</span>
          <label class="switch">
            <input type="checkbox" id="RECURRENT_ALARM_DISABLE_WEEKENDS" name="RecurrentAlarmDisableWeekends" value="on">
            <input type="hidden" id="RECURRENT_ALARM_DISABLE_WEEKENDS_HIDDEN" name="RecurrentAlarmDisableWeekends" value="off">
            <span class="slider"></span>
          </label>
        </div>

        <!-- Interval Selection -->
        <label for="RECURRENT_ALARM_INTERVAL">Trigger Interval</label>
        <select id="RECURRENT_ALARM_INTERVAL" name="RecurrentAlarmInterval" style="width: 100%; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; font-size: 1rem; margin-bottom: 15px;">
          <option value="15min">Every 15 minutes</option>
          <option value="30min">Every 30 minutes</option>
          <option value="1hour">Every 1 Hour</option>
          <option value="3hours">Every 3 Hours</option>
          <option value="6hours">Every 6 Hours</option>
          <option value="midday">At Midday Only (12:00)</option>
        </select>

        <!-- Sound Selection -->
        <label for="RECURRENT_ALARM_CHIRP">Alert Sound</label>
        <div style="display: flex; gap: 10px; margin-bottom: 15px;">
          <select id="RECURRENT_ALARM_CHIRP" name="RecurrentAlarmChirp" style="flex: 1; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; font-size: 1rem;">
            <option value="Gentle Dawn">Gentle Dawn</option>
          </select>
          <button type="button" onclick="previewRecurrentChirp()" class="btn" style="padding: 12px 16px; background: var(--input); border: 1px solid var(--border);" title="Preview Sound">
            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <polygon points="5 3 19 12 5 21 5 3"></polygon>
            </svg>
          </button>
        </div>

        <!-- Clock/Buzzer Dependency Warning -->
        <div style="background: rgba(255, 152, 0, 0.1); border: 1px solid #ff9800; border-radius: 8px; padding: 12px; margin: 15px 0;">
          <p style="font-size: 0.85rem; color: #ff9800; margin: 0;">
            ⚠️ <strong>Dependencies:</strong> Recurrent Alarm requires Clock to be enabled with NTP sync. Sounds respect Global Buzzer setting and Sleep Mode (muted when sleep mode is active).
          </p>
        </div>

      </div>

      <div class="modal-actions" style="grid-template-columns: 1fr 1fr 1fr; margin-top: auto; padding-top: 15px; border-top: 1px solid #333;">
        <button type="button" class="btn" onclick="closeRecurrentAlarmModal()">Close</button>
        <button type="button" class="btn" onclick="saveRecurrentAlarmConfig(event, false)" style="background: #238636; border-color: #238636; color: white;">Apply</button>
        <button type="submit" class="btn btn-confirm">Save & Close</button>
      </div>
    </form>
  </div>
</div>

<!-- Alarm Clock Configuration Modal (Second-level modal - higher z-index) -->
<div id="alarmModal" class="modal-overlay" style="z-index: 10001;">
  <div class="modal" style="max-width: 650px; max-height: 90vh;">
    <h3 style="color: var(--accent); display: flex; align-items: center; justify-content: center; gap: 10px;">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <circle cx="12" cy="13" r="7"/>
        <polyline points="12 9 12 13 15 15"/>
        <path d="M16.51 17.35l-.35 3.83a2 2 0 0 1-2 1.82H9.83a2 2 0 0 1-2-1.82l-.35-3.83m.01-10.7l1.2-1.2a3 3 0 0 1 4.24 0l1.2 1.2"/>
      </svg>
      Alarm Clock
    </h3>
    <p id="alarmModalSubtitle" style="color: var(--subtext); margin-bottom: 20px;">Configure alarm clocks</p>

    <form id="alarm_config_form" onsubmit="saveAlarmConfig(event)" style="display: flex; flex-direction: column; overflow: hidden; height: 100%;">
      <div class="modal-scroll-content">

        <!-- Master Enable Toggle -->
        <div class="toggle-card">
          <span class="label-text">Enable Alarm Clock</span>
          <label class="switch">
            <input type="checkbox" id="ALARM_MASTER_ENABLED" name="AlarmMasterEnabled" value="on">
            <input type="hidden" id="ALARM_MASTER_ENABLED_HIDDEN" name="AlarmMasterEnabled" value="off">
            <span class="slider"></span>
          </label>
        </div>

        <!-- Stop All Alerts Button -->
        <button type="button" onclick="stopAllAlerts()" class="btn" style="width: 100%; background: #da3633; border-color: #da3633; color: white; margin: 15px 0;">
          Stop All Active Alarms
        </button>

        <!-- Clock Dependency Warning -->
        <div style="background: rgba(255, 152, 0, 0.1); border: 1px solid #ff9800; border-radius: 8px; padding: 12px; margin: 15px 0;">
          <p style="font-size: 0.85rem; color: #ff9800; margin: 0;">
            ⚠️ <strong>Clock Dependency:</strong> Alarm Clock requires Clock to be enabled with NTP sync. Alarms respect Sleep Mode (muted during sleep hours).
          </p>
        </div>

        <!-- Alarm Items Container -->
        <div id="ALARM_ITEMS_CONTAINER">
          <!-- JavaScript will populate 10 alarm item sections here -->
        </div>

      </div>

      <div class="modal-actions" style="grid-template-columns: 1fr 1fr 1fr; margin-top: auto; padding-top: 15px; border-top: 1px solid #333;">
        <button type="button" class="btn" onclick="closeAlarmModal()">Close</button>
        <button type="button" class="btn" onclick="saveAlarmConfig(event, false)" style="background: #238636; border-color: #238636; color: white;">Apply</button>
        <button type="submit" class="btn btn-confirm">Save & Close</button>
      </div>
    </form>
  </div>
</div>

<!-- Crypto Price Ticker Modal -->
<div id="cryptoModal" class="modal-overlay">
  <div class="modal" style="max-width: 580px;">
    <h3 style="color: var(--accent); display: flex; align-items: center; justify-content: center; gap: 10px;">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <circle cx="12" cy="12" r="10"/>
        <path d="M9 8h5a2 2 0 0 1 0 4H9v4h5a2 2 0 0 0 0-4"/>
        <line x1="9" y1="12" x2="14" y2="12"/>
      </svg>
      Crypto Price Ticker
    </h3>
    <p style="color: var(--subtext); margin-bottom: 20px;">Display live cryptocurrency prices via CoinPaprika API (free, no API key required)</p>

    <form id="crypto_config_form" onsubmit="saveCryptoConfig(event)" style="display: flex; flex-direction: column; overflow: hidden; height: 100%;">
      <div class="modal-scroll-content">

        <!-- Enable Toggle -->
        <div class="toggle-card">
          <span class="label-text">Enable Crypto Ticker</span>
          <label class="switch">
            <input type="checkbox" id="CRYPTOENABLED" name="CryptoEnabled" value="on">
            <span class="slider"></span>
          </label>
        </div>

        <!-- API Key (optional) -->
        <label for="CRYPTO_API_KEY">CoinPaprika API Key <span style="color: var(--subtext); font-weight: normal;">(optional)</span></label>
        <input type="text" id="CRYPTO_API_KEY" name="CryptoApiKey" placeholder="Leave empty for unauthenticated access" autocomplete="off">
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 5px 0 15px 0;">
          Without a key: 1,000 requests/day limit, IP-based. With a free registered key: 20,000/month, account-based.
          Register free at <a href="https://coinpaprika.com/api/" target="_blank" style="color: var(--accent);">coinpaprika.com/api</a>.
        </p>

        <!-- Coins -->
        <label for="CRYPTO_COINS">
          Coin IDs (comma-separated, max 10)
          <span id="CRYPTO_COIN_COUNT" style="font-size: 0.8rem; color: var(--subtext); margin-left: 8px;">2/10 coins</span>
        </label>
        <textarea id="CRYPTO_COINS" name="CryptoCoins" rows="3"
                  oninput="updateCoinCount()"
                  style="font-family: monospace; font-size: 0.9rem; padding: 8px; background: var(--surface); border: 1px solid var(--border); border-radius: 6px; color: var(--text); resize: vertical; width: 100%; box-sizing: border-box;">btc-bitcoin,eth-ethereum</textarea>
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 5px 0 5px 0;">
          Uses CoinPaprika coin IDs — format: <code style="color: var(--accent);">btc-bitcoin</code>, <code style="color: var(--accent);">eth-ethereum</code>
        </p>
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 0 0 15px 0;">
          Find all IDs at <a href="https://coinpaprika.com/coins/" target="_blank" style="color: var(--accent);">coinpaprika.com/coins</a>.
          Popular IDs: <code>sol-solana</code>, <code>bnb-binance-coin</code>, <code>xrp-xrp</code>, <code>ada-cardano</code>, <code>doge-dogecoin</code>, <code>dot-polkadot</code>, <code>ltc-litecoin</code>, <code>avax-avalanche</code>
        </p>

        <!-- Display Currency -->
        <label>Display Currency</label>
        <div style="display: flex; flex-wrap: wrap; gap: 15px; margin: 10px 0 20px 0;">
          <label style="display: flex; align-items: center; gap: 8px; cursor: pointer;">
            <input type="radio" name="CryptoCurrency" value="USD" checked style="width: auto;">
            <span>USD ($)</span>
          </label>
          <label style="display: flex; align-items: center; gap: 8px; cursor: pointer;">
            <input type="radio" name="CryptoCurrency" value="EUR" style="width: auto;">
            <span>EUR</span>
          </label>
          <label style="display: flex; align-items: center; gap: 8px; cursor: pointer;">
            <input type="radio" name="CryptoCurrency" value="GBP" style="width: auto;">
            <span>GBP</span>
          </label>
          <label style="display: flex; align-items: center; gap: 8px; cursor: pointer;">
            <input type="radio" name="CryptoCurrency" value="JPY" style="width: auto;">
            <span>JPY</span>
          </label>
          <label style="display: flex; align-items: center; gap: 8px; cursor: pointer;">
            <input type="radio" name="CryptoCurrency" value="BTC" style="width: auto;">
            <span>BTC</span>
          </label>
          <label style="display: flex; align-items: center; gap: 8px; cursor: pointer;">
            <input type="radio" name="CryptoCurrency" value="ETH" style="width: auto;">
            <span>ETH</span>
          </label>
        </div>

        <!-- Fetch Interval -->
        <label for="CRYPTO_FETCH_INTERVAL">Fetch Interval: <span id="CRYPTO_FETCH_INTERVAL_VAL">30 min</span></label>
        <input type="range" id="CRYPTO_FETCH_INTERVAL" min="0" max="8" value="2" step="1"
               oninput="document.getElementById('CRYPTO_FETCH_INTERVAL_VAL').textContent=stepsLabel(CRYPTO_FETCH_STEPS[this.value])"
               style="width: 100%; margin-bottom: 5px;">
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 0 0 15px 0;">How often to fetch fresh prices from the API. (15 min – 4h) &nbsp;&mdash;&nbsp; At 30-min intervals with 10 coins: ~7,200 calls/month, within the 20,000/month free limit.</p>

        <!-- Display Interval -->
        <label for="CRYPTO_DISPLAY_INTERVAL">Display Interval: <span id="CRYPTO_DISPLAY_INTERVAL_VAL">5 min</span></label>
        <input type="range" id="CRYPTO_DISPLAY_INTERVAL" min="0" max="12" value="2" step="1"
               oninput="document.getElementById('CRYPTO_DISPLAY_INTERVAL_VAL').textContent=stepsLabel(DISPLAY_INTERVAL_STEPS[this.value])"
               style="width: 100%; margin-bottom: 5px;">
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 0 0 15px 0;">How often to interrupt the clock to show prices. Can be more frequent than the fetch interval since data is cached. (1 min – 4h)</p>

        <!-- Scroll Repetitions -->
        <label for="CRYPTO_DISPLAY_REPEAT">Scroll Repetitions: <span id="CRYPTO_REPEAT_VAL">2</span></label>
        <input type="range" id="CRYPTO_DISPLAY_REPEAT" name="CryptoDisplayRepeat" min="1" max="5" value="2" step="1" oninput="document.getElementById('CRYPTO_REPEAT_VAL').textContent=this.value" style="width: 100%; margin-bottom: 5px;">
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 0 0 15px 0;">Number of times to scroll prices before returning to clock. Current scroll always completes before exiting.</p>

        <!-- Brightness -->
        <label for="CRYPTO_BRIGHTNESS">Brightness: <span id="CRYPTO_BRI_VAL">5</span></label>
        <input type="range" id="CRYPTO_BRIGHTNESS" name="CryptoBrightness" min="0" max="15" value="5" oninput="document.getElementById('CRYPTO_BRI_VAL').textContent=this.value" style="width: 100%; margin-bottom: 15px;">

        <!-- Current Status -->
        <div style="background: #252525; border: 1px solid var(--border); border-radius: 8px; padding: 15px; margin: 15px 0;">
          <div style="font-size: 0.85rem; color: var(--subtext);">Current Prices</div>
          <div id="CRYPTO_STATUS" style="font-size: 1rem; color: var(--text); word-break: break-all;">Not configured</div>
          <div id="CRYPTO_LAST_UPDATE" style="font-size: 0.8rem; color: var(--subtext); margin-top: 5px;"></div>
        </div>

        <!-- Refresh Button -->
        <button type="button" onclick="refreshCrypto()" class="btn" style="background: #0366d6; border-color: #0366d6; margin: 10px 0;">
          Refresh Prices Now
        </button>

        <!-- Clock Dependency Note -->
        <div style="background: rgba(255, 152, 0, 0.1); border: 1px solid #ff9800; border-radius: 8px; padding: 12px; margin: 15px 0;">
          <p style="font-size: 0.85rem; color: #ff9800; margin: 0;">
            Note: Crypto display requires Clock to be enabled. Prices will show periodically between clock updates.
          </p>
        </div>

        <!-- Attribution -->
        <div style="background: rgba(255,255,255,0.03); border: 1px solid var(--border); border-radius: 8px; padding: 12px; margin: 15px 0;">
          <p style="font-size: 0.8rem; color: var(--subtext); margin: 0;">
            Data provided by <a href="https://coinpaprika.com/" target="_blank" style="color: var(--accent);">CoinPaprika</a> (free — API key optional, recommended for higher rate limits).
            <strong style="color: var(--text);">Personal use only</strong> per CoinPaprika&rsquo;s free plan terms &mdash; commercial redistribution is not permitted.
            Users are responsible for staying within the 20,000 requests/month free limit.
            See <a href="https://coinpaprika.com/terms-of-use/" target="_blank" style="color: var(--accent);">Terms of Use</a>.
          </p>
        </div>

      </div>

      <div class="modal-actions" style="grid-template-columns: 1fr 1fr 1fr; margin-top: auto; padding-top: 15px; border-top: 1px solid #333;">
        <button type="button" class="btn" onclick="closeCryptoModal()">Close</button>
        <button type="button" class="btn" onclick="saveCryptoConfig(event, false)" style="background: #238636; border-color: #238636; color: white;">Apply</button>
        <button type="submit" class="btn btn-confirm">Save &amp; Close</button>
      </div>
    </form>
  </div>
</div>

<!-- Weather Configuration Modal -->
<div id="weatherModal" class="modal-overlay">
  <div class="modal" style="max-width: 550px;">
    <h3 style="color: var(--accent); display: flex; align-items: center; justify-content: center; gap: 10px;">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
        <path d="M17.5 19H9a7 7 0 1 1 6.71-9h1.79a4.5 4.5 0 1 1 0 9z"/>
      </svg>
      Weather Configuration
    </h3>
    <p style="color: var(--subtext); margin-bottom: 20px;">Display weather information from Open-Meteo API</p>

    <form id="weather_config_form" onsubmit="saveWeatherConfig(event)" style="display: flex; flex-direction: column; overflow: hidden; height: 100%;">
      <div class="modal-scroll-content">

        <!-- Enable Weather Toggle -->
        <div class="toggle-card">
          <span class="label-text">Enable Weather Display</span>
          <label class="switch">
            <input type="checkbox" id="WEATHERENABLED" name="WeatherEnabled" value="on">
            <input type="hidden" id="WEATHERENABLEDHIDDEN" name="WeatherEnabled" value="off">
            <span class="slider"></span>
          </label>
        </div>

        <!-- Location Name -->
        <label for="WEATHER_LOCATION">Location Name (optional)</label>
        <input type="text" id="WEATHER_LOCATION" name="WeatherLocation" placeholder="e.g., New York, NY">
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 5px 0 15px 0;">
          For your reference only - not used for weather lookup
        </p>

        <!-- Coordinates -->
        <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 15px; margin: 10px 0;">
          <div>
            <label for="WEATHER_LATITUDE">Latitude</label>
            <input type="text" id="WEATHER_LATITUDE" name="WeatherLatitude" placeholder="e.g., 40.7128" required pattern="-?\d+\.?\d*">
          </div>
          <div>
            <label for="WEATHER_LONGITUDE">Longitude</label>
            <input type="text" id="WEATHER_LONGITUDE" name="WeatherLongitude" placeholder="e.g., -74.0060" required pattern="-?\d+\.?\d*">
          </div>
        </div>
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 0 0 15px 0;">
          Find coordinates at <a href="https://www.latlong.net/" target="_blank" style="color: var(--accent);">latlong.net</a>
        </p>

        <!-- Temperature Unit -->
        <label>Temperature Unit</label>
        <div style="display: flex; gap: 15px; margin: 10px 0 20px 0;">
          <label style="display: flex; align-items: center; gap: 8px; cursor: pointer;">
            <input type="radio" id="WEATHER_UNIT_C" name="WeatherTempUnit" value="C" checked style="width: auto;">
            <span>Celsius (C)</span>
          </label>
          <label style="display: flex; align-items: center; gap: 8px; cursor: pointer;">
            <input type="radio" id="WEATHER_UNIT_F" name="WeatherTempUnit" value="F" style="width: auto;">
            <span>Fahrenheit (F)</span>
          </label>
        </div>

        <!-- Fetch Interval -->
        <label for="WEATHER_UPDATE_INTERVAL">Fetch Interval: <span id="WEATHER_INTERVAL_VAL">30 min</span></label>
        <input type="range" id="WEATHER_UPDATE_INTERVAL" min="0" max="8" value="4" step="1"
               oninput="document.getElementById('WEATHER_INTERVAL_VAL').textContent=stepsLabel(WEATHER_FETCH_STEPS[this.value])"
               style="width: 100%; margin-bottom: 5px;">
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 0 0 15px 0;">How often to fetch fresh data from the API. (5 min – 2h)</p>

        <!-- Display Interval -->
        <label for="WEATHER_DISPLAY_INTERVAL">Display Interval: <span id="WEATHER_DISPLAY_INTERVAL_VAL">5 min</span></label>
        <input type="range" id="WEATHER_DISPLAY_INTERVAL" min="0" max="12" value="2" step="1"
               oninput="document.getElementById('WEATHER_DISPLAY_INTERVAL_VAL').textContent=stepsLabel(DISPLAY_INTERVAL_STEPS[this.value])"
               style="width: 100%; margin-bottom: 5px;">
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 0 0 15px 0;">How often to interrupt the clock to show weather. Can be more frequent than the fetch interval since data is cached. (1 min – 4h)</p>

        <!-- Scroll Repetitions -->
        <label for="WEATHER_DISPLAY_REPEAT">Scroll Repetitions: <span id="WEATHER_REPEAT_VAL">2</span></label>
        <input type="range" id="WEATHER_DISPLAY_REPEAT" name="WeatherDisplayRepeat" min="1" max="5" value="2" step="1" oninput="document.getElementById('WEATHER_REPEAT_VAL').textContent=this.value" style="width: 100%; margin-bottom: 5px;">
        <p style="font-size: 0.85rem; color: var(--subtext); margin: 0 0 15px 0;">Number of times to scroll weather before returning to clock. Current scroll always completes before exiting.</p>

        <!-- Brightness Slider -->
        <label for="WEATHER_BRIGHTNESS">Brightness: <span id="WEATHER_BRI_VAL">5</span></label>
        <input type="range" id="WEATHER_BRIGHTNESS" name="WeatherBrightness" min="0" max="15" value="5" oninput="document.getElementById('WEATHER_BRI_VAL').textContent=this.value" style="width: 100%; margin-bottom: 15px;">

        <!-- Current Weather Status -->
        <div style="background: #252525; border: 1px solid var(--border); border-radius: 8px; padding: 15px; margin: 15px 0;">
          <div style="font-size: 0.85rem; color: var(--subtext);">Current Weather</div>
          <div id="WEATHER_STATUS" style="font-size: 1.2rem; color: var(--text);">Not configured</div>
          <div id="WEATHER_FORECAST" style="font-size: 0.9rem; color: var(--subtext); margin-top: 5px;"></div>
        </div>

        <!-- Refresh Button -->
        <button type="button" onclick="refreshWeather()" class="btn" style="background: #0366d6; border-color: #0366d6; margin: 10px 0;">
          Refresh Weather Data
        </button>

        <!-- Clock Dependency Warning -->
        <div style="background: rgba(255, 152, 0, 0.1); border: 1px solid #ff9800; border-radius: 8px; padding: 12px; margin: 15px 0;">
          <p style="font-size: 0.85rem; color: #ff9800; margin: 0;">
            Note: Weather display requires Clock to be enabled. Weather will show periodically between clock updates.
          </p>
        </div>

      </div>

      <div class="modal-actions" style="grid-template-columns: 1fr 1fr 1fr; margin-top: auto; padding-top: 15px; border-top: 1px solid #333;">
        <button type="button" class="btn" onclick="closeWeatherModal()">Close</button>
        <button type="button" class="btn" onclick="saveWeatherConfig(event, false)" style="background: #238636; border-color: #238636; color: white;">Apply</button>
        <button type="submit" class="btn btn-confirm">Save & Close</button>
      </div>
    </form>
  </div>
</div>
</body>
</html>
)=====";

const char CHANGEMQTTCONFIG_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>MQTT Config</title>
<style>
:root { --bg: #121212; --card: #1e1e1e; --text: #e0e0e0; --accent: #2ea44f; --input: #2d2d2d; --border: #404040; --subtext: #888; }
body { background: var(--bg); color: var(--text); font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; margin: 0; padding: 20px; text-align: center; height: 100vh; box-sizing: border-box; display: flex; flex-direction: column; overflow: hidden; }
.container { max-width: 650px; margin: 0 auto; background: var(--card); padding: 25px; border-radius: 12px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); max-height: 100%; display: flex; flex-direction: column; width: 100%; box-sizing: border-box; }
.app-header { margin-bottom: 25px; border-bottom: 1px solid var(--border); padding-bottom: 15px; flex-shrink: 0; }
h1 { font-size: 1.8rem; margin: 5px 0; color: #fff; }
.hostname-display { font-size: 0.9rem; color: var(--subtext); background: rgba(255,255,255,0.05); display: inline-block; padding: 4px 12px; border-radius: 12px; margin-top: 5px; }
.hostname-display span { color: #fff; font-weight: 500; }
label { display: block; text-align: left; margin: 10px 0 5px; font-weight: 500; font-size: 0.9rem; }
input[type=text], input[type=password], input[type=number] { width: 100%; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; box-sizing: border-box; font-size: 1rem; }
.not-supported-text { font-size: 0.7rem; color: #ff9800; margin-top: 5px; font-weight: 600; display: block; }
.toggles-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; margin: 15px 0; }
.toggle-card { display: flex; flex-direction: column; align-items: center; justify-content: center; background: #252525; padding: 8px; border-radius: 8px; }
.toggle-card .label-text { margin: 0 0 4px 0; font-weight: 500; font-size: 0.85rem; }
@media (max-width: 500px) { .toggles-grid { grid-template-columns: 1fr; } .toggle-card { flex-direction: row; justify-content: space-between; } .toggle-card .label-text { margin: 0; } }

/* Toggle Switch CSS */
.switch { position: relative; display: inline-block; width: 50px; height: 26px; flex-shrink: 0; }
.switch input { opacity: 0; width: 100%; height: 100%; position: absolute; z-index: 999; cursor: pointer; left: 0; top: 0; margin: 0; }
.slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #404040; transition: .4s; border-radius: 34px; z-index: 1; }
.slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 3px; bottom: 3px; background-color: white; transition: .4s; border-radius: 50%; box-shadow: 0 2px 4px rgba(0,0,0,0.2); }
input:checked ~ .slider { background-color: var(--accent); }
input:focus ~ .slider { box-shadow: 0 0 1px var(--accent); }
input:checked ~ .slider:before { transform: translateX(24px); }
button, input[type=submit] { background: var(--accent); color: white; border: none; padding: 12px; border-radius: 6px; font-size: 1rem; font-weight: 600; cursor: pointer; width: 100%; margin-top: 15px; transition: opacity 0.2s; }
input[type=submit]:hover { opacity: 0.9; }
/* Navigation Tabs */
.nav-tabs { display: flex; justify-content: center; gap: 4px; margin: 0 0 20px; flex-shrink: 0; }
.nav-tabs a { flex: 1; display: flex; align-items: center; justify-content: center; color: #888; text-decoration: none; padding: 12px 20px; background: #252525; border-radius: 8px; font-size: 0.85rem; font-weight: 600; transition: all 0.3s ease; text-transform: uppercase; letter-spacing: 0.5px; }
.nav-tabs a:hover { color: #fff; background: #2d2d2d; }
.nav-tabs a.active { color: #fff; background: var(--accent); box-shadow: 0 2px 8px rgba(46, 164, 79, 0.3); }
.footer-separator { height: 1px; background: #fff; opacity: 0.1; margin: 20px 0 10px; flex-shrink: 0; display: block; }
.footer-app-title { color: var(--accent); font-size: 0.8rem; letter-spacing: 1px; text-transform: uppercase; font-weight: 700; margin-bottom: 5px; flex-shrink: 0; }
.info { font-size: 0.8rem; color: #666; width: 100%; text-align: center; flex-shrink: 0; }
.info span { color: #888; }

.toast { visibility: hidden; min-width: 200px; margin-left: -100px; background-color: #333; color: #fff; text-align: center; border-radius: 6px; padding: 12px 16px; position: fixed; z-index: 20000; left: 50%; bottom: 30px; font-size: 0.9rem; }
.toast.show { visibility: visible; animation: fadein 0.3s, fadeout 0.3s 2.7s; }
.toast.success { background-color: var(--accent); }
.toast.error { background-color: #da3633; }
@keyframes fadein { from { bottom: 0; opacity: 0; } to { bottom: 30px; opacity: 1; } }
@keyframes fadeout { from { bottom: 30px; opacity: 1; } to { bottom: 0; opacity: 0; } }

/* Modal */
.modal-overlay { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.8); z-index: 1000; align-items: center; justify-content: center; }
.modal-overlay.show { display: flex; }
.modal { background: var(--card); border: 1px solid var(--border); border-radius: 12px; padding: 25px; max-width: 400px; width: 90%; text-align: center; }
.modal h3 { margin: 0 0 10px 0; color: #fff; }
.modal p { color: var(--subtext); margin: 0 0 20px 0; font-size: 0.9rem; }
.modal-actions { display: grid; gap: 10px; }
.btn { background: #333; border: 1px solid var(--border); color: #aaa; padding: 10px 16px; border-radius: 6px; cursor: pointer; font-size: 0.9rem; transition: all 0.2s; }
.btn-confirm { background: var(--accent); border: 1px solid var(--accent); color: white; }

/* Scrollable Content & Fixed Footer */
form { display: flex; flex-direction: column; flex: 1; min-height: 0; }
.scrollable-content { flex: 1; overflow-y: auto; overflow-x: hidden; padding: 0 15px; margin-bottom: 15px; }
.scrollable-content::-webkit-scrollbar { width: 8px; }
.scrollable-content::-webkit-scrollbar-track { background: #1a1a1a; border-radius: 4px; }
.scrollable-content::-webkit-scrollbar-thumb { background: #404040; border-radius: 4px; }
.scrollable-content::-webkit-scrollbar-thumb:hover { background: #505050; }
.modal-footer { display: flex; gap: 10px; padding-top: 15px; border-top: 1px solid var(--border); flex-shrink: 0; }
.modal-footer .btn { flex: 1; margin: 0; padding: 12px; }
.btn-primary { background: var(--accent); color: white; border: 1px solid var(--accent); }
.btn-secondary { background: #333; color: #aaa; border: 1px solid var(--border); }
.btn-primary:hover { opacity: 0.9; }
.btn-secondary:hover { background: #3d3d3d; color: #fff; }

</style>
<script>
function removeTrailingInvalidTopic(str) {
  str = str.replace(/\/*$/, '').replace(/\#.*$/, '#').replace(/([^\/])(\#$)/, '$1/#').replace(/([^\/])(\+$)/, '$1/+');
  return str;
}
function SubmitFunction() {
  if(document.getElementById("MQTTONOFF").checked) document.getElementById("MQTTONOFFHIDDEN").disabled = true;
  if(document.getElementById("MQTTANONYMOUS").checked) document.getElementById("MQTTANONYMOUSHIDDEN").disabled = true;
  if(document.getElementById("MQTTALERT").checked) document.getElementById("MQTTALERTHIDDEN").disabled = true;
  if(document.getElementById("MQTTHADISCOVERY").checked) document.getElementById("MQTTHADISCOVERYHIDDEN").disabled = true;
  if(document.getElementById("MQTTMESSAGESENABLE").checked) document.getElementById("MQTTMESSAGESENABLEHIDDEN").disabled = true;
  if(document.getElementById("MQTTTLSENABLED").checked) document.getElementById("MQTTTLSENABLEDHIDDEN").disabled = true;
  var topicInput = document.getElementById("MQTTTOPICPREFIX");
  topicInput.value = removeTrailingInvalidTopic(topicInput.value);
}
function getData() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var parser = new DOMParser();
      var xmlDoc = parser.parseFromString(this.responseText, "text/xml");
      
      document.getElementById("clientid_val").innerText = xmlDoc.getElementsByTagName("clientid")[0].childNodes[0].nodeValue;
      document.getElementById("version_val").innerText = xmlDoc.getElementsByTagName("version")[0].childNodes[0].nodeValue;
      if(xmlDoc.getElementsByTagName("maxdevices").length > 0) {
        document.getElementById("maxdevices_val").innerText = xmlDoc.getElementsByTagName("maxdevices")[0].childNodes[0].nodeValue;
      }

      var getVal = function(tag) { return xmlDoc.getElementsByTagName(tag)[0].childNodes[0].nodeValue; };

      document.getElementById("MQTTONOFF").checked = (getVal("mqttonoff") == "on");
      document.getElementById("MQTTALERT").checked = (getVal("mqttalert") == "on");
      document.getElementById("MQTTHADISCOVERY").checked = (getVal("mqtthadiscovery") == "on");
      document.getElementById("MQTTMESSAGESENABLE").checked = (getVal("mqttmessagesenable") == "on");

      var anon = (getVal("mqttanonymous") == "on");
      var anonCheck = document.getElementById("MQTTANONYMOUS");
      anonCheck.checked = anon;
      toggleAuthFields(anon);

      document.getElementById("MQTTUSERNAME").value = getVal("mqttusername");
      document.getElementById("MQTTSERVERADDRESS").value = getVal("mqttserveraddress");
      document.getElementById("MQTTSERVERPORT").value = getVal("mqttserverport");
      document.getElementById("MQTTTOPICPREFIX").value = removeTrailingInvalidTopic(getVal("mqtttopicprefix"));
      
      // Handle platform and TLS (ESP32 only)
      var platform = xmlDoc.getElementsByTagName("platform").length > 0 ? getVal("platform") : "ESP8266";
      initTLS(platform, xmlDoc);
      
      toggleDependents();
    }
  };
  request.open("GET", "mqttpagevars", true);
  request.send();
}
function toggleAuthFields(isAnonymous) {
  var fields = [document.getElementById("MQTTUSERNAME"), document.getElementById("MQTTPASSWORD")];
  fields.forEach(f => {
    f.disabled = isAnonymous;
    if(isAnonymous) f.value = "";
  });
}
function toggleDependents() {
  var isEnabled = document.getElementById("MQTTONOFF").checked;
  var dependents = ["MQTTANONYMOUS", "MQTTALERT", "MQTTHADISCOVERY", "MQTTMESSAGESENABLE"];
  dependents.forEach(function(id) {
    var el = document.getElementById(id);
    el.disabled = !isEnabled;
    // Visually dim the parent card
    var card = el.closest(".toggle-card");
    if(card) {
      card.style.opacity = isEnabled ? "1" : "0.5";
      card.style.pointerEvents = isEnabled ? "auto" : "none";
    }
  });
}
function showToast(msg, type) {
  var t = document.getElementById('toast');
  t.innerText = msg;
  t.className = 'toast ' + type + ' show';
  setTimeout(function(){ t.className = 'toast'; }, 3000);
}
function saveMqttConfig(event) {
  event.preventDefault();
  SubmitFunction(); // Run preprocessing
  
  var form = document.getElementById('mqtt_form');
  var formData = new FormData(form);
  
  // Re-enable fields immediately so they work for next save without refresh
  document.getElementById("MQTTONOFFHIDDEN").disabled = false;
  document.getElementById("MQTTANONYMOUSHIDDEN").disabled = false;
  document.getElementById("MQTTALERTHIDDEN").disabled = false;
  document.getElementById("MQTTHADISCOVERYHIDDEN").disabled = false;
  document.getElementById("MQTTMESSAGESENABLEHIDDEN").disabled = false;
  document.getElementById("MQTTTLSENABLEDHIDDEN").disabled = false;

  var params = new URLSearchParams(formData).toString();
  
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        getData();  // Refresh all values from server after successful save
        document.getElementById('successModal').classList.add('show');
      } else {
        showToast('Failed to save MQTT settings!', 'error');
      }
    }
  };
  request.open('POST', 'applymqttconfig', true);
  request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  request.send(params);
}
function closeSuccessModal() {
  document.getElementById('successModal').classList.remove('show');
}
// TLS Functions (ESP32 only)
function initTLS(platform, xmlDoc) {
  var getVal = function(tag) { 
    var elements = xmlDoc.getElementsByTagName(tag);
    return elements.length > 0 && elements[0].childNodes.length > 0 ? elements[0].childNodes[0].nodeValue : null;
  };
  
  if (platform === "ESP32") {
    // ESP32: Enable TLS functionality
    var tlsEnabled = getVal("tlsenabled") === "on";
    var certExists = getVal("certexists") === "true";
    
    document.getElementById("MQTTTLSENABLED").checked = tlsEnabled;
    document.getElementById("MQTTTLSENABLED").disabled = false;
    document.getElementById("TLS_TOGGLE_CARD").style.opacity = "1";
    document.getElementById("TLS_TOGGLE_CARD").style.pointerEvents = "auto";
    
    // Show/hide certificate section based on TLS toggle
    toggleTLSSection(tlsEnabled);
    updateCertStatus(certExists);
    
    // Add event listener for TLS toggle
    document.getElementById("MQTTTLSENABLED").addEventListener('change', function() {
      toggleTLSSection(this.checked);
      autoSwitchPort(this.checked);
    });
  } else {
    // ESP8266: Disable TLS functionality
    document.getElementById("MQTTTLSENABLED").checked = false;
    document.getElementById("MQTTTLSENABLED").disabled = true;
    document.getElementById("TLS_TOGGLE_CARD").style.opacity = "0.7";
    document.getElementById("TLS_TOGGLE_CARD").style.pointerEvents = "none";
    document.getElementById("TLS_CERT_SECTION").style.display = "none";
    
    // Show 'Not Supported' message and hide slider
    document.getElementById("TLS_NOT_SUPPORTED_MSG").style.display = "block";
    document.querySelector("#TLS_TOGGLE_CARD .switch").style.display = "none";
  }
}
function toggleTLSSection(show) {
  document.getElementById("TLS_CERT_SECTION").style.display = show ? "block" : "none";
}
function autoSwitchPort(tlsEnabled) {
  var portInput = document.getElementById("MQTTSERVERPORT");
  var currentPort = portInput.value;
  
  if (tlsEnabled && currentPort === "1883") {
    portInput.value = "8883";
    showToast('Port auto-switched to 8883 (TLS)', 'success');
  } else if (!tlsEnabled && currentPort === "8883") {
    portInput.value = "1883";
    showToast('Port auto-switched to 1883 (Plain)', 'success');
  }
}
function updateCertStatus(certExists) {
  var statusText = document.getElementById("CERT_STATUS_TEXT");
  var deleteBtn = document.getElementById("DELETE_CERT_BTN");
  var statusDiv = document.getElementById("CERT_STATUS");
  
  if (certExists) {
    statusText.innerText = "\u2714 Certificate uploaded";
    statusDiv.style.background = "#1a3a1a";
    statusDiv.style.border = "1px solid var(--accent)";
    deleteBtn.style.display = "block";
  } else {
    statusText.innerText = "\u26A0 No certificate uploaded";
    statusDiv.style.background = "#3a1a1a";
    statusDiv.style.border = "1px solid #da3633";
    deleteBtn.style.display = "none";
  }
}
function uploadCertificate() {
  var fileInput = document.getElementById("CERT_FILE");
  var file = fileInput.files[0];
  
  if (!file) {
    showToast('Please select a certificate file', 'error');
    return;
  }
  
  var formData = new FormData();
  formData.append('certificate', file);
  
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        showToast('Certificate uploaded successfully!', 'success');
        updateCertStatus(true);
        fileInput.value = '';
      } else {
        showToast('Failed to upload certificate!', 'error');
      }
    }
  };
  request.open('POST', 'upload_cert', true);
  request.send(formData);
}
function deleteCertificate() {
  if (!confirm('Delete the CA certificate? TLS connections will fail until a new certificate is uploaded.')) {
    return;
  }
  
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        showToast('Certificate deleted', 'success');
        updateCertStatus(false);
      } else {
        showToast('Failed to delete certificate', 'error');
      }
    }
  };
  request.open('GET', 'delete_cert', true);
  request.send();
}
function init() {
  document.getElementById("MQTTANONYMOUS").addEventListener('change', function() {
    toggleAuthFields(this.checked);
  });
  document.getElementById("MQTTONOFF").addEventListener('change', toggleDependents);
  getData();
}
window.onload = init;
</script>
</head>
<body>
<div class="container">
  <div class="app-header">
    <h1>MQTT Config</h1>
    <div class="hostname-display">Hostname: <span id="clientid_val">...</span></div>
  </div>

  <nav class="nav-tabs">
    <a href="/">Home</a>
    <a href="/deviceconfig">Config</a>
    <a class="active" href="/mqttconfig">MQTT</a>
    <a href="/system">System</a>
  </nav>

  <form id="mqtt_form" onsubmit="saveMqttConfig(event)">
    <!-- Scrollable Content Area -->
    <div class="scrollable-content">
    
    <div class="toggles-grid">
      <div class="toggle-card">
        <span class="label-text">Enable MQTT</span>
        <label class="switch">
          <input type="checkbox" id="MQTTONOFF" name="MQTTONOFF">
          <span class="slider"></span>
        </label>
        <input type="hidden" id="MQTTONOFFHIDDEN" name="MQTTONOFF" value="off">
      </div>

      <div class="toggle-card">
        <span class="label-text">Anonymous Mode</span>
        <label class="switch">
          <input type="checkbox" id="MQTTANONYMOUS" name="MQTTANONYMOUS">
          <span class="slider"></span>
        </label>
        <input type="hidden" id="MQTTANONYMOUSHIDDEN" name="MQTTANONYMOUS" value="off">
      </div>

      <div class="toggle-card">
        <span class="label-text">Connect Alert</span>
        <label class="switch">
          <input type="checkbox" id="MQTTALERT" name="MQTTALERT">
          <span class="slider"></span>
        </label>
        <input type="hidden" id="MQTTALERTHIDDEN" name="MQTTALERT" value="off">
      </div>

      <div class="toggle-card">
        <span class="label-text">Home Assistant</span>
        <label class="switch">
          <input type="checkbox" id="MQTTHADISCOVERY" name="MQTTHADISCOVERY">
          <span class="slider"></span>
        </label>
        <input type="hidden" id="MQTTHADISCOVERYHIDDEN" name="MQTTHADISCOVERY" value="off">
      </div>

      <!-- MQTT Messages Toggle -->
      <div class="toggle-card">
        <span class="label-text">MQTT Messages</span>
        <label class="switch">
          <input type="checkbox" id="MQTTMESSAGESENABLE" name="MQTTMESSAGESENABLE">
          <span class="slider"></span>
        </label>
        <input type="hidden" id="MQTTMESSAGESENABLEHIDDEN" name="MQTTMESSAGESENABLE" value="off">
      </div>

      <!-- TLS Toggle (ESP32 only, grayed out on ESP8266) -->
      <div class="toggle-card" id="TLS_TOGGLE_CARD">
        <span class="label-text">TLS/SSL</span>
        <label class="switch">
          <input type="checkbox" id="MQTTTLSENABLED" name="MQTTTLSENABLED">
          <span class="slider"></span>
        </label>
        <span id="TLS_NOT_SUPPORTED_MSG" class="not-supported-text" style="display:none;">Not Supported (ESP32 only)</span>
        <input type="hidden" id="MQTTTLSENABLEDHIDDEN" name="MQTTTLSENABLED" value="off">
      </div>
    </div>

    <label for="MQTTUSERNAME">Username</label>
    <input type="text" id="MQTTUSERNAME" name="MQTTUSERNAME" placeholder="mqtt-user">

    <label for="MQTTPASSWORD">Password</label>
    <input type="password" id="MQTTPASSWORD" name="MQTTPASSWORD" placeholder="Leave empty to keep unchanged">

    <label for="MQTTSERVERADDRESS">Server Address</label>
    <input type="text" id="MQTTSERVERADDRESS" name="MQTTSERVERADDRESS" placeholder="192.168.1.100">

    <label for="MQTTSERVERPORT">Server Port</label>
    <input type="number" id="MQTTSERVERPORT" name="MQTTSERVERPORT" placeholder="1883">

    <label for="MQTTTOPICPREFIX">Topic Prefix</label>
    <input type="text" id="MQTTTOPICPREFIX" name="MQTTTOPICPREFIX" placeholder="rdadotmatrix/generic">

    <div id="TLS_CERT_SECTION" style="display: none; margin-top: 20px; padding: 15px; background: rgba(255,255,255,0.03); border: 1px solid var(--border); border-radius: 8px;">
      <label style="margin: 0 0 10px 0; display: block; font-weight: 600;">TLS Certificate Management</label>
      
      <div id="CERT_STATUS_TEXT" style="padding: 10px; border-radius: 6px; margin-bottom: 10px; border: 2px solid var(--border);">
        <span style="font-weight: 500;">&#9888; No certificate uploaded</span>
      </div>

      <div style="display: flex; gap: 10px; margin-top: 10px;">
        <input type="file" id="CERT_FILE" accept=".pem,.crt,.cer" style="flex: 1; padding: 10px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px;">
        <button type="button" onclick="uploadCertificate()" style="background: var(--accent); padding: 10px 20px; width: auto; margin: 0;">Upload</button>
      </div>

      <button type="button" id="DELETE_CERT_BTN" onclick="deleteCertificate()" style="background: #da3633; margin-top: 10px; display: none;">Delete Certificate</button>
      
      <p style="font-size: 0.85rem; color: var(--subtext); margin: 10px 0 0 0;">
        Upload a CA certificate (.pem) to enable secure MQTT connections. Port will auto-switch to 8883 when TLS is enabled.
      </p>
    </div>

    </div>
    <!-- End Scrollable Content -->

    <!-- Fixed Footer with Buttons -->
    <div class="modal-footer">
      <button type="button" class="btn btn-secondary" onclick="window.location.href='/'">Cancel</button>
      <button type="submit" class="btn btn-primary">Save Changes</button>
    </div>
  </form>

  <div class="footer-separator"></div>
  <div class="footer-app-title">RDA Msg Board</div>
  <div class="info">
    <div>Version: <span id="version_val">...</span> | Modules: <span id="maxdevices_val">...</span></div>
  </div>
</div>

<div id="toast" class="toast"></div>
<div id="successModal" class="modal-overlay">
  <div class="modal">
    <h3 style="color: var(--accent);">&#10003; Settings Saved!</h3>
    <p>Your MQTT configuration has been updated successfully.</p>
    <div class="modal-actions" style="grid-template-columns: 1fr;">
      <button type="button" class="btn btn-confirm" onclick="closeSuccessModal()">OK</button>
    </div>
  </div>
</div>
</body>
</html>
)=====";
