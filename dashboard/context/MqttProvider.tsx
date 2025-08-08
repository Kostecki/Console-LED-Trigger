import type { MqttClient } from "mqtt";
import mqtt from "mqtt";
import {
	createContext,
	type ReactNode,
	useEffect,
	useRef,
	useState,
} from "react";

export const MqttContext = createContext<MqttClient | null>(null);

export const MqttProvider = ({ children }: { children: ReactNode }) => {
	const [client, setClient] = useState<MqttClient | null>(null);
	const clientRef = useRef<MqttClient | null>(null);

	const MQTT_URL = import.meta.env.VITE_MQTT_URL;
	const MQTT_USERNAME = import.meta.env.VITE_MQTT_USERNAME;
	const MQTT_PASSWORD = import.meta.env.VITE_MQTT_PASSWORD; // This isn't actually very secure, but it's fine for lan only 🤷‍♂️

	useEffect(() => {
		if (clientRef.current) {
			return;
		}

		const mqttClient = mqtt.connect(MQTT_URL, {
			clientId: `dashboard-${Math.random().toString(16).slice(2)}`,
			username: MQTT_USERNAME,
			password: MQTT_PASSWORD,
			reconnectPeriod: 1000,
		});

		clientRef.current = mqttClient;

		const handleConnect = () => {
			console.log("MQTT client connected");
			console.log("URL: ", MQTT_URL);
			setClient(mqttClient);
		};

		mqttClient.on("connect", handleConnect);

		return () => {
			mqttClient.off("connect", handleConnect);
			mqttClient.end(true);
			clientRef.current = null;
		};
	}, []);

	return <MqttContext.Provider value={client}>{children}</MqttContext.Provider>;
};
