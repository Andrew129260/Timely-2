var Clay = require('@rebble/clay');
var clayConfig = require('./config.json');

// Filter manually because this Clay version predates Emery's capability names.
var watchInfo = null;
try { watchInfo = Pebble.getActiveWatchInfo(); } catch (ignore) {}

if (!watchInfo || watchInfo.platform !== 'emery') {
  clayConfig.forEach(function (section) {
    if (section.items) {
      section.items = section.items.filter(function (item) {
        return item.messageKey !== 'complication_mode' &&
               item.messageKey !== 'tap_to_cycle';
      });
    }
  });
}

var clay = new Clay(clayConfig, null, {autoHandleEvents: false});

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

    ES: { trans_connected: 'Conectado', trans_disconnected: 'Sin link', trans_abbr_sunday: 'Do', trans_abbr_monday: 'Lu', trans_abbr_tuesday: 'Ma', trans_abbr_wedsday: 'Mi', trans_abbr_thursday: 'Ju', trans_abbr_friday: 'Vi', trans_abbr_saturday: 'Sa', trans_january: 'Enero', trans_february: 'Febrero', trans_march: 'Marzo', trans_april: 'Abril', trans_may: 'Mayo', trans_june: 'Junio', trans_july: 'Julio', trans_august: 'Agosto', trans_september: 'Septiembre', trans_october: 'Octubre', trans_november: 'Noviembre', trans_december: 'Diciembre', trans_sunday: 'Domingo', trans_monday: 'Lunes', trans_tuesday: 'Martes', trans_wedsday: 'Miércoles', trans_thursday: 'Jueves', trans_friday: 'Viernes', trans_saturday: 'Sábado' },

    SV: { trans_connected: 'Ansluten', trans_disconnected: 'Frånkoppl', trans_abbr_sunday: 'Sö', trans_abbr_monday: 'Må', trans_abbr_tuesday: 'Ti', trans_abbr_wedsday: 'On', trans_abbr_thursday: 'To', trans_abbr_friday: 'Fr', trans_abbr_saturday: 'Lö', trans_january: 'Januari', trans_february: 'Februari', trans_march: 'Mars', trans_april: 'April', trans_may: 'Maj', trans_june: 'Juni', trans_july: 'Juli', trans_august: 'Augusti', trans_september: 'September', trans_october: 'Oktober', trans_november: 'November', trans_december: 'December', trans_sunday: 'Söndag', trans_monday: 'Måndag', trans_tuesday: 'Tisdag', trans_wedsday: 'Onsdag', trans_thursday: 'Torsdag', trans_friday: 'Fredag', trans_saturday: 'Lördag' }
};

var CLIMACON = {
  'cloud': '!',
  'cloud_day': '"',
  'cloud_night': '#',
  'rain': '$',
  'rain_day': '%',
  'rain_night': '&',
  'showers': "'",
  'showers_day': '(',
  'showers_night': ')',
  'downpour': '*',
  'downpour_day': '+',
  'downpour_night': ',',
  'drizzle': '-',
  'drizzle_day': '.',
  'drizzle_night': '/',
  'sleet': '0',
  'sleet_day': '1',
  'sleet_night': '2',
  'hail': '3',
  'hail_day': '4',
  'hail_night': '5',
  'flurries': '6',
  'flurries_day': '7',
  'flurries_night': '8',
  'snow': '9',
  'snow_day': ':',
  'snow_night': ';',
  'fog': '<',
  'fog_day': '=',
  'fog_night': '>',
  'haze': '?',
  'haze_day': '@',
  'haze_night': 'A',
  'wind': 'B',
  'wind_cloud': 'C',
  'wind_cloud_day': 'D',
  'wind_cloud_night': 'E',
  'lightning': 'F',
  'lightning_day': 'G',
  'lightning_night': 'H',
  'sun': 'I',
  'set': 'J',
  'rise': 'K',
  'low': 'L',
  'lower': 'M',
  'moon': 'N',
  'new': 'O',
  'wax_cresc': 'P',
  'wax_quart': 'Q',
  'wax_gib': 'R',
  'full': 'S',
  'wane_cresc': 'T',
  'wane_quart': 'U',
  'wane_gib': 'V',
  'snowflake': 'W',
  'tornado': 'X',
  'thermometer': 'Y',
  'temp_low': 'Z',
  'temp_med-low': '[',
  'temp_med-high': "\\",
  'temp_high': ']',
  'temp_full': '^',
  'celsius': '`',
  'fahrenheit': '_',
  'compass': 'a',
  'north': 'b',
  'east': 'c',
  'south': 'd',
  'west': 'e',
  'umbrella': 'f',
  'sunglasses': 'g',
  'cloud_refresh': 'h',
  'cloud_up': 'i',
  'cloud_down': 'j'
};

