import { createFileRoute } from "@tanstack/react-router";
import { MqttProvider } from "context/MqttProvider";
import { useBoards } from "hooks/useBoards";
import { ConsoleCard } from "../components/ConsoleCard";

export const Route = createFileRoute("/")({
	component: Home,
});

function Home() {
	return (
		<MqttProvider>
			<BoardsDashboard />
		</MqttProvider>
	);
}

function BoardsDashboard() {
	const boards = useBoards();

	console.log("Boards:", boards);

	return (
		<>
			{boards.map((board) => (
				<ConsoleCard key={board.id} board={board} />
			))}
		</>
	);
}
