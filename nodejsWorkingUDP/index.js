// const i2c = require('i2c-bus');
// const ads1x15 = require('ads1x15');
const fs = require('fs');
const Struct = require('struct');
const { unzipSync } = require('zlib');


const express = require('express');
const app = express();
const server = require('http').Server(app);
const io = require('socket.io')(server);

const dgram = require('dgram');


let positionData = {
    X: 0.0,
    Y: 0.0,
    Z: 0.0,
    A: 0.0,
    B: 0.0,
    C: 0.0,
    L1: 0.0,
    L2: 0.0
};

let firstInput = 0;


app.use(express.static(__dirname));

app.get('/gui.html', function (req, res) {
    res.sendFile(__dirname + '/public' + '/gui.html');
});

io.on('connection', function (socket) {
    console.log('a user connected');

    socket.on('disconnect', function () {
        console.log('user disconnected');
    });

    socket.on('send-xyz-coordinates', function (data) {
        console.log("Sending XYZ Coordinates: " + data);
        // Code to send the XYZ coordinates to the robot arm controller
        port.write(data + '\n');
    });

    socket.on('send-joint-values', function (data) {
        console.log("Sending Joint Values: " + data);
        // Code to send the joint values to the robot arm controller
        port.write(data + '\n');
    });



    socket.on('changePositionOneByOne-event', function (data) {
        console.log("Received new pos data: " + data);
        
        
    });

    


});

server.listen(3000, function () {
    console.log('listening on *:3000');
});


let robotControlSend = Struct()
  .word16Ule('Command')         // Command
  .word16Ule('reservestart')        // Command data type designation
  .word16Ule('RecvType1')       // Reply data-type specification 3
  .word16Ule('reserve1')
  .floatle('pos1')
  .floatle('pos2')
  .floatle('pos3')
  .floatle('pos4')
  .floatle('pos5')
  .floatle('pos6')
  .floatle('pos7')
  .floatle('pos8')
  .word64Ule('respos1')
  .word16Ule('SendIOType')      // Send input/output signal data designation
  .word16Ule('RecvIOType')      // Return input/output signal data designation
  .word32Ule('inputSignalData')          // Head bit No.
  .word32Ule('outputSignalData')         // Transmission bit mask pattern desination (0x0001-0xffff)
  .word32Ule('CCount')          // Transmission data counter value
  .word16Ule('RecvType2')       // Reply data-type specification 1
  .word16Ule('reserve2')
  .floatle('pos1recv1')
  .floatle('pos2recv1')
  .floatle('pos3recv1')       // XYZ type [mm/rad] 
  .floatle('pos4recv1')       // Joint type [rad]
  .floatle('pos5recv1')
  .floatle('pos6recv1')
  .floatle('pos7recv1')
  .floatle('pos8recv1')
  .word64Ule('respos2')
  .word16Ule('RecvType3')       // Reply data-type specification 2
  .word16Ule('reserve3')
  .word32Ule('pos1recv2')
  .word32Ule('pos2recv2')
  .word32Ule('pos3recv2')
  .word32Ule('pos4recv2')
  .word32Ule('pos5recv2')
  .word32Ule('pos6recv2')
  .word32Ule('pos7recv2')
  .word32Ule('pos8recv2')
  .word64Ule('respos3')
  .word16Ule('RecvType4')       // Reply data-type specification 3
  .word16Ule('reserve4')
  .word32Ule('pos1recv3')
  .word32Ule('pos2recv3')
  .word32Ule('pos3recv3')
  .word32Ule('pos4recv3')
  .word32Ule('pos5recv3')
  .word32Ule('pos6recv3')
  .word32Ule('pos7recv3')
  .word32Ule('pos8recv3')
  .word64Ule('respos4');



//let fifoNode2C = fs.createWriteStream('/home/project/Desktop/CR751-driver-main/LinuxFiles/node2c.fifo');


