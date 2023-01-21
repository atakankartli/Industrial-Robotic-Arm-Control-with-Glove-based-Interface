const json = require('body-parser/lib/types/json');
const express = require('express');
const http = require('http');
const Struct = require('struct');
const usbDetect = require('usb-detection');
const drivelist = require('drivelist');
const { SerialPort } = require('serialport');
const { InterByteTimeoutParser } = require('@serialport/parser-inter-byte-timeout');
const Usb = require('usb');
const crc16 = require('crc/crc16ccitt');
const { Server } = require("socket.io");
const ads1x15 = require('ads1x15');
const adc = new ads1x15();
const si = require('systeminformation');

setInterval(adcMeasure, 1000);
adc.openBus(1);

var app = express();
const server = http.createServer(app);  // http server
const io = new Server(server);          // socket.io server

server.listen(3000, () => {
    let host = server.address().address
    let port = server.address().port

    usbDetect.startMonitoring();  
    console.log("Example app listening at http://%s:%s", host, port)
})

app.use(express.static(__dirname + '/public'));

app.get('/mainPage.html', function (req, res) {
   res.sendFile( __dirname + "/public/" + "mainPage.html" );
})

app.get('/index.html', function (req, res) {
    res.sendFile( __dirname + "/public/" + "indexPage.html" );
})

let serials = [];
let parsers = [];
let UsbStates = [];
let connectedDeviceNumber = 0;

class UsbState {
    constructor() {
        this.slotNum = "";
        this.freq = "";
        this.volume = "";
        this.outputLevel = "";
        this.batteryLevel = "";
        this.bleName = "";
        this.bleKey = "";
        this.freqTable = [];
        this.CRC = 0;
        this.expectedCRC = 0;
        this.isDataChanged = false;
        this.isSendSuccess = 0;
        this.isConfSent = 0;
        this.isFirstMsg = false;
    }
}

let dummyPath = "/dev/ttyUSB";

for (let v = 0; v < 17; v++) {
    UsbStates.push(new UsbState());
    serials.push(new SerialPort({path: dummyPath + v, baudRate: 920600, autoOpen: false,}))
}

var usbPeriodicStruct = Struct()
    .word8Ule('SOD')
    .word8Ule('sourceAddress')
    .word8Ule('destAddress')
    .word8Ule('dataType')
    .word8Ule('commandNo')
    .word8Ule('payloadType')
    .word8Ule('payloadLength')
    .word8Ule('reserved1')
    .word16Ule('freq')
    .word8Ule('volume')
    .word8Ule('outputLevel')
    .word8Ule('aesStatus')
    .word8Ule('bleStatus')
    .chars('aesKey', 32)
    .word8Ule('timeSync')
    .word8Ule('ptt')
    .word8Ule('batteryLevel')
    .word8Ule('bleNameLength')
    .chars('bleName',24)
    .word8Ule('bleKeyLength')
    .chars('bleKey',24)
    .array('freqTable',10,'word16Ube')
    .word8Ule('isSendSuccess')
    .array('reserved2',4,'word8Ule')
    .word16Ule('CRC')
    .word8Ule('reserved3')
    .word8Ule('EOD');


app.get('/usbstates', async (req, res) => { 
    let jsonString = JSON.stringify(UsbStates[req.query.slotNum]);
    console.log(jsonString)
    res.json(jsonString);
    console.log(req.query.slotNum);
    console.log("AAAAAAAAAAAAAAAAAAAAA");
    
})



io.on('connection', clientListener);

