import fs from "node:fs/promises";
import path from "node:path";
import { createServerFileRoute } from "@tanstack/react-start/server";

const fileName = "latest.bin";

export const ServerRoute = createServerFileRoute("/api/upload").methods({
  POST: async ({ request }) => {
    const arrayBuffer = await request.arrayBuffer();
    const buffer = Buffer.from(arrayBuffer);

    const firmwareDir = path.join(process.cwd(), "public", "firmware");
    const destPath = path.join(firmwareDir, fileName);

    await fs.mkdir(firmwareDir, { recursive: true });
    await fs.writeFile(destPath, buffer);

    const origin = new URL(request.url).origin;
    const fullUrl = new URL(`/firmware/${fileName}`, origin).toString();

    return new Response(JSON.stringify({ url: fullUrl }), {
      status: 200,
      headers: { "Content-Type": "application/json" },
    });
  },
});
