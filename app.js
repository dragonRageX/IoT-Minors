let express = require("express");
let bodyParser = require("body-parser");
let app = express();
const dotenv = require("dotenv").config();

let mqttHandler = require("./mqtt_handler");

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

let mqttClient = new mqttHandler();
mqttClient.connect();

// Routes
app.post("/send-mqtt", (req, res) => {
    mqttClient.sendMessage(req.body.message);
    res.status(200).send("Message sent to mqtt");
});

app.listen(process.env.PORT, () => console.log(`Server is running on port ${process.env.PORT}`));