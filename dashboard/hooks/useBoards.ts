import { MqttContext } from "context/MqttProvider";
import { useContext, useEffect, useState } from "react";
import type { Board, OnlineStatus } from "types/board";

const defaultLedState = {
  color: "#000000",
  brightness: 0,
  status: 0,
  bootTime: Date.now() / 1000,
};

export function useBoards(): Board[] {
  const client = useContext(MqttContext);
  const [boards, setBoards] = useState<Record<string, Board>>({});

  useEffect(() => {
    if (!client) return;

    const mergeBoard = (id: string, partial: Partial<Board>) => {
      setBoards((prev) => {
        const prevBoard = prev[id] ?? {
          id,
          name: id,
          status: 0,
          leds: {
            color: defaultLedState.color,
            brightness: defaultLedState.brightness,
            status: defaultLedState.status,
          },
          bootTime: defaultLedState.bootTime,
        };

        return {
          ...prev,
          [id]: {
            ...prevBoard,
            ...partial,
            name: partial.name ?? prevBoard.name,
            leds: {
              ...prevBoard.leds,
              ...partial.leds,
            },
          },
        };
      });
    };

    const updateStatus = (topic: string, payload: string) => {
      const id = topic.split("/")[1];
      const isOnline: OnlineStatus = payload === "1" ? 1 : 0;
      mergeBoard(id, { status: isOnline });
    };

    const updateState = (topic: string, payload: string) => {
      const id = topic.split("/")[1];

      try {
        const parsed = JSON.parse(payload);

        if (typeof parsed.bootTime !== "number") {
          console.warn(`Invalid bootTime for board ${id}:`, parsed.bootTime);
          return;
        }

        mergeBoard(id, {
          name: parsed.name,
          bootTime: parsed.bootTime || defaultLedState.bootTime,
          leds: {
            color: parsed.color || defaultLedState.color,
            brightness: parsed.brightness || defaultLedState.brightness,
            status: parsed.status || defaultLedState.status,
          },
        });
      } catch (error) {
        console.warn(`Failed to parse state for board ${id}:`, error);
      }
    };

    const handler = (topic: string, buffer: Buffer) => {
      const payload = buffer.toString();

      if (topic.match(/^console\/[^/]+\/status$/)) {
        updateStatus(topic, payload);
      } else if (topic.match(/^console\/[^/]+\/state$/)) {
        updateState(topic, payload);
      }
    };

    client.subscribe("console/+/status", { qos: 1 });
    client.subscribe("console/+/state", { qos: 1 });
    client.on("message", handler);

    return () => {
      client.unsubscribe("console/+/status");
      client.unsubscribe("console/+/state");
      client.off("message", handler);
    };
  }, [client]);

  return Object.values(boards).sort((a, b) => a.id.localeCompare(b.id));
}
