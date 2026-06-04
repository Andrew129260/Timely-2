var Clay = require('@rebble/clay');
var clayConfig = require('./config.json');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

var useCelsius = false; // Default to Fahrenheit
var batteryIntervalId = null;
var weatherIntervalId = null;

Pebble.addEventListener('ready', function(e) {
    console.log('PebbleKit JS ready!');
    startPolling();
});

var translations = {
    EN: { trans_connected: 'Linked', trans_disconnected: 'No Link', trans_abbr_sunday: 'Su', trans_abbr_monday: 'Mo', trans_abbr_tuesday: 'Tu', trans_abbr_wedsday: 'We', trans_abbr_thursday: 'Th', trans_abbr_friday: 'Fr', trans_abbr_saturday: 'Sa', trans_january: 'January', trans_february: 'February', trans_march: 'March', trans_april: 'April', trans_may: 'May', trans_june: 'June', trans_july: 'July', trans_august: 'August', trans_september: 'September', trans_october: 'October', trans_november: 'November', trans_december: 'December', trans_sunday: 'Sunday', trans_monday: 'Monday', trans_tuesday: 'Tuesday', trans_wedsday: 'Wednesday', trans_thursday: 'Thursday', trans_friday: 'Friday', trans_saturday: 'Saturday' },
    DA: { trans_connected: 'Forbundet', trans_disconnected: 'Afbrudt', trans_abbr_sunday: 'Sø', trans_abbr_monday: 'Ma', trans_abbr_tuesday: 'Ti', trans_abbr_wedsday: 'On', trans_abbr_thursday: 'To', trans_abbr_friday: 'Fr', trans_abbr_saturday: 'Lø', trans_january: 'Januar', trans_february: 'Februar', trans_march: 'Marts', trans_april: 'April', trans_may: 'Maj', trans_june: 'Juni', trans_july: 'Juli', trans_august: 'August', trans_september: 'September', trans_october: 'Oktober', trans_november: 'November', trans_december: 'Desember', trans_sunday: 'Søndag', trans_monday: 'Mandag', trans_tuesday: 'Tirsdag', trans_wedsday: 'Onsdag', trans_thursday: 'Torsdag', trans_friday: 'Fredag', trans_saturday: 'Lørdag' },
    NL: { trans_connected: 'Verbonden', trans_disconnected: 'Geen link', trans_abbr_sunday: 'Zo', trans_abbr_monday: 'Ma', trans_abbr_tuesday: 'Di', trans_abbr_wedsday: 'Wo', trans_abbr_thursday: 'Do', trans_abbr_friday: 'Vr', trans_abbr_saturday: 'Za', trans_january: 'Januari', trans_february: 'Februari', trans_march: 'Maart', trans_april: 'April', trans_may: 'Mei', trans_june: 'Juni', trans_july: 'Juli', trans_august: 'Augustus', trans_september: 'September', trans_october: 'Oktober', trans_november: 'November', trans_december: 'December', trans_sunday: 'Zondag', trans_monday: 'Maandag', trans_tuesday: 'Dinsdag', trans_wedsday: 'Woensdag', trans_thursday: 'Donderdag', trans_friday: 'Vrijdag', trans_saturday: 'Zaterdag' },
    FI: { trans_connected: 'Kytketty', trans_disconnected: 'Katkaistu', trans_abbr_sunday: 'Su', trans_abbr_monday: 'Ma', trans_abbr_tuesday: 'Ti', trans_abbr_wedsday: 'Ke', trans_abbr_thursday: 'To', trans_abbr_friday: 'Pe', trans_abbr_saturday: 'La', trans_january: 'Tammikuu', trans_february: 'Helmikuu', trans_march: 'Maaliskuu', trans_april: 'Huhtikuu', trans_may: 'Toukokuu', trans_june: 'Kesäkuu', trans_july: 'Heinäkuu', trans_august: 'Elokuu', trans_september: 'Syyskuu', trans_october: 'Lokakuu', trans_november: 'Marraskuu', trans_december: 'Joulukuu', trans_sunday: 'Sunnuntai', trans_monday: 'Maanantai', trans_tuesday: 'Tiistai', trans_wedsday: 'Keskiviikko', trans_thursday: 'Torstai', trans_friday: 'Perjantai', trans_saturday: 'Lauantai' },
    FR: { trans_connected: 'Connecté', trans_disconnected: 'Sans lien', trans_abbr_sunday: 'Di', trans_abbr_monday: 'Lu', trans_abbr_tuesday: 'Ma', trans_abbr_wedsday: 'Me', trans_abbr_thursday: 'Je', trans_abbr_friday: 'Ve', trans_abbr_saturday: 'Sa', trans_january: 'Janvier', trans_february: 'Février', trans_march: 'Mars', trans_april: 'Avril', trans_may: 'Mai', trans_june: 'Juin', trans_july: 'Juillet', trans_august: 'Août', trans_september: 'Septembre', trans_october: 'Octobre', trans_november: 'Novembre', trans_december: 'Décembre', trans_sunday: 'Dimanche', trans_monday: 'Lundi', trans_tuesday: 'Mardi', trans_wedsday: 'Mercredi', trans_thursday: 'Jeudi', trans_friday: 'Vendredi', trans_saturday: 'Samedi' },
    DE: { trans_connected: 'Verbunden', trans_disconnected: 'Getrennt', trans_abbr_sunday: 'So', trans_abbr_monday: 'Mo', trans_abbr_tuesday: 'Di', trans_abbr_wedsday: 'Mi', trans_abbr_thursday: 'Do', trans_abbr_friday: 'Fr', trans_abbr_saturday: 'Sa', trans_january: 'Januar', trans_february: 'Februar', trans_march: 'März', trans_april: 'April', trans_may: 'Mai', trans_june: 'Juni', trans_july: 'Juli', trans_august: 'August', trans_september: 'September', trans_october: 'Oktober', trans_november: 'November', trans_december: 'Dezember', trans_sunday: 'Sonntag', trans_monday: 'Montag', trans_tuesday: 'Dienstag', trans_wedsday: 'Mittwoch', trans_thursday: 'Donnerstag', trans_friday: 'Freitag', trans_saturday: 'Samstag' },
    IT: { trans_connected: 'Connesso', trans_disconnected: 'Sconnesso', trans_abbr_sunday: 'Do', trans_abbr_monday: 'Lu', trans_abbr_tuesday: 'Ma', trans_abbr_wedsday: 'Me', trans_abbr_thursday: 'Gi', trans_abbr_friday: 'Ve', trans_abbr_saturday: 'Sa', trans_january: 'Gennaio', trans_february: 'Febbraio', trans_march: 'Marzo', trans_april: 'Aprile', trans_may: 'Maggio', trans_june: 'Giugno', trans_july: 'Luglio', trans_august: 'Agosto', trans_september: 'Settembre', trans_october: 'Ottobre', trans_november: 'Novembre', trans_december: 'Dicembre', trans_sunday: 'Domenica', trans_monday: 'Lunedì', trans_tuesday: 'Martedì', trans_wedsday: 'Mercoledì', trans_thursday: 'Giovedì', trans_friday: 'Venerdì', trans_saturday: 'Sabato' },
    NO: { trans_connected: 'Tilkoblet', trans_disconnected: 'Frakoblet', trans_abbr_sunday: 'Sø', trans_abbr_monday: 'Ma', trans_abbr_tuesday: 'Ti', trans_abbr_wedsday: 'On', trans_abbr_thursday: 'To', trans_abbr_friday: 'Fr', trans_abbr_saturday: 'Lø', trans_january: 'Januar', trans_february: 'Februar', trans_march: 'Mars', trans_april: 'April', trans_may: 'Mai', trans_june: 'Juni', trans_july: 'Juli', trans_august: 'August', trans_september: 'September', trans_october: 'Oktober', trans_november: 'November', trans_december: 'Desember', trans_sunday: 'Søndag', trans_monday: 'Mandag', trans_tuesday: 'Tirsdag', trans_wedsday: 'Onsdag', trans_thursday: 'Torsdag', trans_friday: 'Fredag', trans_saturday: 'Lørdag' },
    PT: { trans_connected: 'Conectado', trans_disconnected: 'Sem link', trans_abbr_sunday: 'Do', trans_abbr_monday: 'Sg', trans_abbr_tuesday: 'Te', trans_abbr_wedsday: 'Qr', trans_abbr_thursday: 'Qi', trans_abbr_friday: 'Sx', trans_abbr_saturday: 'Sb', trans_january: 'Janeiro', trans_february: 'Fevereiro', trans_march: 'Março', trans_april: 'Abril', trans_may: 'Maio', trans_june: 'Junho', trans_july: 'Julho', trans_august: 'Agosto', trans_september: 'Setembro', trans_october: 'Outubro', trans_november: 'Novembro', trans_december: 'Dezembro', trans_sunday: 'Domingo', trans_monday: 'Segunda', trans_tuesday: 'Terça', trans_wedsday: 'Quarta', trans_thursday: 'Quinta', trans_friday: 'Sexta', trans_saturday: 'Sábado' },
    RU: { trans_connected: 'Связан', trans_disconnected: 'Отключен', trans_abbr_sunday: 'Вс', trans_abbr_monday: 'Пн', trans_abbr_tuesday: 'Вт', trans_abbr_wedsday: 'Ср', trans_abbr_thursday: 'Чт', trans_abbr_friday: 'Пт', trans_abbr_saturday: 'Сб', trans_january: 'Январь', trans_february: 'Февраль', trans_march: 'Март', trans_april: 'Апрель', trans_may: 'Май', trans_june: 'Июнь', trans_july: 'Июль', trans_august: 'Август', trans_september: 'Сентябрь', trans_october: 'Октябрь', trans_november: 'Ноябрь', trans_december: 'Декабрь', trans_sunday: 'Воскресенье', trans_monday: 'Понедельник', trans_tuesday: 'Вторник', trans_wedsday: 'Среда', trans_thursday: 'Четверг', trans_friday: 'Пятница', trans_saturday: 'Суббота' },
    ES: { trans_connected: 'Conectado', trans_disconnected: 'Sin link', trans_abbr_sunday: 'Do', trans_abbr_monday: 'Lu', trans_abbr_tuesday: 'Ma', trans_abbr_wedsday: 'Mi', trans_abbr_thursday: 'Ju', trans_abbr_friday: 'Vi', trans_abbr_saturday: 'Sa', trans_january: 'Enero', trans_february: 'Febrero', trans_march: 'Marzo', trans_april: 'Abril', trans_may: 'Mayo', trans_june: 'Junio', trans_july: 'Julio', trans_august: 'Agosto', trans_september: 'Septiembre', trans_october: 'Octubre', trans_november: 'Noviembre', trans_december: 'Diciembre', trans_sunday: 'Domingo', trans_monday: 'Lunes', trans_tuesday: 'Martes', trans_wedsday: 'Miércoles', trans_thursday: 'Jueves', trans_friday: 'Viernes', trans_saturday: 'Sábado' },
    SV: { trans_connected: 'Ansluten', trans_disconnected: 'Frånkoppl', trans_abbr_sunday: 'Sö', trans_abbr_monday: 'Må', trans_abbr_tuesday: 'Ti', trans_abbr_wedsday: 'On', trans_abbr_thursday: 'To', trans_abbr_friday: 'Fr', trans_abbr_saturday: 'Lö', trans_january: 'Januari', trans_february: 'Februari', trans_march: 'Mars', trans_april: 'April', trans_may: 'Maj', trans_june: 'Juni', trans_july: 'Juli', trans_august: 'Augusti', trans_september: 'September', trans_october: 'Oktober', trans_november: 'November', trans_december: 'December', trans_sunday: 'Söndag', trans_monday: 'Måndag', trans_tuesday: 'Tisdag', trans_wedsday: 'Onsdag', trans_thursday: 'Torsdag', trans_friday: 'Fredag', trans_saturday: 'Lördag' }
};
var CLIMACON = { 'cloud': '!', 'cloud_day': '"', 'cloud_night': '#', 'rain': '$', 'rain_day': '%', 'rain_night': '&', 'showers': "'", 'showers_day': '(', 'showers_night': ')', 'downpour': '*', 'downpour_day': '+', 'downpour_night': ',', 'drizzle': '-', 'drizzle_day': '.', 'drizzle_night': '/', 'sleet': '0', 'sleet_day': '1', 'sleet_night': '2', 'hail': '3', 'hail_day': '4', 'hail_night': '5', 'flurries': '6', 'flurries_day': '7', 'flurries_night': '8', 'snow': '9', 'snow_day': ':', 'snow_night': ';', 'fog': '<', 'fog_day': '=', 'fog_night': '>', 'haze': '?', 'haze_day': '@', 'haze_night': 'A', 'wind': 'B', 'wind_cloud': 'C', 'wind_cloud_day': 'D', 'wind_cloud_night': 'E', 'lightning': 'F', 'lightning_day': 'G', 'lightning_night': 'H', 'sun': 'I', 'set': 'J', 'rise': 'K', 'low': 'L', 'lower': 'M', 'moon': 'N', 'new': 'O', 'wax_cresc': 'P', 'wax_quart': 'Q', 'wax_gib': 'R', 'full': 'S', 'wane_cresc': 'T', 'wane_quart': 'U', 'wane_gib': 'V', 'snowflake': 'W', 'tornado': 'X', 'thermometer': 'Y', 'temp_low': 'Z', 'temp_med-low': '[', 'temp_med-high': "\\", 'temp_high': ']', 'temp_full': '^', 'celsius': '`', 'fahrenheit': '_', 'compass': 'a', 'north': 'b', 'east': 'c', 'south': 'd', 'west': 'e', 'umbrella': 'f', 'sunglasses': 'g', 'cloud_refresh': 'h', 'cloud_up': 'i', 'cloud_down': 'j' };
var WMOclimacon = { 0: CLIMACON['sun'], 1: CLIMACON['sun'], 2: CLIMACON['cloud_day'], 3: CLIMACON['cloud'], 45: CLIMACON['fog'], 48: CLIMACON['fog'], 51: CLIMACON['drizzle'], 53: CLIMACON['drizzle'], 55: CLIMACON['drizzle'], 56: CLIMACON['hail'], 57: CLIMACON['hail'], 61: CLIMACON['rain'], 63: CLIMACON['rain'], 65: CLIMACON['downpour'], 66: CLIMACON['hail'], 67: CLIMACON['hail'], 71: CLIMACON['snow'], 73: CLIMACON['snow'], 75: CLIMACON['snow'], 77: CLIMACON['flurries'], 80: CLIMACON['showers'], 81: CLIMACON['showers'], 82: CLIMACON['downpour'], 85: CLIMACON['snow'], 86: CLIMACON['snow'], 95: CLIMACON['lightning'], 96: CLIMACON['lightning'], 99: CLIMACON['lightning'] };
Pebble.removeEventListener('webviewclosed');
Pebble.addEventListener('webviewclosed', function(e) {
    if (!e || !e.response || e.response === 'CANCELLED') return;
    try {
        var raw = clay.getSettings(decodeURIComponent(e.response), false);
        var settings = { "message_type": 99 };
        Object.keys(raw).forEach(function(k) {
            var v = (raw[k] && typeof raw[k] === 'object' && raw[k].hasOwnProperty('value')) ? raw[k].value : raw[k];
            if (typeof v === 'string' && /^-?\d+$/.test(v)) v = parseInt(v, 10);
            if (k !== "NaN") settings[k] = v;
        });
        
        // Update weather logic actively on save
        if (settings.weather_fmt !== undefined) {
            useCelsius = (settings.weather_fmt === 1 || settings.weather_fmt === true);
        }

        var lang = settings.language || 'EN';
        if (translations[lang]) {
            Object.keys(translations[lang]).forEach(function(k) { settings[k] = translations[lang][k]; });
        }
        Pebble.sendAppMessage(settings, function() {
            localStorage.setItem('last_battery_fetch', '0'); // Reset timestamp to force fetch
            startPolling(); // Restart the intervals with the new settings
        });
    } catch (err) { console.log("Config Error: " + err); }
});

