// import KioskBoard from 'kioskboard';
let KioskBoard = import("/kioskboard/dist/kioskboard-aio-2.2.0.min.js");
let selectedSlot = 1;

let batteryStatuses = [];

var socket = io();

const toastSuccess = new bootstrap.Toast(document.getElementById("toastSuccess"));
const toastFailed = new bootstrap.Toast(document.getElementById("toastFailed"));
let popUpCheck = {bleName: '', bleKey: '', aesKey: '', outputLevel: '', freq: 0, volume: 0, isConfSent: false, slot: 0};

class deviceStatus {
	constructor(isConnected, bleName, bleKey, outputLevel, aesKey, freq, volume, batteryLevel)
	{
		this.isConnected = isConnected;
		this.bleName = bleName;
		this.bleKey = bleKey;
		this.outputLevel = outputLevel;
		this.aesKey = aesKey;
		this.freq = freq;
		this.volume = volume;
		this.batteryLevel = batteryLevel;
	}
}

let cs = [];
let ctxs = [];
let devices = [];

/*********************************FUNCTIONS*************************************/


function init() {

    cs.push(document.getElementById("modalBatCanvas"));
    ctxs.push(cs[0].getContext("2d"));

	cs.push(document.getElementById("mainBatCanvas"));
    ctxs.push(cs[1].getContext("2d"));
	
	for(let i = 0; i < 17; i++)
	{
		devices.push(new deviceStatus());
	}
}

function updateVolumeRange(value){

	document.querySelector("[id='volumeLabel']").value = value;

}



function clearBattery(slot) {

	ctxs[slot].fillStyle = "#FFFFFF", ctxs[slot].fillRect(0, 0, 1900, 1000);
	ctxs[slot].fillStyle = "#000000", ctxs[slot].fillRect(1900, 350, 100, 300);
	ctxs[slot].lineWidth = 25;
	ctxs[slot].strokeRect(0, 0, 1900, 1000);

	document.getElementById("BatteryLevel" + slot).innerHTML = '- %';

}



function displayModalBattery(slotNum)
{

	let batteryLevel = devices[slotNum].batteryLevel;
	let selectedSlotBatteryLevel = devices[selectedSlot].batteryLevel;

	document.getElementById("modalBatteryLevel").innerHTML = selectedSlotBatteryLevel + '%';
	document.getElementById("slot" + slotNum + "progressbar").style.width = batteryLevel + "%";

	if(slotNum == selectedSlot)
	{
		if(batteryLevel <= 20) {

			ctxs[0].fillStyle = "#FFFFFF", ctxs[0].fillRect(500, 250, 1000, 500);
			ctxs[0].fillStyle = "#FF2E00", ctxs[0].fillRect(500, 250, batteryLevel*10, 500);
	
		}
		else if(batteryLevel > 20 && batteryLevel <= 50) {
			ctxs[0].fillStyle = "#FFFFFF", ctxs[0].fillRect(500, 250, 1000, 500);
			ctxs[0].fillStyle = "#FF7A00", ctxs[0].fillRect(500, 250, batteryLevel*10, 500);
		}
	
		else if(batteryLevel > 50) {
			ctxs[0].fillStyle = "#FFFFFF", ctxs[0].fillRect(500, 250, 1000, 500);
			ctxs[0].fillStyle = "#00CC00", ctxs[0].fillRect(500, 250, batteryLevel*10, 500);
		}
	
		ctxs[0].fillStyle = "#000000", ctxs[0].fillRect(1500, 400, 50, 200);
		ctxs[0].lineWidth = 25;
		ctxs[0].strokeRect(500, 250, 1000, 500);
	}
}