let robotControlRecv = Struct()
  .word16Ule('Command')         // Command
  .word16Ule('reservestart')        // Command data type designation
  .word16Ule('RecvType1')       // Reply data-type specification 3
  .word16Ule('reserve1')
  .floatle('pos1')
  .floatle('pos2')
  .floatle('pos3')
  .floatle('pos4')
  .floatle('pos5')
  .floatle('pos6')
  .floatle('pos7')
  .floatle('pos8')
  .word64Ule('respos1')
  .word16Ule('SendIOType')      // Send input/output signal data designation
  .word16Ule('RecvIOType')      // Return input/output signal data designation
  .word32Ule('inputSignalData')          // Head bit No.
  .word32Ule('outputSignalData')         // Transmission bit mask pattern desination (0x0001-0xffff)
  .word32Ule('CCount')          // Transmission data counter value
  .word16Ule('RecvType2')       // Reply data-type specification 1
  .word16Ule('reserve2')
  .floatle('pos1recv1')
  .floatle('pos2recv1')
  .floatle('pos3recv1')       // XYZ type [mm/rad] 
  .floatle('pos4recv1')       // Joint type [rad]
  .floatle('pos5recv1')
  .floatle('pos6recv1')
  .floatle('pos7recv1')
  .floatle('pos8recv1')
  .word64Ule('respos2')
  .word16Ule('RecvType3')       // Reply data-type specification 2
  .word16Ule('reserve3')
  .word32Ule('pos1recv2')
  .word32Ule('pos2recv2')
  .word32Ule('pos3recv2')
  .word32Ule('pos4recv2')
  .word32Ule('pos5recv2')
  .word32Ule('pos6recv2')
  .word32Ule('pos7recv2')
  .word32Ule('pos8recv2')
  .word64Ule('respos3')
  .word16Ule('RecvType4')       // Reply data-type specification 3
  .word16Ule('reserve4')
  .word32Ule('pos1recv3')
  .word32Ule('pos2recv3')
  .word32Ule('pos3recv3')
  .word32Ule('pos4recv3')
  .word32Ule('pos5recv3')
  .word32Ule('pos6recv3')
  .word32Ule('pos7recv3')
  .word32Ule('pos8recv3')
  .word64Ule('respos4');



robotControlSend.allocate();
let robotControlSendBuffer = robotControlSend.buffer();
let robotControlSendFields = robotControlSend.fields;

robotControlRecv.allocate();
let robotControlRecvBuffer = robotControlSend.buffer();
let robotControlRecvFields = robotControlSend.fields;

robotControlSendFields.Command = 0x0000;
robotControlSendFields.reservestart = 0x0000;
robotControlSendFields.reserve1 = 0x0001;
robotControlSendFields.RecvType1 = 0x0001;
robotControlSendFields.reserve2 = 0x0001;
robotControlSendFields.RecvType2 = 0x0001;
robotControlSendFields.reserve3 = 0x0001;
robotControlSendFields.RecvType3 = 0x0001;
robotControlSendFields.reserve4 = 0x0001;
robotControlSendFields.RecvType4 = 0x0001;

robotControlSendFields.SendIOType = 0x0000;

// robotControlSendFields.Command = 0x0001;
// robotControlSendFields.reservestart = 0x0002;
// robotControlSendFields.reserve1 = 0x0002;
// robotControlSendFields.RecvType1 = 0x0002;
// robotControlSendFields.reserve2 = 0x0002;
// robotControlSendFields.RecvType2 = 0x0002;
// robotControlSendFields.reserve3 = 0x0002;
// robotControlSendFields.RecvType3 = 0x0002;
// robotControlSendFields.reserve4 = 0x0002;
// robotControlSendFields.RecvType4 = 0x0002;


// robotControlSendFields.pos1 = 0;
// robotControlSendFields.pos2 = 0;
// robotControlSendFields.po3 = 0;
// robotControlSendFields.pos4 = 0;
// robotControlSendFields.pos5 = 0;
// robotControlSendFields.pos6 = 0;
// robotControlSendFields.pos7 = 0;
// robotControlSendFields.pos8 = 0;

// robotControlSendFields.respos4 = 0xffffffff;

console.log("DATA: " + robotControlSendBuffer.toString('hex'));

let calcVoltage;

// const main = async () => {

//   const adc = new ads1x15();
//   // open i2c bus. 0 for /dev/i2c-0 and 1 for /dev/i2c-1
//   await adc.openBus(1);

