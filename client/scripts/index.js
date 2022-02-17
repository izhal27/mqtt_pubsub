const colors = ["red", "yellow", "green"];
let statusObj = { red: 0, yellow: 0, green: 0 };

insertElements(colors);

// select all image container
const ledImageArray = getElements("img[id*='led-']");
// select all button on and off
const buttons = getElements("a[id*='button-']");

// event handler for incoming message
client.on("message", (topic, message, packet) => {
  console.log(
    "Received Message: " + message.toString() + "\nOn topic: " + topic
  );

  try {
    const msgStr = message.toString();
    statusObj = { ...JSON.parse(msgStr) };
    Object.keys(statusObj).forEach((color) => {
      setLedAndButton(color);
    });
  } catch (error) {
    console.log(error);
  }
});

client.on("error", (err) => {
  console.log("Connection error: ", err);
  client.end();
});

client.on("reconnect", () => {
  console.log("Reconnecting");
});

client.on("connect", () => {
  document.querySelector("#connection-status").innerHTML = "Open";

  client.subscribe(topic, { qos: 0 });
});
