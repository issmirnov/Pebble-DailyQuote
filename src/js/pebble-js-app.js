/** DAILY QUOTE - Javascript phone app companion
 * Author: Ivan Smirnov (http://ivansmirnov.name), isgsmirnov@gmail.com
*/

/* Performs an async GET to the iheartquotes API for a one line quote, displays it. */
function downloadQuote() {
  console.log("downloading quote");
  var URL = "http://www.iheartquotes.com/api/v1/random?max_lines=1&format=json";
  // Download data
  var req = new XMLHttpRequest();
  var response;
  // build the GET request
  req.open('GET', URL, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      // 200 - HTTP OK
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        if (response.quote) {
          console.log("sending quote: " + response.quote);
          sendMessage(response.quote);
        }
      } else {
        console.log("Request returned error code " + req.status.toString());
      }
    }
  };
  req.send(null);
}

/* Wrapper function - takes a string quote and sends it to the Pebble. */
function sendMessage(quote) {
   Pebble.sendAppMessage({"status": 0, "message" : quote});
}
												
/* Called when incoming message from the Pebble is received */
Pebble.addEventListener("appmessage", function(e) {
    console.log("Received Status: " + e.payload.status);
    downloadQuote();
});