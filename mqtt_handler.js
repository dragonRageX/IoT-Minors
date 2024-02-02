const mqtt = require("mqtt");
const dotenv = require("dotenv").config();

class MqttHandler
{
    constructor()
    {
        this.mqttClient = "null";
        this.host = `${process.env.YOUR_HOST}`;   // used https://testclient-cloud.mqtt.cool/ here, for testing purposes. It has an advantage of setting up a MQTT Broker (along with this, it also sets up a new client so that we can play around with it) without the need to signup to services like https://console.hivemq.cloud/?utm_source=hivemq-com&utm_medium=getting-started-post&utm_campaign=cloud , which provide a similar service. Check https://mqtt.org/software/ for more info
        this.username = "YOUR_USER";   // mqtt credentials, if these are needed to connect
        this.password = "YOUR_PASSWORD";
    }

    connect()
    {
        // Connect to mqtt with credentials (in case if needed, otherwise we can omit 2nd param)
        this.mqttClient = mqtt.connect(this.host/*, { username: this.username, password: this.password }*/);

        // Mqtt error calback
        this.mqttClient.on("error", (err) => {
            console.log(err);
            this.mqttClient.end();
        });

        // Connection callback
        this.mqttClient.on("connect", () => {
            console.log(`MQTT Client Connected!`);
        });

        // mqtt subscriptions
        this.mqttClient.subscribe("mytopic", { qos: 0 });   //Quality of Service set to - level 0

        // when a message arrives, log it
        this.mqttClient.on("message", (topic, message) => {
            console.log(message.toString());
        });

        //close connection - client.end() in mqtt.js file
        this.mqttClient.on("close", () => {
            console.log("MQTT Client Disconnected!");
        })
    }

    // Sends a mqtt message to topic: mytopic
    sendMessage(message)
    {
        this.mqttClient.publish("mytopic", message);
    }
}

module.exports = MqttHandler;