function displayMainBattery(mainBatteryLevel)
{

	document.getElementById("mainBatteryLevel").innerHTML = mainBatteryLevel + '%';

	
	if(mainBatteryLevel <= 20) {

		ctxs[1].fillStyle = "#FFFFFF", ctxs[1].fillRect(500, 250, 1000, 500);
		ctxs[1].fillStyle = "#FF2E00", ctxs[1].fillRect(500, 250, mainBatteryLevel*10, 500);

	}
	else if(mainBatteryLevel > 20 && mainBatteryLevel <= 50) {
		ctxs[1].fillStyle = "#FFFFFF", ctxs[1].fillRect(500, 250, 1000, 500);
		ctxs[1].fillStyle = "#FF7A00", ctxs[1].fillRect(500, 250, mainBatteryLevel*10, 500);
	}

	else if(mainBatteryLevel > 50) {
		ctxs[1].fillStyle = "#FFFFFF", ctxs[1].fillRect(500, 250, 1000, 500);
		ctxs[1].fillStyle = "#00CC00", ctxs[1].fillRect(500, 250, mainBatteryLevel*10, 500);
	}

	ctxs[1].fillStyle = "#000000", ctxs[1].fillRect(1500, 400, 50, 200);
	ctxs[1].lineWidth = 25;
	ctxs[1].strokeRect(500, 250, 1000, 500);
	
}


function sendToDevice() {

	console.log("configSubmit");

	let bleName = document.querySelector("[name='bleName']");
	let bleNameValue = bleName.value;
	let bleNameLength = bleNameValue.length;
	popUpCheck.bleName = bleNameValue;

	let bleKey = document.querySelector("[name='bleKey']");
	let bleKeyValue = bleKey.value;
	let bleKeyLength = bleKeyValue.length;
	popUpCheck.bleKey = bleKeyValue;

	let aesKey = document.querySelector("[name='aesKey']");
	let aesKeyValue = aesKey.value;
	popUpCheck.aesKey = aesKeyValue;

    let outputLevel = document.querySelector("[name='outputLevel']");
	let outputLevelValue = outputLevel.value;
	popUpCheck.outputLevel = outputLevelValue;

	let freq = document.querySelector("[name='freq']");
	let freqValue = freq.value;
	popUpCheck.freq = freqValue;

	let volume = document.querySelector("[name='rangeInput']");
	let volumeValue = volume.value;
	popUpCheck.volume = volumeValue;

	let freqTableValues = [];

	let freqTable1 = document.querySelector("[name='freqTable1']");
	freqTableValues[0] = freqTable1.value;

	let freqTable2 = document.querySelector("[name='freqTable2']");
	freqTableValues[1] = freqTable2.value;
	
	let freqTable3 = document.querySelector("[name='freqTable3']");
	freqTableValues[2] = freqTable3.value;
	
	let freqTable4 = document.querySelector("[name='freqTable4']");
	freqTableValues[3] = freqTable4.value;
	
	let freqTable5 = document.querySelector("[name='freqTable5']");
	freqTableValues[4] = freqTable5.value;
	
	let freqTable6 = document.querySelector("[name='freqTable6']");
	freqTableValues[5] = freqTable6.value;
	
	let freqTable7 = document.querySelector("[name='freqTable7']");
	freqTableValues[6] = freqTable7.value;
	
	let freqTable8 = document.querySelector("[name='freqTable8']");
	freqTableValues[7] = freqTable8.value;
	
	let freqTable9 = document.querySelector("[name='freqTable9']");
	freqTableValues[8] = freqTable9.value;

	let freqTable10 = document.querySelector("[name='freqTable10']");
	freqTableValues[9] = freqTable10.value;


	popUpCheck.slot = selectedSlot;
	
	console.log("----------SEND--------------");
	console.log("bleNameValue", bleNameValue);
	console.log("bleKeyValue", bleKeyValue);
	console.log("outputLevelValue", outputLevelValue);
	console.log("freqValue", freqValue);
	console.log("freqTableValues", freqTableValues);
	console.log("volume", volumeValue);
	console.log("----------SEND--------------");


	socket.emit("send-config_event", {bleName: bleNameValue, bleNameLength: bleNameLength, bleKey: bleKeyValue, bleKeyLength: bleKeyLength, aesKey: aesKeyValue,
	outputLevel: outputLevelValue, freq: freqValue, volume: volumeValue, slot: selectedSlot, freqTableValues: freqTableValues});


	// fetch("/sendConfig"+"?"+"bleName="+bleNameValue+"&bleKey="+bleKeyValue+"&aesKey="+aesKeyValue+"&outputLevel="+outputLevelValue+"&freq="+freqValue+"&volume="+volumeValue+"&slot="+selectedSlot)
	// .then(response => response.json())
	// .then(json => {
	// 	let jsonData = JSON.parse(json)
	// 	bleName.value = jsonData.bleName;
	// 	bleKey.value = jsonData.bleKey;
	// 	document.getElementById(jsonData.freq).selected = true;
	// 	document.getElementById(jsonData.outputLevel).selected = true;
	// 	document.getElementById('volumeLabel').innerHTML = jsonData.volume; 
	// 	document.getElementById('volumeRange').value = jsonData.volume;
	// 	console.log(jsonData.freq)
	// 	toastSuccess.show();
	// })
	// .catch(err => console.log('Failed', err));

}