function clientListener(socket) {
    socket.on('send-config_event', (clientConfig) => {

        usbPeriodicStruct.allocate();
        let configBuf = usbPeriodicStruct.buffer();
        const configBufFields = usbPeriodicStruct.fields;

        let configBufferTemp = new Array(128);

        configBufFields.bleName = clientConfig.bleName;
        configBufFields.bleNameLength = clientConfig.bleNameLength;
        configBufFields.bleKey = clientConfig.bleKey;
        configBufFields.bleKeyLength = clientConfig.bleKeyLength;
        configBufFields.aesKey = clientConfig.aesKey;
        configBufFields.freq = clientConfig.freq;
        configBufFields.outputLevel = clientConfig.outputLevel;
        configBufFields.volume = clientConfig.volume;

        for(let i=0; i<10; i++){
            configBufFields.freqTable[i] = clientConfig.freqTableValues[i];
        } 

        configBufFields.SOD = 0xAA;
        configBufFields.sourceAddress = 0x80;
        configBufFields.destAddress = 0x02;
        configBufFields.dataType = 0x01;
        configBufFields.commandNo = 0x0C;
        configBufFields.payloadLength = 0x72;
        
        for (let z = 0; z < 128; z++) {
            configBufferTemp[z] = configBuf[z];
        }
        
        crcCalculatorArray = configBufferTemp.slice(0, 124);
        calculatedCRC = crc16(crcCalculatorArray);
        
        UsbStates[clientConfig.slot].expectedCRC = calculatedCRC;
        

        configBufFields.CRC = calculatedCRC;
        configBufFields.EOD = 0xFF;
        
        slotNum = Number(clientConfig.slot);
        let path = slotToPathMap[slotNum];
        let pathNum = Number(path.slice(11));
        UsbStates[clientConfig.slot].isConfSent = 1;

        serials[pathNum].write(configBuf);
        
        console.log("serials[pathNum]:  ",serials[pathNum]);
        console.log("SENDDDDDDDDDD");
        console.log(configBuf);
        console.log("------------");
        
    });
}



// app.get('/sendConfig', async (req, res) => { 
//     usbPeriodicStruct.allocate();
//     let configBuf = usbPeriodicStruct.buffer();
//     const configBufFields = usbPeriodicStruct.fields;

//     let configBufferTemp = new Array(128);

//     configBufFields.bleName = req.query.bleName;
//     configBufFields.bleKey = req.query.bleKey;
//     configBufFields.aesKey = req.query.aesKey;
//     configBufFields.freq = req.query.freq;
//     configBufFields.outputLevel = req.query.outputLevel;
//     configBufFields.volume = req.query.volume;

//     configBufFields.SOD = 0xAA;
//     configBufFields.sourceAddress = 0x80;
//     configBufFields.destAddress = 0x02;
//     configBufFields.dataType = 0x01;
//     configBufFields.commandNo = 0xF0;
//     configBufFields.payloadLength = 0x72;

//     for (let z = 0; z < 128; z++) {
//         configBufferTemp[z] = configBuf[z];
//     }

//     crcCalculatorArray = configBufferTemp.slice(0, 124);
//     calculatedCRC = crc16(crcCalculatorArray);

    
//     configBufFields.CRC = calculatedCRC;
//     configBufFields.EOD = 0xFF;
    
//     slotNum = Number(req.query.slot);
//     let path = slotToPathMap[slotNum];
//     let pathNum = Number(path.slice(11));


//     serials[pathNum].write(configBuf);

//     console.log(configBuf);

//     res.json({}); 
// })


usbDetect.on('remove', async function(device) { 

    console.log(device);
    portList = await SerialPort.list();

    let foundPath = "";
    for (const path in pathToSlotMap) {
        slot = pathToSlotMap[path]

        const found = portList.find((p) => {
            return p.path == path
        })

        if (!found) {
            foundPath = path
            break
        }
    }
    

    console.log(foundPath)
    console.log(pathToSlotMap);

    if (io) {
        io.emit("usb-status_event", {slotNum:pathToSlotMap[foundPath], slot: "slot" + pathToSlotMap[foundPath], path: foundPath, status: "Disconnected"})
    }

    connectedDeviceNumber--;
    if (connectedDeviceNumber < 0){
        connectedDeviceNumber = 0;
    }

    delete slotToPathMap[pathToSlotMap[foundPath]];
    delete pathToSlotMap[foundPath]; 

    let pathNum = Number(foundPath.slice(11));

    serials[pathNum].unpipe();
    parsers[pathNum] = null;
    
    console.log(pathToSlotMap);
});    

