# Stage 1: Build the app
FROM node:24-slim AS builder

ENV CI=true

WORKDIR /app
RUN corepack enable

# Copy everything
COPY shared ./shared
COPY dashboard/. .

# Install dependencies
RUN pnpm install --frozen-lockfile

# Build the app
ENV NODE_ENV=production
RUN pnpm run build

# Stage 2: Runtime container
FROM node:24-slim

ENV CI=true

WORKDIR /app
RUN corepack enable && corepack prepare pnpm@10.14.0 --activate

# Copy only what's needed for runtime
COPY --from=builder /app/.output .output
COPY --from=builder /app/package.json .
COPY --from=builder /app/pnpm-lock.yaml .
COPY --from=builder /app/node_modules ./node_modules

EXPOSE 3000

CMD ["node", ".output/server/index.mjs"]