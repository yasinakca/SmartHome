function mqttConnect() {
    console.log("connecting to mqtt host");
    //mqttConnection Files
    client = new Paho.MQTT.Client(host, port, "web_");
    var reconnectTimeout = 2000;
    var connected_flag = 0;

    // set callback handlers

    var options = {
        useSSL: true,
        userName: "username",
        password: "psw",
        timeout: 3,
        onSuccess: onConnect,
        onFailure: doFail
    }
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;


    // connect the client
    client.connect(options);
    return false;
}

// called when the client connects
function onConnect() {
    connected_flag = 1;
    document.getElementById("status").innerHTML = "Connected to Mqtt Server";
    // Once a connection has been made, make a subscription and send a message.
    console.log("onConnect" + connected_flag);
    client.subscribe("relay", 0);
    client.subscribe("nem", 1);
    client.subscribe("sicaklik", 2);
    client.subscribe("ALERT", 3);
    message = new Paho.MQTT.Message("Welcome to our Smart Home");
    message.destinationName = "relay";
    client.send(message);
}

function doFail(message) {
    console.log("Failed", message);
    document.getElementById("messages").innerHTML = "Connection Failed- Retrying";
    setTimeout(mqttConnect, reconnectTimeout);
}

// called when the client loses its connection
function onConnectionLost() {
    document.getElementById("status").innerHTML = "Connection Lost";
    document.getElementById("messages").innerHTML = "Connection Lost";
    console.log("onConnectionLost:" + responseObject.errorMessage);
    connected_flag = 0;
}

function onConnected(recon, url) {
    console.log("in connected" + recon);
    document.getElementById("status").innerHTML = "Connected";
}

// called when a message arrives
function onMessageArrived(msg) {
    var value = msg.payloadString;
    var dataStream = msg.destinationName;
    console.log("Topic :" + dataStream + ", message :" + value);
    if (dataStream == "ALERT") {
        setTimeout();
        document.getElementById("Alert").innerHTML = "PLEASE GO YOUR HOME.ITS ERGANT!!"
    }
    if (dataStream == "nem") {
        document.getElementById("nem").innerHTML = value;
    }
    if (dataStream == "sicaklik") {
        document.getElementById("sicaklik").innerHTML = value;
    }
}

function lightOn() {
    isikAc = new Paho.MQTT.Message("d0_on");
    isikAc.destinationName = "relay";
    client.send(isikAc);
    document.getElementById("messages").innerHTML = "LIGHT ON";
}
function lightOff() {
    isikKapat = new Paho.MQTT.Message("d0_off");
    isikKapat.destinationName = "relay";
    client.send(isikKapat);
    document.getElementById("messages").innerHTML = "LIGHT OFF";
}
function perdeAc() {
    servoAc = new Paho.MQTT.Message("perde_ac");
    servoAc.destinationName = "relay";
    client.send(servoAc);
    document.getElementById("messages").innerHTML = "CERTAIN ON";
}

function perdeKapat() {
    servoKapat = new Paho.MQTT.Message("perde_kapat");
    servoKapat.destinationName = "relay";
    client.send(servoKapat);
    document.getElementById("messages").innerHTML = "CERTAIN OFF";
}
function gazOku() {
    gaz = new Paho.MQTT.Message("gazOku");
    gaz.destinationName = "relay";
    client.send(gaz);
}
function sicaklikOku() {
    sicaklik = new Paho.MQTT.Message("sicaklikOku");
    sicaklik.destinationName = "relay";
    client.send(sicaklik);
    document.getElementById("sicaklik").innerHTML
}
function clearBox(elementID) {
    document.getElementById(elementID).innerHTML = "";
}
mqttConnect();