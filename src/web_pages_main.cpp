#include "web_pages_main.h"

const char MAIN_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>MAX7219 RDA - Message Board</title>
<style>
:root { --bg: #121212; --card: #1e1e1e; --text: #e0e0e0; --accent: #2ea44f; --input: #2d2d2d; --border: #404040; --subtext: #888; }
body { background: var(--bg); color: var(--text); font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; margin: 0; padding: 20px; text-align: center; height: 100vh; box-sizing: border-box; display: flex; flex-direction: column; overflow: hidden; }
.container { max-width: 650px; margin: 0 auto; background: var(--card); padding: 25px 25px 10px 25px; border-radius: 12px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); display: flex; flex-direction: column; max-height: 100%; box-sizing: border-box; width: 100%; }
.main-scroll-content { flex: 1; overflow-y: auto; padding: 0 10px 0 0; box-sizing: border-box; }
.main-scroll-content::-webkit-scrollbar { width: 6px; }
.main-scroll-content::-webkit-scrollbar-track { background: rgba(255, 255, 255, 0.05); border-radius: 3px; }
.main-scroll-content::-webkit-scrollbar-thumb { background: rgba(255, 255, 255, 0.2); border-radius: 3px; }
.app-header { margin-bottom: 25px; border-bottom: 1px solid var(--border); padding-bottom: 15px; }
h1 { font-size: 1.8rem; margin: 5px 0; color: #fff; }
.hostname-display { font-size: 0.9rem; color: var(--subtext); background: rgba(255,255,255,0.05); display: inline-block; padding: 4px 12px; border-radius: 12px; margin-top: 5px; }
.hostname-display span { color: #fff; font-weight: 500; }
label { display: block; text-align: left; margin: 10px 0 5px; font-weight: 500; font-size: 0.9rem; }
input[type=text], input[type=password], input[type=number] { width: 100%; padding: 12px; background: var(--input); border: 1px solid var(--border); color: #fff; border-radius: 6px; box-sizing: border-box; font-size: 1rem; }
.icon-btn { border: none; border-radius: 50%; width: 42px; height: 42px; display: flex; align-items: center; justify-content: center; cursor: pointer; flex-shrink: 0; box-shadow: 0 2px 5px rgba(0,0,0,0.2); transition: all 0.2s; margin: 0; }
.icon-btn:hover { transform: scale(1.05); }
.icon-btn:active { transform: scale(0.95); }
.icon-btn svg { width: 22px; height: 22px; fill: white; }
.btn-primary { background: var(--accent); }
.btn-secondary { background: #333; border: 1px solid var(--border); }
.btn-secondary:hover { background: #444; border-color: #666; }
.input-wrapper textarea + .clear-btn { top: 12px; transform: none; }
input[type=range] { flex: 1; margin: 0; accent-color: var(--accent); }
button, input[type=submit] { background: var(--accent); color: white; border: none; padding: 12px; border-radius: 6px; font-size: 1rem; font-weight: 600; cursor: pointer; width: 100%; margin-top: 15px; transition: opacity 0.2s; }
button:hover, input[type=submit]:hover { opacity: 0.9; }
/* Navigation Tabs */
.nav-tabs { display: flex; justify-content: center; gap: 4px; margin: 0 0 20px; flex-shrink: 0; }
.nav-tabs a { flex: 1; display: flex; align-items: center; justify-content: center; color: #888; text-decoration: none; padding: 12px 20px; background: #252525; border-radius: 8px; font-size: 0.85rem; font-weight: 600; transition: all 0.3s ease; text-transform: uppercase; letter-spacing: 0.5px; }
.nav-tabs a:hover { color: #fff; background: #2d2d2d; }
.nav-tabs a.active { color: #fff; background: var(--accent); box-shadow: 0 2px 8px rgba(46, 164, 79, 0.3); }
.footer-separator { height: 1px; background: #fff; opacity: 0.1; margin: 20px 0 10px; }
.footer-app-title { color: var(--accent); font-size: 0.8rem; letter-spacing: 1px; text-transform: uppercase; font-weight: 700; margin-bottom: 5px; }
.info { font-size: 0.8rem; color: #666; width: 100%; text-align: center; }
.info span { color: #888; }
.val-label { color: var(--accent); min-width: 28px; text-align: right; font-weight: 500; }
/* Input wrapper for clear button */
.message-row { display: flex; gap: 10px; align-items: center; margin-bottom: 15px; }
.input-wrapper { position: relative; flex: 1; }
.input-wrapper input { padding-right: 36px; }
.clear-btn { position: absolute; right: 8px; top: 50%; transform: translateY(-50%); background: transparent; border: none; color: #888; cursor: pointer; font-size: 1.1rem; padding: 4px 8px; margin: 0; width: auto; line-height: 1; }
.clear-btn:hover { color: #da3633; }
/* Slider row with default button */
.slider-row { display: flex; align-items: center; gap: 8px; margin: 10px 0; }
.default-btn { background: #333; border: 1px solid var(--border); color: #888; padding: 6px 10px; border-radius: 4px; cursor: pointer; font-size: 0.85rem; margin: 0; width: auto; min-width: 32px; transition: all 0.2s; }
.default-btn:hover { background: var(--accent); color: white; border-color: var(--accent); }
.default-btn:active { transform: scale(0.95); }
/* Controls card */
.controls-card { background: #252525; border: 1px solid var(--border); border-radius: 8px; padding: 15px; margin: 0; }
.controls-card label { margin-top: 0; }
.controls-card label:not(:first-child) { margin-top: 12px; }
.reset-btn { background: transparent; border: 1px solid var(--border); color: #888; padding: 8px 16px; border-radius: 6px; cursor: pointer; font-size: 0.85rem; margin-top: 15px; width: 100%; display: flex; align-items: center; justify-content: center; gap: 6px; transition: all 0.2s; }
.reset-btn:hover { background: #333; color: #fff; border-color: #555; }
.reset-btn:active { transform: scale(0.98); }
#txt_form { display: flex; flex-direction: column; flex: 1; min-height: 0; overflow: hidden; }
/* Modal */
.modal-overlay { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.8); z-index: 1000; align-items: center; justify-content: center; }
.modal-overlay.show { display: flex; }
.modal { background: var(--card); border: 1px solid var(--border); border-radius: 12px; padding: 25px; max-width: 400px; width: 90%; text-align: center; box-sizing: border-box; }
.modal h3 { margin: 0 0 10px 0; color: #fff; }

/* Scrollable Modal Content */
.modal-scroll-content { max-height: 50vh; overflow-y: auto; overflow-x: hidden; padding: 5px 15px 5px 5px; box-sizing: border-box; width: 100%; text-align: left; margin-bottom: 15px; }
.modal-scroll-content::-webkit-scrollbar { width: 8px; }
.modal-scroll-content::-webkit-scrollbar-track { background: rgba(255,255,255,0.05); border-radius: 4px; }
.modal-scroll-content::-webkit-scrollbar-thumb { background: var(--border); border-radius: 4px; }
.modal-scroll-content::-webkit-scrollbar-thumb:hover { background: #666; }
/* Toast */
.toast { visibility: hidden; min-width: 200px; background-color: #333; color: #fff; text-align: center; border-radius: 6px; padding: 12px 16px; position: fixed; z-index: 1000; left: 50%; transform: translateX(-50%); bottom: 30px; font-size: 0.9rem; box-shadow: 0 4px 15px rgba(0,0,0,0.4); font-weight: 500; }
.toast.show { visibility: visible; animation: fadein 0.3s, fadeout 0.3s 2.7s; }
.toast.success { background-color: var(--accent); }
.toast.error { background-color: #da3633; }
@keyframes fadein { from { bottom: 0; opacity: 0; } to { bottom: 30px; opacity: 1; } }
@keyframes fadeout { from { bottom: 30px; opacity: 1; } to { bottom: 0; opacity: 0; } }
/* Shake Animation */
@keyframes shake { 0%, 100% { transform: translateX(0); } 20%, 60% { transform: translateX(-5px); } 40%, 80% { transform: translateX(5px); } }
.shake { animation: shake 0.4s cubic-bezier(.36,.07,.19,.97) both; border-color: #da3633 !important; }
</style>
<script>
function showToast(msg, type) {
  var el = document.getElementById("toast");
  el.innerText = msg;
  // Reset classes to ensure animation restarts
  el.className = "toast " + type + " show";
  // clear any existing timeout
  if (el.timeoutId) clearTimeout(el.timeoutId);
  el.timeoutId = setTimeout(function(){ el.className = "toast"; }, 3000);
}

var STORAGE_KEY = 'rda_msg_settings';

function saveToLocalStorage() {
  var data = {
    MSG: document.getElementById('MSG').value,
    REP: document.getElementById('REP').value,
    BUZ: document.getElementById('BUZ').value,
    DEL: document.getElementById('DEL').value,
    BRI: document.getElementById('BRI').value,
    ALERTCHIRP: document.getElementById('ALERTCHIRP').value
  };
  localStorage.setItem(STORAGE_KEY, JSON.stringify(data));
}

function loadFromLocalStorage() {
  try {
    var data = JSON.parse(localStorage.getItem(STORAGE_KEY));
    return data || null;
  } catch(e) { return null; }
}

function clearMessage() {
  document.getElementById('MSG').value = '';
  saveToLocalStorage();
}

function clearDisplay() {
  var request = new XMLHttpRequest();
  var params = 'MSG=&REP=1&BUZ=0&DEL=35&BRI=7&ASC=1&ALERTCHIRP=Silent';
  request.open('GET', 'arg?' + params, true);
  request.onreadystatechange = function() {
    if (this.readyState == 4 && (this.status == 200 || this.status == 204)) {
      showToast('Display Cleared!', 'success');
    }
  };
  request.send(null);
}

function setDefault(type) {
  var value = document.getElementById(type).value;
  var request = new XMLHttpRequest();
  request.open('GET', 'setdefault?type=' + type + '&value=' + value, true);
  request.onreadystatechange = function() {
    if (this.readyState == 4 && (this.status == 200 || this.status == 204)) {
      showToast('Default saved!', 'success');
    }
  };
  request.send();
}

function showResetModal() {
  document.getElementById('resetModal').classList.add('show');
}

function closeResetModal() {
  document.getElementById('resetModal').classList.remove('show');
}

function confirmResetDefaults() {
  closeResetModal();
  var request = new XMLHttpRequest();
  request.open('GET', 'resetdefaults', true);
  request.onreadystatechange = function() {
    if (this.readyState == 4 && (this.status == 200 || this.status == 204)) {
      // Parse the response to get hardcoded defaults
      var defaults = JSON.parse(this.responseText);
      // Update UI with hardcoded defaults
      ['REP', 'BUZ', 'DEL', 'BRI'].forEach(function(id) {
        var val = defaults[id];
        document.getElementById(id).value = val;
        document.getElementById(id + '_LABEL').innerText = val;
      });
      // Update ALERTCHIRP dropdown and refresh buzzer slider range
      if (defaults.ALERTCHIRP) {
        document.getElementById('ALERTCHIRP').value = defaults.ALERTCHIRP;
        updateBuzzerSliderMax();
      }
      // Clear localStorage
      localStorage.removeItem(STORAGE_KEY);
      showToast('Reset to defaults!', 'success');
    }
  };
  request.send();
}

function SendText() {
  var msgInput = document.getElementById('MSG');
  if (!msgInput.value.trim()) {
    msgInput.classList.add('shake');
    setTimeout(function(){ msgInput.classList.remove('shake'); }, 500);
    showToast('Please enter a message', 'error');
    return;
  }
  
  var request = new XMLHttpRequest();
  var params = 'MSG=' + encodeURIComponent(msgInput.value) +
               '&REP=' + document.getElementById('REP').value +
               '&BUZ=' + document.getElementById('BUZ').value +
               '&DEL=' + document.getElementById('DEL').value +
               '&BRI=' + document.getElementById('BRI').value +
               '&ALERTCHIRP=' + encodeURIComponent(document.getElementById('ALERTCHIRP').value) +
               '&ASC=1';

  request.open('GET', 'arg?' + params, true);
  
  request.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200 || this.status == 204) {
        showToast('Message Sent!', 'success');
      } else if (this.status == 403) {
        showToast('Blocked: Sleep mode is active', 'error');
      } else {
        showToast('Error: ' + this.status, 'error');
      }
    }
  };
  
  try {
    request.send(null);
  } catch(e) {
    showToast('Request failed', 'error');
  }
}

function getData() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var parser = new DOMParser();
      var xmlDoc = parser.parseFromString(this.responseText, 'text/xml');

      document.getElementById('clientid_val').innerText = xmlDoc.getElementsByTagName('clientid')[0].childNodes[0].nodeValue;
      document.getElementById('version_val').innerText = xmlDoc.getElementsByTagName('version')[0].childNodes[0].nodeValue;
      document.getElementById('maxdevices_val').innerText = xmlDoc.getElementsByTagName('maxdevices')[0].childNodes[0].nodeValue;

      // Get flash defaults first
      var flashDefaults = {
        REP: xmlDoc.getElementsByTagName('repeat')[0].childNodes[0].nodeValue,
        BUZ: xmlDoc.getElementsByTagName('buzzer')[0].childNodes[0].nodeValue,
        DEL: xmlDoc.getElementsByTagName('delay')[0].childNodes[0].nodeValue,
        BRI: xmlDoc.getElementsByTagName('brightness')[0].childNodes[0].nodeValue,
        ALERTCHIRP: xmlDoc.getElementsByTagName('alertchirp')[0].childNodes[0].nodeValue
      };

      // Populate chirp dropdown
      var chirpSelect = document.getElementById('ALERTCHIRP');
      chirpSelect.innerHTML = '';
      var chirps = xmlDoc.getElementsByTagName('chirp');
      for (var i = 0; i < chirps.length; i++) {
        var opt = document.createElement('option');
        opt.value = chirps[i].childNodes[0].nodeValue;
        opt.text = chirps[i].childNodes[0].nodeValue;
        chirpSelect.appendChild(opt);
      }

      // Check for localStorage values
      var saved = loadFromLocalStorage();

      // Apply values: localStorage first, then flash defaults
      ['REP', 'BUZ', 'DEL', 'BRI'].forEach(function(id) {
        var val = (saved && saved[id]) ? saved[id] : flashDefaults[id];
        document.getElementById(id).value = val;
        document.getElementById(id + '_LABEL').innerText = val;
      });

      // Apply ALERTCHIRP
      var chirpVal = (saved && saved.ALERTCHIRP) ? saved.ALERTCHIRP : flashDefaults.ALERTCHIRP;
      chirpSelect.value = chirpVal;

      // Load message from localStorage if available
      if (saved && saved.MSG) {
        document.getElementById('MSG').value = saved.MSG;
      }

      // Set up input handlers with localStorage saving
      ['REP', 'BUZ', 'DEL', 'BRI'].forEach(function(id) {
        document.getElementById(id).oninput = function() {
          document.getElementById(id + '_LABEL').innerText = this.value;
          saveToLocalStorage();
        };
      });

      document.getElementById('MSG').oninput = function() {
        saveToLocalStorage();
      };

      chirpSelect.onchange = function() {
        updateBuzzerSliderMax();
        saveToLocalStorage();
      };

      // Apply initial buzzer slider max based on alert sound
      updateBuzzerSliderMax();
    }
  };
  request.open('GET', 'mainpagevars', true);
  request.send();
}

// Update BUZ slider max based on selected alert sound
// Fast Beep: max 100 (legacy rapid beep)
// Other sounds: max 3 (longer chirps)
function updateBuzzerSliderMax() {
  var chirpSelect = document.getElementById('ALERTCHIRP');
  var buzSlider = document.getElementById('BUZ');
  var buzLabel = document.getElementById('BUZ_LABEL');
  var currentVal = parseInt(buzSlider.value);
  var previousMax = parseInt(buzSlider.max);

  if (chirpSelect.value === 'Fast Beep') {
    buzSlider.max = 100;
    // Set to 10 if switching from long alert sounds (previous max was 3)
    if (previousMax <= 3 && currentVal <= 3) {
      buzSlider.value = 10;
      buzLabel.innerText = '10';
      saveToLocalStorage();
    }
  } else {
    buzSlider.max = 3;
    // Set to 1 if current value exceeds new max (most common setting for longer sounds)
    if (currentVal > 3) {
      buzSlider.value = 1;
      buzLabel.innerText = '1';
      saveToLocalStorage();
    }
  }
  // Force visual refresh of slider position
  buzSlider.style.setProperty('--value', buzSlider.value);
  buzSlider.dispatchEvent(new Event('input', { bubbles: true }));
}

window.onload = getData;
</script>
</head>
<body>
<div class="container">
  <div class="app-header">
    <h1>Message Controller</h1>
    <div class="hostname-display">Hostname: <span id="clientid_val">...</span></div>
  </div>

  <nav class="nav-tabs">
    <a class="active" href="/">Home</a>
    <a href="/deviceconfig">Config</a>
    <a href="/mqttconfig">MQTT</a>
    <a href="/system">System</a>
  </nav>

  <form id="txt_form" onsubmit="event.preventDefault(); SendText();">
    <label>Message</label>
    <div class="message-row">
      <div class="input-wrapper">
        <input type="text" id="MSG" maxlength="999" placeholder="Enter message...">
        <button type="button" class="clear-btn" onclick="clearMessage()" title="Clear text">&times;</button>
      </div>
      <button type="button" class="icon-btn btn-secondary" onclick="clearDisplay()" title="Clear Display">
        <svg viewBox="0 0 24 24"><path d="M19.36 2.72l1.42 1.42-5.72 5.71c1.07 1.54 1.22 3.39.32 4.59L9.06 8.12c1.2-.9 3.05-.75 4.59.32l5.71-5.72zM5.93 17.57c-2.01-2.01-3.24-4.41-3.58-6.65l4.89-2.09 7.44 7.44-2.09 4.89c-2.24-.34-4.64-1.57-6.66-3.59z"/></svg>
      </button>
      <button type="submit" class="icon-btn btn-primary" title="Send Message">
         <svg viewBox="0 0 24 24"><path d="M2.01 21L23 12 2.01 3 2 10l15 2-15 2z"/></svg>
      </button>
    </div>
    
    <div class="main-scroll-content">
      <div class="controls-card">
        <label>Repeat</label>
        <div class="slider-row">
          <input type="range" id="REP" min="0" max="100">
          <span id="REP_LABEL" class="val-label">0</span>
          <button type="button" class="default-btn" onclick="setDefault('REP')" title="Set as default">&#128204;</button>
        </div>

        <label>Scroll Delay</label>
        <div class="slider-row">
          <input type="range" id="DEL" min="0" max="100">
          <span id="DEL_LABEL" class="val-label">0</span>
          <button type="button" class="default-btn" onclick="setDefault('DEL')" title="Set as default">&#128204;</button>
        </div>
        
        <label>Brightness</label>
        <div class="slider-row">
          <input type="range" id="BRI" min="0" max="15">
          <span id="BRI_LABEL" class="val-label">0</span>
          <button type="button" class="default-btn" onclick="setDefault('BRI')" title="Set as default">&#128204;</button>
        </div>

        <label>Buzzer</label>
        <div class="slider-row">
          <input type="range" id="BUZ" min="0" max="100">
          <span id="BUZ_LABEL" class="val-label">0</span>
          <button type="button" class="default-btn" onclick="setDefault('BUZ')" title="Set as default">&#128204;</button>
        </div>

        <label>Alert Sound</label>
        <div class="slider-row">
          <select id="ALERTCHIRP" style="flex:1;padding:8px;background:var(--input);border:1px solid var(--border);color:#fff;border-radius:6px;font-size:0.9rem;"></select>
          <button type="button" class="default-btn" onclick="setDefault('ALERTCHIRP')" title="Set as default">&#128204;</button>
        </div>

        <button type="button" class="reset-btn" onclick="showResetModal()" title="Reset all to factory defaults">&#8634; Reset to Defaults</button>
      </div>
    </div>
  </form>

  <div class="footer-separator"></div>
  <div class="footer-app-title">RDA Msg Board</div>
  <div class="info">
    <div>Version: <span id="version_val">...</span> | Modules: <span id="maxdevices_val">...</span></div>
  </div>
</div>
<div id="toast" class="toast"></div>

<!-- Reset Confirmation Modal -->
<div id="resetModal" class="modal-overlay">
  <div class="modal">
    <h3>Reset to Defaults?</h3>
    <p>This will reset all slider values (Repeat, Buzzer, Scroll Delay, and Brightness) to factory defaults.</p>
    <div class="modal-actions">
      <button type="button" class="btn" onclick="closeResetModal()">Cancel</button>
      <button type="button" class="btn btn-confirm" onclick="confirmResetDefaults()">Reset</button>
    </div>
  </div>
</div>
</body>
</html>
)=====";

const char UPDATE_page[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>System</title>
<style>
:root { --bg: #121212; --card: #1e1e1e; --text: #e0e0e0; --accent: #2ea44f; --danger: #da3633; --input: #2d2d2d; --border: #404040; --btn-sec: #30363d; --subtext: #888; --warning: #d29922; }
body { background: var(--bg); color: var(--text); font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; margin: 0; padding: 20px; text-align: center; height: 100vh; box-sizing: border-box; display: flex; flex-direction: column; overflow: hidden; }
.container { max-width: 650px; margin: 0 auto; background: var(--card); padding: 25px; border-radius: 12px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); box-sizing: border-box; display: flex; flex-direction: column; max-height: 100%; width: 100%; }
.app-header { margin-bottom: 25px; border-bottom: 1px solid var(--border); padding-bottom: 15px; flex-shrink: 0; }
.main-scroll-content { flex: 1; overflow-y: auto; padding: 5px 15px 5px 5px; box-sizing: border-box; }
.main-scroll-content::-webkit-scrollbar { width: 6px; }
.main-scroll-content::-webkit-scrollbar-track { background: rgba(255, 255, 255, 0.05); border-radius: 3px; }
.main-scroll-content::-webkit-scrollbar-thumb { background: rgba(255, 255, 255, 0.2); border-radius: 3px; }
h1 { font-size: 1.8rem; margin: 5px 0; color: #fff; }
.hostname-display { font-size: 0.9rem; color: var(--subtext); background: rgba(255,255,255,0.05); display: inline-block; padding: 4px 12px; border-radius: 12px; margin-top: 5px; }
.hostname-display span { color: #fff; font-weight: 500; }
/* Tab Navigation */
.tab-nav { display: flex; gap: 8px; margin-bottom: 20px; border-bottom: 2px solid var(--border); }
.tab-btn { background: transparent; border: none; color: var(--subtext); padding: 12px 20px; cursor: pointer; font-size: 0.95rem; font-weight: 500; border-bottom: 2px solid transparent; margin-bottom: -2px; transition: all 0.2s; }
.tab-btn:hover { color: var(--text); }
.tab-btn.active { color: var(--accent); border-bottom-color: var(--accent); }
.tab-content { display: none; }
.tab-content.active { display: block; }
/* Drop Zone CSS */
.drop-zone { width: 100%; max-width: 100%; padding: 40px 20px; background: var(--input); border: 2px dashed var(--border); border-radius: 10px; box-sizing: border-box; margin: 20px 0; cursor: pointer; transition: all 0.2s; position: relative; }
.drop-zone:hover { border-color: var(--accent); background: #333; }
.drop-zone--over { border-style: solid; border-color: var(--accent); background: rgba(46, 164, 79, 0.1); }
.drop-zone__input { display: none; }
.drop-zone__prompt { color: #888; font-size: 1rem; pointer-events: none; }
.drop-zone__prompt i { display: block; font-size: 2rem; margin-bottom: 10px; color: var(--accent); font-style: normal; }
/* End Drop Zone CSS */
button, input[type=submit] { background: var(--accent); color: white; border: none; padding: 12px; border-radius: 6px; font-size: 1rem; font-weight: 600; cursor: pointer; width: 100%; transition: opacity 0.2s; }
button:hover, input[type=submit]:hover { opacity: 0.9; }
.actions { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 15px; }
.btn-danger { background: var(--danger); }
.btn-sec { background: var(--btn-sec); }
.backup-actions { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-bottom: 15px; }
.btn { background: #333; border: 1px solid var(--border); color: #aaa; padding: 10px 16px; border-radius: 6px; cursor: pointer; font-size: 0.9rem; transition: all 0.2s; display: flex; align-items: center; justify-content: center; gap: 6px; }
.btn:hover { background: #444; color: #fff; border-color: #555; }
.btn-primary { background: var(--accent); border-color: var(--accent); color: white; }
.btn-primary:hover { opacity: 0.9; background: var(--accent); }
.file-input-wrapper { position: relative; overflow: hidden; display: block; }
.file-input-wrapper input[type=file] { position: absolute; left: 0; top: 0; opacity: 0; cursor: pointer; width: 100%; height: 100%; }
/* Navigation Tabs */
.nav-tabs { display: flex; justify-content: center; gap: 4px; margin: 0 0 20px; flex-shrink: 0; }
.nav-tabs a { flex: 1; display: flex; align-items: center; justify-content: center; color: #888; text-decoration: none; padding: 12px 20px; background: #252525; border-radius: 8px; font-size: 0.85rem; font-weight: 600; transition: all 0.3s ease; text-transform: uppercase; letter-spacing: 0.5px; }
.nav-tabs a:hover { color: #fff; background: #2d2d2d; }
.nav-tabs a.active { color: #fff; background: var(--accent); box-shadow: 0 2px 8px rgba(46, 164, 79, 0.3); }
.footer-separator { height: 1px; background: #fff; opacity: 0.1; margin: 20px 0 10px; flex-shrink: 0; }
.footer-app-title { color: var(--accent); font-size: 0.8rem; letter-spacing: 1px; text-transform: uppercase; font-weight: 700; margin-bottom: 5px; flex-shrink: 0; }
.info { font-size: 0.8rem; color: #666; width: 100%; text-align: center; flex-shrink: 0; }
.info span { color: #888; }
/* Toast */
.toast { visibility: hidden; min-width: 200px; margin-left: -100px; background-color: #333; color: #fff; text-align: center; border-radius: 6px; padding: 12px 16px; position: fixed; z-index: 2000; left: 50%; bottom: 30px; font-size: 0.9rem; }
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
.modal .filename { background: var(--input); padding: 8px 12px; border-radius: 4px; font-family: monospace; font-size: 0.85rem; color: var(--accent); margin-bottom: 20px; word-break: break-all; }
.modal-actions { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
.modal-actions .btn { margin: 0; }
.btn-cancel { background: #333; border: 1px solid var(--border); color: #aaa; }
.btn-confirm { background: var(--accent); border: 1px solid var(--accent); color: white; }
/* Overlay & Spinner */
.overlay { position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.85); display: flex; align-items: center; justify-content: center; z-index: 1000; flex-direction: column; }
.overlay .modal { background: var(--card); padding: 40px; border-radius: 12px; box-shadow: 0 10px 25px rgba(0,0,0,0.5); text-align: center; border: 1px solid var(--border); max-width: 90%; }
.spinner { border: 4px solid rgba(255, 255, 255, 0.1); border-left: 4px solid var(--accent); border-radius: 50%; width: 50px; height: 50px; animation: spin 1s linear infinite; margin: 0 auto 20px; }
@keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
.sub-text { color: var(--subtext); margin-top: 10px; font-size: 0.9rem; }
</style>
<script>
function showToast(msg, type) {
  var t = document.getElementById('toast');
  t.innerText = msg;
  t.className = 'toast ' + type + ' show';
  setTimeout(function(){ t.className = 'toast'; }, 3000);
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
      if(xmlDoc.getElementsByTagName("defaulthostname").length > 0) {
        document.getElementById("default_ssid").innerText = xmlDoc.getElementsByTagName("defaulthostname")[0].childNodes[0].nodeValue;
      }
      if(xmlDoc.getElementsByTagName("defaultuser").length > 0) {
        document.getElementById("default_user").innerText = xmlDoc.getElementsByTagName("defaultuser")[0].childNodes[0].nodeValue;
      }
      if(xmlDoc.getElementsByTagName("defaultpass").length > 0) {
        document.getElementById("default_pass").innerText = xmlDoc.getElementsByTagName("defaultpass")[0].childNodes[0].nodeValue;
      }
    }
  };
  request.open("GET", "updatevars", true);
  request.send();
}

function switchTab(tabName) {
  // Hide all tabs
  var tabs = document.querySelectorAll('.tab-content');
  tabs.forEach(function(tab) { tab.classList.remove('active'); });
  
  // Remove active from all buttons
  var btns = document.querySelectorAll('.tab-btn');
  btns.forEach(function(btn) { btn.classList.remove('active'); });
  
  // Show selected tab
  document.getElementById(tabName).classList.add('active');
  document.getElementById(tabName + '-btn').classList.add('active');
}

function initDropZone(dropZoneClass, inputClass, promptClass, acceptedTypes) {
  const dropZoneElement = document.querySelector(dropZoneClass);
  const inputElement = document.querySelector(inputClass);
  const promptElement = document.querySelector(promptClass);

  dropZoneElement.addEventListener("click", e => inputElement.click());

  inputElement.addEventListener("change", e => {
    if (inputElement.files.length) {
      updateThumbnail(promptElement, inputElement.files[0]);
    }
  });

  dropZoneElement.addEventListener("dragover", e => {
    e.preventDefault();
    dropZoneElement.classList.add("drop-zone--over");
  });

  ['dragleave', 'dragend'].forEach(type => {
    dropZoneElement.addEventListener(type, e => {
      dropZoneElement.classList.remove("drop-zone--over");
    });
  });

  dropZoneElement.addEventListener("drop", e => {
    e.preventDefault();
    if (e.dataTransfer.files.length) {
      inputElement.files = e.dataTransfer.files;
      updateThumbnail(promptElement, e.dataTransfer.files[0]);
    }
    dropZoneElement.classList.remove("drop-zone--over");
  });

  function updateThumbnail(promptElement, file) {
    promptElement.innerHTML = `<i>&#128196;</i>Selected: <span style='color:white'>${file.name}</span>`;
  }
}

function performUpdate(e) {
  e.preventDefault();
  var fI = document.querySelector('.drop-zone__input--firmware');
  if(!fI.files.length) { alert('Select a file!'); return; }
  document.getElementById('updateOverlay').style.display = 'flex';
  var formData = new FormData(document.getElementById('update_form'));
  var req = new XMLHttpRequest();
  req.onreadystatechange = function() {
    if (this.readyState == 4) {
      var h3 = document.querySelector('#updateOverlay .modal h3');
      var p = document.querySelector('#updateOverlay .sub-text');
      if (this.status == 200 && !this.responseText.includes("Failed")) {
        h3.innerText = 'Update Successful!';
        h3.style.color = 'var(--accent)';
        p.innerText = 'Device Rebooting... Redirecting in 30s.';
        setTimeout(function(){ window.location.href = '/'; }, 30000);
      } else {
        h3.innerText = 'Update Failed!';
        h3.style.color = '#da3633';
        p.innerText = 'Please check file and try again.';
        document.querySelector('.spinner').style.borderColor = '#da3633';
      }
    }
  };
  req.open('POST', '/submitupdate');
  req.send(formData);
}

function exportConfig() {
  var request = new XMLHttpRequest();
  request.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var blob = new Blob([this.responseText], {type: 'application/json'});
      var url = URL.createObjectURL(blob);
      var a = document.createElement('a');
      a.href = url;
      var hostname = document.getElementById('clientid_val').innerText || 'rda';
      a.download = hostname + '_config_backup.json';
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
      showToast('Config exported!', 'success');
    }
  };
  request.open('GET', 'exportconfig', true);
  request.send();
}

function triggerImport() {
  var input = document.querySelector('.drop-zone__input--config');
  if (!input.files.length) {
    showToast('Please select a config file first!', 'error');
    return;
  }
  var file = input.files[0];
  window.pendingImportFile = file;
  document.getElementById('importFilename').innerText = file.name;
  document.getElementById('importModal').classList.add('show');
}

function cancelImport() {
  document.getElementById('importModal').classList.remove('show');
  window.pendingImportFile = null;
}

function confirmImport() {
  document.getElementById('importModal').classList.remove('show');
  
  var file = window.pendingImportFile;
  if (!file) return;
  
  // Validate JSON before sending
  var reader = new FileReader();
  reader.onload = function(e) {
    var content = e.target.result;
    
    try {
      var parsed = JSON.parse(content);
      if (!parsed.hostname && !parsed.mqtt && !parsed.defaults) {
        showToast('Invalid config format!', 'error');
        return;
      }
    } catch(err) {
      showToast('Invalid JSON file!', 'error');
      return;
    }
    
    // Send as multipart form data to trigger upload handler
    var formData = new FormData();
    formData.append('config', file);
    
    var request = new XMLHttpRequest();
    request.onreadystatechange = function() {
      if (this.readyState == 4) {
        if (this.status == 200) {
          showToast('Config restored successfully! Rebooting...', 'success');
          
          // Trigger reboot
          var req = new XMLHttpRequest();
          req.open('GET', 'reboot', true);
          req.send();
          
          document.getElementById('rebootOverlay').style.display = 'flex';
          setTimeout(function(){ location.href = '/'; }, 30000);
        } else {
          showToast('Import failed: ' + this.responseText, 'error');
        }
      }
    };
    request.open('POST', 'importconfig', true);
    // Don't set Content-Type - let browser set it with boundary for multipart
    request.send(formData);
  };
  reader.readAsText(file);
  window.pendingImportFile = null;
}


function confirmAction(action) {
  var modal = document.getElementById('confirmModal');
  var h3 = modal.querySelector('h3');
  var p = modal.querySelector('p');
  var btn = document.getElementById('confirmBtn');
  
  // Reset basic styles
  btn.style.background = '';
  btn.style.borderColor = '';
  
  if(action === 'reboot') {
    h3.innerText = 'Reboot System?';
    p.innerText = 'Are you sure you want to reboot the device?';
    btn.innerText = 'Reboot';
    btn.onclick = function() {
      // Fire reboot request
      var req = new XMLHttpRequest();
      req.open('GET', 'reboot', true);
      req.send();
      
      closeConfirm();
      document.getElementById('rebootOverlay').style.display = 'flex';
      setTimeout(function(){ location.href = '/'; }, 20000);
    };
  } else if(action === 'wipe') {
    h3.innerText = 'Factory Reset?';
    p.innerText = 'This will wipe all configuration settings! This cannot be undone.';
    btn.innerText = 'Wipe Config';
    btn.style.background = '#da3633';
    btn.style.borderColor = '#da3633';
    btn.onclick = function() {
      // Fire reset request
      var req = new XMLHttpRequest();
      req.open('GET', 'factoryreset', true);
      req.send();
      
      closeConfirm();
      document.getElementById('resetOverlay').style.display = 'flex';
      // No auto-redirect - user will click button when ready
    };
  }
  modal.classList.add('show');
}

function closeConfirm() {
  document.getElementById('confirmModal').classList.remove('show');
}

window.onload = function() {
  getData();
  switchTab('backup-tab'); // Show backup tab by default
  initDropZone('.drop-zone--firmware', '.drop-zone__input--firmware', '.drop-zone__prompt--firmware');
  initDropZone('.drop-zone--config', '.drop-zone__input--config', '.drop-zone__prompt--config');
};
</script>
</head>
<body>
<div class="container">
  <div class="app-header">
    <h1>System</h1>
    <div class="hostname-display">Hostname: <span id="clientid_val">...</span></div>
  </div>

  <nav class="nav-tabs">
    <a href="/">Home</a>
    <a href="/deviceconfig">Config</a>
    <a href="/mqttconfig">MQTT</a>
    <a class="active" href="/system">System</a>
  </nav>

  <div class="main-scroll-content">
    <div class="tab-nav">
      <button id="backup-tab-btn" class="tab-btn" onclick="switchTab('backup-tab')">Config Backup</button>
      <button id="firmware-tab-btn" class="tab-btn" onclick="switchTab('firmware-tab')">Firmware Update</button>
    </div>
    
    <!-- Config Backup Tab -->
    <div id="backup-tab" class="tab-content">
      <div class="drop-zone drop-zone--config">
        <span class="drop-zone__prompt drop-zone__prompt--config"><i>&#8681;</i>Drop config file here or click to upload</span>
        <input type="file" name="config" accept=".json" class="drop-zone__input drop-zone__input--config">
      </div>
      
      <div class="backup-actions">
        <button type="button" class="btn btn-primary" onclick="exportConfig()">&#128190; Export</button>
        <button type="button" class="btn btn-primary" onclick="triggerImport()">&#128194; Import</button>
      </div>
    </div>
    
    <!-- Firmware Update Tab -->
    <div id="firmware-tab" class="tab-content">
      <form id="update_form" method='POST' action='/submitupdate' enctype='multipart/form-data' onsubmit='performUpdate(event)'>
        <div class="drop-zone drop-zone--firmware">
          <span class="drop-zone__prompt drop-zone__prompt--firmware"><i>&#8681;</i>Drop firmware file here or click to upload</span>
          <input type="file" name="update" class="drop-zone__input drop-zone__input--firmware">
        </div>
        <input type="submit" value="Update">
      </form>
    </div>

    <div class="footer-separator" style="margin: 30px 0 20px;"></div>
    
    <div class="actions">
      <button onclick="confirmAction('wipe')" class="btn-danger">Wipe Config</button>
      <button onclick="confirmAction('reboot')" class="btn-sec">Reboot</button>
    </div>
  </div>

  <div class="footer-separator"></div>
  <div class="footer-app-title">RDA Msg Board</div>
  <div class="info">
    <div>Version: <span id="version_val">...</span> | Modules: <span id="maxdevices_val">...</span></div>
  </div>
</div>

<div id="toast" class="toast"></div>

<div id="confirmModal" class="modal-overlay">
  <div class="modal">
    <h3>Are you sure?</h3>
    <p>Confirm action</p>
    <div class="modal-actions">
      <button type="button" class="btn btn-cancel" onclick="closeConfirm()">Cancel</button>
      <button type="button" class="btn btn-confirm" id="confirmBtn">Confirm</button>
    </div>
  </div>
</div>

<div id="importModal" class="modal-overlay">
  <div class="modal">
    <h3>Restore Configuration?</h3>
    <p>This will replace your current settings with the backup file and reboot the device.</p>
    <div class="filename" id="importFilename"></div>
    <div class="modal-actions">
      <button type="button" class="btn btn-cancel" onclick="cancelImport()">Cancel</button>
      <button type="button" class="btn btn-confirm" onclick="confirmImport()">Restore</button>
    </div>
  </div>
</div>

<div id="rebootOverlay" class="overlay" style="display:none;">
  <div class="modal">
    <div class="spinner"></div>
    <h3 style="color: var(--warning);">Device Rebooting...</h3>
    <p class="sub-text">Redirecting in 20 seconds.</p>
  </div>
</div>

<div id="resetOverlay" class="overlay" style="display:none;">
  <div class="modal">
    <div class="spinner"></div>
    <h3 style="color: var(--danger);">Resetting Device...</h3>
    <p class="sub-text">Wiping config and rebooting.</p>
    <div style="margin-top: 15px; text-align: left; background: #333; padding: 15px; border-radius: 6px; font-size: 0.85rem; border: 1px solid var(--border);">
      <p style="margin:0 0 8px; color: #aaa;">After reset, please:</p>
      <p style="margin:0 0 5px; color: #fff;">1. Join WiFi: <strong style="color: var(--accent);" id="default_ssid">...</strong></p>
      <p style="margin:0 0 5px; color: #fff;">2. If not automatically redirected, click button below (Refresh if you get a black page)</p>
      <p style="margin:0 0 5px; color: #fff;">3. Configure WiFi and check device display for IP address</p>
      <p style="margin:0 0 15px; color: #fff;">4. Browse to device IP with login: <strong style="color: var(--accent);" id="default_user">...</strong> / <strong style="color: var(--accent);" id="default_pass">...</strong></p>
      <button onclick="window.location.href='http://192.168.4.1/wifi'" style="width: 100%; background: var(--accent); color: white; border: none; padding: 10px; border-radius: 6px; font-size: 0.9rem; font-weight: 600; cursor: pointer;">Open Device WiFi Manager</button>
    </div>
  </div>
</div>

<div id="updateOverlay" class="overlay" style="display:none;">
  <div class="modal">
    <div class="spinner"></div>
    <h3>Updating Firmware</h3>
    <p class="sub-text">Uploading and flashing... Please wait.</p>
  </div>
</div>

</body>
</html>
)=====";

