const EARTH_RADIUS = 6371 * 1000 // km to m

// globally accessible variables that hold angles and targets
var bearingGlobal;
var altitudeGlobal;
var bearingTargetGlobal;
var altitudeTargetGlobal;

// globally accessible chart var
var chart;

// globally accessible glider pos
var globalGliderDict;
var globalAccDict;

//Converts deg to rad and vice versa
function rad2deg(rad) {
  return rad * 180 / Math.PI;
}
function deg2rad(deg) {
  return deg * Math.PI / 180;
}

// Simple Bearing Calculations
function bearingCalc(devLat, devLong, targLat, targLong) {
  var dLong = devLong - targLong;
  var a = Math.cos(targLat) * Math.sin(dLong);
  var b = Math.cos(devLat) * Math.sin(targLat) - Math.sin(devLat) * Math.cos(targLat) * Math.cos(dLong);
  var bearing = rad2deg(Math.atan2(a, b));

  return bearing;
}

// Simple Altitude Calculations
function altCalc(devAlt, targAlt, distance) {
  dAlt = targAlt - devAlt
  phi = Math.atan(dAlt / distance)
  return rad2deg(phi);
}

// Computes Distance between Phone and Object, as if both were on earth's surface
function distCalc(devLat, devLong, targLat, targLong) {

  devLat = deg2rad(devLat);
  devLong = deg2rad(devLong);
  targLat = deg2rad(targLat);
  targLong = deg2rad(targLong);

  var devPoint = {
    Xpos: (EARTH_RADIUS * Math.cos(devLat) * Math.cos(devLong)),
    Ypos: (EARTH_RADIUS * Math.cos(devLat) * Math.sin(devLong)),
    Zpos: (EARTH_RADIUS * Math.sin(devLat))
  };

  var targPoint = {
    Xpos: (EARTH_RADIUS * Math.cos(targLat) * Math.cos(targLong)),
    Ypos: (EARTH_RADIUS * Math.cos(targLat) * Math.sin(targLong)),
    Zpos: (EARTH_RADIUS * Math.sin(targLat))
  };


  deltaX = devPoint.Xpos - targPoint.Xpos;
  deltaY = devPoint.Ypos - targPoint.Ypos;
  deltaZ = devPoint.Zpos - targPoint.Zpos;

  return Math.sqrt(Math.pow(deltaX, 2) + Math.pow(deltaY, 2) + Math.pow(deltaZ, 2));
}

//Grabbing Location Data
function getCurrPosition() {
  if (navigator.geolocation) {
    navigator.geolocation.getCurrentPosition(grabPositions, handleError);
  } else {
    console.log("Geolocation is not supported by this browser.");
  }
}

// Grabs positions of both target and phone
function grabPositions(position) {
    locationHandler(position, globalGliderDict);
}

// Handles Errors in Perms
function handleError(error) {
  switch (error.code) {
    case error.PERMISSION_DENIED:
      console.log("User denied the request for Geolocation.");
      break;
    case error.POSITION_UNAVAILABLE:
      console.log("Location information is unavailable.");
      break;
    case error.TIMEOUT:
      console.log("The request to get user location timed out.");
      break;
    case error.UNKNOWN_ERROR:
      console.log("An unknown error occurred.");
      break;
  }
}

// Handles Locations
function locationHandler(phonePos, targPos) {

  var latitude = phonePos.coords.latitude;
  var longitude = phonePos.coords.longitude;
  var targLatitude = targPos.latitude;
  var targLongitude = targPos.longitude;

  //m
  var alt = phonePos.coords.altitude;
  var targAlt = targPos.altitude;

  var phonePosElem = document.getElementById("phone-loc");
  var gliderPosElem = document.getElementById("glider-loc");

  // Calculations for Bearing
  pointDegree = bearingCalc(latitude, longitude, targLatitude, targLongitude);
  if (pointDegree < 0) {
    pointDegree = pointDegree + 360;
  }

  // Altitude Calculations
  distanceBtwn = distCalc(latitude, longitude, targLatitude, targLongitude);
  altDegree = altCalc(alt, targAlt, distanceBtwn);
  displayTargets(pointDegree, altDegree);

  phonePosElem.textContent = "Phone Pos:\n-> Lat: " + latitude + "\n-> Long: " + longitude + "\n-> Alt: " + alt;
  gliderPosElem.textContent = "Glider Pos:\n-> Lat: " + targLatitude + "\n-> Long: " + targLongitude + "\n-> Alt: " + targAlt;

}