var WMOclimacon = {
  0: CLIMACON['sun'],
  1: CLIMACON['sun'],
  2: CLIMACON['cloud_day'],
  3: CLIMACON['cloud'],
  45: CLIMACON['fog'],
  48: CLIMACON['fog'],
  51: CLIMACON['drizzle'],
  53: CLIMACON['drizzle'],
  55: CLIMACON['drizzle'],
  56: CLIMACON['hail'],
  57: CLIMACON['hail'],
  61: CLIMACON['rain'],
  63: CLIMACON['rain'],
  65: CLIMACON['downpour'],
  66: CLIMACON['hail'],
  67: CLIMACON['hail'],
  71: CLIMACON['snow'],
  73: CLIMACON['snow'],
  75: CLIMACON['snow'],
  77: CLIMACON['flurries'],
  80: CLIMACON['showers'],
  81: CLIMACON['showers'],
  82: CLIMACON['downpour'],
  85: CLIMACON['snow'],
  86: CLIMACON['snow'],
  95: CLIMACON['lightning'],
  96: CLIMACON['lightning'],
  99: CLIMACON['lightning']
};

// The watch owns weather scheduling. The phone polls only its own battery.
var batteryIntervalId = null;
var batteryBusy = false;
var lastBatterySent = null;
var lastCoordinates = null;
var useCelsius = false;
var weatherJob = null;
var weatherGeneration = 0;
var weatherRetryAfter = 0;
var weatherShown = false;

var PHONE_BATTERY_SUPPORTED =
  typeof navigator !== 'undefined' &&
  typeof navigator.getBattery === 'function';
var LOCATION_CACHE_MS = 60 * 60000;

var messageQueue = [];
var activeMessage = null;
var settingsByKey = {};
var pendingConfiguration = null;

clayConfig.forEach(function (section) {
  (section.items || []).forEach(function (item) {
    if (item.messageKey) {
      settingsByKey[item.messageKey] = item;
    }
  });
});

function storedJSON(key, fallback) {
  try {
    return JSON.parse(localStorage.getItem(key)) || fallback;
  } catch (error) {
    return fallback;
  }
}

function saveJSON(key, value) {
  try {
    localStorage.setItem(key, JSON.stringify(value));
  } catch (error) {
    console.warn('Storage error: ' + error);
  }
}

function has(object, key) {
  return Object.prototype.hasOwnProperty.call(object, key);
}

function finiteNumber(value) {
  return typeof value === 'number' && isFinite(value);
}

function normalizeSettings(raw) {
  var settings = {};

  raw = raw && typeof raw === 'object' ? raw : {};

  Object.keys(settingsByKey).forEach(function (key) {
    var item = settingsByKey[key];
    var value = has(raw, key) ? raw[key] : item.defaultValue;

    if (
      value &&
      typeof value === 'object' &&
      has(value, 'value')
    ) {
      value = value.value;
    }

    // The previous DD.MM.YYYY UI accidentally stored 1; its C format is 215.
    if (
      key === 'intl_fmt_date' &&
      String(value) === '1'
    ) {
      value = '215';
    }

    if (item.type === 'toggle') {
      value =
        value === true ||
        value === 1 ||
        value === '1';
    } else if (item.options) {
      var valid = item.options.some(function (option) {
        return String(option.value) === String(value);
      });

      if (
        key === 'weather_update' &&
        /^\d+$/.test(String(value))
      ) {
        var interval = Number(value);
        valid =
          interval === 0 ||
          interval === 10 ||
          interval === 15 ||
          interval === 30 ||
          interval === 60;
      }

      if (
        key === 'PhoneBatteryInterval' &&
        /^\d+$/.test(String(value))
      ) {
        var batteryInterval = Number(value);
        valid =
          batteryInterval === 10 ||
          batteryInterval === 30 ||
          batteryInterval === 60;
      }

      if (!valid) {
        value = item.defaultValue;
      }
    }

    if (
      typeof value === 'string' &&
      /^-?\d+$/.test(value)
    ) {
      value = parseInt(value, 10);
    }

    if (key === 'track_battery' && !PHONE_BATTERY_SUPPORTED) {
      value = 0;
    }

    settings[key] = value;
  });

  return settings;
}

function readSettings() {
  return normalizeSettings(
    storedJSON('clay-settings', {})
  );
}

