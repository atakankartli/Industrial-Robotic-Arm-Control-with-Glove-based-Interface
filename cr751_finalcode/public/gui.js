var socket = io();


socket.on('printCoordinate', function(data) {
    document.getElementById("currentX").value = data.coordinateX;
    document.getElementById("currentY").value = data.coordinateY;
    document.getElementById("currentZ").value = data.coordinateZ;
    document.getElementById("recvData").value = data.coordinateZ;
});


function sendXYZCoordinates() {
    var x = document.getElementById("inputX").value;
    var y = document.getElementById("inputY").value;
    var z = document.getElementById("inputZ").value;
    console.log("Sending XYZ Coordinates: " + x + ", " + y + ", " + z);
    // Code to send the XYZ coordinates to the robot arm controller
}

function changePositionOneByOne(axis, direction) {
    console.log("AXIS: " + axis + " DIRECTION: " + direction);
    socket.emit("changePositionOneByOne-event", {axis: axis, direction: direction});
}

let rangeX = document.getElementById("rangeX");
let rangeY = document.getElementById("rangeY");
let rangeZ = document.getElementById("rangeZ");

rangeX.addEventListener("input", function() {
    socket.emit("changePositionRange-event", {axis: 'x', rangeValue: (rangeX.value - 5.0) / 10});
}, false); 

rangeY.addEventListener("input", function() {
    socket.emit("changePositionRange-event", {axis: 'y', rangeValue: (rangeX.value - 5.0) / 10});
}, false); 

rangeZ.addEventListener("input", function() {
    socket.emit("changePositionRange-event", {axis: 'z', rangeValue: (rangeX.value - 5.0) / 10});
}, false); 


function mouseDown(axis, direction) {
    socket.emit("changePositionMouseDown-event", {axis: axis, direction: direction});
}
  
function mouseUp(axis, direction) {
    socket.emit("changePositionMouseUp-event", {axis: axis, direction: direction});
}








