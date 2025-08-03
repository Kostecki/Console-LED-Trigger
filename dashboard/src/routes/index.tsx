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
			brightness: 255,
			status: 0,
		},
	},
	{
		id: "board-3",
		name: "Xbox Series X",
		status: 1,
		leds: {
			color: "#0000FF",
			brightness: 200,
			status: 1,
		},
	},
	{
		id: "board-4",
		name: "Sega Genesis",
		status: 0,
		leds: {
			color: "#FFFF00",
			brightness: 100,
			status: 0,
		},
	},
];

const boardsSorted = boards.sort((a, b) => a.id.localeCompare(b.id));

function Home() {
	return (
		<>
			{boardsSorted.map((board) => (
				<ConsoleCard key={board.id} board={board} />
			))}
		</>
	);
}