function numberBoolean(value) {
  return (
    value === true ||
    value === 1 ||
    value === '1'
  ) ? 1 : 0;
}

// Only one AppMessage is outstanding. Coalesce queued status updates and retry
// delivery failures a maximum of three times, with increasing delays.
function enqueueMessage(
  tag,
  data,
  onSuccess,
  onFailure
) {
  for (
    var i = 0;
    i < messageQueue.length;
    ++i
  ) {
    if (messageQueue[i].tag === tag) {
      messageQueue[i] = {
        tag: tag,
        data: data,
        attempts: 0,
        success: onSuccess,
        failure: onFailure
      };

      pumpMessages();
      return;
    }
  }

  messageQueue.push({
    tag: tag,
    data: data,
    attempts: 0,
    success: onSuccess,
    failure: onFailure
  });

  pumpMessages();
}

function pumpMessages() {
  if (
    activeMessage ||
    !messageQueue.length
  ) {
    return;
  }

  activeMessage =
    messageQueue.shift();

  sendActiveMessage();
}

function sendActiveMessage() {
  var message = activeMessage;

  if (!message) {
    return;
  }

  ++message.attempts;

  var settled = false;

  function finish(ok, error) {
    if (
      settled ||
      activeMessage !== message
    ) {
      return;
    }

    settled = true;

    if (
      !ok &&
      message.attempts < 3
    ) {
      setTimeout(
        sendActiveMessage,
        500 *
          Math.pow(
            2,
            message.attempts - 1
          )
      );

      return;
    }

    activeMessage = null;

    if (
      ok &&
      message.success
    ) {
      message.success();
    }

    if (!ok) {
      console.warn(
        'AppMessage ' +
        message.tag +
        ' failed: ' +
        JSON.stringify(
          error || {}
        )
      );

      if (message.failure) {
        message.failure(error);
      }
    }

    pumpMessages();
  }

  try {
    Pebble.sendAppMessage(
      message.data,

      function () {
        finish(true);
      },

      function (event) {
        finish(false, event);
      }
    );
  } catch (error) {
    finish(
      false,
      String(error)
    );
  }
}

function dropQueuedWeather() {
  messageQueue =
    messageQueue.filter(
      function (message) {
        return (
          message.tag !== 'weather'
        );
      }
    );
}

function prepareConfiguration(settings) {
  var values = {
    message_type: 99
  };

  Object.keys(
    settingsByKey
  ).forEach(function (key) {
    if (has(settings, key)) {
      values[key] =
        typeof settings[key] === 'boolean'
          ? numberBoolean(settings[key])
          : settings[key];
    }
  });

  var language =
    has(
      translations,
      settings.language
    )
      ? settings.language
      : 'EN';

  values.language = language;

  Object.keys(
    translations[language]
  ).forEach(function (key) {
    values[key] =
      translations[language][key];
  });

  // This conversion both validates declared keys and normalizes Clay values.
  return Clay.prepareSettingsForAppMessage(
    values
  );
}

function sendConfiguration(
  settings,
  onSuccess,
  onFailure
) {
  var values =
    prepareConfiguration(settings);

  pendingConfiguration =
    values;

  saveJSON(
    'timely-pending-config',
    {
      settings: settings
    }
  );

  enqueueMessage(
    'configuration',
    values,

    function () {
      if (
        pendingConfiguration ===
        values
      ) {
        pendingConfiguration =
          null;

        localStorage.removeItem(
          'timely-pending-config'
        );

        startPolling(true);
      }

      if (onSuccess) {
        onSuccess();
      }
    },

    onFailure
  );
}

function abortWeather() {
  ++weatherGeneration;

  var oldJob =
    weatherJob;

  weatherJob = null;

  if (
    oldJob &&
    oldJob.xhr
  ) {
    try {
      oldJob.xhr.abort();
    } catch (ignore) {}
  }

  dropQueuedWeather();

  weatherRetryAfter = 0;
}

function applyPhoneSettings(settings) {
  var nextCelsius =
    numberBoolean(
      settings.weather_fmt
    ) === 1;

  if (
    nextCelsius !== useCelsius ||
    !settings.weather_update
  ) {
    abortWeather();
    weatherShown = false;
  }

  useCelsius = nextCelsius;

  saveJSON(
    'clay-settings',
    settings
  );
}

