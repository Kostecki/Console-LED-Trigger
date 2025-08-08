/// <reference types="vite/client" />

import {
	ColorSchemeScript,
	Container,
	createTheme,
	MantineProvider,
	mantineHtmlProps,
} from "@mantine/core";
import { Notifications } from "@mantine/notifications";
import {
	createRootRoute,
	HeadContent,
	Outlet,
	Scripts,
} from "@tanstack/react-router";
import { TanStackRouterDevtools } from "@tanstack/react-router-devtools";
import type { ReactNode } from "react";

import "@mantine/core/styles.css";
import "@mantine/notifications/styles.css";

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
				title: "Console LED Trigger",
			},
			{
				name: "apple-mobile-web-app-title",
				content: "Console LED Trigger",
			},
		],
		links: [
			{
				rel: "icon",
				type: "image/png",
				href: "/favicon/favicon-96x96.png",
				sizes: "96x96",
			},
			{
				rel: "icon",
				type: "image/svg+xml",
				href: "/favicon/favicon.svg",
			},
			{
				rel: "shortcut icon",
				href: "/favicon/favicon.ico",
			},
			{
				rel: "apple-touch-icon",
				href: "/favicon/apple-touch-icon.png",
				sizes: "180x180",
			},
			{
				rel: "manifest",
				href: "/favicon/site.webmanifest",
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
					<Notifications />
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
