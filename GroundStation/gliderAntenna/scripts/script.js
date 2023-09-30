// Global Variables...
var phonelocation = {
  latitude:0,
  longitude:0,
  altitude:0
}
var gliderlocation = {
  latitude:0,
  longitude:0,
  altitude:0
}




if ("geolocation" in navigator) {
  navigator.geolocation.getCurrentPosition(handlePositionData, handleError);
} else {
  // Geolocation is not available in this browser
  console.log("Geolocation not available in this browser");
}

function handlePositionData(position) {
  // Handle successful position retrieval
  phonelocation.latitude = position.coords.latitude;
  phonelocation.longitude = position.coords.longitude;
  phonelocation.altitude = position.coords.altitude;
  // Use latitude and longitude data
  document.getElementById("latlong").textContent = phonelocation.latitude + ", " +
                                                   phonelocation.longitude + ", " +
                                                   phonelocation.altitude;
  console.log(phonelocation);
}

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
  console.log(error);
}