function updatePhoneBattery(force) {
  var settings =
    readSettings();

  if (
    !numberBoolean(
      settings.track_battery
    ) ||
    !settings.PhoneBatteryInterval ||
    batteryBusy ||
    !PHONE_BATTERY_SUPPORTED
  ) {
    return;
  }

  batteryBusy = true;

  try {
    navigator.getBattery().then(
      function (battery) {
        batteryBusy = false;

        if (
          !numberBoolean(
            readSettings().track_battery
          )
        ) {
          return;
        }

        if (
          !finiteNumber(
            battery.level
          ) ||
          battery.level < 0 ||
          battery.level > 1
        ) {
          return;
        }

        var percent =
          Math.round(
            battery.level * 100
          );

        if (
          !force &&
          percent ===
            lastBatterySent
        ) {
          return;
        }

        enqueueMessage(
          'battery',

          {
            PhoneBattery:
              percent
          },

          function () {
            lastBatterySent =
              percent;

            localStorage.setItem(
              'last_battery_fetch',
              String(Date.now())
            );
          }
        );
      },

      function (error) {
        batteryBusy = false;

        console.warn(
          'Phone battery unavailable: ' +
          error
        );
      }
    );
  } catch (error) {
    batteryBusy = false;

    console.warn(
      'Phone battery unavailable: ' +
      error
    );
  }
}

function startPolling(force) {
  if (
    batteryIntervalId !== null
  ) {
    clearInterval(
      batteryIntervalId
    );
  }

  batteryIntervalId = null;

  var settings =
    readSettings();

  if (
    numberBoolean(
      settings.track_battery
    ) &&
    settings.PhoneBatteryInterval > 0 &&
    PHONE_BATTERY_SUPPORTED
  ) {
    updatePhoneBattery(
      !!force
    );

    batteryIntervalId =
      setInterval(
        function () {
          updatePhoneBattery(false);
        },

        settings.PhoneBatteryInterval *
          60000
      );
  }
}

function validWeatherCache(
  cache,
  units
) {
  return (
    cache &&
    cache.version === 1 &&
    cache.units === units &&
    finiteNumber(
      cache.temperature
    ) &&
    cache.temperature >= -200 &&
    cache.temperature <= 200 &&
    typeof cache.icon ===
      'string' &&
    cache.icon.length === 1 &&
    finiteNumber(
      cache.timestamp
    ) &&
    cache.timestamp > 0 &&
    Date.now() >=
      cache.timestamp &&
    Date.now() -
      cache.timestamp <=
      6 * 60 * 60000
  );
}

function sendWeatherCache(cache) {
  if (
    !readSettings().weather_update ||
    cache.units !==
      (useCelsius ? 1 : 0)
  ) {
    return;
  }

  var icon = cache.icon;

  if (
    (
      isItNight() &&
      icon === CLIMACON.sun
    ) ||
    icon === CLIMACON.moon
  ) {
    icon =
      getMoonIcon();
  }

  enqueueMessage(
    'weather',
    {
      message_type: 106,
      weather_temp:
        Math.round(
          cache.temperature
        ),
      weather_cond: icon,
      weather_fmt:
        cache.units
    }
  );

  weatherShown = true;
}

function weatherFailed(
  job,
  reason
) {
  if (
    weatherJob !== job
  ) {
    return;
  }

  weatherJob = null;

  weatherRetryAfter =
    Date.now() +
    5 * 60000;

  console.warn(
    'Weather update failed: ' +
    reason
  );
}