//   // Reading in Single shot mode channels 0-3
//   console.log('Reading Single shot:');
//   for await (let channel of [0]) {
//     const measure = await adc.readSingleEnded({channel});
//     calcVoltage = (measure*0.02095) - 24.76;
//     console.log("VOLTAGE: ", calcVoltage);

//     if(calcVoltage > 2)
//     {
//       robotControlSendFields.pos1 = robotControlSendFields.pos + 0.01;


//     }

//     // fifoNode2C.write(JSON.stringify(data.PTPConfigFromWeb));
//   }

// }

let sock = dgram.createSocket('udp4');
sock.send((robotControlSendBuffer), 10000, '192.168.0.20');


let counter = 0;

sock.on('message', function(msg, rinfo) {
    // console.log('DATA: ' + msg.toString('hex'));

    msg.copy(robotControlRecvBuffer);

  
    // Writing 32bit or 4 byte floating point
    // values to the buffer and printing
    // returned value to console
    console.log('POS1 REC: ' + robotControlRecvFields.pos1recv1);
    console.log('POS2 REC: ' + robotControlRecvFields.pos2);
    console.log('POS3 REC: ' + robotControlRecvFields.pos3);
    // console.log('POS4: ' + robotControlRecvFields.pos4);
    // console.log('POS5: ' + robotControlRecvFields.pos5);
    // console.log('POS6: ' + robotControlRecvFields.pos6);
    // console.log('POS7: ' + robotControlRecvFields.pos7);
    // console.log('POS8: ' + robotControlRecvFields.pos8);

    // console.log('robotControlRecvBuffer: ' + robotControlRecvBuffer.toString('hex'));

    if(firstInput == 0) {

        positionData.X = robotControlRecvFields.pos1recv1;
        positionData.Y = robotControlRecvFields.pos1recv2;
        positionData.Z = robotControlRecvFields.pos1recv3;
        positionData.A = robotControlRecvFields.pos1recv4;
        positionData.B = robotControlRecvFields.pos1recv5;
        positionData.C = robotControlRecvFields.pos1recv6;
        positionData.L1 = robotControlRecvFields.pos1recv7;
        positionData.L2 = robotControlRecvFields.pos1recv8;
        firstInput++;

    }

    io.emit("printCoordinate", {coordinateX: positionData.X, 
        coordinateY: positionData.Y, 
        coordinateZ: positionData.Z });

    sendPos();
    

});

let delta = 0.3;
let ratio = 1.0;

let increment = 0.0;

function sendPos() {

    robotControlRecvBuffer.copy(robotControlSendBuffer);

    robotControlSendFields.Command = 0x0001;
    robotControlSendFields.reservestart = 0x0001;
    robotControlSendFields.reserve1 = 0x0001;
    robotControlSendFields.RecvType1 = 0x0001;
    robotControlSendFields.reserve2 = 0x0001;
    robotControlSendFields.RecvType2 = 0x0001;
    robotControlSendFields.reserve3 = 0x0001;
    robotControlSendFields.RecvType3 = 0x0001;
    robotControlSendFields.reserve4 = 0x0001;
    robotControlSendFields.RecvType4 = 0x0001;


    robotControlSendFields.pos1 = positionData.X;
    robotControlSendFields.pos2 = positionData.Y;
    robotControlSendFields.pos3 = positionData.Z;
    robotControlSendFields.pos4 = positionData.A;
    robotControlSendFields.pos5 = positionData.B;
    robotControlSendFields.pos6 = positionData.C;
    robotControlSendFields.pos7 = positionData.L1;
    robotControlSendFields.pos8 = positionData.L2;




    robotControlSendFields.respos4 = 0xffffffff;

    console.log('POS2 TRX: ' + robotControlSendFields.pos2);

    // robotControlSendFields.Command = 0x0000;
    // robotControlSendFields.reservestart = 0x0000;

    sock.send((robotControlSendBuffer), 10000, '192.168.0.20');

}



// setInterval(main, 1000);
// setInterval(sendingFunc, 500);

function main() {
    console.log("SENT");
    io.emit("printCoordinate", {coordinateX: 12.43});
}