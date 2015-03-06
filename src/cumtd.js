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
    console.log("Time: " + nextBusTime);
    nextBusTime = nextBusTime+ " minutes";
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
  xhrRequest(url, 'GET',
    reaction
             
             /*function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      
      // Temperature in Kelvin requires adjustment
      var stop_id = json.stops[0].stop_id;
      console.log("Stop ID is " + stop_id);
   
      // Conditions
      var stop_name = json.stops[0].stop_name;
      console.log("Stop name is " + stop_name);
      
      var url2 = "https://developer.cumtd.com/api/v2.2/json/GetDeparturesByStop?key=9a1fc4aa940a4e5384a8950cc476b5e0&stop_id=" + stop_id;
  
      // Send request to the CUMTD
      xhrRequest(url2, 'GET',
        function(responseText) {
          // responseText contains a JSON object with weather info
          var json2 = JSON.parse(responseText);

          var expected = json2.departures[0].expected;
          console.log("Expected time to arrive: " + expected);

          var headsign = json2.departures[0].headsign;
          console.log("Trip Headsign is " + headsign);
          
          // Assemble dictionary using our keys
          var dictionary = {
            "KEY_STOPID": stop_id,
            "KEY_STOPNAME": stop_name,
            "KEY_ARRIVAL": expected,
            "KEY_SIGN": headsign,
          };
          
          // Send to Pebble
          Pebble.sendAppMessage(dictionary,
            function(e) {
              console.log("Stops info sent to Pebble successfully!");
            },
            function(e) {
              console.log("Error sending stops info to Pebble!");
            });
        });
    }*/
  );
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