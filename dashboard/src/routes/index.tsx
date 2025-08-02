import { createFileRoute } from "@tanstack/react-router";

import type { Board } from "../../types/board";
import { ConsoleCard } from "../components/ConsoleCard";

export const Route = createFileRoute("/")({
	component: Home,
});

const boards: Board[] = [
	{
		id: "board-1",
		name: "Playstation 5",
		status: 1,
		leds: {
			color: "#ff0000",
			brightness: 128,
			status: 1,
		},
	},
	{
		id: "board-2",
		name: "Nintendo 64",
		status: 0,
		leds: {
			color: "#00FF00",
			brightness: 128,
			status: 0,
		},
	},
];

function Home() {
	return (
		<>
			{boards.map((board) => (
				<ConsoleCard key={board.id} board={board} />
			))}
		</>
	);
}
