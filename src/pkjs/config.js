//Author: Ed Dam

module.exports = [
  {
    "type": "heading",
    "defaultValue": "Digital v1.5"
  },
  {
    "type": "text",
    "defaultValue": "by Edward Dam"
  },
    {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Time"
      },
      {
        "type": "text",
        "defaultValue": "Please Choose 24 Hours or 12 Hours"
      },
      {
        "type": "radiogroup",
        "messageKey": "hours",
        "options": [
          { 
            "label": "24 Hours", 
            "value": "twentyfour" 
          },
          { 
            "label": "12 Hours", 
            "value": "twelve" 
          },
        ],
        "defaultValue": "twentyfour"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Temperature"
      },
      {
        "type": "text",
        "defaultValue": "Please Choose Celsius or Fahrenheit"
      },
      {
        "type": "radiogroup",
        "messageKey": "degrees",
        "options": [
          { 
            "label": "Celsius °C", 
            "value": "celsius" 
          },
          { 
            "label": "Fahrenheit °F", 
            "value": "fahrenheit" 
          },
        ],
        "defaultValue": "celsius"
      }
    ]
  },
  {
    "type": "text",
    "defaultValue": "Thanks for using Digital v1.5 by Edward Dam"
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];