var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    if (this.status == 200) {
      callback(this.responseText);
    } else {
      xhrRequest(url, type, callback);
    }
  };
  xhr.open(type, url);
  xhr.send();
};

function reaction(response){
     //We are parsing the json
     var json = JSON.parse(response);
     console.log(json);
  
     var stop = json[0];
  
     //collecting the nearest bus stop Id
     var stop_id = stop.id;
     console.log('ID: ' + stop_id);
     
     //Collecting the name of that bus stop
     var stop_name = stop.name;
     console.log('Name of the bus stop:' + stop_name);
  
    //get the bus name
    var busName = stop.buses[0].name;
    console.log("The next bus that is coming: " + busName);
    
    var nextBusTime = stop.buses[0].time;
    nextBusTime = nextBusTime + " minutes";
    console.log("Time: " + nextBusTime);
    //Assemble the information into a dictionary before passing it back to the C
    var dictionary = {
              "KEY_STOPID": stop_id,
              "KEY_STOPNAME": stop_name,
              "KEY_ARRIVAL": nextBusTime,
              "KEY_SIGN": busName,
            };
            
    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
                          function(e) {
                            console.log("Stops info sent to Pebble successfully!");
                          },
                          function(e) {
                            console.log("Error sending stops info to Pebble!");
                          });
}
function locationSuccess(pos) {
  // Construct URL
  var url = "http://pebblebus.appspot.com/closest-stops/@" + pos.coords.latitude + "," + pos.coords.longitude;
  // Send request to the PebbleBus Backend
  xhrRequest(url, 'GET', reaction);
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getStop() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
  function(e) {
    console.log("PebbleKit JS ready!");
    
    // Get initial weather
    getStop();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getStop();
  }
);