function fetchWeatherAt(
  job,
  latitude,
  longitude
) {
  if (
    weatherJob !== job ||
    job.generation !==
      weatherGeneration
  ) {
    return;
  }

  if (
    !finiteNumber(latitude) ||
    !finiteNumber(longitude) ||
    Math.abs(latitude) > 90 ||
    Math.abs(longitude) > 180
  ) {
    weatherFailed(
      job,
      'invalid location'
    );

    return;
  }

  // About city/neighborhood precision, rather than transmitting exact GPS coordinates.
  latitude =
    Math.round(
      latitude * 100
    ) / 100;

  longitude =
    Math.round(
      longitude * 100
    ) / 100;

  var request =
    new XMLHttpRequest();

  job.xhr = request;

  request.open(
    'GET',

    'https://api.open-meteo.com/v1/forecast?latitude=' +
      latitude +
      '&longitude=' +
      longitude +
      '&current=temperature_2m,weather_code' +
      (
        job.units
          ? ''
          : '&temperature_unit=fahrenheit'
      ) +
      '&timezone=auto',

    true
  );

  request.timeout = 10000;

  request.onerror =
    function () {
      weatherFailed(
        job,
        'network error'
      );
    };

  request.ontimeout =
    function () {
      weatherFailed(
        job,
        'request timed out'
      );
    };

  request.onload =
    function () {
      if (
        weatherJob !== job ||
        job.generation !==
          weatherGeneration
      ) {
        return;
      }

      if (
        request.status !== 200
      ) {
        weatherFailed(
          job,
          'HTTP ' +
            request.status
        );

        return;
      }

      try {
        var response =
          JSON.parse(
            request.responseText
          );

        var current =
          response &&
          response.current;

        if (
          !current ||
          !finiteNumber(
            current.temperature_2m
          ) ||
          current.temperature_2m <
            -200 ||
          current.temperature_2m >
            200 ||
          !finiteNumber(
            current.weather_code
          ) ||
          Math.floor(
            current.weather_code
          ) !==
            current.weather_code
        ) {
          weatherFailed(
            job,
            'invalid response values'
          );

          return;
        }

        var cache = {
          version: 1,

          temperature:
            Math.round(
              current.temperature_2m
            ),

          icon:
            has(
              WMOclimacon,
              current.weather_code
            )
              ? WMOclimacon[
                  current.weather_code
                ]
              : CLIMACON.cloud,

          units: job.units,

          timestamp:
            Date.now()
        };

        weatherJob = null;

        weatherRetryAfter = 0;

        saveJSON(
          'timely-weather-cache',
          cache
        );

        sendWeatherCache(
          cache
        );
      } catch (error) {
        weatherFailed(
          job,
          'invalid JSON'
        );
      }
    };

  try {
    request.send(null);
  } catch (error) {
    weatherFailed(
      job,
      String(error)
    );
  }
}

function requestWeather() {
  var settings =
    readSettings();

  if (
    !settings.weather_update
  ) {
    return;
  }

  var units =
    useCelsius ? 1 : 0;

  var cache =
    storedJSON(
      'timely-weather-cache',
      null
    );

  var valid =
    validWeatherCache(
      cache,
      units
    );

  if (
    valid &&
    Date.now() -
      cache.timestamp <
      settings.weather_update *
        60000
  ) {
    sendWeatherCache(cache);
    return;
  }

  if (
    valid &&
    !weatherShown
  ) {
    sendWeatherCache(cache);
  }

  if (
    weatherJob ||
    Date.now() <
      weatherRetryAfter
  ) {
    return;
  }

  var job = {
    generation:
      weatherGeneration,

    units: units,

    xhr: null
  };

  weatherJob = job;

  if (
    lastCoordinates &&
    finiteNumber(
      lastCoordinates.timestamp
    ) &&
    Date.now() >=
      lastCoordinates.timestamp &&
    Date.now() -
      lastCoordinates.timestamp <
      LOCATION_CACHE_MS
  ) {
    fetchWeatherAt(
      job,
      lastCoordinates.latitude,
      lastCoordinates.longitude
    );

    return;
  }

  if (
    !navigator.geolocation
  ) {
    weatherFailed(
      job,
      'geolocation unavailable'
    );

    return;
  }

  try {
    navigator.geolocation
      .getCurrentPosition(
        function (position) {
          if (
            weatherJob !== job ||
            job.generation !==
              weatherGeneration
          ) {
            return;
          }

          if (
            !position ||
            !position.coords
          ) {
            weatherFailed(
              job,
              'missing coordinates'
            );

            return;
          }

          var latitude =
            Math.round(
              position.coords.latitude *
                100
            ) / 100;

          var longitude =
            Math.round(
              position.coords.longitude *
                100
            ) / 100;

          var now =
            Date.now();

          var coordinateTimestamp =
            finiteNumber(
              position.timestamp
            ) &&
            position.timestamp > 0 &&
            position.timestamp <= now
              ? position.timestamp
              : now;

          lastCoordinates = {
            latitude: latitude,
            longitude: longitude,
            timestamp:
              coordinateTimestamp
          };

          saveJSON(
            'timely-coordinates',
            lastCoordinates
          );

          fetchWeatherAt(
            job,
            latitude,
            longitude
          );
        },

        function (error) {
          weatherFailed(
            job,
            error &&
              error.message
              ? error.message
              : 'location unavailable'
          );
        },

        {
          maximumAge:
            LOCATION_CACHE_MS,

          timeout: 15000,

          enableHighAccuracy:
            false
        }
      );
  } catch (error) {
    weatherFailed(
      job,
      String(error)
    );
  }
}

function timezoneOffset() {
  return Math.round(
    new Date()
      .getTimezoneOffset() /
      15
  );
}

function announceReady() {
  enqueueMessage(
    'ready',
    {
      message_type: 104,
      timezone_offset:
        timezoneOffset()
    }
  );
}