/*****************************************************************************/

/*********************************EVENTS*************************************/

socket.on('usb-status_event', function(data) {
	console.log(data.slot)

	if(data.status == "Connected") {
		document.getElementById(data.slot).disabled = false;
        document.getElementById("slot" + data.slotNum + "progressbar").style.width = "0%";
        document.getElementById("slot" + data.slotNum + "progress").removeAttribute("hidden");
	}

	else if(data.status == "Disconnected") {
		document.getElementById(data.slot).disabled = true;
        document.getElementById("slot" + data.slotNum + "progressbar").style.width = "0%";
        document.getElementById("slot" + data.slotNum + "progress").setAttribute("hidden", "hidden");
	}
});



socket.on("periodic-change_event", function(data) {

	// Canta batarya kontrolu
	displayMainBattery(data.batteryPerc);
	document.getElementById("batteryVoltage").innerHTML = data.batteryLevel + "V";
	document.getElementById("cpuTemp").innerHTML = data.cpuTemp + "°C";
	document.getElementById("connectedDeviceNumber").innerHTML = data.connectedDeviceNumber;
	console.log(data);

});


socket.on("status-update_event", function(data) {

	for (let i = 0; i < 17; i++) {

		devices[i] = structuredClone(data.status[i]);
	}

	console.log("---------STATUS UPDATE------------");
	console.log(devices);
	console.log("----------------------------------");
	
	const bleName = slotModal.querySelector('#bleName');
	const bleKey = slotModal.querySelector('#bleKey');
	
	if (data.slot == selectedSlot){

		if(data.messageType == "queryAnswer"){

			if(devices[data.slot].isSendSuccess){
				toastSuccess.show();
				popUpCheck.isConfSent = false;
	
				bleName.value = devices[data.slot].bleName;
				bleKey.value = devices[data.slot].bleKey;
				document.getElementById(devices[data.slot].freq).selected = true;
				document.getElementById(devices[data.slot].outputLevel).selected = true;
				document.getElementById('volumeLabel').innerHTML = devices[data.slot].volume; 
				document.getElementById('volumeRange').value = devices[data.slot].volume;

				document.querySelector("[name='freqTable1']").value = devices[data.slot].freqTable[0];
				document.querySelector("[name='freqTable2']").value = devices[data.slot].freqTable[1];
				document.querySelector("[name='freqTable3']").value = devices[data.slot].freqTable[2];
				document.querySelector("[name='freqTable4']").value = devices[data.slot].freqTable[3];
				document.querySelector("[name='freqTable5']").value = devices[data.slot].freqTable[4];
				document.querySelector("[name='freqTable6']").value = devices[data.slot].freqTable[5];
				document.querySelector("[name='freqTable7']").value = devices[data.slot].freqTable[6];
				document.querySelector("[name='freqTable8']").value = devices[data.slot].freqTable[7];
				document.querySelector("[name='freqTable9']").value = devices[data.slot].freqTable[8];
				document.querySelector("[name='freqTable10']").value = devices[data.slot].freqTable[9];
				
			}else if((devices[data.slot].isSendSuccess == 0) && (devices[data.slot].isConfSent == 2)){	
				toastFailed.show();
				popUpCheck.isConfSent = false;
			}

		}

		if(devices[data.slot].isFirstMsg){

			console.log("-------FIRST MESSAGE-------");

			bleName.value = devices[data.slot].bleName;
			bleKey.value = devices[data.slot].bleKey;
			document.getElementById(devices[data.slot].freq).selected = true;
			document.getElementById(devices[data.slot].outputLevel).selected = true;
			document.getElementById('volumeLabel').innerHTML = devices[data.slot].volume; 
			document.getElementById('volumeRange').value = devices[data.slot].volume;

			document.querySelector("[name='freqTable1']").value = devices[data.slot].freqTable[0];
			document.querySelector("[name='freqTable2']").value = devices[data.slot].freqTable[1];
			document.querySelector("[name='freqTable3']").value = devices[data.slot].freqTable[2];
			document.querySelector("[name='freqTable4']").value = devices[data.slot].freqTable[3];
			document.querySelector("[name='freqTable5']").value = devices[data.slot].freqTable[4];
			document.querySelector("[name='freqTable6']").value = devices[data.slot].freqTable[5];
			document.querySelector("[name='freqTable7']").value = devices[data.slot].freqTable[6];
			document.querySelector("[name='freqTable8']").value = devices[data.slot].freqTable[7];
			document.querySelector("[name='freqTable9']").value = devices[data.slot].freqTable[8];
			document.querySelector("[name='freqTable10']").value = devices[data.slot].freqTable[9];

			devices[data.slot].isFirstMsg = false;
		}
	
		
	// 	}else {
	// 		console.log("debug3");
	// 		if (devices[data.slot].isConfSent == 2){
	
	// 			toastFailed.show();
	// 			popUpCheck.isConfSent = false;
	// 			console.log("TOASTFAILED");
	// 		}
	
	// 	}
	}

	displayModalBattery(data.slot);

	// if ((popUpCheck.isConfSent == true) && (popUpCheck.slot == devices[data.slot].slotNum)){
	// 	if ((popUpCheck.bleKey == devices[data.slot].bleKey) &&
	// 	(popUpCheck.bleName == devices[data.slot].bleName) &&
	// 	(popUpCheck.freq == devices[data.slot].freq) &&
	// 	(popUpCheck.outputLevel == devices[data.slot].outputLevel) &&
	// 	(popUpCheck.volume == devices[data.slot].volume)) {

	// 		toastSuccess.show();
	// 		popUpCheck.isConfSent = false;
		
	// 	} else {
	// 		toastFailed.show();
	// 		popUpCheck.isConfSent = false;
	// 		console.log("TOASTFAILED");
	// 	}

	// 	popUpCheck.isConfSent = false;
	// 	popUpCheck.aesKey = '';
	// 	popUpCheck.bleKey = '';
	// 	popUpCheck.bleName = '';
	// 	popUpCheck.freq = 0;
	// 	popUpCheck.outputLevel = 0;
	// 	popUpCheck.volume = 0;
	// 	popUpCheck.slot = 0;
	// }
	
});


