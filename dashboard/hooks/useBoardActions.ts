import { MqttContext } from "context/MqttProvider";
import { useContext } from "react";

export function useBoardActions() {
  const client = useContext(MqttContext);

  const setBoardName = async (id: string, name: string): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${id}/set`;
    const payload = JSON.stringify({ name });

    return new Promise((resolve, reject) => {
      client.publish(topic, payload, { qos: 1 }, (err) => {
        if (err) {
          console.error(`Failed to publish to ${topic}:`, err);
          reject(err);
        } else {
          resolve();
        }
      });
    });
  };

  const setColorSettings = async (
    id: string,
    color: string,
    brightness: number
  ): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${id}/set`;
    const payload = JSON.stringify({ color, brightness });

    return new Promise((resolve, reject) => {
      client.publish(topic, payload, { qos: 1 }, (err) => {
        if (err) {
          console.error(`Failed to publish to ${topic}:`, err);
          reject(err);
        } else {
          resolve();
        }
      });
    });
  };

  const identifyBoard = async (id: string): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${id}/identify`;
    const payload = "1";

    return new Promise((resolve, reject) => {
      client.publish(topic, payload, { qos: 1 }, (err) => {
        if (err) {
          console.error(`Failed to publish to ${topic}:`, err);
          reject(err);
        } else {
          resolve();
        }
      });
    });
  };

  const sendFirmwareUpdate = async (
    id: string,
    firmwareUrl: string
  ): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${id}/update`;
    const payload = JSON.stringify({ firmwareUrl });

    return new Promise((resolve, reject) => {
      client.publish(topic, payload, { qos: 1 }, (err) => {
        if (err) {
          console.error(`Failed to publish to ${topic}:`, err);
          reject(err);
        } else {
          resolve();
        }
      });
    });
  };

  const rebootBoard = async (id: string): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${id}/reboot`;
    const payload = "1";

    return new Promise((resolve, reject) => {
      client.publish(topic, payload, { qos: 1 }, (err) => {
        if (err) {
          console.error(`Failed to publish to ${topic}:`, err);
          reject(err);
        } else {
          resolve();
        }
      });
    });
  };

  return {
    setBoardName,
    setColorSettings,
    identifyBoard,
    sendFirmwareUpdate,
    rebootBoard,
  };
}
