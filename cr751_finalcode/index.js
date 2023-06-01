// const i2c = require('i2c-bus');
// const ads1x15 = require('ads1x15');
const fs = require('fs');
const Struct = require('struct');
const { unzipSync } = require('zlib');
const readline = require('readline');
const Gpio = require('pigpio').Gpio;

const express = require('express');
const app = express();
const server = require('http').Server(app);
const io = require('socket.io')(server);

const dgram = require('dgram');


const i2c = require('i2c-bus');
const ads1x15 = require('ads1x15');


var mpu6050 = require('mpu6050');



let motorSpeed = 150;
let motorIn1 = 20;
let motorIn2 = 21;
let motorIn3 = 19;
let motorIn4 = 26;

let motorEnA = 18;
let motorEnB = 13;


const limitXpositive = 400;
const limitXnegative = -150;

const limitYpositive = 300;
const limitYnegative = -300;

let gripperStatus = 1; //0 -> acik, 1 -> kapali
let gripperInProgress = 0; //0 -> not busy, 1 -> busy





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


let positionData = {
    X: 0.0,
    Y: 0.0,
    Z: 0.0,
    A: 0.0,
    B: 0.0,
    C: 0.0,
    L1: 0.0,
    L2: 0.0,
    rangeX: 0.0,
    rangeY: 0.0,
    rangeZ: 0.0,
    mouseX: 0,
    mouseY: 0,
    mouseZ: 0,
    adcX: 0,
    adcY: 0,
    adcZ: 0
};

let firstInput = 0;
///////////////////////////
// let dutyCycle = (255) * motorSpeed / 100 ;
// const enablePin1 = new Gpio(motorEnA,{mode: Gpio.OUTPUT});
// const In1 = new Gpio(motorIn1,{mode: Gpio.OUTPUT});
// const In2 = new Gpio(motorIn2,{mode: Gpio.OUTPUT});

// In1.digitalWrite(1);
// In2.digitalWrite(0);

// enablePin1.pwmWrite(parseInt(motorSpeed));


const motorServo = new Gpio(motorEnA, {mode: Gpio.OUTPUT});

const button = new Gpio(26, {
    mode: Gpio.INPUT,
    pullUpDown: Gpio.PUD_DOWN,
    edge: Gpio.EITHER_EDGE
});

const buttonGripper = new Gpio(17, {
    mode: Gpio.INPUT,
    pullUpDown: Gpio.PUD_DOWN,
    edge: Gpio.EITHER_EDGE
});


const motor = new Gpio(10, {mode: Gpio.OUTPUT});

let pulseWidth = 500;
let zincrement = 100;


let currentTime = Date.now();



buttonGripper.glitchFilter(50000);

buttonGripper.on('interrupt', (level) =>  {

    if (level) {

        if(gripperStatus == 1) {
            openGripper();
        }
        else if(gripperStatus == 0) {
            closeGripper();
        }
    }
});



//2500 -> kapatma, 500 -> acma
function openGripper() {

    if(gripperInProgress == 0) {
        console.log("openGripper");
        motor.servoWrite(500); 
        setTimeout(stopGripper, 4500);
        gripperInProgress = 1;
        gripperStatus = 0;
    }
    
}

function closeGripper() {

    if(gripperInProgress == 0) {
        console.log("closeGripper");
        motor.servoWrite(2500); 
        setTimeout(stopGripper, 4500);
        gripperInProgress = 1;
        gripperStatus = 1;
    }
}

function stopGripper() {
    console.log("stopGripper");
    motor.servoWrite(0);
    gripperInProgress = 0;
    
}


// setInterval(() => {
//   motor.servoWrite(pulseWidth);
//     console.log(pulseWidth);
//   pulseWidth += zincrement;
//   if (pulseWidth >= 2500) {
//     zincrement = -100;
//   } else if (pulseWidth <= 500) {
//     zincrement = 100;
//   }
// },300);


// button.on('interrupt', (level) =>  {
//     if(!level) {
//         // enablePin1.pwmWrite(parseInt(0));

//         const millis = Date.now() - currentTime;
//         console.log("Time elapsed: ", millis);
//         currentTime = Date.now();
//         motor.servoWrite(0);

//     }
//     else if (level) {
//         // enablePin1.pwmWrite(parseInt(motorSpeed));
        
//         const millis = Date.now() - currentTime;
//         console.log("Time elapsed: ", millis);
//         currentTime = Date.now();
//         motor.servoWrite(0);
//     }

// });





// const rl = readline.createInterface({
//         input: process.stdin,
//         output:process.stdout
// });

// rl.on('line', function(input){
//     if(input == 'q'){
//         enablePin1.pwmWrite(0);

//     }else if(input== '10'){
        
