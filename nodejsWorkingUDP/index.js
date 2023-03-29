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
});

server.listen(3000, function () {
    console.log('listening on *:3000');
});



// typedef struct {
// 	float j1; // J1 axis angle (radian)
// 	float j2; // J2 axis angle (radian)
// 	float j3; // J3 axis angle (radian)
// 	float j4; // J4 axis angle (radian)
// 	float j5; // J5 axis angle (radian)
// 	float j6; // J6 axis angle (radian)
// 	float j7; // Additional axis 1 (J7 axis angle) (radian)
// 	float j8; // Additional axis 2 (J8 axis angle) (radian)
// } JOINT;

/*************************************************************/

// typedef struct {
// 	float x; // X axis coordinate value (mm)
// 	float y; // Y axis coordinate value (mm)
// 	float z; // Z axis coordinate value (mm)
// 	float a; // A axis coordinate value (radian)
// 	float b; // B axis coordinate value (radian)
// 	float c; // C axis coordinate value (radian)
// 	float l1; // Additional axis 1 (mm or radian)
// 	float l2; // Additional axis 2 (mm or radian)
// } WORLD;

// typedef struct {
// 	WORLD w;
// 	unsigned intsflg1; // Structural flag 1
// 	unsigned intsflg2; // Structural flag 2
// } POSE;


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

    io.emit("printCoordinate", {coordinateX: robotControlRecvFields.pos1recv1, 
        coordinateY: robotControlRecvFields.pos1recv2, 
        coordinateZ: robotControlRecvFields.pos1recv3, });

    sendPos();
    

});

let delta = 0.3;
let ratio = 1.0;

let increment = 0.0;

function sendPos() {

    increment = delta * ratio * 3.141592 / 180.0;

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


    counter ++;

    if(counter == 1) {

        delta = delta + 1000.0;
        robotControlSendFields.pos1 = robotControlSendFields.pos1;
        robotControlSendFields.pos2 = robotControlSendFields.pos2;
        robotControlSendFields.pos3 = robotControlSendFields.pos3 + 0.1;
        robotControlSendFields.pos4 = robotControlSendFields.pos4;
        robotControlSendFields.pos5 = robotControlSendFields.pos5;
        robotControlSendFields.pos6 = robotControlSendFields.pos6;
        robotControlSendFields.pos7 = robotControlSendFields.pos7;
        robotControlSendFields.pos8 = robotControlSendFields.pos8;
        counter = 0;

        console.log('ARTTI');
    }

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