Pebble.addEventListener('showConfiguration', function() { Pebble.openURL(clay.generateUrl()); });
function getWatchVersion() { Pebble.sendAppMessage({ message_type: 104 }, function(e) { console.log("Sent watch version."); }); }
function saveWatchVersion(e) { window.localStorage.version_watch = e.payload.send_watch_version; window.localStorage.version_config = e.payload.send_config_version; }
function sendWeather(temp, cond_icon) {
    if (isItNight() && cond_icon == CLIMACON['sun']) cond_icon = getMoonIcon();
    if (cond_icon == CLIMACON['moon']) cond_icon = getMoonIcon();
    Pebble.sendAppMessage({ "message_type": 106, "weather_temp": temp, "weather_cond": cond_icon });
}
function fetchOpenMeteoWeather(lat, lon) {
    var unit = useCelsius ? "" : "&temperature_unit=fahrenheit";
    var req = new XMLHttpRequest();
    req.open('GET', "https://api.open-meteo.com/v1/forecast?latitude="+lat+"&longitude="+lon+"&current=temperature_2m,weather_code" + unit, true);
    
    // 1. Force the phone to give up if the network hangs for more than 10 seconds
    req.timeout = 10000;
    
    req.onload = function() { 
        if (req.status == 200) { 
            var res = JSON.parse(req.responseText); 
            sendWeather(Math.round(res.current.temperature_2m), WMOclimacon[res.current.weather_code]); 
        } else {
            // API returned a bad status (e.g., 404, 500)
            console.warn("Weather API returned status: " + req.status);
            sendWeather(999, 'h'); // 'h' is the default refresh icon
        }
    };
    
    // 2. Handle a complete drop in cellular/wifi signal
    req.onerror = function() {
        console.warn("Weather request failed due to network error.");
        sendWeather(999, 'h');
    };
    
    // 3. Handle the explicit 10-second timeout
    req.ontimeout = function() {
        console.warn("Weather request timed out after 10 seconds.");
        sendWeather(999, 'h');
    };
    
    req.send(null);
}
function weatherLocationSuccess(pos) { lastCoordinates = pos.coords; fetchOpenMeteoWeather(lastCoordinates.latitude, lastCoordinates.longitude); }
function locationError(err) { console.warn('Weather error: ' + err.message); }
function updatePhoneBattery() {
    if (navigator.getBattery) {
        navigator.getBattery().then(function(b) {
            localStorage.setItem('last_battery_fetch', Date.now()); // Save the exact time
            Pebble.sendAppMessage({"PhoneBattery": Math.round(b.level * 100)});
        });
    }
}

