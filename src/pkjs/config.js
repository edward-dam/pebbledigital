//Author: Ed Dam

module.exports = [
  { "type": "heading", "defaultValue": "Digital v1.5" },
  { "type": "text", "defaultValue": "by Edward Dam" },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Time" },
    { "type": "text", "defaultValue": "Please Choose 24 Hours or 12 Hours" },
    { "type": "radiogroup", "messageKey": "apihour", "options": [
      { "label": "24 Hours", "value": "24hrs" },
      { "label": "12 Hours", "value": "12hrs" } ],
    "defaultValue": "24hrs" } ]
  },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Date" },
    { "type": "text", "defaultValue": "Please Choose Date Order" },
    { "type": "radiogroup", "messageKey": "apidate", "options": [
      { "label": "Day - Month", "value": "daymonth" },
      { "label": "Month - Day", "value": "monthday" } ],
    "defaultValue": "daymonth" } ]
  },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Temperature" },
    { "type": "text", "defaultValue": "Please Choose Celsius or Fahrenheit" },
    { "type": "radiogroup", "messageKey": "apitemp", "options": [
      { "label": "Celsius °C", "value": "celsius" },
      { "label": "Fahrenheit °F", "value": "fahrenheit" } ],
    "defaultValue": "celsius" } ]
  },
  { "type": "text", "defaultValue": "Thank you for using Digital v1.5 by Edward Dam" },
  { "type": "submit", "defaultValue": "Save Settings" }
];