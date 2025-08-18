import { MqttContext } from "context/MqttProvider";
import { useContext } from "react";
import { percentageToBrightness, swatches } from "src/utils";

export function useBoardActions() {
  const client = useContext(MqttContext);

  const setBoardName = async (boardId: string, name: string): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${boardId}/set`;
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
    boardId: string,
    color: string,
    brightness: number
  ): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const colorIndex = swatches.indexOf(color);
    const jsonBody = {
      color: colorIndex >= 0 ? colorIndex : -1,
      customColor: colorIndex >= 0 ? null : color,
      brightness: percentageToBrightness(brightness),
    };

    const topic = `console/${boardId}/set`;
    const payload = JSON.stringify(jsonBody);

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

  const identifyBoard = async (boardId: string): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${boardId}/identify`;
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
    boardId: string,
    firmwareUrl: string
  ): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${boardId}/fw-update`;
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

  const rebootBoard = async (boardId: string): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${boardId}/reboot`;
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

  const startCalibration = async (boardId: string): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${boardId}/calibrate`;
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

  const setCurrentThresholdOffset = async (
    boardId: string,
    offsetValue: number
  ): Promise<void> => {
    if (!client || !client.connected) {
      throw new Error("MQTT client not connected");
    }

    const topic = `console/${boardId}/set`;
    const jsonBody = {
      thresholdOffset: offsetValue,
    };
    const payload = JSON.stringify(jsonBody);

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
    startCalibration,
    setCurrentThresholdOffset,
  };
}
