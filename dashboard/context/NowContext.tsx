import type React from "react";
import { createContext, useContext, useEffect, useState } from "react";

const UPDATE_INTERVAL = 60000; // 1 minute

const NowContext = createContext(Date.now());

export const NowProvider = ({ children }: { children: React.ReactNode }) => {
	const [now, setNow] = useState(Date.now());

	useEffect(() => {
		const id = setInterval(() => setNow(Date.now()), UPDATE_INTERVAL);
		return () => clearInterval(id);
	}, []);
	return <NowContext.Provider value={now}>{children}</NowContext.Provider>;
};

export const useNow = () => useContext(NowContext);