//         enablePin1.pwmWrite(parseInt(255/10));
//     }else if(input== '50'){
        
//         enablePin1.pwmWrite(parseInt(255/2));
//     }
// })

//////////////////////////
app.use(express.static(__dirname));

app.get('/gui.html', function (req, res) {
    res.sendFile(__dirname + '/public' + '/gui.html');
});

server.listen(3000, function () {
    console.log('listening on *:3000');
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

    socket.on('changePositionMouseDown-event', function (data) {
        switch(data.axis)
        {
            case 'x':
                if(data.direction == '+') {
                    positionData.mouseX = 1;console.log("X + GİRDİM");
                }
                    
                else {

                    positionData.mouseX = -1;console.log("X - GİRDİM");
                }
            break;

            case 'y':
                if(data.direction == '+')
                positionData.mouseY = 1;
                else
                positionData.mouseY = -1;
            break;

            case 'z':
                if(data.direction == '+')
                positionData.mouseZ = 1;
                else
                positionData.mouseZ = -1;
            break;

            default:
            break;
        }
    });

    socket.on('changePositionMouseUp-event', function (data) {
        switch(data.axis)
        {
            case 'x':
                positionData.mouseX = 0;console.log("X + ÇIKTIM");
            break;

            case 'y':
                positionData.mouseY = 0;
            break;

            case 'z':
                positionData.mouseZ = 0;
            break;

            default:
            break;
        }
        
    });


    socket.on('changePositionRange-event', function (data) {

        console.log("AXIS: ", data.axis, " RANGE: ", data.rangeValue);
        
        switch(data.axis)
        {
            case 'x':
                positionData.rangeX = data.rangeValue;
            break;

            case 'y':
                positionData.rangeY = data.rangeValue;
            break;

            case 'z':
                positionData.rangeZ = data.rangeValue;
            break;

            default:
            break;
        }
        
        console.log("RANGEZ: ", positionData.rangeX);
        
    });

});


//let fifoNode2C = fs.createWriteStream('/home/project/Desktop/CR751-driver-main/LinuxFiles/node2c.fifo');

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


let calcVoltage;


let sock = dgram.createSocket('udp4');
sock.send((robotControlSendBuffer), 10000, '192.168.0.20');


let counter = 0;

sock.on('message', function(msg, rinfo) {
    msg.copy(robotControlRecvBuffer);

    positionData.X = robotControlRecvFields.pos1recv1;
    positionData.Y = robotControlRecvFields.pos2recv1;
    positionData.Z = robotControlRecvFields.pos3recv1;
    positionData.A = robotControlRecvFields.pos4recv1;
    positionData.B = robotControlRecvFields.pos5recv1;
    positionData.C = robotControlRecvFields.pos6recv1;
    positionData.L1 = robotControlRecvFields.pos7recv1;
    positionData.L2 = robotControlRecvFields.pos8recv1;
    firstInput++;

    io.emit("printCoordinate", {coordinateX: positionData.X, coordinateY: positionData.Y, coordinateZ: positionData.Z, recvData: robotControlRecvBuffer.toString('hex')});
    sendPos();
    
});

let delta = 0.3;
let ratio = 1.0;

let increment = 0.0;