Pebble.addEventListener(
  'ready',

  function () {
    console.log(
      'Timely phone services ready'
    );

    var settings =
      readSettings();

    applyPhoneSettings(
      settings
    );

    // Synchronize the watch-side battery display with this phone's actual
    // capability. This also handles a watch moving between Android and iOS.
    enqueueMessage(
      'battery-capability',
      {
        message_type: 99,
        track_battery: PHONE_BATTERY_SUPPORTED
          ? numberBoolean(settings.track_battery)
          : 0
      }
    );

    // Battery polling is independent of the watch-version handshake, so start
    // it immediately rather than waiting for a round-trip AppMessage.
    startPolling(true);

    lastCoordinates =
      storedJSON(
        'timely-coordinates',
        null
      );

    var pending =
      storedJSON(
        'timely-pending-config',
        null
      );

    if (
      pending &&
      pending.settings
    ) {
      settings =
        normalizeSettings(
          pending.settings
        );

      applyPhoneSettings(
        settings
      );

      sendConfiguration(
        settings,
        announceReady,
        announceReady
      );
    } else {
      announceReady();
    }
  }
);

Pebble.addEventListener(
  'showConfiguration',

  function () {
    // Normalize obsolete saved values before Clay renders its select controls.
    saveJSON(
      'clay-settings',
      readSettings()
    );

    Pebble.openURL(
      clay.generateUrl()
    );
  }
);

Pebble.addEventListener(
  'webviewclosed',

  function (event) {
    if (
      !event ||
      !event.response ||
      event.response ===
        'CANCELLED'
    ) {
      return;
    }

    try {
      var settings =
        normalizeSettings(
          clay.getSettings(
            event.response,
            false
          )
        );

      applyPhoneSettings(
        settings
      );

      sendConfiguration(
        settings,

        function () {
          if (
            settings.weather_update
          ) {
            requestWeather();
          }
        }
      );
    } catch (error) {
      console.warn(
        'Configuration error: ' +
        error
      );
    }
  }
);

Pebble.addEventListener(
  'appmessage',

  function (event) {
    var payload =
      event &&
      event.payload;

    if (!payload) {
      return;
    }

    var type =
      Number(
        payload.message_type
      );

    if (
      type === 103
    ) {
      enqueueMessage(
        'timezone',

        {
          message_type: 103,
          timezone_offset:
            timezoneOffset()
        }
      );

      // A type-103 request is also sent on Bluetooth reconnect whenever phone
      // battery tracking is enabled. Force a fresh battery value immediately.
      updatePhoneBattery(true);
    } else if (
      type === 104 ||
      type === 106
    ) {
      // A not-yet-acknowledged settings save takes precedence over an older snapshot.
      if (
        !pendingConfiguration
      ) {
        var settings =
          readSettings();

        if (
          payload.weather_fmt ===
            0 ||
          payload.weather_fmt ===
            1
        ) {
          settings.weather_fmt =
            payload.weather_fmt ===
            1;
        }

        if (
          finiteNumber(
            payload.weather_update
          ) &&
          (
            payload.weather_update === 0 ||
            payload.weather_update === 10 ||
            payload.weather_update === 15 ||
            payload.weather_update === 30 ||
            payload.weather_update === 60
          )
        ) {
          settings.weather_update =
            payload.weather_update;
        }

        if (
          payload.track_battery ===
            0 ||
          payload.track_battery ===
            1
        ) {
          settings.track_battery =
            payload.track_battery;
        }

        if (
          has(
            settingsByKey,
            'complication_mode'
          ) &&
          finiteNumber(
            payload.complication_mode
          ) &&
          payload.complication_mode >=
            0 &&
          payload.complication_mode <=
            4 &&
          Math.floor(
            payload.complication_mode
          ) ===
            payload.complication_mode
        ) {
          settings.complication_mode =
            payload.complication_mode;
        }

        if (
          has(
            settingsByKey,
            'tap_to_cycle'
          ) &&
          (
            payload.tap_to_cycle ===
              0 ||
            payload.tap_to_cycle ===
              1
          )
        ) {
          settings.tap_to_cycle =
            payload.tap_to_cycle ===
            1;
        }

        applyPhoneSettings(
          settings
        );
      }

      if (
        type === 104
      ) {
        if (
          payload.send_watch_version !==
          undefined
        ) {
          localStorage.setItem(
            'version_watch',
            String(
              payload.send_watch_version
            )
          );
        }

        if (
          payload.send_config_version !==
          undefined
        ) {
          localStorage.setItem(
            'version_config',
            String(
              payload.send_config_version
            )
          );
        }

        // Polling already starts in the JS ready handler. Re-arm the interval
        // without forcing a duplicate battery transmission after the handshake.
        startPolling(false);
      } else {
        requestWeather();
      }
    }
  }
);