function startPolling() {
    // Read settings directly from Pebble's local storage
    var settings = {};
    try {
        settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
    } catch (e) {}

    var bInt = settings.PhoneBatteryInterval !== undefined ? parseInt(settings.PhoneBatteryInterval, 10) : 30;
    var wInt = settings.weather_update !== undefined ? parseInt(settings.weather_update, 10) : 30;

    // Clear existing intervals if they exist
    if (batteryIntervalId) clearInterval(batteryIntervalId);
    if (weatherIntervalId) clearInterval(weatherIntervalId);

    // Only start the battery interval if track_battery is enabled (default to true if not set yet)
    var trackBatt = settings.track_battery !== undefined ? (settings.track_battery == 1 || settings.track_battery == "1" || settings.track_battery === true) : true;
    
    if (bInt > 0 && trackBatt) {
        var lastFetch = parseInt(localStorage.getItem('last_battery_fetch') || "0", 10);
        var intervalMs = bInt * 60 * 1000;
        
        // Only fetch immediately if it has been longer than the chosen interval
        if (Date.now() - lastFetch > intervalMs) {
            updatePhoneBattery(); 
        }
        
        batteryIntervalId = setInterval(updatePhoneBattery, intervalMs);
    }
    
    if (wInt > 0) {
        weatherIntervalId = setInterval(function() { navigator.geolocation.getCurrentPosition(weatherLocationSuccess, locationError); }, wInt * 60 * 1000);
    }
}

