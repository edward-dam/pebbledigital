//Author: Ed Dam

module.exports = [
  { "type": "heading", "defaultValue": "Digital v2.0" },
  { "type": "text", "defaultValue": "by Edward Dam" },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Time" },
    { "type": "text", "defaultValue": "Please Choose 24 Hours or 12 Hours" },
    { "type": "radiogroup", "messageKey": "HOUR", "options": [
      { "label": "24 Hours", "value": "false" },
      { "label": "12 Hours", "value": "true" } ],
    "defaultValue": "false" } ]
  },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Date" },
    { "type": "text", "defaultValue": "Please Choose Date Order" },
    { "type": "radiogroup", "messageKey": "DATE", "options": [
      { "label": "Day - Month", "value": "false" },
      { "label": "Month - Day", "value": "true" } ],
    "defaultValue": "false" } ]
  },
  { "type": "section", "capabilities": ["HEALTH"], "items": [
    { "type": "heading", "defaultValue": "Location & Health" },
    { "type": "text", "defaultValue": "Please Choose Location or Health" },
    { "type": "radiogroup", "messageKey": "HEALTH", "options": [
      { "label": "Location", "value": "false" },
      { "label": "Steps/Sleep", "value": "true" } ],
    "defaultValue": "false" } ]
  },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Temperature" },
    { "type": "text", "defaultValue": "Please Choose Fahrenheit or Celsius" },
    { "type": "radiogroup", "messageKey": "DEGREE", "options": [
      { "label": "Fahrenheit °F", "value": "false" },
      { "label": "Celsius °C", "value": "true" } ],
    "defaultValue": "false" } ]
  },
  { "type": "text", "defaultValue": "Thank you for using my watchface." },
  { "type": "submit", "defaultValue": "Save Settings" }
];
