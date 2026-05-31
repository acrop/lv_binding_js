// Local httpbin-compatible endpoints for offline fetch tests.

export function createEchoServer() {
    const server = tjs.serve({
        port: 0,
        fetch: async (req) => {
            const url = new URL(req.url);
            const pathname = url.pathname;

            const delayMatch = pathname.match(/^\/delay\/(\d+)$/);
            if (delayMatch) {
                const seconds = parseInt(delayMatch[1], 10);
                await new Promise((r) => setTimeout(r, seconds * 1000));
                return Response.json({ delay: seconds });
            }

            if (pathname === '/get' && req.method === 'GET') {
                const headers = {};
                for (const [ k, v ] of req.headers) {
                    headers[k] = v;
                }
                const args = {};
                for (const [ k, v ] of url.searchParams) {
                    args[k] = v;
                }
                return Response.json({ args, headers, url: req.url });
            }

            if (pathname === '/post' && req.method === 'POST') {
                const contentType = req.headers.get('content-type') || '';
                const body = await req.text();
                const headers = {};
                for (const [ k, v ] of req.headers) {
                    headers[k] = v;
                }

                let data;
                if (contentType.includes('application/json')) {
                    try {
                        data = JSON.parse(body);
                    } catch {
                        data = body;
                    }
                } else {
                    data = body;
                }

                return Response.json({ data, headers, url: req.url });
            }

            return new Response('Not Found', { status: 404 });
        },
    });

    const baseUrl = `http://127.0.0.1:${server.port}`;
    return { server, baseUrl };
}