// Perm Request
function requestPermission() {
  if (window.DeviceOrientationEvent && window.DeviceMotionEvent && navigator.geolocation) {
    // Request orientation data
    if (typeof DeviceOrientationEvent.requestPermission === 'function') {
      DeviceOrientationEvent.requestPermission()
        .then(permissionState => {
          if (permissionState === 'granted') {
            console.log('Orientation permission granted');
            // Start listening to orientation data
            window.addEventListener('deviceorientation', startOrientationTracking, true);
          }
        })
        .catch(console.error);
    } else {
      console.log('Orientation permission not required');
      // Start listening to orientation data
      window.addEventListener('deviceorientation', handleDeviceOrientation, true);
    }

    // Request location data
    if (typeof navigator.permissions !== 'undefined' && typeof navigator.permissions.query === 'function') {
      navigator.permissions.query({ name: 'geolocation' })
        .then(permissionStatus => {
          if (permissionStatus.state === 'granted') {
            console.log('Location permission granted');
            // Get location data
            navigator.geolocation.getCurrentPosition(grabPositions, handleError);
          }
        })
        .catch(console.error);
    } else if ('geolocation' in navigator) {
      console.log('Location permission not required');
      // Get location data
      navigator.geolocation.getCurrentPosition(grabPositions, handleError);
    } else {
      console.log('Geolocation API not supported');
    }
  } else {
    console.log('Device orientation, motion, or geolocation not supported');
  }
}

// Init Orientation
function startOrientationTracking() {
  window.addEventListener("deviceorientation", handleDeviceOrientation, true);
}

// Updates Whenever a change in phone orientation is detected
function handleDeviceOrientation(event) {
  var alpha = event.alpha || 0;  // device orientation value (in degrees)
  var beta = event.beta || 0;    // device tilt front-to-back (in degrees)
  //var gamma = event.gamma || 0;  // device tilt left-to-right (in degrees)

  //Check if absolute values have been sent
  if (typeof event.webkitCompassHeading !== "undefined") {
    alpha = event.webkitCompassHeading; //iOS non-standard
  }

  displayOrientation(alpha, beta);
}

// Displays Orientation Angles to HTML
function displayOrientation(alpha, beta) {
  var bearingAngleElement = document.getElementById("bearing-angle");
  var altitudeAngleElement = document.getElementById("altitude-angle");

  bearingAngleElement.textContent = "Bearing Angle: " + alpha.toFixed(2) + "°";
  altitudeAngleElement.textContent = "Altitude Angle: " + beta.toFixed(2) + "°";

  bearingGlobal = alpha;
  altitudeGlobal = beta;
}

// Displays Target Angles to HTML
function displayTargets(bearing, altitudeAngle) {
  var bearingTargetElement = document.getElementById("bearing-target");
  var altitudeTargetElement = document.getElementById("altitude-target");

  bearingTargetElement.textContent = "Bearing Target: " + bearing.toFixed(2) + "°";
  altitudeTargetElement.textContent = "Altitude Target: " + altitudeAngle.toFixed(2) + "°";

  bearingTargetGlobal = bearing || 0;
  altitudeTargetGlobal = altitudeAngle || 0;
}

// Initiailizes Chart Properites
function initChart() {
  // Initialize chart data
  var data = {
    labels: ['Crosshair', 'Red Dot'],
    datasets: [
      {
        label: 'Points',
        data: [],
        pointBackgroundColor: ['black', 'red'],
        pointRadius: 10,
        fill: false,
      }
    ]
  };

  // Chart options
  var options = {
    scales: {
      x: {
        min: -180,
        max: 180,
        ticks: {
          stepSize: 60
        }
      },
      y: {
        min: -120,
        max: 120,
        ticks: {
          stepSize: 60
        }
      }
    }
  };

  // Initialize chart
  var ctx = document.getElementById('myChart').getContext('2d');
  chart = new Chart(ctx, {
    type: 'scatter',
    data: data,
    options: options
  });
  chart.options.animation = false; // disables all animations
}

// Function to update chart data
function updateChart() {

  var bearingDiff = bearingGlobal - bearingTargetGlobal;
  if (bearingDiff > 180) {
    bearingDiff = -1 * (360 - bearingDiff);
  }

  var altDiff = altitudeGlobal - altitudeTargetGlobal;

  chart.data.datasets[0].data = [
    { x: bearingDiff, y: altDiff },   // Crosshair
    { x: 0, y: 0 }                    // Red Dot
  ];

  if (Math.abs(altDiff) < 4 && Math.abs(bearingDiff) < 4) {
    document.body.style.backgroundColor = "green";
  }
  else {
    document.body.style.backgroundColor = "white";
  }

  chart.update();
}

function fetchPythonData() {
  fetch('/get_data')
    .then(response => response.json())
    .then(data => {
      // Do something with the received data
      gps = data["gps"]
      acc = data["acc"]
      globalGliderDict = {latitude: gps[0], longitude: gps[1], altitude: gps[2]}
      globalAccDict = {x: acc[0], y: acc[1], z: acc[2]}
      console.log(data)
    })
    .catch(error => {
      console.error('Error:', error);
    });
}

// Starts Perms
function init() {
  // deletes button
  document.getElementById('perm-button').remove();
  document.getElementById('note').remove();


  // inits
  requestPermission();
  initChart();

  // loop starters
  setInterval(getCurrPosition);
  setInterval(updateChart);
  setInterval(fetchPythonData, 1000);
}