setInterval(timeElapsed, 1000);

async function timeElapsed() {

	let currentMSeconds = new Date(performance.now());
	let differenceMSeconds = (currentMSeconds) / 1000;

	hhmmssFormat = currentMSeconds.toISOString().substring(11, 19);
    document.getElementById("timeElapsed").innerHTML = hhmmssFormat;
}


const slotModal = document.getElementById('slotModal');
slotModal.addEventListener('show.bs.modal', event => {

	const button = event.relatedTarget
	const recipient = button.getAttribute('data-bs-slotNo')

	const modalTitle = slotModal.querySelector('.modal-title');
	const modalBodyInput = slotModal.querySelector('.modal-body');
	const bleName = slotModal.querySelector('#bleName');
	const bleKey = slotModal.querySelector('#bleKey');
	const freq = slotModal.querySelector('#freq');
	const freqTable1 = slotModal.querySelector("[name='freqTable1']");
	const freqTable2 = slotModal.querySelector("[name='freqTable2']");
	const freqTable3 = slotModal.querySelector("[name='freqTable3']");
	const freqTable4 = slotModal.querySelector("[name='freqTable4']");
	const freqTable5 = slotModal.querySelector("[name='freqTable5']");
	const freqTable6 = slotModal.querySelector("[name='freqTable6']");
	const freqTable7 = slotModal.querySelector("[name='freqTable7']");
	const freqTable8 = slotModal.querySelector("[name='freqTable8']");
	const freqTable9 = slotModal.querySelector("[name='freqTable9']");
	const freqTable10 = slotModal.querySelector("[name='freqTable10']");
	modalTitle.textContent = `${recipient} Telsiz Konfigürasyonu`

	let slotNum = recipient.slice(5);
	selectedSlot = slotNum;

	displayModalBattery(selectedSlot);

  	fetch("/usbstates?slotNum=" + slotNum)
	.then(response => response.json())
	.then(json => {
		let jsonData = JSON.parse(json)
		
		devices[slotNum].bleName = jsonData.bleName;
		devices[slotNum].bleKey = jsonData.bleKey;
		devices[slotNum].outputLevel = jsonData.outputLevel;
		devices[slotNum].freq = jsonData.freq;
		devices[slotNum].volume = jsonData.volume;
		devices[slotNum].batteryLevel = jsonData.batteryLevel;
		devices[slotNum].isConnected = true;
		devices[slotNum].freqTable[0] = jsonData.freqTable[0];
		devices[slotNum].freqTable[1] = jsonData.freqTable[1];
		devices[slotNum].freqTable[2] = jsonData.freqTable[2];
		devices[slotNum].freqTable[3] = jsonData.freqTable[3];
		devices[slotNum].freqTable[4] = jsonData.freqTable[4];
		devices[slotNum].freqTable[5] = jsonData.freqTable[5];
		devices[slotNum].freqTable[6] = jsonData.freqTable[6];
		devices[slotNum].freqTable[7] = jsonData.freqTable[7];
		devices[slotNum].freqTable[8] = jsonData.freqTable[8];
		devices[slotNum].freqTable[9] = jsonData.freqTable[9];
		
	})
	.catch(err => console.log('Failed', err));

//TODO: ACMAYI UNUTMA

	bleName.value = devices[slotNum].bleName;
	bleKey.value = devices[slotNum].bleKey;
	freqTable1.value = devices[slotNum].freqTable[0];
	freqTable2.value = devices[slotNum].freqTable[1];
	freqTable3.value = devices[slotNum].freqTable[2];
	freqTable4.value = devices[slotNum].freqTable[3];
	freqTable5.value = devices[slotNum].freqTable[4];
	freqTable6.value = devices[slotNum].freqTable[5];
	freqTable7.value = devices[slotNum].freqTable[6];
	freqTable8.value = devices[slotNum].freqTable[7];
	freqTable9.value = devices[slotNum].freqTable[8];
	freqTable10.value = devices[slotNum].freqTable[9];
	document.getElementById(devices[slotNum].freq).selected = true;
	document.getElementById(devices[slotNum].outputLevel).selected = true;
	document.getElementById('volumeLabel').innerHTML = devices[slotNum].volume; 
	document.getElementById('volumeRange').value = devices[slotNum].volume;

})

/*****************************************************************************/