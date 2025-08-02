/// <reference types="vite/client" />

import {
	ColorSchemeScript,
	Container,
	createTheme,
	MantineProvider,
	mantineHtmlProps,
} from "@mantine/core";
import {
	createRootRoute,
	HeadContent,
	Outlet,
	Scripts,
} from "@tanstack/react-router";
import { TanStackRouterDevtools } from "@tanstack/react-router-devtools";
import type { ReactNode } from "react";

import "@mantine/core/styles.css";

const theme = createTheme({});

export const Route = createRootRoute({
	head: () => ({
		meta: [
			{
				charSet: "utf-8",
			},
			{
				name: "viewport",
				content: "width=device-width, initial-scale=1",
			},
			{
				title: "TanStack Start Starter",
			},
		],
	}),
	component: RootComponent,
});

function RootComponent() {
	return (
		<RootDocument>
			<Outlet />
		</RootDocument>
	);
}

function RootDocument({ children }: Readonly<{ children: ReactNode }>) {
	return (
		<html {...mantineHtmlProps}>
			<head>
				<HeadContent />
				<ColorSchemeScript />
			</head>
			<body>
				<MantineProvider theme={theme}>
					<Container mt="xl" size="xs">
						{children}
					</Container>
					<TanStackRouterDevtools position="bottom-right" />
					<Scripts />
				</MantineProvider>
			</body>
		</html>
	);
}