var SunCalc;

(function () {
  "use strict";

  var PI = Math.PI;
  var sin = Math.sin;
  var cos = Math.cos;
  var tan = Math.tan;
  var asin = Math.asin;
  var atan = Math.atan2;
  var acos = Math.acos;
  var rad = PI / 180;

  var dayMs =
    1000 * 60 * 60 * 24;

  var J1970 = 2440588;
  var J2000 = 2451545;

  function toJulian(date) {
    return (
      date.valueOf() /
      dayMs -
      0.5 +
      J1970
    );
  }

  function fromJulian(j) {
    return new Date(
      (
        j +
        0.5 -
        J1970
      ) *
      dayMs
    );
  }

  function toDays(date) {
    return (
      toJulian(date) -
      J2000
    );
  }

  function getRightAscension(
    l,
    b
  ) {
    return atan(
      sin(l) *
        cos(
          rad * 23.4397
        ) -
        tan(b) *
          sin(
            rad * 23.4397
          ),

      cos(l)
    );
  }

  function getDeclination(
    l,
    b
  ) {
    return asin(
      sin(b) *
        cos(
          rad * 23.4397
        ) +
        cos(b) *
          sin(
            rad * 23.4397
          ) *
          sin(l)
    );
  }

  function getAzimuth(
    H,
    phi,
    dec
  ) {
    return atan(
      sin(H),

      cos(H) *
        sin(phi) -
        tan(dec) *
          cos(phi)
    );
  }

  function getAltitude(
    H,
    phi,
    dec
  ) {
    return asin(
      sin(phi) *
        sin(dec) +
        cos(phi) *
          cos(dec) *
          cos(H)
    );
  }

  function getSiderealTime(
    d,
    lw
  ) {
    return (
      rad *
        (
          280.16 +
          360.9856235 *
            d
        ) -
      lw
    );
  }

  function getSolarMeanAnomaly(
    d
  ) {
    return (
      rad *
      (
        357.5291 +
        0.98560028 *
          d
      )
    );
  }

  function getEquationOfCenter(
    M
  ) {
    return (
      rad *
      (
        1.9148 *
          sin(M) +
        0.02 *
          sin(2 * M) +
        0.0003 *
          sin(3 * M)
      )
    );
  }

  function getEclipticLongitude(
    M,
    C
  ) {
    return (
      M +
      C +
      rad *
        102.9372 +
      PI
    );
  }

  function getSunCoords(d) {
    var M =
      getSolarMeanAnomaly(d);

    var C =
      getEquationOfCenter(M);

    var L =
      getEclipticLongitude(
        M,
        C
      );

    return {
      dec:
        getDeclination(
          L,
          0
        ),

      ra:
        getRightAscension(
          L,
          0
        )
    };
  }

  SunCalc = {};

  SunCalc.getPosition =
    function (
      date,
      lat,
      lng
    ) {
      var lw =
        rad * -lng;

      var phi =
        rad * lat;

      var d =
        toDays(date);

      var c =
        getSunCoords(d);

      var H =
        getSiderealTime(
          d,
          lw
        ) -
        c.ra;

      return {
        azimuth:
          getAzimuth(
            H,
            phi,
            c.dec
          ),

        altitude:
          getAltitude(
            H,
            phi,
            c.dec
          )
      };
    };

  var times = [
    [
      -0.83,
      'sunrise',
      'sunset'
    ],

    [
      -0.3,
      'sunriseEnd',
      'sunsetStart'
    ],

    [
      -6,
      'dawn',
      'dusk'
    ],

    [
      -12,
      'nauticalDawn',
      'nauticalDusk'
    ],

    [
      -18,
      'nightEnd',
      'night'
    ],

    [
      6,
      'goldenHourEnd',
      'goldenHour'
    ]
  ];

  function getJulianCycle(
    d,
    lw
  ) {
    return Math.round(
      d -
      0.0009 -
      lw /
        (2 * PI)
    );
  }

  function getApproxTransit(
    Ht,
    lw,
    n
  ) {
    return (
      0.0009 +
      (
        Ht +
        lw
      ) /
        (2 * PI) +
      n
    );
  }

  function getSolarTransitJ(
    ds,
    M,
    L
  ) {
    return (
      J2000 +
      ds +
      0.0053 *
        sin(M) -
      0.0069 *
        sin(2 * L)
    );
  }

  function getHourAngle(
    h,
    phi,
    d
  ) {
    return acos(
      (
        sin(h) -
        sin(phi) *
          sin(d)
      ) /
      (
        cos(phi) *
        cos(d)
      )
    );
  }

  SunCalc.getTimes =
    function (
      date,
      lat,
      lng
    ) {
      var lw =
        rad * -lng;

      var phi =
        rad * lat;

      var d =
        toDays(date);

      var n =
        getJulianCycle(
          d,
          lw
        );

      var ds =
        getApproxTransit(
          0,
          lw,
          n
        );

      var M =
        getSolarMeanAnomaly(
          ds
        );

      var C =
        getEquationOfCenter(
          M
        );

      var L =
        getEclipticLongitude(
          M,
          C
        );

      var dec =
        getDeclination(
          L,
          0
        );

      var Jnoon =
        getSolarTransitJ(
          ds,
          M,
          L
        );

      function getSetJ(h) {
        var w =
          getHourAngle(
            h,
            phi,
            dec
          );

        var a =
          getApproxTransit(
            w,
            lw,
            n
          );

        return getSolarTransitJ(
          a,
          M,
          L
        );
      }

      var result = {
        solarNoon:
          fromJulian(
            Jnoon
          ),

        nadir:
          fromJulian(
            Jnoon -
            0.5
          )
      };

      var i;
      var len;
      var time;
      var Jset;
      var Jrise;

      for (
        i = 0,
        len = times.length;
        i < len;
        i += 1
      ) {
        time =
          times[i];

        Jset =
          getSetJ(
            time[0] *
              rad
          );

        Jrise =
          Jnoon -
          (
            Jset -
            Jnoon
          );

        result[
          time[1]
        ] =
          fromJulian(
            Jrise
          );

        result[
          time[2]
        ] =
          fromJulian(
            Jset
          );
      }

      return result;
    };

  function getMoonCoords(d) {
    var L =
      rad *
      (
        218.316 +
        13.176396 *
          d
      );

    var M =
      rad *
      (
        134.963 +
        13.064993 *
          d
      );

    var F =
      rad *
      (
        93.272 +
        13.229350 *
          d
      );

    var l =
      L +
      rad *
        6.289 *
        sin(M);

    var b =
      rad *
        5.128 *
        sin(F);

    var dt =
      385001 -
      20905 *
        cos(M);

    return {
      ra:
        getRightAscension(
          l,
          b
        ),

      dec:
        getDeclination(
          l,
          b
        ),

      dist: dt
    };
  }

  SunCalc.getMoonIllumination =
    function (date) {
      var d =
        toDays(date);

      var s =
        getSunCoords(d);

      var m =
        getMoonCoords(d);

      var sdist =
        149598000;

      var phi =
        acos(
          sin(s.dec) *
            sin(m.dec) +
          cos(s.dec) *
            cos(m.dec) *
            cos(
              s.ra -
              m.ra
            )
        );

      var inc =
        atan(
          sdist *
            sin(phi),

          m.dist -
            sdist *
              cos(phi)
        );

      return {
        fraction:
          (
            1 +
            cos(inc)
          ) /
          2,

        angle:
          atan(
            cos(s.dec) *
              sin(
                s.ra -
                m.ra
              ),

            sin(s.dec) *
              cos(m.dec) -
              cos(s.dec) *
                sin(m.dec) *
                cos(
                  s.ra -
                  m.ra
                )
          )
      };
    };
}());

function isItNight() {
  if (
    typeof lastCoordinates ===
      'undefined' ||
    !lastCoordinates
  ) {
    return false;
  }

  var now =
    new Date();

  var sunInfo =
    SunCalc.getTimes(
      now,
      lastCoordinates.latitude,
      lastCoordinates.longitude
    );

  return (
    sunInfo.sunset < now ||
    now < sunInfo.sunrise
  );
}

function getMoonIcon() {
  var now =
    new Date();

  var moonInfo =
    SunCalc.getMoonIllumination(
      now
    );

  if (
    moonInfo.fraction <=
    0.05
  ) {
    return "O";
  }

  if (
    moonInfo.fraction >=
    0.95
  ) {
    return "S";
  }

  if (
    moonInfo.angle < 0
  ) {
    return (
      moonInfo.fraction <= 0.35
    )
      ? "P"
      : (
          moonInfo.fraction <= 0.65
        )
        ? "Q"
        : "R";
  }

  return (
    moonInfo.fraction <= 0.35
  )
    ? "T"
    : (
        moonInfo.fraction <= 0.65
      )
      ? "U"
      : "V";
}