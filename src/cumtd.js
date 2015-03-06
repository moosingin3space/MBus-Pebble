var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getBus(response, stop_id, stop_name){
  
  //get the bus name
  var json = JSON.parse(response);
  var busName = json.name;
  console.log("The next bus that is coming: " + busName);
  
    var nextBusTime = json.time;
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
function reaction(response){
     //We are parsing the json
     var json = JSON.parse(response);
     console.log(json);
     
     //collecting the nearest bus stop Id
     var stop_id = json.id;
     console.log('ID: ' + stop_id);
     
     //Collecting the name of that bus stop
     var stop_name = json.name;
     console.log('Name of the bus stop:' + stop_name);
  
     //Trying to gain access to the details of the incoming bus
     //Calling the other API
     var url = "http://pebblebus.herokuapp.com/next-bus?stop="+ stop_id;
     
     //Send request to the API provider
     xhrRequest(url, 'GET', function(response) {
       getBus(response, stop_id, stop_name);
     });
     
}
function locationSuccess(pos) {
  // Construct URL
  var url =  "http://pebblebus.herokuapp.com/closest-stop?lat=" + pos.coords.latitude + "&lon=" + pos.coords.longitude;
  // Send request to the CUMTD
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