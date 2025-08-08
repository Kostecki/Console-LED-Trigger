import { MqttContext } from "context/MqttProvider";
import { useContext, useEffect, useState } from "react";
import type { Board, OnlineStatus } from "types/board";

const defaultLedState = {
  colorMode: "palette",
  customColor: "#000000",
  colorIndex: 0,
  brightness: 0,
  status: 0,
  bootTime: Date.now() / 1000,
};

export function useBoards(): { boards: Board[]; ready: boolean } {
  const client = useContext(MqttContext);
  const [boards, setBoards] = useState<Record<string, Board>>({});
  const [ready, setReady] = useState(false);

  useEffect(() => {
    if (!client) return;

    let gotMessage = false;

    const mergeBoard = (id: string, partial: Partial<Board>) => {
      setBoards((prev) => {
        const prevBoard = prev[id] ?? {
          id,
          name: id,
          status: 0,
          leds: {
            colorMode: defaultLedState.colorMode,
            customColor: defaultLedState.customColor,
            colorIndex: defaultLedState.colorIndex,
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
            colorMode: parsed.colorMode || defaultLedState.colorMode,
            colorIndex: parsed.colorIndex || defaultLedState.colorIndex,
            customColor: parsed.customColor || defaultLedState.customColor,
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

      gotMessage = true;
      setReady(true);
    };

    client.subscribe("console/+/status", { qos: 1 });
    client.subscribe("console/+/state", { qos: 1 });
    client.on("message", handler);

    // Fallback: mark ready even if we got nothing after a short delay
    const t = setTimeout(() => {
      if (!gotMessage) setReady(true);
    }, 500);

    return () => {
      clearTimeout(t);
      client.unsubscribe("console/+/status");
      client.unsubscribe("console/+/state");
      client.off("message", handler);
    };
  }, [client]);

  return {
    boards: Object.values(boards).sort((a, b) => a.id.localeCompare(b.id)),
    ready,
  };
}