let pathToSlotMap = {}
let slotToPathMap = {}
const portToSlotMap = {
    213: 1,
    212: 2,
    2113: 3,
    2112: 4,
    2111: 5,
    2114: 6,
    2143: 7,
    2142: 8,
    223: 9,
    222: 10,
    2213: 11,
    2212: 12,
    2211: 13,
    2214: 14,
    2243: 15,
    2242: 16,
}

usbDetect.on('add', async function(device) { 
    const deviceUSB = Usb.findByIds(0x0403, 0x6001);

    if(deviceUSB) {
  
        console.log('---------- device ---------- ')
        console.log(device)

        console.log('---------- deviceUSB.portNumbers ----------')
        console.log(deviceUSB.portNumbers)
        let portNumbers = ''
        deviceUSB.portNumbers.forEach(e => {
            portNumbers += e
        })
        
        console.log(portNumbers)
        const slotNumber = portToSlotMap[portNumbers]
        
        portList = await SerialPort.list();
        let path = ""
        portList.forEach(e => {
            if (e.manufacturer == "FTDI" && !pathToSlotMap[e.path]) {
                pathToSlotMap[e.path] = slotNumber;
                slotToPathMap[slotNumber] = e.path;
                path = e.path;

                let pathNum = Number(path.slice(11));

                console.log(pathNum);

                parsers[pathNum] = serials[pathNum].pipe(new InterByteTimeoutParser({ 
                    maxBufferSize: 256,
                    interval: 100
                }))

                serials[pathNum].open();

                parsers[pathNum].on('data', NewDataParser(slotNumber, path));

                
                io.emit("usb-status_event", {slot: "slot" + slotNumber, path: path, status: "Connected", slotNum: slotNumber});
                connectedDeviceNumber++;
                if (connectedDeviceNumber > 16){
                    connectedDeviceNumber = 16;
                }

                console.log("DEBUG PURPOSES: ");
                console.log("slotNumber: ", slotNumber);


                UsbStates[slotNumber].isFirstMsg = true;
            }
        })
    }
});


async function adcMeasure(arg) {
    let calcVoltageLin, calcVoltageQuad, calcVoltagePerc, cpuTemp;
    const measure = await adc.readSingleEnded(0);
    calcVoltageLin = (measure*0.02095) - 24.76;
    calcVoltageQuad = (Math.pow(measure, 2)*(-6.91*1e-8)) + (measure*(0.0212) - 24.97);
    
    if((calcVoltageQuad > 19.64) && (calcVoltageQuad < 25.2))
        calcVoltagePerc = (calcVoltageQuad - 19.64) / 0.0556;

    else if(calcVoltageQuad < 19.64)
        calcVoltagePerc = 0;

    else if (calcVoltageQuad > 25.2)
        calcVoltagePerc = 100;

        
    cpuTemp = await si.cpuTemperature();
    
    io.emit("periodic-change_event", {batteryLevel: Math.round(calcVoltageQuad * 100) / 100, batteryPerc: Math.round(calcVoltagePerc), cpuTemp: cpuTemp.main, connectedDeviceNumber: connectedDeviceNumber})
}


