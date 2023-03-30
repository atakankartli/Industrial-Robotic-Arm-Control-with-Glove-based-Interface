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

function changePositionOneByOne(axis, direction) {



}