async function sendPos() {

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

    robotControlSendFields.pos3 = positionData.Z + positionData.rangeZ + positionData.mouseZ + positionData.adcZ;

    if((robotControlSendFields.pos3 > 570) || (robotControlSendFields.pos3 < 250)) {
        robotControlSendFields.pos3 = positionData.Z;
    }

    let limit = ((robotControlSendFields.pos3 - 250) / 3.2);

    // console.log("limit: ", limit);

    robotControlSendFields.pos1 = positionData.X + positionData.rangeX + positionData.mouseX + positionData.adcX;

    if(robotControlSendFields.pos1 < 0) {
        if((limitXnegative + limit) > robotControlSendFields.pos1) {
            robotControlSendFields.pos1 = positionData.X;
        }     
    }
    else if (robotControlSendFields.pos1 > 0) {
        if((limitXpositive - limit) < robotControlSendFields.pos1) {
            robotControlSendFields.pos1 = positionData.X;
        }     
    }
       
    
    robotControlSendFields.pos2 = positionData.Y + positionData.rangeY + positionData.mouseY + positionData.adcY;

    if(robotControlSendFields.pos2 < 0) {
        if((limitYnegative + limit) > robotControlSendFields.pos2) {
            robotControlSendFields.pos2 = positionData.Y;
        }     
    }
    else if (robotControlSendFields.pos2 > 0) {
        if((limitYpositive - limit) < robotControlSendFields.pos2) {
            robotControlSendFields.pos2 = positionData.Y;
        }     
    }

    if(((robotControlSendFields.pos1 > -150) && (robotControlSendFields.pos1 < 150))) {

        if(((robotControlSendFields.pos2 > -150) && (robotControlSendFields.pos2 < 150))) {

            robotControlSendFields.pos2 = positionData.Y;
        }

    }

    if(((robotControlSendFields.pos2 > -150) && (robotControlSendFields.pos2 < 150))) {

        if(((robotControlSendFields.pos1 > -150) && (robotControlSendFields.pos1 < 150))) {

            robotControlSendFields.pos1 = positionData.X;
        }

    }



    if((robotControlSendFields.pos1 > 325) && (robotControlSendFields.pos2 > 225)){

        if(robotControlSendFields.pos3 > 475) {

            robotControlSendFields.pos3 = positionData.Z;
        }
    }

    if((robotControlSendFields.pos1 > 325) && (robotControlSendFields.pos2 < -225)){
        
        if(robotControlSendFields.pos3 > 475) {

            robotControlSendFields.pos3 = positionData.Z;
        }
    }

    if((robotControlSendFields.pos1 < -75) && (robotControlSendFields.pos2 > 225)){
        
        if(robotControlSendFields.pos3 > 475) {

            robotControlSendFields.pos3 = positionData.Z;
        }
    }

    if((robotControlSendFields.pos1 < -75) && (robotControlSendFields.pos2 < -225)){
        
        if(robotControlSendFields.pos3 > 475) {

            robotControlSendFields.pos3 = positionData.Z;
        }
    }



    
    robotControlSendFields.pos4 = positionData.A;
    robotControlSendFields.pos5 = positionData.B;
    robotControlSendFields.pos6 = positionData.C;
    robotControlSendFields.pos7 = positionData.L1;
    robotControlSendFields.pos8 = positionData.L2;


    robotControlSendFields.respos4 = 0xffffffff;


    sock.send((robotControlSendBuffer), 10000, '192.168.0.20');


}



// setInterval(main, 1000);
// setInterval(sendingFunc, 500);

function main() {
    console.log("SENT");
    io.emit("printCoordinate", {coordinateX: 12.43});
}


// Instantiate and initialize.
var mpu = new mpu6050();
mpu.initialize();
 
// Test the connection before using.



const adcRead = async () => {

  const adc = new ads1x15();
  await adc.openBus(1);
  let middleFingerVoltage;
  let indexFingerVoltage;

  //0 -> middle finger
  //1 -> index finger

  for await (let channel of [0,1]) {

    const measure = await adc.readSingleEnded({channel});
    calcVoltage = (measure*0.02095) - 24.76;

        switch(channel) {
            case 0:
                middleFingerVoltage = calcVoltage;
            break;

            case 1:
                indexFingerVoltage = calcVoltage;
            break;

            default:
            break;
        }
        // console.log("VOLTAGE", channel,": ", calcVoltage);
    }

    // console.log("indexFingerVoltage: ", indexFingerVoltage);
    // console.log("middleFingerVoltage: ", middleFingerVoltage);

    if(indexFingerVoltage > 2) {

        positionData.adcZ = +1;
        
    }

    else if (middleFingerVoltage > 2) {
        positionData.adcZ = -1;

    }

    else {
        positionData.adcZ = 0;
    }



    //   if(calcVoltage > 2) {

    //     positionData.adcZ = -1;
        
    //   }

    //   else {
    //     positionData.adcZ = 0;
    //   }

  //1 -> asagi yukari
  // -10000<  demek asagi bakiyor demek
  //  10000>  demek yukari bakiyor demek


  //2 -> sag sol
  // -10000<  demek sag yatti demek
  //  10000>  demek sola yatti demek

  mpu.testConnection(function(err, testPassed) {
    if (testPassed) {
      mpu.getAcceleration(function(err, data){
        // console.log(data);
        data[0] = Math.ceil(data[0]/10)*10;
        data[1] = Math.ceil(data[1]/10)*10;
        data[2] = Math.ceil(data[2]/10)*10;
        
        
        if(data[2] < -10000) {
            // console.log("SAGA YATTI");
            positionData.adcY = -1;
        }
        else if(data[2] > 10000) {
            // console.log("SOLA YATTI");
            positionData.adcY = 1;
        }
        else {
            // console.log("DÜZ DURUYOR");
            positionData.adcY = 0;
        }

        if(data[1] < -10000) {
            // console.log("ASAGI BAKIYOR");
            positionData.adcX = 1;
        }
        else if(data[1] > 10000) {
            // console.log("YUKARI BAKIYOR");
            positionData.adcX = -1;
        }
        else {
            // console.log("DÜZ DURUYOR");
            positionData.adcX = 0;
        }
      });
      // Put the MPU6050 back to sleep.
      // mpu.setSleepEnabled(1);
    }
  });


}

setInterval(adcRead, 300);



 
