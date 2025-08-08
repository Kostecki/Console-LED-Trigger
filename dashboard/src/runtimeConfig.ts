type RawConfig = {
  MQTT_URL?: string;
  MQTT_USERNAME?: string;
  MQTT_PASSWORD?: string;
};

export type ResolvedConfig = {
  MQTT_URL: string;
  MQTT_USERNAME: string;
  MQTT_PASSWORD: string;
};

declare global {
  interface Window {
    __APP_CONFIG__?: RawConfig;
  }
}

function readConfigFromDom(): RawConfig {
  const el = document.getElementById("app-config") as HTMLScriptElement | null;
  if (!el) return {};
  try {
    return JSON.parse(el.textContent || "{}");
  } catch {
    return {};
  }
}

export function getRuntimeConfig() {
  const isServer = typeof window === "undefined";
  if (isServer) {
    return {
      MQTT_URL: process.env.MQTT_URL ?? "",
      MQTT_USERNAME: process.env.MQTT_USERNAME ?? "",
      MQTT_PASSWORD: process.env.MQTT_PASSWORD ?? "",
    };
  }
  const cfg = (window.__APP_CONFIG__ as RawConfig) ?? readConfigFromDom();
  const dev = import.meta.env.DEV;
  return {
    MQTT_URL: cfg.MQTT_URL ?? (dev ? import.meta.env.VITE_MQTT_URL ?? "" : ""),
    MQTT_USERNAME:
      cfg.MQTT_USERNAME ??
      (dev ? import.meta.env.VITE_MQTT_USERNAME ?? "" : ""),
    MQTT_PASSWORD:
      cfg.MQTT_PASSWORD ??
      (dev ? import.meta.env.VITE_MQTT_PASSWORD ?? "" : ""),
  };
}