Pebble.addEventListener("appmessage", function (e) {
    switch (e.payload.message_type) {
        case 100: updatePhoneBattery(); break;
        case 106: 
            if (e.payload.weather_fmt !== undefined) {
                useCelsius = (e.payload.weather_fmt === 1 || e.payload.weather_fmt === true);
            }
            navigator.geolocation.getCurrentPosition(weatherLocationSuccess, locationError); 
            break;
    }
});

var SunCalc; (function () { "use strict"; var PI = Math.PI, sin = Math.sin, cos = Math.cos, tan = Math.tan, asin = Math.asin, atan = Math.atan2, acos = Math.acos, rad = PI / 180; var dayMs = 1000 * 60 * 60 * 24, J1970 = 2440588, J2000 = 2451545; function toJulian(date) { return date.valueOf() / dayMs - 0.5 + J1970; } function fromJulian(j) { return new Date((j + 0.5 - J1970) * dayMs); } function toDays(date) { return toJulian(date) - J2000; } function getRightAscension(l, b) { return atan(sin(l) * cos(rad * 23.4397) - tan(b) * sin(rad * 23.4397), cos(l)); } function getDeclination(l, b) { return asin(sin(b) * cos(rad * 23.4397) + cos(b) * sin(rad * 23.4397) * sin(l)); } function getAzimuth(H, phi, dec) { return atan(sin(H), cos(H) * sin(phi) - tan(dec) * cos(phi)); } function getAltitude(H, phi, dec) { return asin(sin(phi) * sin(dec) + cos(phi) * cos(dec) * cos(H)); } function getSiderealTime(d, lw) { return rad * (280.16 + 360.9856235 * d) - lw; } function getSolarMeanAnomaly(d) { return rad * (357.5291 + 0.98560028 * d); } function getEquationOfCenter(M) { return rad * (1.9148 * sin(M) + 0.02 * sin(2 * M) + 0.0003 * sin(3 * M)); } function getEclipticLongitude(M, C) { return M + C + rad * 102.9372 + PI; } function getSunCoords(d) { var M = getSolarMeanAnomaly(d), C = getEquationOfCenter(M), L = getEclipticLongitude(M, C); return { dec: getDeclination(L, 0), ra: getRightAscension(L, 0) }; } SunCalc = {}; SunCalc.getPosition = function (date, lat, lng) { var lw = rad * -lng, phi = rad * lat, d = toDays(date), c = getSunCoords(d), H = getSiderealTime(d, lw) - c.ra; return { azimuth: getAzimuth(H, phi, c.dec), altitude: getAltitude(H, phi, c.dec) }; }; var times = [ [-0.83, 'sunrise', 'sunset' ], [ -0.3, 'sunriseEnd', 'sunsetStart' ], [ -6, 'dawn', 'dusk' ], [ -12, 'nauticalDawn', 'nauticalDusk'], [ -18, 'nightEnd', 'night' ], [ 6, 'goldenHourEnd', 'goldenHour' ] ]; function getJulianCycle(d, lw) { return Math.round(d - 0.0009 - lw / (2 * PI)); } function getApproxTransit(Ht, lw, n) { return 0.0009 + (Ht + lw) / (2 * PI) + n; } function getSolarTransitJ(ds, M, L) { return J2000 + ds + 0.0053 * sin(M) - 0.0069 * sin(2 * L); } function getHourAngle(h, phi, d) { return acos((sin(h) - sin(phi) * sin(d)) / (cos(phi) * cos(d))); } SunCalc.getTimes = function (date, lat, lng) { var lw = rad * -lng, phi = rad * lat, d = toDays(date), n = getJulianCycle(d, lw), ds = getApproxTransit(0, lw, n), M = getSolarMeanAnomaly(ds), C = getEquationOfCenter(M), L = getEclipticLongitude(M, C), dec = getDeclination(L, 0), Jnoon = getSolarTransitJ(ds, M, L); function getSetJ(h) { var w = getHourAngle(h, phi, dec), a = getApproxTransit(w, lw, n); return getSolarTransitJ(a, M, L); } var result = { solarNoon: fromJulian(Jnoon), nadir: fromJulian(Jnoon - 0.5) }; var i, len, time, Jset, Jrise; for (i = 0, len = times.length; i < len; i += 1) { time = times[i]; Jset = getSetJ(time[0] * rad); Jrise = Jnoon - (Jset - Jnoon); result[time[1]] = fromJulian(Jrise); result[time[2]] = fromJulian(Jset); } return result; }; function getMoonCoords(d) { var L = rad * (218.316 + 13.176396 * d), M = rad * (134.963 + 13.064993 * d), F = rad * (93.272 + 13.229350 * d), l = L + rad * 6.289 * sin(M), b = rad * 5.128 * sin(F), dt = 385001 - 20905 * cos(M); return { ra: getRightAscension(l, b), dec: getDeclination(l, b), dist: dt }; } SunCalc.getMoonIllumination = function (date) { var d = toDays(date), s = getSunCoords(d), m = getMoonCoords(d), sdist = 149598000, phi = acos(sin(s.dec) * sin(m.dec) + cos(s.dec) * cos(m.dec) * cos(s.ra - m.ra)), inc = atan(sdist * sin(phi), m.dist - sdist * cos(phi)); return { fraction: (1 + cos(inc)) / 2, angle: atan(cos(s.dec) * sin(s.ra - m.ra), sin(s.dec) * cos(m.dec) - cos(s.dec) * sin(m.dec) * cos(s.ra - m.ra)) }; }; }());
function isItNight() {
    if (typeof lastCoordinates === 'undefined' || !lastCoordinates) return false;
    var now = new Date();
    var sunInfo = SunCalc.getTimes(now, lastCoordinates.latitude, lastCoordinates.longitude);
    return sunInfo.sunset < now || now < sunInfo.sunrise;
}
function getMoonIcon() {
    var now = new Date();
    var moonInfo = SunCalc.getMoonIllumination(now);
    if (moonInfo.fraction <= 0.05) return "O";
    if (moonInfo.fraction >= 0.95) return "S";
    if (moonInfo.angle < 0) return (moonInfo.fraction <= 0.35) ? "P" : (moonInfo.fraction <= 0.65) ? "Q" : "R";
    return (moonInfo.fraction <= 0.35) ? "T" : (moonInfo.fraction <= 0.65) ? "U" : "V";
}

