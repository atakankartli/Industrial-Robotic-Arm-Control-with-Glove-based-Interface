const i2c = require('i2c-bus');
const ads1x15 = require('ads1x15');
const fs = require('fs');
const Struct = require('struct');
const { unzipSync } = require('zlib');


const express = require('express');
const app = express();
const server = require('http').Server(app);
const io = require('socket.io')(server);


app.use(express.static(__dirname));

app.get('/', function (req, res) {
    res.sendFile(__dirname + '/gui.html');
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


var robotControl = Struct()
  .word16Ule('Command')         // Command
  .word16Ule('SendType')        // Command data type designation
  .word16Ule('RecvType')        // Monitor data type designation
  .word32Ule('pos1')
  .word32Ule('pos2')
  .word32Ule('pos3')
  .word32Ule('pos4')
  .word32Ule('pos5')
  .word32Ule('pos6')
  .word32Ule('pos7')
  .word32Ule('pos8')
  .word16Ule('SendIOType')      // Send input/output signal data designation
  .word16Ule('RecvIOType')      // Return input/output signal data designation
  .word16Ule('BitTop')          // Head bit No.
  .word16Ule('BitMask')         // Transmission bit mask pattern desination (0x0001-0xffff)
  .word16Ule('IoData')          // Input/output signal data (0x0000-0xffff)
  .word16Ule('TCount')          // Timeout time counter value
  .word32Ule('CCount')          // Transmission data counter value
  .word16Ule('RecvType1')       // Reply data-type specification 1
  .word32Ule('pos1recv1')
  .word32Ule('pos2recv1')
  .word32Ule('pos3recv1')       // XYZ type [mm/rad] 
  .word32Ule('pos4recv1')       // Joint type [rad]
  .word32Ule('pos5recv1')
  .word32Ule('pos6recv1')
  .word32Ule('pos7recv1')
  .word32Ule('pos8recv1')
  .word16Ule('RecvType2')       // Reply data-type specification 2
  .word32Ule('pos1recv2')
  .word32Ule('pos2recv2')
  .word32Ule('pos3recv2')
  .word32Ule('pos4recv2')
  .word32Ule('pos5recv2')
  .word32Ule('pos6recv2')
  .word32Ule('pos7recv2')
  .word32Ule('pos8recv2')
  .word16Ule('RecvType3')       // Reply data-type specification 3
  .word32Ule('pos1recv3')
  .word32Ule('pos2recv3')
  .word32Ule('pos3recv3')
  .word32Ule('pos4recv3')
  .word32Ule('pos5recv3')
  .word32Ule('pos6recv3')
  .word32Ule('pos7recv3')
  .word32Ule('pos8recv3');



let fifoNode2C = fs.createWriteStream('/home/timeserverD/cr751/node2c.fifo');



robotControl.allocate();
var robotControlBuffer = robotControl.buffer();
var robotControlFields = robotControl.fields;

let calcVoltage;

const main = async () => {

  const adc = new ads1x15();
  // open i2c bus. 0 for /dev/i2c-0 and 1 for /dev/i2c-1
  await adc.openBus(1);

  // Reading in Single shot mode channels 0-3
  console.log('Reading Single shot:');
  for await (let channel of [0]) {
    const measure = await adc.readSingleEnded({channel});
    calcVoltage = (measure*0.02095) - 24.76;
    console.log("VOLTAGE: ", calcVoltage);

    if(calcVoltage > 2)
    {
      robotControlFields.pos1 = robotControlFields.pos + 0.01;


    }




    // fifoNode2C.write(JSON.stringify(data.PTPConfigFromWeb));
  }

}


setInterval(main, 1000);
// setInterval(sendingFunc, 500);