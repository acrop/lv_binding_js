import assert from 'tjs:assert';
import path from 'tjs:path';

const { createEchoServer } = await import(path.join(import.meta.dirname, '../../helpers/echo-server.js'));

const { server, baseUrl } = createEchoServer();

async function basicFetch() {
    const r = await fetch(`${baseUrl}/get`);
    assert.eq(r.status, 200, 'status is 200');
}

async function abortFetch() {
    const controller = new AbortController();
    const signal = controller.signal;
    setTimeout(() => {
        controller.abort();
    }, 500);
    try {
        await fetch(`${baseUrl}/delay/3`, { signal });
    } catch (e) {
        assert.eq(e.name, 'AbortError', 'fetch was aborted');
    }
}

async function fetchWithPostAndBody() {
    const data = JSON.stringify({ foo: 'bar', bar: 'baz' });
    const r = await fetch(`${baseUrl}/post`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: data,
    });
    assert.eq(r.status, 200, 'status is 200');
    const json = await r.json();
    assert.eq(JSON.stringify(json.data), data, 'sent and received data match');
}

(async () => {
    await basicFetch();
    await abortFetch();
    await fetchWithPostAndBody();
    await server.close();
})();