function NewDataParser(slot, path) {
    return function (data) {
        if ((data.length == 128) && (data[0] === 0xAA) && (data[127] === 0xFF)) {
            let crcCalculatorArray;
            let calculatedCRC;
            let isDataChanged;
            
            crcCalculatorArray = data.slice(0, 124);
            calculatedCRC = crc16(crcCalculatorArray);
            
            usbPeriodicStruct.allocate();
            let receivedBuf = usbPeriodicStruct.buffer();
            const receivedBufFields = usbPeriodicStruct.fields;
            
            for (let z = 0; z < 128; z++) {
                receivedBuf[z] = data[z];
            }
            
            if (calculatedCRC == receivedBufFields.CRC) {
                UsbStates[slot].slotNum = slot;
                UsbStates[slot].freq = "freq" + receivedBufFields.freq;
                UsbStates[slot].volume = receivedBufFields.volume;
                UsbStates[slot].outputLevel = "outputLevel" + receivedBufFields.outputLevel;
                UsbStates[slot].bleName = receivedBufFields.bleName;
                UsbStates[slot].bleKey = receivedBufFields.bleKey;
                UsbStates[slot].freqTable[0] = receivedBufFields.freqTable[0];
                UsbStates[slot].freqTable[1] = receivedBufFields.freqTable[1];
                UsbStates[slot].freqTable[2] = receivedBufFields.freqTable[2];
                UsbStates[slot].freqTable[3] = receivedBufFields.freqTable[3];
                UsbStates[slot].freqTable[4] = receivedBufFields.freqTable[4];
                UsbStates[slot].freqTable[5] = receivedBufFields.freqTable[5];
                UsbStates[slot].freqTable[6] = receivedBufFields.freqTable[6];
                UsbStates[slot].freqTable[7] = receivedBufFields.freqTable[7];
                UsbStates[slot].freqTable[8] = receivedBufFields.freqTable[8];
                UsbStates[slot].freqTable[9] = receivedBufFields.freqTable[9];     
                UsbStates[slot].batteryLevel = receivedBufFields.batteryLevel;
                UsbStates[slot].isSendSuccess = receivedBufFields.isSendSuccess;

                console.log("ISSENDSUCCESS: ", UsbStates[slot].isSendSuccess);
                
                if (calculatedCRC != UsbStates[slot].CRC) {
                    UsbStates[slot].isDataChanged = true;
                }else {
                    UsbStates[slot].isDataChanged = false;
                }
                
                // if (calculatedCRC == UsbStates[slot].expectedCRC) {
                //     UsbStates[slot].isSendSuccess = true;
                // }else {
                //     UsbStates[slot].isSendSuccess = false;
                // }
                
                UsbStates[slot].CRC = receivedBufFields.CRC;
                
                console.log(data, data.length, slot, path);
                console.log("isConfSent: ", UsbStates[slot].isConfSent);
                console.log("isSendSuccess: ", UsbStates[slot].isSendSuccess);
                console.log("isSendSuccessBuf: ", receivedBuf[121]);
                console.log("isDataChanged: ", UsbStates[slot].isDataChanged);
                console.log("CRC: ", UsbStates[slot].CRC);
                console.log("expectedCRC: ", UsbStates[slot].expectedCRC);
                console.log("isFirstMsg: ", UsbStates[slot].isFirstMsg);
                
                console.log("freq: ", UsbStates[slot].freq);
                console.log("volume: ", UsbStates[slot].volume);
                console.log("bleName: ", UsbStates[slot].bleName);
                console.log("bleKey: ", UsbStates[slot].bleKey);
                console.log("freqTable1", UsbStates[slot].freqTable[0]);
                console.log("freqTable2", UsbStates[slot].freqTable[1]);

                let messageType;

                if(receivedBufFields.commandNo == 0x08){
                    messageType = "queryAnswer";
                }else if(receivedBufFields.commandNo == 0xF0){
                    messageType = "periodic";
                }


                io.emit("status-update_event", {status: UsbStates, slot: slot, messageType: messageType});
                UsbStates[slot].isFirstMsg = false;
                
                if (UsbStates[slot].isConfSent == 1){
                    UsbStates[slot].isConfSent = 2;
                }else if (UsbStates[slot].isConfSent == 2){
                    UsbStates[slot].isConfSent = 0;
                }
            
            }
        }
    }
}
