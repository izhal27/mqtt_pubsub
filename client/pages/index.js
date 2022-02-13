import { useEffect, useState } from "react";
import mqtt from "mqtt";

import Head from "next/head";
import styles from "../styles/Home.module.css";
export default function Home() {
  const [mqttClient, setMqttClient] = useState(null);
  const [connectionStatus, setConnectionStatus] = useState(false);
  const [ledMode, setLedMode] = useState(null);

  useEffect(() => {
    const clientId = "mesut-ozil";

    const host = "ws://broker.emqx.io:8083/mqtt";

    const options = {
      keepalive: 60,
      clientId: clientId,
      protocolId: "MQTT",
      protocolVersion: 4,
      clean: true,
      reconnectPeriod: 1000,
      connectTimeout: 30 * 1000,
    };
    console.log("Connecting mqtt client");
    const client = mqtt.connect(host, options);
    console.log(client);

    if (client) {
      client.on("message", (topic, message, packet) => {
        console.log(
          "Received Message: " + message.toString() + "\nOn topic: " + topic
        );
        setLedMode(message.toString());
      });

      client.on("error", (err) => {
        console.log("Connection error: ", err);
        client.end();
      });

      client.on("reconnect", () => {
        console.log("Reconnecting...");
      });

      client.on("connect", () => {
        console.log("Client connected:" + clientId);
        // Subscribe
        client.subscribe("esp32/gooner", { qos: 0 });
        setConnectionStatus(true);
      });

      setMqttClient(client);
    }
  }, []);

  return (
    <div className={styles.container}>
      <Head>
        <title>Create Next App</title>
        <meta name="description" content="Esp32 MQTT PubSub" />
        <link rel="icon" href="/favicon.ico" />
      </Head>

      <main className={styles.main}>
        <h1 className={styles.title}>Esp32 MQTT PubSub</h1>

        <div>
          <h2>Connection status: {connectionStatus ? "Open" : "Closed"}</h2>
          <p>Led status: {ledMode === "ON" ? "ON" : "OFF"}</p>
        </div>
      </main>
    </div>
  );
}
