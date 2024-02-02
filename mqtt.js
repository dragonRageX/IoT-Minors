const mqtt = require("mqtt");

let client = mqtt.connect('mqtt://test.mosquitto.org');

client.on("connect", () => {
    client.subscribe("Topic07");
    console.log("Client has subscribed successfully!");
});

client.on("connect", () => {
    setInterval(() => {
        client.publish("Topic07", "Cricket");
    }, 3000);
});

client.on("message", (topic, message) => {
    console.log(message.toString());
    client.end();
});