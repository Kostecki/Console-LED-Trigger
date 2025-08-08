import { createFileRoute } from "@tanstack/react-router";
import { MqttProvider } from "context/MqttProvider";
import { useBoards } from "hooks/useBoards";
import { LoadingState } from "src/components/LoadingState";
import { EmptyBoards } from "src/components/NoBoards";
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
	const { boards, ready } = useBoards();

	if (!ready) return <LoadingState />;
	if (boards.length === 0) return <EmptyBoards />;

	return boards.map((board) => <ConsoleCard key={board.id} board={board} />);
}
