const random = (Math.random() + 1).toString(36).substring(7);
const clientId = `izhal-${random}`;
const topic = "esp32/gooner";

const client = mqtt.connect("ws://broker.emqx.io:8083/mqtt", {
  keepalive: 60,
  clientId: clientId,
  protocolId: "MQTT",
  protocolVersion: 4,
  clean: true,
  reconnectPeriod: 1000,
  connectTimeout: 30 * 1000,
});
