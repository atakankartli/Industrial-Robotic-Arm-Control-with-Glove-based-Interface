var socket = io();

socket.on('printCoordinate', function(data) {
    console.log(data.coordinateX);
    document.getElementById("currentX").value = data.coordinateX;
    document.getElementById("currentY").value = data.coordinateY;
    document.getElementById("currentZ").value = data.coordinateZ;
});


function sendXYZCoordinates() {
    var x = document.getElementById("inputX").value;
    var y = document.getElementById("inputY").value;
    var z = document.getElementById("inputZ").value;
    console.log("Sending XYZ Coordinates: " + x + ", " + y + ", " + z);
    // Code to send the XYZ coordinates to the robot arm controller
}

var radius = 100;

var sampleJoystick = {
    mode: 'static',
    position: {
      left: '50%',
      top: '50%'
    },
    size: radius*2,
    color: 'black'
};

var joystick;
var position;

joystick = nipplejs.create(sampleJoystick);
joystick.on('start end', function(evt, data) {
  position = data;
}).on('move', function(evt, data) {
  position = data;
}).on('dir:up plain:up dir:left plain:left dir:down' +
      'plain:down dir:right plain:right',
      function(evt, data) {
  //position=data;
}
     ).on('pressure', function(evt, data) {
  position=data;
});
