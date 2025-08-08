import mqtt, { type MqttClient } from "mqtt";

const mqtt_host = "mqtt.lan";
const mqtt_port = 9001;

let client: MqttClient | null = null;

export const getMqttClient = (): MqttClient => {
  if (client) return client;

  const clientId = `console-led-trigger-dashboard-${Math.random()
    .toString(16)
    .slice(2)}`;
  client = mqtt.connect(`ws://${mqtt_host}:${mqtt_port}`, {
    clientId,
    clean: true,
  });

  return client;
};
